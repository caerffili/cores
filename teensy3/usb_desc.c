/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if F_CPU >= 20000000

#define USB_DESC_LIST_DEFINE
#include "usb_desc.h"
#ifdef NUM_ENDPOINTS
#include "usb_names.h"
#include "kinetis.h"
#include "avr_functions.h"

// USB Descriptors are binary data which the USB host reads to
// automatically detect a USB device's capabilities.  The format
// and meaning of every field is documented in numerous USB
// standards.  When working with USB descriptors, despite the
// complexity of the standards and poor writing quality in many
// of those documents, remember descriptors are nothing more
// than constant binary data that tells the USB host what the
// device can do.  Computers will load drivers based on this data.
// Those drivers then communicate on the endpoints specified by
// the descriptors.

// To configure a new combination of interfaces or make minor
// changes to existing configuration (eg, change the name or ID
// numbers), usually you would edit "usb_desc.h".  This file
// is meant to be configured by the header, so generally it is
// only edited to add completely new USB interfaces or features.



// **************************************************************
//   USB Device
// **************************************************************

#define LSB(n) ((n) & 255)
#define MSB(n) (((n) >> 8) & 255)

// USB Device Descriptor.  The USB host reads this first, to learn
// what type of device is connected.
static uint8_t device_descriptor[] = {
        18,                                     // bLength
        1,                                      // bDescriptorType
        0x10, 0x01,                             // bcdUSB
#ifdef DEVICE_CLASS
        DEVICE_CLASS,                           // bDeviceClass
#else
	0,
#endif
#ifdef DEVICE_SUBCLASS
        DEVICE_SUBCLASS,                        // bDeviceSubClass
#else
	0,
#endif
#ifdef DEVICE_PROTOCOL
        DEVICE_PROTOCOL,                        // bDeviceProtocol
#else
	0,
#endif
        EP0_SIZE,                               // bMaxPacketSize0
        LSB(VENDOR_ID), MSB(VENDOR_ID),         // idVendor
        LSB(PRODUCT_ID), MSB(PRODUCT_ID),       // idProduct
#ifdef BCD_DEVICE
	LSB(BCD_DEVICE), MSB(BCD_DEVICE),       // bcdDevice
#else
  // For USB types that don't explicitly define BCD_DEVICE,
  // use the minor version number to help teensy_ports
  // identify which Teensy model is used.
  #if defined(__MKL26Z64__)
        0x73, 0x02,
  #elif defined(__MK20DX128__)
        0x74, 0x02,
  #elif defined(__MK20DX256__)
        0x75, 0x02,
  #elif defined(__MK64FX512__)
        0x76, 0x02,
  #elif defined(__MK66FX1M0__)
        0x77, 0x02,
  #else
        0x00, 0x02,
  #endif
#endif
        1,                                      // iManufacturer
        2,                                      // iProduct
        3,                                      // iSerialNumber
        1                                       // bNumConfigurations
};

// These descriptors must NOT be "const", because the USB DMA
// has trouble accessing flash memory with enough bandwidth
// while the processor is executing from flash.



// **************************************************************
//   HID Report Descriptors
// **************************************************************

// Each HID interface needs a special report descriptor that tells
// the meaning and format of the data.

#ifdef KEYBOARD_INTERFACE
// Keyboard Protocol 1, HID 1.11 spec, Appendix B, page 59-60
static uint8_t keyboard_report_desc[] = {
        0x05, 0x01,                     // Usage Page (Generic Desktop),
        0x09, 0x06,                     // Usage (Keyboard),
        0xA1, 0x01,                     // Collection (Application),
        0x75, 0x01,                     //   Report Size (1),
        0x95, 0x08,                     //   Report Count (8),
        0x05, 0x07,                     //   Usage Page (Key Codes),
        0x19, 0xE0,                     //   Usage Minimum (224),
        0x29, 0xE7,                     //   Usage Maximum (231),
        0x15, 0x00,                     //   Logical Minimum (0),
        0x25, 0x01,                     //   Logical Maximum (1),
        0x81, 0x02,                     //   Input (Data, Variable, Absolute), ;Modifier keys
        0x95, 0x01,                     //   Report Count (1),
        0x75, 0x08,                     //   Report Size (8),
        0x81, 0x03,                     //   Input (Constant),          ;Reserved byte
        0x95, 0x05,                     //   Report Count (5),
        0x75, 0x01,                     //   Report Size (1),
        0x05, 0x08,                     //   Usage Page (LEDs),
        0x19, 0x01,                     //   Usage Minimum (1),
        0x29, 0x05,                     //   Usage Maximum (5),
        0x91, 0x02,                     //   Output (Data, Variable, Absolute), ;LED report
        0x95, 0x01,                     //   Report Count (1),
        0x75, 0x03,                     //   Report Size (3),
        0x91, 0x03,                     //   Output (Constant),         ;LED report padding
        0x95, 0x06,                     //   Report Count (6),
        0x75, 0x08,                     //   Report Size (8),
        0x15, 0x00,                     //   Logical Minimum (0),
        0x25, 0x7F,                     //   Logical Maximum(104),
        0x05, 0x07,                     //   Usage Page (Key Codes),
        0x19, 0x00,                     //   Usage Minimum (0),
        0x29, 0x7F,                     //   Usage Maximum (104),
        0x81, 0x00,                     //   Input (Data, Array),       ;Normal keys
        0xC0                            // End Collection
};
#endif

#ifdef KEYMEDIA_INTERFACE
static uint8_t keymedia_report_desc[] = {
        0x05, 0x0C,                     // Usage Page (Consumer)
        0x09, 0x01,                     // Usage (Consumer Controls)
        0xA1, 0x01,                     // Collection (Application)
        0x75, 0x0A,                     //   Report Size (10)
        0x95, 0x04,                     //   Report Count (4)
        0x19, 0x00,                     //   Usage Minimum (0)
        0x2A, 0x9C, 0x02,               //   Usage Maximum (0x29C)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x26, 0x9C, 0x02,               //   Logical Maximum (0x29C)
        0x81, 0x00,                     //   Input (Data, Array)
        0x05, 0x01,                     //   Usage Page (Generic Desktop)
        0x75, 0x08,                     //   Report Size (8)
        0x95, 0x03,                     //   Report Count (3)
        0x19, 0x00,                     //   Usage Minimum (0)
        0x29, 0xB7,                     //   Usage Maximum (0xB7)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x26, 0xB7, 0x00,               //   Logical Maximum (0xB7)
        0x81, 0x00,                     //   Input (Data, Array)
        0xC0                            // End Collection
};
#endif

#ifdef MOUSE_INTERFACE
// Mouse Protocol 1, HID 1.11 spec, Appendix B, page 59-60, with wheel extension
static uint8_t mouse_report_desc[] = {
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x02,                     // Usage (Mouse)
        0xA1, 0x01,                     // Collection (Application)
        0x85, 0x01,                     //   REPORT_ID (1)
        0x05, 0x09,                     //   Usage Page (Button)
        0x19, 0x01,                     //   Usage Minimum (Button #1)
        0x29, 0x08,                     //   Usage Maximum (Button #8)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x25, 0x01,                     //   Logical Maximum (1)
        0x95, 0x08,                     //   Report Count (8)
        0x75, 0x01,                     //   Report Size (1)
        0x81, 0x02,                     //   Input (Data, Variable, Absolute)
        0x05, 0x01,                     //   Usage Page (Generic Desktop)
        0x09, 0x30,                     //   Usage (X)
        0x09, 0x31,                     //   Usage (Y)
        0x09, 0x38,                     //   Usage (Wheel)
        0x15, 0x81,                     //   Logical Minimum (-127)
        0x25, 0x7F,                     //   Logical Maximum (127)
        0x75, 0x08,                     //   Report Size (8),
        0x95, 0x03,                     //   Report Count (3),
        0x81, 0x06,                     //   Input (Data, Variable, Relative)
        0x05, 0x0C,                     //   Usage Page (Consumer)
        0x0A, 0x38, 0x02,               //   Usage (AC Pan)
        0x15, 0x81,                     //   Logical Minimum (-127)
        0x25, 0x7F,                     //   Logical Maximum (127)
        0x75, 0x08,                     //   Report Size (8),
        0x95, 0x01,                     //   Report Count (1),
        0x81, 0x06,                     //   Input (Data, Variable, Relative)
        0xC0,                           // End Collection
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x02,                     // Usage (Mouse)
        0xA1, 0x01,                     // Collection (Application)
        0x85, 0x02,                     //   REPORT_ID (2)
        0x05, 0x01,                     //   Usage Page (Generic Desktop)
        0x09, 0x30,                     //   Usage (X)
        0x09, 0x31,                     //   Usage (Y)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x26, 0xFF, 0x7F,               //   Logical Maximum (32767)
        0x75, 0x10,                     //   Report Size (16),
        0x95, 0x02,                     //   Report Count (2),
        0x81, 0x02,                     //   Input (Data, Variable, Absolute)
        0xC0                            // End Collection
};
#endif

#ifdef JOYSTICK_INTERFACE
#if JOYSTICK_SIZE == 12
static uint8_t joystick_report_desc[] = {
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x04,                     // Usage (Joystick)
        0xA1, 0x01,                     // Collection (Application)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x25, 0x01,                     //   Logical Maximum (1)
        0x75, 0x01,                     //   Report Size (1)
        0x95, 0x20,                     //   Report Count (32)
        0x05, 0x09,                     //   Usage Page (Button)
        0x19, 0x01,                     //   Usage Minimum (Button #1)
        0x29, 0x20,                     //   Usage Maximum (Button #32)
        0x81, 0x02,                     //   Input (variable,absolute)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x25, 0x07,                     //   Logical Maximum (7)
        0x35, 0x00,                     //   Physical Minimum (0)
        0x46, 0x3B, 0x01,               //   Physical Maximum (315)
        0x75, 0x04,                     //   Report Size (4)
        0x95, 0x01,                     //   Report Count (1)
        0x65, 0x14,                     //   Unit (20)
        0x05, 0x01,                     //   Usage Page (Generic Desktop)
        0x09, 0x39,                     //   Usage (Hat switch)
        0x81, 0x42,                     //   Input (variable,absolute,null_state)
        0x05, 0x01,                     //   Usage Page (Generic Desktop)
        0x09, 0x01,                     //   Usage (Pointer)
        0xA1, 0x00,                     //   Collection ()
        0x15, 0x00,                     //     Logical Minimum (0)
        0x26, 0xFF, 0x03,               //     Logical Maximum (1023)
        0x75, 0x0A,                     //     Report Size (10)
        0x95, 0x04,                     //     Report Count (4)
        0x09, 0x30,                     //     Usage (X)
        0x09, 0x31,                     //     Usage (Y)
        0x09, 0x32,                     //     Usage (Z)
        0x09, 0x35,                     //     Usage (Rz)
        0x81, 0x02,                     //     Input (variable,absolute)
        0xC0,                           //   End Collection
        0x15, 0x00,                     //   Logical Minimum (0)
        0x26, 0xFF, 0x03,               //   Logical Maximum (1023)
        0x75, 0x0A,                     //   Report Size (10)
        0x95, 0x02,                     //   Report Count (2)
        0x09, 0x36,                     //   Usage (Slider)
        0x09, 0x36,                     //   Usage (Slider)
        0x81, 0x02,                     //   Input (variable,absolute)
        0xC0                            // End Collection
};
#elif JOYSTICK_SIZE == 64
// extreme joystick  (to use this, edit JOYSTICK_SIZE to 64 in usb_desc.h)
//  128 buttons   16
//    6 axes      12
//   17 sliders   34
//    4 pov        2
static uint8_t joystick_report_desc[] = {
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x04,                     // Usage (Joystick)
        0xA1, 0x01,                     // Collection (Application)
        0x15, 0x00,                     // Logical Minimum (0)
        0x25, 0x01,                     // Logical Maximum (1)
        0x75, 0x01,                     // Report Size (1)
        0x95, 0x80,                     // Report Count (128)
        0x05, 0x09,                     // Usage Page (Button)
        0x19, 0x01,                     // Usage Minimum (Button #1)
        0x29, 0x80,                     // Usage Maximum (Button #128)
        0x81, 0x02,                     // Input (variable,absolute)
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x01,                     // Usage (Pointer)
        0xA1, 0x00,                     // Collection ()
        0x15, 0x00,                     // Logical Minimum (0)
        0x27, 0xFF, 0xFF, 0, 0,         // Logical Maximum (65535)
        0x75, 0x10,                     // Report Size (16)
        0x95, 23,                       // Report Count (23)
        0x09, 0x30,                     // Usage (X)
        0x09, 0x31,                     // Usage (Y)
        0x09, 0x32,                     // Usage (Z)
        0x09, 0x33,                     // Usage (Rx)
        0x09, 0x34,                     // Usage (Ry)
        0x09, 0x35,                     // Usage (Rz)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x81, 0x02,                     // Input (variable,absolute)
        0xC0,                           // End Collection
        0x15, 0x00,                     // Logical Minimum (0)
        0x25, 0x07,                     // Logical Maximum (7)
        0x35, 0x00,                     // Physical Minimum (0)
        0x46, 0x3B, 0x01,               // Physical Maximum (315)
        0x75, 0x04,                     // Report Size (4)
        0x95, 0x04,                     // Report Count (4)
        0x65, 0x14,                     // Unit (20)
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x39,                     // Usage (Hat switch)
        0x09, 0x39,                     // Usage (Hat switch)
        0x09, 0x39,                     // Usage (Hat switch)
        0x09, 0x39,                     // Usage (Hat switch)
        0x81, 0x42,                     // Input (variable,absolute,null_state)
        0xC0                            // End Collection
};
#endif // JOYSTICK_SIZE
#endif // JOYSTICK_INTERFACE

#ifdef FFBJOYSTICK_INTERFACE
#if FFBJOYSTICK_SIZE_IN == 13
static uint8_t ffbjoystick_report_desc[] = {
        0x05, 0x01,                     // Usage Page (Generic Desktop) - Global Item
        0x09, 0x04,                     // Usage (Joystick) - Local Item
        0xA1, 0x01,                     // Collection (Application)
		0xA1, 0x00,   // COLLECTION (Physical)
		0x85, 0x01, 			        //   REPORT_ID (1)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x25, 0x01,                     //   Logical Maximum (1)
        0x75, 0x01,                     //   Report Size (1)
        0x95, 0x20,                     //   Report Count (32)
        0x05, 0x09,                     //   Usage Page (Button)
        0x19, 0x01,                     //   Usage Minimum (Button #1)
        0x29, 0x20,                     //   Usage Maximum (Button #32)
        0x81, 0x02,                     //   Input (variable,absolute)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x25, 0x07,                     //   Logical Maximum (7)
        0x35, 0x00,                     //   Physical Minimum (0)
        0x46, 0x3B, 0x01,               //   Physical Maximum (315)
        0x75, 0x04,                     //   Report Size (4)
        0x95, 0x01,                     //   Report Count (1)
        0x65, 0x14,                     //   Unit (20)
        0x05, 0x01,                     //   Usage Page (Generic Desktop)
        0x09, 0x39,                     //   Usage (Hat switch)
        0x81, 0x42,                     //   Input (variable,absolute,null_state)
        0x05, 0x01,                     //   Usage Page (Generic Desktop)
        0x09, 0x01,                     //   Usage (Pointer)
        0xA1, 0x00,                     //   Collection ()
        0x15, 0x00,                     //     Logical Minimum (0)
        0x26, 0xFF, 0x03,               //     Logical Maximum (1023)
        0x75, 0x0A,                     //     Report Size (10)
        0x95, 0x04,                     //     Report Count (4)
        0x09, 0x30,                     //     Usage (X)
        0x09, 0x31,                     //     Usage (Y)
        0x09, 0x32,                     //     Usage (Z)
        0x09, 0x35,                     //     Usage (Rz)
        0x81, 0x02,                     //     Input (variable,absolute)
        0xC0,                           //   End Collection
        0x15, 0x00,                     //   Logical Minimum (0)
        0x26, 0xFF, 0x03,               //   Logical Maximum (1023)
        0x75, 0x0A,                     //   Report Size (10)
        0x95, 0x02,                     //   Report Count (2)
        0x09, 0x36,                     //   Usage (Slider)
        0x09, 0x36,                     //   Usage (Slider)
        0x81, 0x02,                     //   Input (variable,absolute)
		
        0xC0,                           // End Collection*/
     

	 
		0x05, 0x0F, // USAGE_PAGE (Physical Interface)
		0x09, 0x92, // USAGE (PID State Report)
		0xA1, 0x02, // COLLECTION (Logical)
		0x85, 0x02, //  REPORT_ID (02)
		0x09, 0x9F, //  USAGE (Device Paused)
		0x09, 0xA0, //  USAGE (Actuators Enabled)
		0x09, 0xA4, //  USAGE (Safety Switch)
		0x09, 0xA5, //  USAGE (Actuator Override Switch)
		0x09, 0xA6, //  USAGE (Actuator Power)
		0x15, 0x00, //  LOGICAL_MINIMUM (00)
		0x25, 0x01, //  LOGICAL_MINIMUM (01)
		0x35, 0x00, //  PHYSICAL_MINIMUM (00)
		0x45, 0x01, //  PHYSICAL_MAXIMUM (01)
		0x75, 0x01, //  REPORT_SIZE (01)
		0x95, 0x05, //  REPORT_COUNT (05)
		0x81, 0x02, //  INPUT (Data,Var,Abs)
		0x95, 0x03, //  REPORT_COUNT (03)
		0x81, 0x03, //  INPUT (Constant,Var,Abs)
		0x09, 0x94, //  USAGE (Effect Playing)
		0x15, 0x00, //  LOGICAL_MINIMUM (00)
		0x25, 0x01, //  LOGICAL_MAXIMUM (01)
		0x35, 0x00, //  PHYSICAL_MINIMUM (00)
		0x45, 0x01, //  PHYSICAL_MAXIMUM (01)
		0x75, 0x01, //  REPORT_SIZE (01)
		0x95, 0x01, //  REPORT_COUNT (01)
		0x81, 0x02, //  INPUT (Data,Var,Abs)
		0x09, 0x22, //  USAGE (Effect Block Index)
		0x15, 0x01, //  LOGICAL_MINIMUM (01)
		0x25, 0x28, //  LOGICAL_MAXIMUM (28)
		0x35, 0x01, //  PHYSICAL_MINIMUM (01)
		0x45, 0x28, //  PHYSICAL_MAXIMUM (28)
		0x75, 0x07, //  REPORT_SIZE (07)
		0x95, 0x01, //  REPORT_COUNT (01)
		0x81, 0x02, //  INPUT (Data,Var,Abs)
		0xC0,       // END COLLECTION ()
		  

    0x09, 0x21,       // USAGE (Set Effect Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x01,       // REPORT_ID (01)
    0x09, 0x22,       // USAGE (Effect Block Index)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x28,       // LOGICAL_MAXIMUM (28)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x28,       // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x25,       // USAGE (25)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x09, 0x26,       // USAGE (26)
    0x09, 0x27,       // USAGE (27)
    0x09, 0x30,       // USAGE (30)
    0x09, 0x31,       // USAGE (31)
    0x09, 0x32,       // USAGE (32)
    0x09, 0x33,       // USAGE (33)
    0x09, 0x34,       // USAGE (34)
    0x09, 0x40,       // USAGE (40)
    0x09, 0x41,       // USAGE (41)
    0x09, 0x42,       // USAGE (42)
    0x09, 0x43,       // USAGE (43)
    0x09, 0x28,       // USAGE (28)
    0x25, 0x0C,       // LOGICAL_MAXIMUM (0C)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x0C,       // PHYSICAL_MAXIMUM (0C)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x00,       // OUTPUT (Data)
    0xC0,             // END COLLECTION ()
    0x09, 0x50,       // USAGE (Duration)
    0x09, 0x54,       // USAGE (Trigger Repeat Interval)
    0x09, 0x51,       // USAGE (Sample Period)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x7F, // LOGICAL_MAXIMUM (7F FF)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0xFF, 0x7F, // PHYSICAL_MAXIMUM (7F FF)
    0x66, 0x03, 0x10, // UNIT (Eng Lin:Time)
    0x55, 0xFD,       // UNIT_EXPONENT (-3)
    0x75, 0x10,       // REPORT_SIZE (10)
    0x95, 0x03,       // REPORT_COUNT (03)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x55, 0x00,       // UNIT_EXPONENT (00)
    0x66, 0x00, 0x00, // UNIT (None)
    0x09, 0x52,       // USAGE (Gain)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (00 FF)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0x10, 0x27, // PHYSICAL_MAXIMUM (10000)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x53,       // USAGE (Trigger Button)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x08,       // LOGICAL_MAXIMUM (08)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x08,       // PHYSICAL_MAXIMUM (08)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x55,       // USAGE (Axes Enable)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
    0x09, 0x30,       // USAGE (X)
    0x09, 0x31,       // USAGE (Y)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x25, 0x01,       // LOGICAL_MAXIMUM (01)
    0x75, 0x01,       // REPORT_SIZE (01)
    0x95, 0x02,       // REPORT_COUNT (02)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0xC0,             // END COLLECTION ()
    0x05, 0x0F,       // USAGE_PAGE (Physical Interface)
    0x09, 0x56,       // USAGE (Direction Enable)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x95, 0x05,       // REPORT_COUNT (05)
    0x91, 0x03,       // OUTPUT (Constant,Var,Abs)
    0x09, 0x57,       // USAGE (Direction)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x0B, 0x01, 0x00, 0x0A, 0x00,
    0x0B, 0x02, 0x00, 0x0A, 0x00,
    0x66, 0x14, 0x00,             // UNIT (Eng Rot:Angular Pos)
    0x55, 0xFE,                   // UNIT_EXPONENT (FE)
    0x15, 0x00,                   // LOGICAL_MINIMUM (00)
    0x26, 0xB4, 0x00,             // LOGICAL_MAXIMUM (00 B4)
    0x35, 0x00,                   // PHYSICAL_MINIMUM (00)
    0x47, 0xA0, 0x8C, 0x00, 0x00, // PHYSICAL_MAXIMUM (00 00 8C A0)
    0x66, 0x00, 0x00,             // UNIT (None)
    0x75, 0x08,                   // REPORT_SIZE (08)
    0x95, 0x02,                   // REPORT_COUNT (02)
    0x91, 0x02,                   // OUTPUT (Data,Var,Abs)
    0x55, 0x00,                   // UNIT_EXPONENT (00)
    0x66, 0x00, 0x00,             // UNIT (None)
    0xC0,                         // END COLLECTION ()
    0x05, 0x0F,                   // USAGE_PAGE (Physical Interface)
                                  //		0x09,0xA7,	// USAGE (Start Delay)
    0x66, 0x03, 0x10,             // UNIT (Eng Lin:Time)
    0x55, 0xFD,                   // UNIT_EXPONENT (-3)
    0x15, 0x00,                   // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x7F,             // LOGICAL_MAXIMUM (7F FF)
    0x35, 0x00,                   // PHYSICAL_MINIMUM (00)
    0x46, 0xFF, 0x7F,             // PHYSICAL_MAXIMUM (7F FF)
    0x75, 0x10,                   // REPORT_SIZE (10)
    0x95, 0x01,                   // REPORT_COUNT (01)
                                  //		0x91,0x02,	// OUTPUT (Data,Var,Abs)
    0x66, 0x00, 0x00,             // UNIT (None)
    0x55, 0x00,                   // UNIT_EXPONENT (00)
    0xC0,                         // END COLLECTION ()
	// 0xC0,                           // End Collection*/

    0x05, 0x0F,       // USAGE_PAGE (Physical Interface)
    0x09, 0x5A,       // USAGE (Set Envelope Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x02,       // REPORT_ID (02)
    0x09, 0x22,       // USAGE (Effect Block Index)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x28,       // LOGICAL_MAXIMUM (28)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x28,       // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x5B,       // USAGE (Attack Level)
    0x09, 0x5D,       // USAGE (Fade Level)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (00 FF)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0x10, 0x27, // PHYSICAL_MAXIMUM (10000)
    0x95, 0x02,       // REPORT_COUNT (02)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x5C,       // USAGE (5C)
    0x09, 0x5E,       // USAGE (5E)
    0x66, 0x03, 0x10, // UNIT (Eng Lin:Time)
    0x55, 0xFD,       // UNIT_EXPONENT (-3)
    0x26, 0xFF, 0x7F, // LOGICAL_MAXIMUM (7F FF)
    0x46, 0xFF, 0x7F, // PHYSICAL_MAXIMUM (7F FF)
    0x75, 0x10,       // REPORT_SIZE (10)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x45, 0x00,       // PHYSICAL_MAXIMUM (00)
    0x66, 0x00, 0x00, // UNIT (None)
    0x55, 0x00,       // UNIT_EXPONENT (00)
    0xC0,             // END COLLECTION ()

    		// SetConditionReport
		0x09,0x5F,	// USAGE (Set Condition Report)
		0xA1,0x02,	// COLLECTION (Logical)
			0x85,0x03,	// REPORT_ID (03)
			0x09,0x22,	// USAGE (Effect Block Index)
			0x15,0x01,	// LOGICAL_MINIMUM (01)
			0x25,0x28,	// LOGICAL_MAXIMUM (28)
			0x35,0x01,	// PHYSICAL_MINIMUM (01)
			0x45,0x28,	// PHYSICAL_MAXIMUM (28)
			0x75,0x08,	// REPORT_SIZE (08)
			0x95,0x01,	// REPORT_COUNT (01)
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0x09,0x23,	// USAGE (Parameter Block Offset)
			0x15,0x00,	// LOGICAL_MINIMUM (00)
			0x25,0x01,	// LOGICAL_MAXIMUM (01)
			0x35,0x00,	// PHYSICAL_MINIMUM (00)
			0x45,0x01,	// PHYSICAL_MAXIMUM (01)
			0x75,0x04,	// REPORT_SIZE (04)
			0x95,0x01,	// REPORT_COUNT (01)
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0x09,0x58,	// USAGE (Type Specific Block Offset)
			0xA1,0x02,	// COLLECTION (Logical)
				0x0B,0x01,0x00,0x0A,0x00,	// USAGE (Instance 1)
				0x0B,0x02,0x00,0x0A,0x00,	// USAGE (Instance 2)
				0x75,0x02,	// REPORT_SIZE (02)
				0x95,0x02,	// REPORT_COUNT (02)
				0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0xC0,	// END COLLECTION ()
			0x15,0x80,	// LOGICAL_MINIMUM (80)
			0x25,0x7F,	// LOGICAL_MAXIMUM (7F)
			0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
			0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
			0x09,0x60,	// USAGE (CP Offset)
			0x75,0x08,	// REPORT_SIZE (08)
			0x95,0x01,	// REPORT_COUNT (01)
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0x36,0xF0,0xD8,	// PHYSICAL_MINIMUM (-10000)
			0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
			0x09,0x61,	// USAGE (Positive Coefficient)
			0x09,0x62,	// USAGE (Negative Coefficient)
			0x95,0x02,	// REPORT_COUNT (02)	
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0x15,0x00,	// LOGICAL_MINIMUM (00)
			0x26,0xFF,0x00,	// LOGICAL_MAXIMUM (00 FF)
			0x35,0x00,	// PHYSICAL_MINIMUM (00)
			0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
			0x09,0x63,	// USAGE (Positive Saturation)
			0x09,0x64,	// USAGE (Negative Saturation)
			0x75,0x08,	// REPORT_SIZE (08)
			0x95,0x02,	// REPORT_COUNT (02)
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
			0x09,0x65,	// USAGE (Dead Band )
			0x46,0x10,0x27,	// PHYSICAL_MAXIMUM (10000)
			0x95,0x01,	// REPORT_COUNT (01)
			0x91,0x02,	// OUTPUT (Data,Var,Abs)
		0xC0,	// END COLLECTION ()

    0x09, 0x6E,                   // USAGE (Set Periodic Report)
    0xA1, 0x02,                   // COLLECTION (Logical)
    0x85, 0x04,                   // REPORT_ID (04)
    0x09, 0x22,                   // USAGE (Effect Block Index)
    0x15, 0x01,                   // LOGICAL_MINIMUM (01)
    0x25, 0x28,                   // LOGICAL_MAXIMUM (28)
    0x35, 0x01,                   // PHYSICAL_MINIMUM (01)
    0x45, 0x28,                   // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,                   // REPORT_SIZE (08)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0x91, 0x02,                   // OUTPUT (Data,Var,Abs)
    0x09, 0x70,                   // USAGE (Magnitude)
    0x15, 0x00,                   // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x00,             // LOGICAL_MAXIMUM (00 FF)
    0x35, 0x00,                   // PHYSICAL_MINIMUM (00)
    0x46, 0x10, 0x27,             // PHYSICAL_MAXIMUM (10000)
    0x75, 0x08,                   // REPORT_SIZE (08)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0x91, 0x02,                   // OUTPUT (Data,Var,Abs)
    0x09, 0x6F,                   // USAGE (Offset)
    0x15, 0x80,                   // LOGICAL_MINIMUM (80)
    0x25, 0x7F,                   // LOGICAL_MAXIMUM (7F)
    0x36, 0xF0, 0xD8,             // PHYSICAL_MINIMUM (-10000)
    0x46, 0x10, 0x27,             // PHYSICAL_MAXIMUM (10000)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0x91, 0x02,                   // OUTPUT (Data,Var,Abs)
    0x09, 0x71,                   // USAGE (Phase)
    0x66, 0x14, 0x00,             // UNIT (Eng Rot:Angular Pos)
    0x55, 0xFE,                   // UNIT_EXPONENT (FE)
    0x15, 0x00,                   // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x00,             // LOGICAL_MAXIMUM (00 FF)
    0x35, 0x00,                   // PHYSICAL_MINIMUM (00)
    0x47, 0xA0, 0x8C, 0x00, 0x00, // PHYSICAL_MAXIMUM (00 00 8C A0)
    0x91, 0x02,                   // OUTPUT (Data,Var,Abs)
    0x09, 0x72,                   // USAGE (Period)
    0x26, 0xFF, 0x7F,             // LOGICAL_MAXIMUM (7F FF)
    0x46, 0xFF, 0x7F,             // PHYSICAL_MAXIMUM (7F FF)
    0x66, 0x03, 0x10,             // UNIT (Eng Lin:Time)
    0x55, 0xFD,                   // UNIT_EXPONENT (-3)
    0x75, 0x10,                   // REPORT_SIZE (10)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0x91, 0x02,                   // OUTPUT (Data,Var,Abs)
    0x66, 0x00, 0x00,             // UNIT (None)
    0x55, 0x00,                   // UNIT_EXPONENT (00)
    0xC0,                         // END COLLECTION ()

    0x09, 0x73,       // USAGE (Set Constant Force Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x05,       // REPORT_ID (05)
    0x09, 0x22,       // USAGE (Effect Block Index)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x28,       // LOGICAL_MAXIMUM (28)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x28,       // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x70,       // USAGE (Magnitude)
    0x16, 0x01, 0xFF, // LOGICAL_MINIMUM (-255)
    0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (255)
    0x36, 0xF0, 0xD8, // PHYSICAL_MINIMUM (-10000)
    0x46, 0x10, 0x27, // PHYSICAL_MAXIMUM (10000)
    0x75, 0x10,       // REPORT_SIZE (10)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0xC0,             // END COLLECTION ()

    0x09, 0x74,       // USAGE (Set Ramp Force Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x06,       // REPORT_ID (06)
    0x09, 0x22,       // USAGE (Effect Block Index)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x28,       // LOGICAL_MAXIMUM (28)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x28,       // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x75,       // USAGE (Ramp Start)
    0x09, 0x76,       // USAGE (Ramp End)
    0x15, 0x80,       // LOGICAL_MINIMUM (-128)
    0x25, 0x7F,       // LOGICAL_MAXIMUM (127)
    0x36, 0xF0, 0xD8, // PHYSICAL_MINIMUM (-10000)
    0x46, 0x10, 0x27, // PHYSICAL_MAXIMUM (10000)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x02,       // REPORT_COUNT (02)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0xC0,             // END COLLECTION ()

    0x09, 0x68,       // USAGE (Custom Force Data Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x07,       // REPORT_ID (07)
    0x09, 0x22,       // USAGE (Effect Block Index)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x28,       // LOGICAL_MAXIMUM (28)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x28,       // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x6C,       // USAGE (Custom Force Data Offset)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0x10, 0x27, // LOGICAL_MAXIMUM (10000)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0x10, 0x27, // PHYSICAL_MAXIMUM (10000)
    0x75, 0x10,       // REPORT_SIZE (10)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x69,       // USAGE (Custom Force Data)
    0x15, 0x81,       // LOGICAL_MINIMUM (-127)
    0x25, 0x7F,       // LOGICAL_MAXIMUM (127)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0xFF, 0x00, // PHYSICAL_MAXIMUM (255)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x0C,       // REPORT_COUNT (0C)
    0x92, 0x02, 0x01, // OUTPUT ( Data,Var,Abs,Buf)
    0xC0,             // END COLLECTION ()

    0x09, 0x66,       // USAGE (Download Force Sample)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x08,       // REPORT_ID (08)
    0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
    0x09, 0x30,       // USAGE (X)
    0x09, 0x31,       // USAGE (Y)
    0x15, 0x81,       // LOGICAL_MINIMUM (-127)
    0x25, 0x7F,       // LOGICAL_MAXIMUM (127)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0xFF, 0x00, // PHYSICAL_MAXIMUM (255)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x02,       // REPORT_COUNT (02)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0xC0,             // END COLLECTION ()

    0x05, 0x0F,       // USAGE_PAGE (Physical Interface)
    0x09, 0x77,       // USAGE (Effect Operation Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x0A,       // REPORT_ID (0A)
    0x09, 0x22,       // USAGE (Effect Block Index)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x28,       // LOGICAL_MAXIMUM (28)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x28,       // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x78,       // USAGE (78)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x09, 0x79,       // USAGE (Op Effect Start)
    0x09, 0x7A,       // USAGE (Op Effect Start Solo)
    0x09, 0x7B,       // USAGE (Op Effect Stop)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x03,       // LOGICAL_MAXIMUM (03)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x00,       // OUTPUT (Data,Ary,Abs)
    0xC0,             // END COLLECTION ()
    0x09, 0x7C,       // USAGE (Loop Count)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (00 FF)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0xFF, 0x00, // PHYSICAL_MAXIMUM (00 FF)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0xC0,             // END COLLECTION ()

    0x09, 0x90, // USAGE (PID Block Free Report)
    0xA1, 0x02, // COLLECTION (Logical)
    0x85, 0x0B, // REPORT_ID (0B)
    0x09, 0x22, // USAGE (Effect Block Index)
    0x25, 0x28, // LOGICAL_MAXIMUM (28)
    0x15, 0x01, // LOGICAL_MINIMUM (01)
    0x35, 0x01, // PHYSICAL_MINIMUM (01)
    0x45, 0x28, // PHYSICAL_MAXIMUM (28)
    0x75, 0x08, // REPORT_SIZE (08)
    0x95, 0x01, // REPORT_COUNT (01)
    0x91, 0x02, // OUTPUT (Data,Var,Abs)
    0xC0,       // END COLLECTION ()

    0x09, 0x96, // USAGE (PID Device Control)
    0xA1, 0x02, // COLLECTION (Logical)
    0x85, 0x0C, // REPORT_ID (0C)
    0x09, 0x97, // USAGE (DC Enable Actuators)
    0x09, 0x98, // USAGE (DC Disable Actuators)
    0x09, 0x99, // USAGE (DC Stop All Effects)
    0x09, 0x9A, // USAGE (DC Device Reset)
    0x09, 0x9B, // USAGE (DC Device Pause)
    0x09, 0x9C, // USAGE (DC Device Continue)
    0x15, 0x01, // LOGICAL_MINIMUM (01)
    0x25, 0x06, // LOGICAL_MAXIMUM (06)
    0x75, 0x08, // REPORT_SIZE (08)
    0x95, 0x01, // REPORT_COUNT (01)
    0x91, 0x00, // OUTPUT (Data)
    0xC0,       // END COLLECTION ()

    0x09, 0x7D,       // USAGE (Device Gain Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x0D,       // REPORT_ID (0D)
    0x09, 0x7E,       // USAGE (Device Gain)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (00 FF)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0x10, 0x27, // PHYSICAL_MAXIMUM (10000)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0xC0,             // END COLLECTION ()

    0x09, 0x6B,       // USAGE (Set Custom Force Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x0E,       // REPORT_ID (0E)
    0x09, 0x22,       // USAGE (Effect Block Index)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x25, 0x28,       // LOGICAL_MAXIMUM (28)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x28,       // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x6D,       // USAGE (Sample Count)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x00, // LOGICAL_MAXIMUM (00 FF)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0xFF, 0x00, // PHYSICAL_MAXIMUM (00 FF)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x09, 0x51,       // USAGE (Sample Period)
    0x66, 0x03, 0x10, // UNIT (Eng Lin:Time)
    0x55, 0xFD,       // UNIT_EXPONENT (-3)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x7F, // LOGICAL_MAXIMUM (32767)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0xFF, 0x7F, // PHYSICAL_MAXIMUM (32767)
    0x75, 0x10,       // REPORT_SIZE (10)
    0x95, 0x01,       // REPORT_COUNT (01)
    0x91, 0x02,       // OUTPUT (Data,Var,Abs)
    0x55, 0x00,       // UNIT_EXPONENT (00)
    0x66, 0x00, 0x00, // UNIT (None)
    0xC0,             // END COLLECTION ()

    0x09, 0xAB,       // USAGE (Create New Effect Report)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x85, 0x05,       // REPORT_ID (05)
    0x09, 0x25,       // USAGE (Effect Type)
    0xA1, 0x02,       // COLLECTION (Logical)
    0x09, 0x26,       // USAGE (26)
    0x09, 0x27,       // USAGE (27)
    0x09, 0x30,       // USAGE (30)
    0x09, 0x31,       // USAGE (31)
    0x09, 0x32,       // USAGE (32)
    0x09, 0x33,       // USAGE (33)
    0x09, 0x34,       // USAGE (34)
    0x09, 0x40,       // USAGE (40)
    0x09, 0x41,       // USAGE (41)
    0x09, 0x42,       // USAGE (42)
    0x09, 0x43,       // USAGE (43)
    0x09, 0x28,       // USAGE (28)
    0x25, 0x0C,       // LOGICAL_MAXIMUM (0C)
    0x15, 0x01,       // LOGICAL_MINIMUM (01)
    0x35, 0x01,       // PHYSICAL_MINIMUM (01)
    0x45, 0x0C,       // PHYSICAL_MAXIMUM (0C)
    0x75, 0x08,       // REPORT_SIZE (08)
    0x95, 0x01,       // REPORT_COUNT (01)
    0xB1, 0x00,       // FEATURE (Data)
    0xC0,             // END COLLECTION ()
    0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
    0x09, 0x3B,       // USAGE (Byte Count)
    0x15, 0x00,       // LOGICAL_MINIMUM (00)
    0x26, 0xFF, 0x01, // LOGICAL_MAXIMUM (511)
    0x35, 0x00,       // PHYSICAL_MINIMUM (00)
    0x46, 0xFF, 0x01, // PHYSICAL_MAXIMUM (511)
    0x75, 0x0A,       // REPORT_SIZE (0A)
    0x95, 0x01,       // REPORT_COUNT (01)
    0xB1, 0x02,       // FEATURE (Data,Var,Abs)
    0x75, 0x06,       // REPORT_SIZE (06)
    0xB1, 0x01,       // FEATURE (Constant,Ary,Abs)
    0xC0,             // END COLLECTION ()

    0x05, 0x0F,                   // USAGE_PAGE (Physical Interface)
    0x09, 0x89,                   // USAGE (PID Block Load Report)
    0xA1, 0x02,                   // COLLECTION (Logical)
    0x85, 0x06,                   // REPORT_ID (06)
    0x09, 0x22,                   // USAGE (Effect Block Index)
    0x25, 0x28,                   // LOGICAL_MAXIMUM (28)
    0x15, 0x01,                   // LOGICAL_MINIMUM (01)
    0x35, 0x01,                   // PHYSICAL_MINIMUM (01)
    0x45, 0x28,                   // PHYSICAL_MAXIMUM (28)
    0x75, 0x08,                   // REPORT_SIZE (08)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0xB1, 0x02,                   // FEATURE (Data,Var,Abs)
    0x09, 0x8B,                   // USAGE (Block Load Status)
    0xA1, 0x02,                   // COLLECTION (Logical)
    0x09, 0x8C,                   // USAGE (Block Load Success)
    0x09, 0x8D,                   // USAGE (Block Load Full)
    0x09, 0x8E,                   // USAGE (Block Load Error)
    0x25, 0x03,                   // LOGICAL_MAXIMUM (03)
    0x15, 0x01,                   // LOGICAL_MINIMUM (01)
    0x35, 0x01,                   // PHYSICAL_MINIMUM (01)
    0x45, 0x03,                   // PHYSICAL_MAXIMUM (03)
    0x75, 0x08,                   // REPORT_SIZE (08)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0xB1, 0x00,                   // FEATURE (Data)
    0xC0,                         // END COLLECTION ()
    0x09, 0xAC,                   // USAGE (RAM Pool Available)
    0x15, 0x00,                   // LOGICAL_MINIMUM (00)
    0x27, 0xFF, 0xFF, 0x00, 0x00, // LOGICAL_MAXIMUM (00 00 FF FF)
    0x35, 0x00,                   // PHYSICAL_MINIMUM (00)
    0x47, 0xFF, 0xFF, 0x00, 0x00, // PHYSICAL_MAXIMUM (00 00 FF FF)
    0x75, 0x10,                   // REPORT_SIZE (10)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0xB1, 0x00,                   // FEATURE (Data)
    0xC0,                         // END COLLECTION ()

    0x09, 0x7F,                   // USAGE (PID Pool Report)
    0xA1, 0x02,                   // COLLECTION (Logical)
    0x85, 0x07,                   // REPORT_ID (07)
    0x09, 0x80,                   // USAGE (RAM Pool Size)
    0x75, 0x10,                   // REPORT_SIZE (10)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0x15, 0x00,                   // LOGICAL_MINIMUM (00)
    0x35, 0x00,                   // PHYSICAL_MINIMUM (00)
    0x27, 0xFF, 0xFF, 0x00, 0x00, // LOGICAL_MAXIMUM (00 00 FF FF)
    0x47, 0xFF, 0xFF, 0x00, 0x00, // PHYSICAL_MAXIMUM (00 00 FF FF)
    0xB1, 0x02,                   // FEATURE (Data,Var,Abs)
    0x09, 0x83,                   // USAGE (Simultaneous Effects Max)
    0x26, 0xFF, 0x00,             // LOGICAL_MAXIMUM (00 FF)
    0x46, 0xFF, 0x00,             // PHYSICAL_MAXIMUM (00 FF)
    0x75, 0x08,                   // REPORT_SIZE (08)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0xB1, 0x02,                   // FEATURE (Data,Var,Abs)
    0x09, 0xA9,                   // USAGE (Device Managed Pool)
    0x09, 0xAA,                   // USAGE (Shared Parameter Blocks)
    0x75, 0x01,                   // REPORT_SIZE (01)
    0x95, 0x02,                   // REPORT_COUNT (02)
    0x15, 0x00,                   // LOGICAL_MINIMUM (00)
    0x25, 0x01,                   // LOGICAL_MAXIMUM (01)
    0x35, 0x00,                   // PHYSICAL_MINIMUM (00)
    0x45, 0x01,                   // PHYSICAL_MAXIMUM (01)
    0xB1, 0x02,                   // FEATURE (Data,Var,Abs)
    0x75, 0x06,                   // REPORT_SIZE (06)
    0x95, 0x01,                   // REPORT_COUNT (01)
    0xB1, 0x03,                   // FEATURE ( Cnst,Var,Abs)
    0xC0,                         // END COLLECTION ()
    0xC0                        // END COLLECTION ()	*/	
};
#elif FFBJOYSTICK_SIZE_IN == 64
// extreme joystick  (to use this, edit JOYSTICK_SIZE to 64 in usb_desc.h)
//  128 buttons   16
//    6 axes      12
//   17 sliders   34
//    4 pov        2
static uint8_t ffbjoystick_report_desc[] = {
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x04,                     // Usage (Joystick)
        0xA1, 0x01,                     // Collection (Application)
        0x15, 0x00,                     // Logical Minimum (0)
        0x25, 0x01,                     // Logical Maximum (1)
        0x75, 0x01,                     // Report Size (1)
        0x95, 0x80,                     // Report Count (128)
        0x05, 0x09,                     // Usage Page (Button)
        0x19, 0x01,                     // Usage Minimum (Button #1)
        0x29, 0x80,                     // Usage Maximum (Button #128)
        0x81, 0x02,                     // Input (variable,absolute)
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x01,                     // Usage (Pointer)
        0xA1, 0x00,                     // Collection ()
        0x15, 0x00,                     // Logical Minimum (0)
        0x27, 0xFF, 0xFF, 0, 0,         // Logical Maximum (65535)
        0x75, 0x10,                     // Report Size (16)
        0x95, 23,                       // Report Count (23)
        0x09, 0x30,                     // Usage (X)
        0x09, 0x31,                     // Usage (Y)
        0x09, 0x32,                     // Usage (Z)
        0x09, 0x33,                     // Usage (Rx)
        0x09, 0x34,                     // Usage (Ry)
        0x09, 0x35,                     // Usage (Rz)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x09, 0x36,                     // Usage (Slider)
        0x81, 0x02,                     // Input (variable,absolute)
        0xC0,                           // End Collection
        0x15, 0x00,                     // Logical Minimum (0)
        0x25, 0x07,                     // Logical Maximum (7)
        0x35, 0x00,                     // Physical Minimum (0)
        0x46, 0x3B, 0x01,               // Physical Maximum (315)
        0x75, 0x04,                     // Report Size (4)
        0x95, 0x04,                     // Report Count (4)
        0x65, 0x14,                     // Unit (20)
        0x05, 0x01,                     // Usage Page (Generic Desktop)
        0x09, 0x39,                     // Usage (Hat switch)
        0x09, 0x39,                     // Usage (Hat switch)
        0x09, 0x39,                     // Usage (Hat switch)
        0x09, 0x39,                     // Usage (Hat switch)
        0x81, 0x42,                     // Input (variable,absolute,null_state)
        0xC0                            // End Collection
};
#endif // FFBJOYSTICK_SIZE
#endif // FFBJOYSTICK_INTERFACE

#ifdef MULTITOUCH_INTERFACE
// https://forum.pjrc.com/threads/32331-USB-HID-Touchscreen-support-needed
// https://msdn.microsoft.com/en-us/library/windows/hardware/jj151563%28v=vs.85%29.aspx
// https://msdn.microsoft.com/en-us/library/windows/hardware/jj151565%28v=vs.85%29.aspx
// https://msdn.microsoft.com/en-us/library/windows/hardware/ff553734%28v=vs.85%29.aspx
// https://msdn.microsoft.com/en-us/library/windows/hardware/jj151564%28v=vs.85%29.aspx
// download.microsoft.com/download/a/d/f/adf1347d-08dc-41a4-9084-623b1194d4b2/digitizerdrvs_touch.docx
static uint8_t multitouch_report_desc[] = {
        0x05, 0x0D,                     // Usage Page (Digitizer)
        0x09, 0x04,                     // Usage (Touch Screen)
        0xa1, 0x01,                     // Collection (Application)
        0x09, 0x22,                     //   Usage (Finger)
        0xA1, 0x02,                     //   Collection (Logical)
        0x09, 0x42,                     //     Usage (Tip Switch)
        0x15, 0x00,                     //     Logical Minimum (0)
        0x25, 0x01,                     //     Logical Maximum (1)
        0x75, 0x01,                     //     Report Size (1)
        0x95, 0x01,                     //     Report Count (1)
        0x81, 0x02,                     //     Input (variable,absolute)
        0x09, 0x51,                     //     Usage (Contact Identifier)
        0x25, 0x7F,                     //     Logical Maximum (127)
        0x75, 0x07,                     //     Report Size (7)
        0x95, 0x01,                     //     Report Count (1)
        0x81, 0x02,                     //     Input (variable,absolute)
        0x09, 0x30,                     //     Usage (Pressure)
        0x26, 0xFF, 0x00,               //     Logical Maximum (255)
        0x75, 0x08,                     //     Report Size (8)
        0x95, 0x01,                     //     Report Count (1)
        0x81, 0x02,                     //     Input (variable,absolute)
        0x05, 0x01,                     //     Usage Page (Generic Desktop)
        0x09, 0x30,                     //     Usage (X)
        0x09, 0x31,                     //     Usage (Y)
        0x26, 0xFF, 0x7F,               //     Logical Maximum (32767)
        0x65, 0x00,                     //     Unit (None)  <-- probably needs real units?
        0x75, 0x10,                     //     Report Size (16)
        0x95, 0x02,                     //     Report Count (2)
        0x81, 0x02,                     //     Input (variable,absolute)
        0xC0,                           //   End Collection
        0x05, 0x0D,                     //   Usage Page (Digitizer)
        0x27, 0xFF, 0xFF, 0, 0,         //   Logical Maximum (65535)
        0x75, 0x10,                     //   Report Size (16)
        0x95, 0x01,                     //   Report Count (1)
        0x09, 0x56,                     //   Usage (Scan Time)
        0x81, 0x02,                     //   Input (variable,absolute)
        0x05, 0x0D,                     //   Usage Page (Digitizers)
        0x09, 0x55,                     //   Usage (Contact Count Maximum)
        0x25, MULTITOUCH_FINGERS,       //   Logical Maximum (10)
        0x75, 0x08,                     //   Report Size (8)
        0x95, 0x01,                     //   Report Count (1)
        0xB1, 0x02,                     //   Feature (variable,absolute)
        0xC0                            // End Collection
};
#endif

#ifdef SEREMU_INTERFACE
static uint8_t seremu_report_desc[] = {
        0x06, 0xC9, 0xFF,               // Usage Page 0xFFC9 (vendor defined)
        0x09, 0x04,                     // Usage 0x04
        0xA1, 0x5C,                     // Collection 0x5C
        0x75, 0x08,                     //   report size = 8 bits (global)
        0x15, 0x00,                     //   logical minimum = 0 (global)
        0x26, 0xFF, 0x00,               //   logical maximum = 255 (global)
        0x95, SEREMU_TX_SIZE,           //   report count (global)
        0x09, 0x75,                     //   usage (local)
        0x81, 0x02,                     //   Input
        0x95, SEREMU_RX_SIZE,           //   report count (global)
        0x09, 0x76,                     //   usage (local)
        0x91, 0x02,                     //   Output
        0x95, 0x04,                     //   report count (global)
        0x09, 0x76,                     //   usage (local)
        0xB1, 0x02,                     //   Feature
        0xC0                            // end collection
};
#endif

#ifdef RAWHID_INTERFACE
static uint8_t rawhid_report_desc[] = {
        0x06, LSB(RAWHID_USAGE_PAGE), MSB(RAWHID_USAGE_PAGE),
        0x0A, LSB(RAWHID_USAGE), MSB(RAWHID_USAGE),
        0xA1, 0x01,                     // Collection 0x01
        0x75, 0x08,                     //   report size = 8 bits
        0x15, 0x00,                     //   logical minimum = 0
        0x26, 0xFF, 0x00,               //   logical maximum = 255
        0x95, RAWHID_TX_SIZE,           //   report count
        0x09, 0x01,                     //   usage
        0x81, 0x02,                     //   Input (array)
        0x95, RAWHID_RX_SIZE,           //   report count
        0x09, 0x02,                     //   usage
        0x91, 0x02,                     //   Output (array)
        0xC0                            // end collection
};
#endif

#ifdef FLIGHTSIM_INTERFACE
static uint8_t flightsim_report_desc[] = {
        0x06, 0x1C, 0xFF,               // Usage page = 0xFF1C
        0x0A, 0x39, 0xA7,               // Usage = 0xA739
        0xA1, 0x01,                     // Collection 0x01
        0x75, 0x08,                     //   report size = 8 bits
        0x15, 0x00,                     //   logical minimum = 0
        0x26, 0xFF, 0x00,               //   logical maximum = 255
        0x95, FLIGHTSIM_TX_SIZE,        //   report count
        0x09, 0x01,                     //   usage
        0x81, 0x02,                     //   Input (array)
        0x95, FLIGHTSIM_RX_SIZE,        //   report count
        0x09, 0x02,                     //   usage
        0x91, 0x02,                     //   Output (array)
        0xC0                            // end collection
};
#endif


// **************************************************************
//   USB Descriptor Sizes
// **************************************************************

// pre-compute the size and position of everything in the config descriptor
//
#define CONFIG_HEADER_DESCRIPTOR_SIZE	9

#define CDC_IAD_DESCRIPTOR_POS		CONFIG_HEADER_DESCRIPTOR_SIZE
#ifdef  CDC_IAD_DESCRIPTOR
#define CDC_IAD_DESCRIPTOR_SIZE		8
#else
#define CDC_IAD_DESCRIPTOR_SIZE		0
#endif

#define CDC_DATA_INTERFACE_DESC_POS	CDC_IAD_DESCRIPTOR_POS+CDC_IAD_DESCRIPTOR_SIZE
#ifdef  CDC_DATA_INTERFACE
#define CDC_DATA_INTERFACE_DESC_SIZE	9+5+5+4+5+7+9+7+7
#else
#define CDC_DATA_INTERFACE_DESC_SIZE	0
#endif

#define MIDI_INTERFACE_DESC_POS		CDC_DATA_INTERFACE_DESC_POS+CDC_DATA_INTERFACE_DESC_SIZE
#ifdef  MIDI_INTERFACE
  #if !defined(MIDI_NUM_CABLES) || MIDI_NUM_CABLES < 1 || MIDI_NUM_CABLES > 16
  #error "MIDI_NUM_CABLES must be defined between 1 to 16"
  #endif
#define MIDI_INTERFACE_DESC_SIZE	9+7+((6+6+9+9)*MIDI_NUM_CABLES)+(9+4+MIDI_NUM_CABLES)*2
#else
#define MIDI_INTERFACE_DESC_SIZE	0
#endif

#define KEYBOARD_INTERFACE_DESC_POS	MIDI_INTERFACE_DESC_POS+MIDI_INTERFACE_DESC_SIZE
#ifdef  KEYBOARD_INTERFACE
#define KEYBOARD_INTERFACE_DESC_SIZE	9+9+7
#define KEYBOARD_HID_DESC_OFFSET	KEYBOARD_INTERFACE_DESC_POS+9
#else
#define KEYBOARD_INTERFACE_DESC_SIZE	0
#endif

#define MOUSE_INTERFACE_DESC_POS	KEYBOARD_INTERFACE_DESC_POS+KEYBOARD_INTERFACE_DESC_SIZE
#ifdef  MOUSE_INTERFACE
#define MOUSE_INTERFACE_DESC_SIZE	9+9+7
#define MOUSE_HID_DESC_OFFSET		MOUSE_INTERFACE_DESC_POS+9
#else
#define MOUSE_INTERFACE_DESC_SIZE	0
#endif

#define RAWHID_INTERFACE_DESC_POS	MOUSE_INTERFACE_DESC_POS+MOUSE_INTERFACE_DESC_SIZE
#ifdef  RAWHID_INTERFACE
#define RAWHID_INTERFACE_DESC_SIZE	9+9+7+7
#define RAWHID_HID_DESC_OFFSET		RAWHID_INTERFACE_DESC_POS+9
#else
#define RAWHID_INTERFACE_DESC_SIZE	0
#endif

#define FLIGHTSIM_INTERFACE_DESC_POS	RAWHID_INTERFACE_DESC_POS+RAWHID_INTERFACE_DESC_SIZE
#ifdef  FLIGHTSIM_INTERFACE
#define FLIGHTSIM_INTERFACE_DESC_SIZE	9+9+7+7
#define FLIGHTSIM_HID_DESC_OFFSET	FLIGHTSIM_INTERFACE_DESC_POS+9
#else
#define FLIGHTSIM_INTERFACE_DESC_SIZE	0
#endif

#define SEREMU_INTERFACE_DESC_POS	FLIGHTSIM_INTERFACE_DESC_POS+FLIGHTSIM_INTERFACE_DESC_SIZE
#ifdef  SEREMU_INTERFACE
#define SEREMU_INTERFACE_DESC_SIZE	9+9+7+7
#define SEREMU_HID_DESC_OFFSET		SEREMU_INTERFACE_DESC_POS+9
#else
#define SEREMU_INTERFACE_DESC_SIZE	0
#endif

#define JOYSTICK_INTERFACE_DESC_POS	SEREMU_INTERFACE_DESC_POS+SEREMU_INTERFACE_DESC_SIZE
#ifdef  JOYSTICK_INTERFACE
#define JOYSTICK_INTERFACE_DESC_SIZE	9+9+7
#define JOYSTICK_HID_DESC_OFFSET	JOYSTICK_INTERFACE_DESC_POS+9
#else
#define JOYSTICK_INTERFACE_DESC_SIZE	0
#endif

#define FFBJOYSTICK_INTERFACE_DESC_POS	JOYSTICK_INTERFACE_DESC_POS + JOYSTICK_INTERFACE_DESC_SIZE
#ifdef  FFBJOYSTICK_INTERFACE
#define FFBJOYSTICK_INTERFACE_DESC_SIZE	9+9+7+7
#define FFBJOYSTICK_HID_DESC_OFFSET	FFBJOYSTICK_INTERFACE_DESC_POS+9
#else
#define FFBJOYSTICK_INTERFACE_DESC_SIZE	0
#endif

#define MTP_INTERFACE_DESC_POS		FFBJOYSTICK_INTERFACE_DESC_POS+FFBJOYSTICK_INTERFACE_DESC_SIZE
#ifdef  MTP_INTERFACE
#define MTP_INTERFACE_DESC_SIZE		9+7+7+7
#else
#define MTP_INTERFACE_DESC_SIZE	0
#endif

#define KEYMEDIA_INTERFACE_DESC_POS	MTP_INTERFACE_DESC_POS+MTP_INTERFACE_DESC_SIZE
#ifdef  KEYMEDIA_INTERFACE
#define KEYMEDIA_INTERFACE_DESC_SIZE	9+9+7
#define KEYMEDIA_HID_DESC_OFFSET	KEYMEDIA_INTERFACE_DESC_POS+9
#else
#define KEYMEDIA_INTERFACE_DESC_SIZE	0
#endif

#define AUDIO_INTERFACE_DESC_POS	KEYMEDIA_INTERFACE_DESC_POS+KEYMEDIA_INTERFACE_DESC_SIZE
#ifdef  AUDIO_INTERFACE
#define AUDIO_INTERFACE_DESC_SIZE	8 + 9+10+12+9+12+10+9 + 9+9+7+11+9+7 + 9+9+7+11+9+7+9
#else
#define AUDIO_INTERFACE_DESC_SIZE	0
#endif

#define MULTITOUCH_INTERFACE_DESC_POS	AUDIO_INTERFACE_DESC_POS+AUDIO_INTERFACE_DESC_SIZE
#ifdef  MULTITOUCH_INTERFACE
#define MULTITOUCH_INTERFACE_DESC_SIZE	9+9+7
#define MULTITOUCH_HID_DESC_OFFSET	MULTITOUCH_INTERFACE_DESC_POS+9
#else
#define MULTITOUCH_INTERFACE_DESC_SIZE	0
#endif

#define CONFIG_DESC_SIZE		MULTITOUCH_INTERFACE_DESC_POS+MULTITOUCH_INTERFACE_DESC_SIZE



// **************************************************************
//   USB Configuration
// **************************************************************

// USB Configuration Descriptor.  This huge descriptor tells all
// of the devices capbilities.
static uint8_t config_descriptor[CONFIG_DESC_SIZE] = {
        // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
        9,                                      // bLength;
        2,                                      // bDescriptorType;
        LSB(CONFIG_DESC_SIZE),                 // wTotalLength
        MSB(CONFIG_DESC_SIZE),
        NUM_INTERFACE,                          // bNumInterfaces
        1,                                      // bConfigurationValue
        0,                                      // iConfiguration
        0xC0,                                   // bmAttributes
        50,                                     // bMaxPower

#ifdef CDC_IAD_DESCRIPTOR
        // interface association descriptor, USB ECN, Table 9-Z
        8,                                      // bLength
        11,                                     // bDescriptorType
        CDC_STATUS_INTERFACE,                   // bFirstInterface
        2,                                      // bInterfaceCount
        0x02,                                   // bFunctionClass
        0x02,                                   // bFunctionSubClass
        0x01,                                   // bFunctionProtocol
        0,                                      // iFunction
#endif

#ifdef CDC_DATA_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        CDC_STATUS_INTERFACE,			// bInterfaceNumber
        0,                                      // bAlternateSetting
        1,                                      // bNumEndpoints
        0x02,                                   // bInterfaceClass
        0x02,                                   // bInterfaceSubClass
        0x01,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // CDC Header Functional Descriptor, CDC Spec 5.2.3.1, Table 26
        5,                                      // bFunctionLength
        0x24,                                   // bDescriptorType
        0x00,                                   // bDescriptorSubtype
        0x10, 0x01,                             // bcdCDC
        // Call Management Functional Descriptor, CDC Spec 5.2.3.2, Table 27
        5,                                      // bFunctionLength
        0x24,                                   // bDescriptorType
        0x01,                                   // bDescriptorSubtype
        0x01,                                   // bmCapabilities
        1,                                      // bDataInterface
        // Abstract Control Management Functional Descriptor, CDC Spec 5.2.3.3, Table 28
        4,                                      // bFunctionLength
        0x24,                                   // bDescriptorType
        0x02,                                   // bDescriptorSubtype
        0x06,                                   // bmCapabilities
        // Union Functional Descriptor, CDC Spec 5.2.3.8, Table 33
        5,                                      // bFunctionLength
        0x24,                                   // bDescriptorType
        0x06,                                   // bDescriptorSubtype
        CDC_STATUS_INTERFACE,                   // bMasterInterface
        CDC_DATA_INTERFACE,                     // bSlaveInterface0
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        CDC_ACM_ENDPOINT | 0x80,                // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        CDC_ACM_SIZE, 0,                        // wMaxPacketSize
        64,                                     // bInterval
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        CDC_DATA_INTERFACE,                     // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x0A,                                   // bInterfaceClass
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        CDC_RX_ENDPOINT,                        // bEndpointAddress
        0x02,                                   // bmAttributes (0x02=bulk)
        CDC_RX_SIZE, 0,                         // wMaxPacketSize
        0,                                      // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        CDC_TX_ENDPOINT | 0x80,                 // bEndpointAddress
        0x02,                                   // bmAttributes (0x02=bulk)
        CDC_TX_SIZE, 0,                         // wMaxPacketSize
        0,                                      // bInterval
#endif // CDC_DATA_INTERFACE

#ifdef MIDI_INTERFACE
        // Standard MS Interface Descriptor,
        9,                                      // bLength
        4,                                      // bDescriptorType
        MIDI_INTERFACE,                         // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x01,                                   // bInterfaceClass (0x01 = Audio)
        0x03,                                   // bInterfaceSubClass (0x03 = MIDI)
        0x00,                                   // bInterfaceProtocol (unused for MIDI)
        0,                                      // iInterface
        // MIDI MS Interface Header, USB MIDI 6.1.2.1, page 21, Table 6-2
        7,                                      // bLength
        0x24,                                   // bDescriptorType = CS_INTERFACE
        0x01,                                   // bDescriptorSubtype = MS_HEADER
        0x00, 0x01,                             // bcdMSC = revision 01.00
	LSB(7+(6+6+9+9)*MIDI_NUM_CABLES),       // wTotalLength
	MSB(7+(6+6+9+9)*MIDI_NUM_CABLES),
        // MIDI IN Jack Descriptor, B.4.3, Table B-7 (embedded), page 40
        6,                                      // bLength
        0x24,                                   // bDescriptorType = CS_INTERFACE
        0x02,                                   // bDescriptorSubtype = MIDI_IN_JACK
        0x01,                                   // bJackType = EMBEDDED
        1,                                      // bJackID, ID = 1
        0,                                      // iJack
        // MIDI IN Jack Descriptor, B.4.3, Table B-8 (external), page 40
        6,                                      // bLength
        0x24,                                   // bDescriptorType = CS_INTERFACE
        0x02,                                   // bDescriptorSubtype = MIDI_IN_JACK
        0x02,                                   // bJackType = EXTERNAL
        2,                                      // bJackID, ID = 2
        0,                                      // iJack
        // MIDI OUT Jack Descriptor, B.4.4, Table B-9, page 41
        9,
        0x24,                                   // bDescriptorType = CS_INTERFACE
        0x03,                                   // bDescriptorSubtype = MIDI_OUT_JACK
        0x01,                                   // bJackType = EMBEDDED
        3,                                      // bJackID, ID = 3
        1,                                      // bNrInputPins = 1 pin
        2,                                      // BaSourceID(1) = 2
        1,                                      // BaSourcePin(1) = first pin
        0,                                      // iJack
        // MIDI OUT Jack Descriptor, B.4.4, Table B-10, page 41
        9,
        0x24,                                   // bDescriptorType = CS_INTERFACE
        0x03,                                   // bDescriptorSubtype = MIDI_OUT_JACK
        0x02,                                   // bJackType = EXTERNAL
        4,                                      // bJackID, ID = 4
        1,                                      // bNrInputPins = 1 pin
        1,                                      // BaSourceID(1) = 1
        1,                                      // BaSourcePin(1) = first pin
        0,                                      // iJack
  #if MIDI_NUM_CABLES >= 2
	#define MIDI_INTERFACE_JACK_PAIR(a, b, c, d) \
		6, 0x24, 0x02, 0x01, (a), 0, \
		6, 0x24, 0x02, 0x02, (b), 0, \
		9, 0x24, 0x03, 0x01, (c), 1, (b), 1, 0, \
		9, 0x24, 0x03, 0x02, (d), 1, (a), 1, 0,
	MIDI_INTERFACE_JACK_PAIR(5, 6, 7, 8)
  #endif
  #if MIDI_NUM_CABLES >= 3
	MIDI_INTERFACE_JACK_PAIR(9, 10, 11, 12)
  #endif
  #if MIDI_NUM_CABLES >= 4
	MIDI_INTERFACE_JACK_PAIR(13, 14, 15, 16)
  #endif
  #if MIDI_NUM_CABLES >= 5
	MIDI_INTERFACE_JACK_PAIR(17, 18, 19, 20)
  #endif
  #if MIDI_NUM_CABLES >= 6
	MIDI_INTERFACE_JACK_PAIR(21, 22, 23, 24)
  #endif
  #if MIDI_NUM_CABLES >= 7
	MIDI_INTERFACE_JACK_PAIR(25, 26, 27, 28)
  #endif
  #if MIDI_NUM_CABLES >= 8
	MIDI_INTERFACE_JACK_PAIR(29, 30, 31, 32)
  #endif
  #if MIDI_NUM_CABLES >= 9
	MIDI_INTERFACE_JACK_PAIR(33, 34, 35, 36)
  #endif
  #if MIDI_NUM_CABLES >= 10
	MIDI_INTERFACE_JACK_PAIR(37, 38, 39, 40)
  #endif
  #if MIDI_NUM_CABLES >= 11
	MIDI_INTERFACE_JACK_PAIR(41, 42, 43, 44)
  #endif
  #if MIDI_NUM_CABLES >= 12
	MIDI_INTERFACE_JACK_PAIR(45, 46, 47, 48)
  #endif
  #if MIDI_NUM_CABLES >= 13
	MIDI_INTERFACE_JACK_PAIR(49, 50, 51, 52)
  #endif
  #if MIDI_NUM_CABLES >= 14
	MIDI_INTERFACE_JACK_PAIR(53, 54, 55, 56)
  #endif
  #if MIDI_NUM_CABLES >= 15
	MIDI_INTERFACE_JACK_PAIR(57, 58, 59, 60)
  #endif
  #if MIDI_NUM_CABLES >= 16
	MIDI_INTERFACE_JACK_PAIR(61, 62, 63, 64)
  #endif
        // Standard Bulk OUT Endpoint Descriptor, B.5.1, Table B-11, pae 42
        9,                                      // bLength
        5,                                      // bDescriptorType = ENDPOINT
        MIDI_RX_ENDPOINT,                       // bEndpointAddress
        0x02,                                   // bmAttributes (0x02=bulk)
        MIDI_RX_SIZE, 0,                        // wMaxPacketSize
        0,                                      // bInterval
        0,                                      // bRefresh
        0,                                      // bSynchAddress
        // Class-specific MS Bulk OUT Endpoint Descriptor, B.5.2, Table B-12, page 42
        4+MIDI_NUM_CABLES,                      // bLength
        0x25,                                   // bDescriptorSubtype = CS_ENDPOINT
        0x01,                                   // bJackType = MS_GENERAL
        MIDI_NUM_CABLES,                        // bNumEmbMIDIJack = number of jacks
        1,                                      // BaAssocJackID(1) = jack ID #1
  #if MIDI_NUM_CABLES >= 2
        5,
  #endif
  #if MIDI_NUM_CABLES >= 3
        9,
  #endif
  #if MIDI_NUM_CABLES >= 4
        13,
  #endif
  #if MIDI_NUM_CABLES >= 5
        17,
  #endif
  #if MIDI_NUM_CABLES >= 6
        21,
  #endif
  #if MIDI_NUM_CABLES >= 7
        25,
  #endif
  #if MIDI_NUM_CABLES >= 8
        29,
  #endif
  #if MIDI_NUM_CABLES >= 9
        33,
  #endif
  #if MIDI_NUM_CABLES >= 10
        37,
  #endif
  #if MIDI_NUM_CABLES >= 11
        41,
  #endif
  #if MIDI_NUM_CABLES >= 12
        45,
  #endif
  #if MIDI_NUM_CABLES >= 13
        49,
  #endif
  #if MIDI_NUM_CABLES >= 14
        53,
  #endif
  #if MIDI_NUM_CABLES >= 15
        57,
  #endif
  #if MIDI_NUM_CABLES >= 16
        61,
  #endif
        // Standard Bulk IN Endpoint Descriptor, B.5.1, Table B-11, pae 42
        9,                                      // bLength
        5,                                      // bDescriptorType = ENDPOINT
        MIDI_TX_ENDPOINT | 0x80,                // bEndpointAddress
        0x02,                                   // bmAttributes (0x02=bulk)
        MIDI_TX_SIZE, 0,                        // wMaxPacketSize
        0,                                      // bInterval
        0,                                      // bRefresh
        0,                                      // bSynchAddress
        // Class-specific MS Bulk IN Endpoint Descriptor, B.5.2, Table B-12, page 42
        4+MIDI_NUM_CABLES,                      // bLength
        0x25,                                   // bDescriptorSubtype = CS_ENDPOINT
        0x01,                                   // bJackType = MS_GENERAL
        MIDI_NUM_CABLES,                        // bNumEmbMIDIJack = number of jacks
        3,                                      // BaAssocJackID(1) = jack ID #3
  #if MIDI_NUM_CABLES >= 2
        7,
  #endif
  #if MIDI_NUM_CABLES >= 3
        11,
  #endif
  #if MIDI_NUM_CABLES >= 4
        15,
  #endif
  #if MIDI_NUM_CABLES >= 5
        19,
  #endif
  #if MIDI_NUM_CABLES >= 6
        23,
  #endif
  #if MIDI_NUM_CABLES >= 7
        27,
  #endif
  #if MIDI_NUM_CABLES >= 8
        31,
  #endif
  #if MIDI_NUM_CABLES >= 9
        35,
  #endif
  #if MIDI_NUM_CABLES >= 10
        39,
  #endif
  #if MIDI_NUM_CABLES >= 11
        43,
  #endif
  #if MIDI_NUM_CABLES >= 12
        47,
  #endif
  #if MIDI_NUM_CABLES >= 13
        51,
  #endif
  #if MIDI_NUM_CABLES >= 14
        55,
  #endif
  #if MIDI_NUM_CABLES >= 15
        59,
  #endif
  #if MIDI_NUM_CABLES >= 16
        63,
  #endif
#endif // MIDI_INTERFACE

#ifdef KEYBOARD_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        KEYBOARD_INTERFACE,                     // bInterfaceNumber
        0,                                      // bAlternateSetting
        1,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x01,                                   // bInterfaceSubClass (0x01 = Boot)
        0x01,                                   // bInterfaceProtocol (0x01 = Keyboard)
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(keyboard_report_desc)),      // wDescriptorLength
        MSB(sizeof(keyboard_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        KEYBOARD_ENDPOINT | 0x80,               // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        KEYBOARD_SIZE, 0,                       // wMaxPacketSize
        KEYBOARD_INTERVAL,                      // bInterval
#endif // KEYBOARD_INTERFACE

#ifdef MOUSE_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        MOUSE_INTERFACE,                        // bInterfaceNumber
        0,                                      // bAlternateSetting
        1,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass (0x01 = Boot)
        0x00,                                   // bInterfaceProtocol (0x02 = Mouse)
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(mouse_report_desc)),         // wDescriptorLength
        MSB(sizeof(mouse_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        MOUSE_ENDPOINT | 0x80,                  // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        MOUSE_SIZE, 0,                          // wMaxPacketSize
        MOUSE_INTERVAL,                         // bInterval
#endif // MOUSE_INTERFACE

#ifdef RAWHID_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        RAWHID_INTERFACE,                       // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(rawhid_report_desc)),        // wDescriptorLength
        MSB(sizeof(rawhid_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        RAWHID_TX_ENDPOINT | 0x80,              // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        RAWHID_TX_SIZE, 0,                      // wMaxPacketSize
        RAWHID_TX_INTERVAL,                     // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        RAWHID_RX_ENDPOINT,                     // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        RAWHID_RX_SIZE, 0,                      // wMaxPacketSize
        RAWHID_RX_INTERVAL,			// bInterval
#endif // RAWHID_INTERFACE

#ifdef FLIGHTSIM_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        FLIGHTSIM_INTERFACE,                    // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(flightsim_report_desc)),     // wDescriptorLength
        MSB(sizeof(flightsim_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        FLIGHTSIM_TX_ENDPOINT | 0x80,           // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        FLIGHTSIM_TX_SIZE, 0,                   // wMaxPacketSize
        FLIGHTSIM_TX_INTERVAL,                  // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        FLIGHTSIM_RX_ENDPOINT,                  // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        FLIGHTSIM_RX_SIZE, 0,                   // wMaxPacketSize
        FLIGHTSIM_RX_INTERVAL,			// bInterval
#endif // FLIGHTSIM_INTERFACE

#ifdef SEREMU_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        SEREMU_INTERFACE,                       // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(seremu_report_desc)),        // wDescriptorLength
        MSB(sizeof(seremu_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        SEREMU_TX_ENDPOINT | 0x80,              // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        SEREMU_TX_SIZE, 0,                      // wMaxPacketSize
        SEREMU_TX_INTERVAL,                     // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        SEREMU_RX_ENDPOINT,                     // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        SEREMU_RX_SIZE, 0,                      // wMaxPacketSize
        SEREMU_RX_INTERVAL,			// bInterval
#endif // SEREMU_INTERFACE

#ifdef JOYSTICK_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        JOYSTICK_INTERFACE,                     // bInterfaceNumber
        0,                                      // bAlternateSetting
        1,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(joystick_report_desc)),      // wDescriptorLength
        MSB(sizeof(joystick_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        JOYSTICK_ENDPOINT | 0x80,               // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        JOYSTICK_SIZE, 0,                       // wMaxPacketSize
        JOYSTICK_INTERVAL,                      // bInterval
#endif // JOYSTICK_INTERFACE

#ifdef FFBJOYSTICK_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        FFBJOYSTICK_INTERFACE,                     // bInterfaceNumber
        0,                                      // bAlternateSetting
        2,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(ffbjoystick_report_desc)),      // wDescriptorLength
        MSB(sizeof(ffbjoystick_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        FFBJOYSTICK_ENDPOINT_IN | 0x80,               // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        FFBJOYSTICK_SIZE_IN, 0,                       // wMaxPacketSize
        FFBJOYSTICK_INTERVAL,                      // bInterval
	    // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        FFBJOYSTICK_ENDPOINT_OUT | 0x00,               // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        FFBJOYSTICK_SIZE_OUT, 0,                       // wMaxPacketSize
        FFBJOYSTICK_INTERVAL,                      // bInterval	
		
#endif // FFBJOYSTICK_INTERFACE

#ifdef MTP_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        MTP_INTERFACE,                          // bInterfaceNumber
        0,                                      // bAlternateSetting
        3,                                      // bNumEndpoints
        0x06,                                   // bInterfaceClass (0x06 = still image)
        0x01,                                   // bInterfaceSubClass
        0x01,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        MTP_TX_ENDPOINT | 0x80,                 // bEndpointAddress
        0x02,                                   // bmAttributes (0x02=bulk)
        MTP_TX_SIZE, 0,                         // wMaxPacketSize
        0,                                      // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        MTP_RX_ENDPOINT,                        // bEndpointAddress
        0x02,                                   // bmAttributes (0x02=bulk)
        MTP_RX_SIZE, 0,                         // wMaxPacketSize
        0,                                      // bInterval
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        MTP_EVENT_ENDPOINT | 0x80,              // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        MTP_EVENT_SIZE, 0,                      // wMaxPacketSize
        MTP_EVENT_INTERVAL,                     // bInterval
#endif // MTP_INTERFACE

#ifdef KEYMEDIA_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        KEYMEDIA_INTERFACE,                     // bInterfaceNumber
        0,                                      // bAlternateSetting
        1,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(keymedia_report_desc)),      // wDescriptorLength
        MSB(sizeof(keymedia_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        KEYMEDIA_ENDPOINT | 0x80,               // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        KEYMEDIA_SIZE, 0,                       // wMaxPacketSize
        KEYMEDIA_INTERVAL,                      // bInterval
#endif // KEYMEDIA_INTERFACE

#ifdef AUDIO_INTERFACE
        // interface association descriptor, USB ECN, Table 9-Z
        8,                                      // bLength
        11,                                     // bDescriptorType
        AUDIO_INTERFACE,                        // bFirstInterface
        3,                                      // bInterfaceCount
        0x01,                                   // bFunctionClass
        0x01,                                   // bFunctionSubClass
        0x00,                                   // bFunctionProtocol
        0,                                      // iFunction
	// Standard AudioControl (AC) Interface Descriptor
	// USB DCD for Audio Devices 1.0, Table 4-1, page 36
	9,					// bLength
	4,					// bDescriptorType, 4 = INTERFACE
	AUDIO_INTERFACE,			// bInterfaceNumber
	0,					// bAlternateSetting
	0,					// bNumEndpoints
	1,					// bInterfaceClass, 1 = AUDIO
	1,					// bInterfaceSubclass, 1 = AUDIO_CONTROL
	0,					// bInterfaceProtocol
	0,					// iInterface
	// Class-specific AC Interface Header Descriptor
	// USB DCD for Audio Devices 1.0, Table 4-2, page 37-38
	10,					// bLength
	0x24,					// bDescriptorType, 0x24 = CS_INTERFACE
	0x01,					// bDescriptorSubtype, 1 = HEADER
	0x00, 0x01,				// bcdADC (version 1.0)
	LSB(62), MSB(62),			// wTotalLength
	2,					// bInCollection
	AUDIO_INTERFACE+1,			// baInterfaceNr(1) - Transmit to PC
	AUDIO_INTERFACE+2,			// baInterfaceNr(2) - Receive from PC
	// Input Terminal Descriptor
	// USB DCD for Audio Devices 1.0, Table 4-3, page 39
	12,					// bLength
	0x24,					// bDescriptorType, 0x24 = CS_INTERFACE
	0x02,					// bDescriptorSubType, 2 = INPUT_TERMINAL
	1,					// bTerminalID
	//0x01, 0x02,				// wTerminalType, 0x0201 = MICROPHONE
	//0x03, 0x06,				// wTerminalType, 0x0603 = Line Connector
	0x02, 0x06,				// wTerminalType, 0x0602 = Digital Audio
	0,					// bAssocTerminal, 0 = unidirectional
	2,					// bNrChannels
	0x03, 0x00,				// wChannelConfig, 0x0003 = Left & Right Front
	0,					// iChannelNames
	0, 					// iTerminal
	// Output Terminal Descriptor
	// USB DCD for Audio Devices 1.0, Table 4-4, page 40
	9,					// bLength
	0x24,					// bDescriptorType, 0x24 = CS_INTERFACE
	3,					// bDescriptorSubtype, 3 = OUTPUT_TERMINAL
	2,					// bTerminalID
	0x01, 0x01,				// wTerminalType, 0x0101 = USB_STREAMING
	0,					// bAssocTerminal, 0 = unidirectional
	1,					// bCSourceID, connected to input terminal, ID=1
	0,					// iTerminal
	// Input Terminal Descriptor
	// USB DCD for Audio Devices 1.0, Table 4-3, page 39
	12,					// bLength
	0x24,					// bDescriptorType, 0x24 = CS_INTERFACE
	2,					// bDescriptorSubType, 2 = INPUT_TERMINAL
	3,					// bTerminalID
	0x01, 0x01,				// wTerminalType, 0x0101 = USB_STREAMING
	0,					// bAssocTerminal, 0 = unidirectional
	2,					// bNrChannels
	0x03, 0x00,				// wChannelConfig, 0x0003 = Left & Right Front
	0,					// iChannelNames
	0, 					// iTerminal
	// Volume feature descriptor
	10,					// bLength
	0x24, 				// bDescriptorType = CS_INTERFACE
	0x06, 				// bDescriptorSubType = FEATURE_UNIT
	0x31, 				// bUnitID
	0x03, 				// bSourceID (Input Terminal)
	0x01, 				// bControlSize (each channel is 1 byte, 3 channels)
	0x01, 				// bmaControls(0) Master: Mute
	0x02, 				// bmaControls(1) Left: Volume
	0x02, 				// bmaControls(2) Right: Volume
	0x00,				// iFeature
	// Output Terminal Descriptor
	// USB DCD for Audio Devices 1.0, Table 4-4, page 40
	9,					// bLength
	0x24,					// bDescriptorType, 0x24 = CS_INTERFACE
	3,					// bDescriptorSubtype, 3 = OUTPUT_TERMINAL
	4,					// bTerminalID
	//0x02, 0x03,				// wTerminalType, 0x0302 = Headphones
	0x02, 0x06,				// wTerminalType, 0x0602 = Digital Audio
	0,					// bAssocTerminal, 0 = unidirectional
	0x31,				// bCSourceID, connected to feature, ID=31
	0,					// iTerminal
	// Standard AS Interface Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.5.1, Table 4-18, page 59
	// Alternate 0: default setting, disabled zero bandwidth
	9,					// bLenght
	4,					// bDescriptorType = INTERFACE
	AUDIO_INTERFACE+1,			// bInterfaceNumber
	0,					// bAlternateSetting
	0,					// bNumEndpoints
	1,					// bInterfaceClass, 1 = AUDIO
	2,					// bInterfaceSubclass, 2 = AUDIO_STREAMING
	0,					// bInterfaceProtocol
	0,					// iInterface
	// Alternate 1: streaming data
	9,					// bLenght
	4,					// bDescriptorType = INTERFACE
	AUDIO_INTERFACE+1,			// bInterfaceNumber
	1,					// bAlternateSetting
	1,					// bNumEndpoints
	1,					// bInterfaceClass, 1 = AUDIO
	2,					// bInterfaceSubclass, 2 = AUDIO_STREAMING
	0,					// bInterfaceProtocol
	0,					// iInterface
	// Class-Specific AS Interface Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.5.2, Table 4-19, page 60
	7, 					// bLength
	0x24,					// bDescriptorType = CS_INTERFACE
	1,					// bDescriptorSubtype, 1 = AS_GENERAL
	2,					// bTerminalLink: Terminal ID = 2
	3,					// bDelay (approx 3ms delay, audio lib updates)
	0x01, 0x00,				// wFormatTag, 0x0001 = PCM
	// Type I Format Descriptor
	// USB DCD for Audio Data Formats 1.0, Section 2.2.5, Table 2-1, page 10
	11,					// bLength
	0x24,					// bDescriptorType = CS_INTERFACE
	2,					// bDescriptorSubtype = FORMAT_TYPE
	1,					// bFormatType = FORMAT_TYPE_I
	2,					// bNrChannels = 2
	2,					// bSubFrameSize = 2 byte
	16,					// bBitResolution = 16 bits
	1,					// bSamFreqType = 1 frequency
	LSB(44100), MSB(44100), 0,		// tSamFreq
	// Standard AS Isochronous Audio Data Endpoint Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.6.1.1, Table 4-20, page 61-62
	9, 					// bLength
	5, 					// bDescriptorType, 5 = ENDPOINT_DESCRIPTOR
	AUDIO_TX_ENDPOINT | 0x80,		// bEndpointAddress
	0x09, 					// bmAttributes = isochronous, adaptive
	LSB(AUDIO_TX_SIZE), MSB(AUDIO_TX_SIZE),	// wMaxPacketSize
	1,			 		// bInterval, 1 = every frame
	0,					// bRefresh
	0,					// bSynchAddress
	// Class-Specific AS Isochronous Audio Data Endpoint Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.6.1.2, Table 4-21, page 62-63
	7,  					// bLength
	0x25,  					// bDescriptorType, 0x25 = CS_ENDPOINT
	1,  					// bDescriptorSubtype, 1 = EP_GENERAL
	0x00,  					// bmAttributes
	0,  					// bLockDelayUnits, 1 = ms
	0x00, 0x00,  				// wLockDelay
	// Standard AS Interface Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.5.1, Table 4-18, page 59
	// Alternate 0: default setting, disabled zero bandwidth
	9,					// bLenght
	4,					// bDescriptorType = INTERFACE
	AUDIO_INTERFACE+2,			// bInterfaceNumber
	0,					// bAlternateSetting
	0,					// bNumEndpoints
	1,					// bInterfaceClass, 1 = AUDIO
	2,					// bInterfaceSubclass, 2 = AUDIO_STREAMING
	0,					// bInterfaceProtocol
	0,					// iInterface
	// Alternate 1: streaming data
	9,					// bLenght
	4,					// bDescriptorType = INTERFACE
	AUDIO_INTERFACE+2,			// bInterfaceNumber
	1,					// bAlternateSetting
	2,					// bNumEndpoints
	1,					// bInterfaceClass, 1 = AUDIO
	2,					// bInterfaceSubclass, 2 = AUDIO_STREAMING
	0,					// bInterfaceProtocol
	0,					// iInterface
	// Class-Specific AS Interface Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.5.2, Table 4-19, page 60
	7, 					// bLength
	0x24,					// bDescriptorType = CS_INTERFACE
	1,					// bDescriptorSubtype, 1 = AS_GENERAL
	3,					// bTerminalLink: Terminal ID = 3
	3,					// bDelay (approx 3ms delay, audio lib updates)
	0x01, 0x00,				// wFormatTag, 0x0001 = PCM
	// Type I Format Descriptor
	// USB DCD for Audio Data Formats 1.0, Section 2.2.5, Table 2-1, page 10
	11,					// bLength
	0x24,					// bDescriptorType = CS_INTERFACE
	2,					// bDescriptorSubtype = FORMAT_TYPE
	1,					// bFormatType = FORMAT_TYPE_I
	2,					// bNrChannels = 2
	2,					// bSubFrameSize = 2 byte
	16,					// bBitResolution = 16 bits
	1,					// bSamFreqType = 1 frequency
	LSB(44100), MSB(44100), 0,		// tSamFreq
	// Standard AS Isochronous Audio Data Endpoint Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.6.1.1, Table 4-20, page 61-62
	9, 					// bLength
	5, 					// bDescriptorType, 5 = ENDPOINT_DESCRIPTOR
	AUDIO_RX_ENDPOINT,			// bEndpointAddress
	0x05, 					// bmAttributes = isochronous, asynchronous
	LSB(AUDIO_RX_SIZE), MSB(AUDIO_RX_SIZE),	// wMaxPacketSize
	1,			 		// bInterval, 1 = every frame
	0,					// bRefresh
	AUDIO_SYNC_ENDPOINT | 0x80,		// bSynchAddress
	// Class-Specific AS Isochronous Audio Data Endpoint Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.6.1.2, Table 4-21, page 62-63
	7,  					// bLength
	0x25,  					// bDescriptorType, 0x25 = CS_ENDPOINT
	1,  					// bDescriptorSubtype, 1 = EP_GENERAL
	0x00,  					// bmAttributes
	0,  					// bLockDelayUnits, 1 = ms
	0x00, 0x00,  				// wLockDelay
	// Standard AS Isochronous Audio Synch Endpoint Descriptor
	// USB DCD for Audio Devices 1.0, Section 4.6.2.1, Table 4-22, page 63-64
	9, 					// bLength
	5, 					// bDescriptorType, 5 = ENDPOINT_DESCRIPTOR
	AUDIO_SYNC_ENDPOINT | 0x80,		// bEndpointAddress
	0x11, 					// bmAttributes = isochronous, feedback
	3, 0,					// wMaxPacketSize, 3 bytes
	1,			 		// bInterval, 1 = every frame
	5,					// bRefresh, 5 = 32ms
	0,					// bSynchAddress
#endif

#ifdef MULTITOUCH_INTERFACE
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
        9,                                      // bLength
        4,                                      // bDescriptorType
        MULTITOUCH_INTERFACE,                   // bInterfaceNumber
        0,                                      // bAlternateSetting
        1,                                      // bNumEndpoints
        0x03,                                   // bInterfaceClass (0x03 = HID)
        0x00,                                   // bInterfaceSubClass
        0x00,                                   // bInterfaceProtocol
        0,                                      // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
        9,                                      // bLength
        0x21,                                   // bDescriptorType
        0x11, 0x01,                             // bcdHID
        0,                                      // bCountryCode
        1,                                      // bNumDescriptors
        0x22,                                   // bDescriptorType
        LSB(sizeof(multitouch_report_desc)),    // wDescriptorLength
        MSB(sizeof(multitouch_report_desc)),
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
        7,                                      // bLength
        5,                                      // bDescriptorType
        MULTITOUCH_ENDPOINT | 0x80,             // bEndpointAddress
        0x03,                                   // bmAttributes (0x03=intr)
        MULTITOUCH_SIZE, 0,                     // wMaxPacketSize
        1,                                      // bInterval
#endif // KEYMEDIA_INTERFACE
};


// **************************************************************
//   String Descriptors
// **************************************************************

// The descriptors above can provide human readable strings,
// referenced by index numbers.  These descriptors are the
// actual string data

/* defined in usb_names.h
struct usb_string_descriptor_struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t wString[];
};
*/

extern struct usb_string_descriptor_struct usb_string_manufacturer_name
        __attribute__ ((weak, alias("usb_string_manufacturer_name_default")));
extern struct usb_string_descriptor_struct usb_string_product_name
        __attribute__ ((weak, alias("usb_string_product_name_default")));
extern struct usb_string_descriptor_struct usb_string_serial_number
        __attribute__ ((weak, alias("usb_string_serial_number_default")));

struct usb_string_descriptor_struct string0 = {
        4,
        3,
        {0x0409}
};

struct usb_string_descriptor_struct usb_string_manufacturer_name_default = {
        2 + MANUFACTURER_NAME_LEN * 2,
        3,
        MANUFACTURER_NAME
};
struct usb_string_descriptor_struct usb_string_product_name_default = {
	2 + PRODUCT_NAME_LEN * 2,
        3,
        PRODUCT_NAME
};
struct usb_string_descriptor_struct usb_string_serial_number_default = {
        12,
        3,
        {0,0,0,0,0,0,0,0,0,0}
};
#ifdef MTP_INTERFACE
struct usb_string_descriptor_struct usb_string_mtp = {
	2 + 3 * 2,
	3,
	{'M','T','P'}
};
#endif

void usb_init_serialnumber(void)
{
	char buf[11];
	uint32_t i, num;

	__disable_irq();
#if defined(HAS_KINETIS_FLASH_FTFA) || defined(HAS_KINETIS_FLASH_FTFL)
	FTFL_FSTAT = FTFL_FSTAT_RDCOLERR | FTFL_FSTAT_ACCERR | FTFL_FSTAT_FPVIOL;
	FTFL_FCCOB0 = 0x41;
	FTFL_FCCOB1 = 15;
	FTFL_FSTAT = FTFL_FSTAT_CCIF;
	while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) ; // wait
	num = *(uint32_t *)&FTFL_FCCOB7;
#elif defined(HAS_KINETIS_FLASH_FTFE)
	kinetis_hsrun_disable();
	FTFL_FSTAT = FTFL_FSTAT_RDCOLERR | FTFL_FSTAT_ACCERR | FTFL_FSTAT_FPVIOL;
	*(uint32_t *)&FTFL_FCCOB3 = 0x41070000;
	FTFL_FSTAT = FTFL_FSTAT_CCIF;
	while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) ; // wait
	num = *(uint32_t *)&FTFL_FCCOBB;
	kinetis_hsrun_enable();
#endif
	__enable_irq();
	// add extra zero to work around OS-X CDC-ACM driver bug
	if (num < 10000000) num = num * 10;
	ultoa(num, buf, 10);
	for (i=0; i<10; i++) {
		char c = buf[i];
		if (!c) break;
		usb_string_serial_number_default.wString[i] = c;
	}
	usb_string_serial_number_default.bLength = i * 2 + 2;
}


// **************************************************************
//   Descriptors List
// **************************************************************

// This table provides access to all the descriptor data above.

const usb_descriptor_list_t usb_descriptor_list[] = {
	//wValue, wIndex, address,          length
	{0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
	{0x0200, 0x0000, config_descriptor, sizeof(config_descriptor)},
#ifdef SEREMU_INTERFACE
	{0x2200, SEREMU_INTERFACE, seremu_report_desc, sizeof(seremu_report_desc)},
	{0x2100, SEREMU_INTERFACE, config_descriptor+SEREMU_HID_DESC_OFFSET, 9},
#endif
#ifdef KEYBOARD_INTERFACE
        {0x2200, KEYBOARD_INTERFACE, keyboard_report_desc, sizeof(keyboard_report_desc)},
        {0x2100, KEYBOARD_INTERFACE, config_descriptor+KEYBOARD_HID_DESC_OFFSET, 9},
#endif
#ifdef MOUSE_INTERFACE
        {0x2200, MOUSE_INTERFACE, mouse_report_desc, sizeof(mouse_report_desc)},
        {0x2100, MOUSE_INTERFACE, config_descriptor+MOUSE_HID_DESC_OFFSET, 9},
#endif
#ifdef JOYSTICK_INTERFACE
        {0x2200, JOYSTICK_INTERFACE, joystick_report_desc, sizeof(joystick_report_desc)},
        {0x2100, JOYSTICK_INTERFACE, config_descriptor+JOYSTICK_HID_DESC_OFFSET, 9},
#endif
#ifdef FFBJOYSTICK_INTERFACE
        {0x2200, FFBJOYSTICK_INTERFACE, ffbjoystick_report_desc, sizeof(ffbjoystick_report_desc)},
        {0x2100, FFBJOYSTICK_INTERFACE, config_descriptor+FFBJOYSTICK_HID_DESC_OFFSET, 9},
#endif
#ifdef RAWHID_INTERFACE
	{0x2200, RAWHID_INTERFACE, rawhid_report_desc, sizeof(rawhid_report_desc)},
	{0x2100, RAWHID_INTERFACE, config_descriptor+RAWHID_HID_DESC_OFFSET, 9},
#endif
#ifdef FLIGHTSIM_INTERFACE
	{0x2200, FLIGHTSIM_INTERFACE, flightsim_report_desc, sizeof(flightsim_report_desc)},
	{0x2100, FLIGHTSIM_INTERFACE, config_descriptor+FLIGHTSIM_HID_DESC_OFFSET, 9},
#endif
#ifdef KEYMEDIA_INTERFACE
        {0x2200, KEYMEDIA_INTERFACE, keymedia_report_desc, sizeof(keymedia_report_desc)},
        {0x2100, KEYMEDIA_INTERFACE, config_descriptor+KEYMEDIA_HID_DESC_OFFSET, 9},
#endif
#ifdef MULTITOUCH_INTERFACE
        {0x2200, MULTITOUCH_INTERFACE, multitouch_report_desc, sizeof(multitouch_report_desc)},
        {0x2100, MULTITOUCH_INTERFACE, config_descriptor+MULTITOUCH_HID_DESC_OFFSET, 9},
#endif
#ifdef MTP_INTERFACE
	{0x0304, 0x0409, (const uint8_t *)&usb_string_mtp, 0},
#endif
        {0x0300, 0x0000, (const uint8_t *)&string0, 0},
        {0x0301, 0x0409, (const uint8_t *)&usb_string_manufacturer_name, 0},
        {0x0302, 0x0409, (const uint8_t *)&usb_string_product_name, 0},
        {0x0303, 0x0409, (const uint8_t *)&usb_string_serial_number, 0},
	{0, 0, NULL, 0}
};


// **************************************************************
//   Endpoint Configuration
// **************************************************************

#if 0
// 0x00 = not used
// 0x19 = Recieve only
// 0x15 = Transmit only
// 0x1D = Transmit & Recieve
//
const uint8_t usb_endpoint_config_table[NUM_ENDPOINTS] =
{
	0x00, 0x15, 0x19, 0x15, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#endif


const uint8_t usb_endpoint_config_table[NUM_ENDPOINTS] =
{
#if (defined(ENDPOINT1_CONFIG) && NUM_ENDPOINTS >= 1)
	ENDPOINT1_CONFIG,
#elif (NUM_ENDPOINTS >= 1)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT2_CONFIG) && NUM_ENDPOINTS >= 2)
	ENDPOINT2_CONFIG,
#elif (NUM_ENDPOINTS >= 2)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT3_CONFIG) && NUM_ENDPOINTS >= 3)
	ENDPOINT3_CONFIG,
#elif (NUM_ENDPOINTS >= 3)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT4_CONFIG) && NUM_ENDPOINTS >= 4)
	ENDPOINT4_CONFIG,
#elif (NUM_ENDPOINTS >= 4)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT5_CONFIG) && NUM_ENDPOINTS >= 5)
	ENDPOINT5_CONFIG,
#elif (NUM_ENDPOINTS >= 5)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT6_CONFIG) && NUM_ENDPOINTS >= 6)
	ENDPOINT6_CONFIG,
#elif (NUM_ENDPOINTS >= 6)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT7_CONFIG) && NUM_ENDPOINTS >= 7)
	ENDPOINT7_CONFIG,
#elif (NUM_ENDPOINTS >= 7)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT8_CONFIG) && NUM_ENDPOINTS >= 8)
	ENDPOINT8_CONFIG,
#elif (NUM_ENDPOINTS >= 8)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT9_CONFIG) && NUM_ENDPOINTS >= 9)
	ENDPOINT9_CONFIG,
#elif (NUM_ENDPOINTS >= 9)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT10_CONFIG) && NUM_ENDPOINTS >= 10)
	ENDPOINT10_CONFIG,
#elif (NUM_ENDPOINTS >= 10)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT11_CONFIG) && NUM_ENDPOINTS >= 11)
	ENDPOINT11_CONFIG,
#elif (NUM_ENDPOINTS >= 11)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT12_CONFIG) && NUM_ENDPOINTS >= 12)
	ENDPOINT12_CONFIG,
#elif (NUM_ENDPOINTS >= 12)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT13_CONFIG) && NUM_ENDPOINTS >= 13)
	ENDPOINT13_CONFIG,
#elif (NUM_ENDPOINTS >= 13)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT14_CONFIG) && NUM_ENDPOINTS >= 14)
	ENDPOINT14_CONFIG,
#elif (NUM_ENDPOINTS >= 14)
	ENDPOINT_UNUSED,
#endif
#if (defined(ENDPOINT15_CONFIG) && NUM_ENDPOINTS >= 15)
	ENDPOINT15_CONFIG,
#elif (NUM_ENDPOINTS >= 15)
	ENDPOINT_UNUSED,
#endif
};


#endif // NUM_ENDPOINTS
#endif // F_CPU >= 20 MHz
