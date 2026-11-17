#ifndef _HISI_USB_VBUS_H_FOR_HISI_TCPC_
#define _HISI_USB_VBUS_H_FOR_HISI_TCPC_

int hisi_usb_vbus_status_v800(void);
void hisi_usb_vbus_init_v800(void *tcpc_data);
void hisi_usb_vbus_exit_v800(void);
void hisi_tcpc_vbus_irq_handler_v800(void *data, int vbus_status);

#endif
