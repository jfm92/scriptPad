#include "pico/unique_id.h"
#include "tusb.h"

enum
{
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_GAMEPAD,
  REPORT_ID_COUNT
};


uint8_t webUse = 1;

#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
#define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
                           _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4) | _PID_MAP(ECM_RNDIS, 5) | _PID_MAP(NCM, 5))

#define USB_VID   0xCafe
#define USB_BCD   0x0200

#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
#define EPNUM_HID   0x81

char serial[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

static uint16_t _desc_str[32];

char const* string_desc_arr [] =
{
  (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
  "TinyUSB",                     // 1: Manufacturer
  "TinyUSB Device",              // 2: Product
  serial,                        // 3: Serials, uses the flash ID
};

enum
{
  ITF_NUM_HID,
  ITF_NUM_TOTAL
};

/////////////////////////////////////////////////////////////
enum
{
  ITF_NUM_CDC = 0,
  ITF_NUM_CDC_DATA,
  //ITF_NUM_TOTAL
};

enum
{
  CONFIG_ID_RNDIS = 0,
  CONFIG_ID_ECM   = 1,
  CONFIG_ID_COUNT
};

#define MAIN_CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_RNDIS_DESC_LEN)
#define EPNUM_NET_NOTIF   0x81
#define EPNUM_NET_OUT     0x02
#define EPNUM_NET_IN      0x82

#define CFG_TUD_NET_ENDPOINT_SIZE (TUD_OPT_HIGH_SPEED ? 512 : 64)
/////////////////////////////////////////////////////////////

tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,

    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0101,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x02
};


uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}

uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(REPORT_ID_KEYBOARD         )),
  TUD_HID_REPORT_DESC_MOUSE   ( HID_REPORT_ID(REPORT_ID_MOUSE            )),
  TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL )),
  TUD_HID_REPORT_DESC_GAMEPAD ( HID_REPORT_ID(REPORT_ID_GAMEPAD          ))
};

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
  (void) instance;
  return desc_hid_report;
}

uint8_t const hid_configuration[] =
{
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
  TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 5),
};

uint8_t const rndis_configuration[] =
{
  TUD_CONFIG_DESCRIPTOR(1, 2, 0, MAIN_CONFIG_TOTAL_LEN, 0, 100),
  TUD_RNDIS_DESCRIPTOR(0, 4, EPNUM_NET_NOTIF, 8, EPNUM_NET_OUT, EPNUM_NET_IN, CFG_TUD_NET_ENDPOINT_SIZE)
};

static uint8_t const * const configuration_arr[2] =
{

  [1] = rndis_configuration,
  [0] = hid_configuration

};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  return (webUse == 1) ? rndis_configuration : hid_configuration;
  //return (index < CONFIG_ID_COUNT) ? configuration_arr[index] : NULL;
}

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }
  else if (5 == index)
  {
    // Convert MAC address into UTF-16

    for (unsigned i=0; i<sizeof(tud_network_mac_address); i++)
    {
      _desc_str[1+chr_count++] = "0123456789ABCDEF"[(tud_network_mac_address[i] >> 4) & 0xf];
      _desc_str[1+chr_count++] = "0123456789ABCDEF"[(tud_network_mac_address[i] >> 0) & 0xf];
    }
  }
  else
  {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if (index == 3) pico_get_unique_board_id_string(serial, sizeof(serial));
    if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;
    
    if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

    const char* str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    // Convert ASCII string into UTF-16
    for(uint8_t i=0; i<chr_count; i++)
    {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

  return _desc_str;
}
