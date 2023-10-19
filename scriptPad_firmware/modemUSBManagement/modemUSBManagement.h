#ifndef MODEM_USB_MANAGEMENT
#define MODEM_USB_MANAGEMENT

#include "dhserver.h"

class modemUSBManagement
{
    public:
        
        bool initConfigManager();
        void taskConfig();

        //Singletone class initialization
        static modemUSBManagement& getInstance()
        {
            if (instanceModemUSB == nullptr)
            {
                instanceModemUSB = new modemUSBManagement();
            }
            return *instanceModemUSB;
        }

        //Delete copy class operators
        modemUSBManagement(const modemUSBManagement&) = delete;
        modemUSBManagement& operator=(const modemUSBManagement&) = delete;
            
    private:
        static modemUSBManagement* instanceModemUSB;

        modemUSBManagement(){};
 
        dhcp_config_t dhcp_config;
        dhcp_entry_t entries[3];
        struct netif netif_data;

        void service_traffic(void);
        void init_lwip(void);
        static bool dns_query_proc(const char *name, ip4_addr_t *addr);
        static err_t netif_init_cb(struct netif *netif);
        static err_t ip4_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr);
        static err_t linkoutput_fn(struct netif *netif, struct pbuf *p);

        friend bool tud_network_recv_cb(const uint8_t *src, uint16_t size);
        friend void tud_network_init_cb(void);
        friend uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg);

};


#endif