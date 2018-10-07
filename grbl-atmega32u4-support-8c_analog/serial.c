/*
  serial.c - Low level functions for sending and recieving bytes via the serial port
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011-2012 Sungeun K. Jeon

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

/* This code was initially inspired by the wiring_serial module by David A. Mellis which
   used to be a part of the Arduino project. */

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "serial.h"
#include "config.h"
#include "stepper.h"
#include "spindle_control.h"
#include "nuts_bolts.h"
#include "protocol.h"
#include "settings.h"

#include "Descriptors.h"
#include <LUFA/Drivers/USB/USB.h>


/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs
 */
static FILE USBSerialStream;

bool connected = false, was_connected = false;

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
  {
    .Config =
      {
        .ControlInterfaceNumber   = 0,
        .DataINEndpoint           =
          {
            .Address          = CDC_TX_EPADDR,
            .Size             = CDC_TXRX_EPSIZE,
            .Banks            = 1,
          },
        .DataOUTEndpoint =
          {
            .Address          = CDC_RX_EPADDR,
            .Size             = CDC_TXRX_EPSIZE,
            .Banks            = 1,
          },
        .NotificationEndpoint =
          {
            .Address          = CDC_NOTIFICATION_EPADDR,
            .Size             = CDC_NOTIFICATION_EPSIZE,
            .Banks            = 1,
          },
      },
  };


void serial_init()
{

  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  /* Disable clock division */
  clock_prescale_set(clock_div_1);
  USB_Init();

  /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
  sei();
}

void serial_tick() {

  if (connected && !was_connected) {
    // Print grbl initialization message
    printPgmString(PSTR("\r\nGrbl"), GRBL_VERSION);
    printPgmString(PSTR("\r\n"));
    printPgmString(PSTR("\r\n'$' to dump current settings\r\n"));
    was_connected = true;
  } else if (was_connected && !connected) {
    // TODO: reset the world, and go idle.
    was_connected = false;
  }

  int bytesAvailable = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
  while (bytesAvailable--) {
    protocol_process(CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface));
//    printPgmString(PSTR("\r\nReceived Byte\r\n"));//DEBUG ONLY
  }

  CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
  USB_USBTask();
}

void serial_write(uint8_t data) {
  fputc(data, &USBSerialStream);
}

void serial_reset_read_buffer()
{
  CDC_Device_Flush(&VirtualSerial_CDC_Interface);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{


  CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
  CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo) {
  connected = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR);
}
