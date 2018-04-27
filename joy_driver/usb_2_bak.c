#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/input-polldev.h>
#include <linux/types.h>
MODULE_LICENSE("GPL");

#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x82
#define MAX_PKT_SIZE 512

int var_data1;
int retval;

static struct usb_device *device;
static struct usb_class_driver class;

struct input_polled_dev *j_dev;
int *var_data; 
int dummy1,dummy2;
void scan(struct input_polled_dev *j_dev)
{
/* letter S or s */
struct input_dev *dev_ip=j_dev->input;
 void *var_data=kmalloc(2,GFP_KERNEL);
    
 retval = usb_control_msg(device, usb_rcvctrlpipe(device, 0),
            0,USB_TYPE_VENDOR | USB_DIR_IN,0,0,(uint16_t *)var_data,2,5000);
 //var_data=20;
dummy = *var_data;
input_report_rel(dev_ip, REL_X, dummy1);
//input_sync(dev_ip);
input_report_rel(dev_ip, REL_Y,dummy2);
input_sync(dev_ip);

}

static int pen_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Joystick (%04X:%04X) plugged\n", id->idVendor,
                                id->idProduct);

    int retval;
   // var_data=kmalloc(2,GFP_KERNEL);
    device = interface_to_usbdev(interface);

//    class.name = "usb/pen%d";
//    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0)
    {
        /* Something prevented us from registering this driver */
        printk(KERN_ERR "Not able to get a minor for this device.");
    }
    else
    {
        printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
    }

    //return retval;

    /* initialization */
struct input_dev *dev_ip;
j_dev = input_allocate_polled_device();
if(!j_dev)
{
printk(KERN_INFO"not enough memory for input device \n ");
return -ENOMEM;
}

/* set up descriptive labels */
//dev_ip->name = "demo Device";
j_dev->poll=scan;
j_dev->poll_interval=5000; //msec
dev_ip=j_dev->input;

dev_ip->name="scan";


/*
dev_ip->id.vendor = 0xDEAD;
dev_ip->id.product = 0xBEEF;
dev_ip->id.version = 0xFACE;

//set_bit(EV_KEY, dev_ip->evbit);
//set_bit(KEY_S, dev_ip->keybit);
*/
input_set_capability(dev_ip,EV_REL,REL_X);
input_set_capability(dev_ip,EV_REL,REL_Y);

//input_set_capability(input,EV_KEY,REL_Y);

/* and finally register with the input core */
input_register_polled_device(j_dev);

return retval;
}



static void pen_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "joystick removed\n");
	input_unregister_polled_device(j_dev);
}

static struct usb_device_id pen_table[] =
{
    {USB_DEVICE(0x16c0, 0x05dc)},
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, pen_table);

static struct usb_driver pen_driver =
{
    .name = "pen_driver",
    .id_table = pen_table,
    .probe = pen_probe,
    .disconnect = pen_disconnect,
};

static int __init ip_init(void)
{
return usb_register(&pen_driver);
}

static void __exit ip_exit(void)

{
usb_deregister(&pen_driver);
}

module_init(ip_init);
module_exit(ip_exit);
