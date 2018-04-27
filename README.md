# joystick
Project submission for Device Drivers course.

### Summary
This project aims to demonstrate the functionality of a joystick, that is interfaced via USB. The hardware part include joystick, ATMEGA8 microcontroller,components for interfacing to USB. The firmware part includes a C program that controls PORTB & PORTC output depending on the USB request received from the USB core. The device driver requests to USB core via usb_control_msg(). The driver is automatically plugged in (registered) on detection of an allowed device (allowed devices are controlled via id-table) and unregistered on disconnection.

### Hardware Schematic
![Schematic](https://github.com/h20170172/joystick/blob/master/Capture.PNG)

### Hardware Snapshots
![Snapshot1](https://github.com/h20170172/joystick/blob/master/IMG_20180427_132356.jpg)

### Firmware Description
Firmware is present in the folder /firmware. Relevant files are main.c, Makefile, and ../commands.h. Folder usbdrv and file usbconfig.h are also necessery for successful build. This firmware closely follows the V-USB implementation by Starkjohann. "make hex" builds main.hex file and "make flash" flashes hex file to the hardware but requires hardware to be reset once to enter USBasp mode.
* **Build process**:
```shell
$ make clean
$ make hex
$ make flash
```

### Driver Description
Device Driver is present in the joy_driver directory. Relevant files are usb_2.c, Makefile, and commands.h. Device driver is a input subsystem-class driver implemented over USB interface. "make kern" or "make all" is used to build .o and .ko files. "make insert" is used to insert the kernel-module, and "make remove" can be used to remove the kernel-module. "sudo rmmod usbhid" is required for proper functionality of the custom driver.
* **Build process**:
```shell
$ sudo rmmod usbhid
$ make clean
$ sudo modprobe input-polldev
$ sudo insmod usb_2.ko
```

### User Space Application
For User-space applications we are checking the input axis of joystick.For this we go to /dev/input/ directory and Run sudo evtest event_name.This will shows all the occuring events.


### Authors
Prateek & NIKHIL, ME Embedded Systems, BITS Pilani 

## License
* This project is licensed under the GNU-GPL License.
