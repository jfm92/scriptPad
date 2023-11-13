#include "bsp/board_api.h"
#include "tusb.h"

#include "dhserver.h"
#include "dnserver.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "httpd.h"
#include "lwip/opt.h"

#include "lwip/apps/httpd.h"
#include "lwip/def.h"
#include "lwip/mem.h"

#include "modemUSBManagement.h"
#include "../filesManagement/filesManagement.h"

#define INIT_IP4(a,b,c,d) { PP_HTONL(LWIP_MAKEU32(a,b,c,d)) }

////////////////////////////////////////////////////////
extern "C"
{
  sys_prot_t sys_arch_protect(void);
  void sys_arch_unprotect(sys_prot_t pval);
  uint32_t sys_now(void);
}

static struct pbuf *received_frame;

/////////////////////////////////////////////////////////////

static ip4_addr_t ipaddr;
static ip4_addr_t netmask;
static ip4_addr_t gateway;
//No other way to set MAC Address, it must be harcoded in this way...
uint8_t tud_network_mac_address[6] = {0x02,0x02,0x84,0x6A,0x96,0x00};


/************************** TinyUSB CallBacks *********************************/
bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
  if (received_frame) return false;

  if (size)
  {
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

    if (p)
    {
      memcpy(p->payload, src, size);
      received_frame = p;
    }
  }

  return true;
}

void tud_network_init_cb(void)
{
  /* if the network is re-initializing and we have a leftover packet, we must do a cleanup */
  if (received_frame)
  {
    pbuf_free(received_frame);
    received_frame = NULL;
  }
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
  struct pbuf *p = (struct pbuf *)ref;

  return pbuf_copy_partial(p, dst, p->tot_len, 0);
}


/************************** API POST CallBacks *********************************/
void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{

}

err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                 u16_t http_request_len, int content_len, char *response_uri,
                 u16_t response_uri_len, u8_t *post_auto_wnd)
{
  return ERR_OK;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
  return ERR_OK;
}



bool modemUSBManagement::initConfigManager()
{
  //TODO: Temporary hardcoded, this should be handle in a different way
  ipaddr  = INIT_IP4(192, 168, 7, 1);
  netmask = INIT_IP4(255, 255, 255, 0);
  gateway = INIT_IP4(0, 0, 0, 0);

  entries[0] = { {0}, INIT_IP4(192, 168, 7, 2), 24 * 60 * 60 };
  entries[1] = { {0}, INIT_IP4(192, 168, 7, 3), 24 * 60 * 60 };
  entries[2] = { {0}, INIT_IP4(192, 168, 7, 4), 24 * 60 * 60 };

  dhcp_config.router = INIT_IP4(0, 0, 0, 0);
  dhcp_config.port = 67;
  dhcp_config.dns = INIT_IP4(192, 168, 7, 1);
  dhcp_config.domain = "usb";
  dhcp_config.num_entry = TU_ARRAY_SIZE(entries);
  dhcp_config.entries = entries;

  tud_init(0);
  init_lwip();
  while (!netif_is_up(&netif_data));
  while (dhserv_init(&dhcp_config) != ERR_OK);
  while (dnserv_init(IP_ADDR_ANY, 53, dns_query_proc) != ERR_OK);
  httpd_init();
  
  return true;
}

void modemUSBManagement::taskConfig()
{
  tud_task();
  service_traffic();
}

void modemUSBManagement::service_traffic(void)
{
  /* handle any packet received by tud_network_recv_cb() */
  if (received_frame)
  {
    ethernet_input(received_frame, &netif_data);
    pbuf_free(received_frame);
    received_frame = NULL;
    tud_network_recv_renew();
  }

  sys_check_timeouts();
}

void modemUSBManagement::init_lwip(void)
{
  struct netif *netif = &netif_data;

  lwip_init();

  /* the lwip virtual MAC address must be different from the host's; to ensure this, we toggle the LSbit */
  netif->hwaddr_len = sizeof(tud_network_mac_address);
  memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));
  netif->hwaddr[5] ^= 0x01;

  netif = netif_add(netif, &ipaddr, &netmask, &gateway, NULL, netif_init_cb, ip_input);
#if LWIP_IPV6
  netif_create_ip6_linklocal_address(netif, 1);
#endif
  netif_set_default(netif);
}

bool modemUSBManagement::dns_query_proc(const char *name, ip4_addr_t *addr)
{
  if (0 == strcmp(name, "tiny.usb"))
  {
    *addr = ipaddr;
    return true;
  }
  return false;
}

err_t modemUSBManagement::netif_init_cb(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  netif->mtu = CFG_TUD_NET_MTU;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
  netif->state = NULL;
  netif->name[0] = 'E';
  netif->name[1] = 'X';
  netif->linkoutput = linkoutput_fn;
  netif->output = ip4_output_fn;

  return ERR_OK;
}

err_t modemUSBManagement::ip4_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr)
{
  return etharp_output(netif, p, addr);
}

err_t modemUSBManagement::linkoutput_fn(struct netif *netif, struct pbuf *p)
{
  (void)netif;

  for (;;)
  {
    // if TinyUSB isn't ready, we must signal back to lwip that there is nothing we can do 
    if (!tud_ready())
      return ERR_USE;

    // if the network driver can accept another packet, we make it happen 
    if (tud_network_can_xmit(p->tot_len))
    {
      tud_network_xmit(p, 0 );
      return ERR_OK;
    }

    // transfer execution to TinyUSB in the hopes that it will finish transmitting the prior packet
    tud_task();
  }
}


//////////////////////////////////////External///////////////////////////////////////////
uint32_t sys_now(void)
{
  return board_millis();
}

sys_prot_t sys_arch_protect(void)
{
  return 0;
}
void sys_arch_unprotect(sys_prot_t pval)
{
  (void)pval;
}
