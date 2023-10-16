#define BOARD_DEVICE_RHPORT_NUM     0
#define BOARD_DEVICE_RHPORT_SPEED   OPT_MODE_FULL_SPEED
#define CFG_TUSB_RHPORT0_MODE     (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)

#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(4)))
#define CFG_TUD_ENDPOINT0_SIZE    64

#define CFG_TUH_ENUMERATION_BUFSIZE 256

#define CFG_TUH_HUB               0
#define CFG_TUD_HID               1
#define CFG_TUD_CDC               0
#define CFG_TUD_MSC               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0

#define CFG_TUD_HID_EP_BUFSIZE    16

/////////////////////////////////////////////////////////
//Modem stuff
/////////////////////////////////////////////////////////
/*#define CFG_TUD_ENABLED       1
//#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

#define CFG_TUD_ECM_RNDIS     1
#define CFG_TUD_NCM           (1-CFG_TUD_ECM_RNDIS)

#define CFG_TUH_DEVICE_MAX          2*/