/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

/** INCLUDES *******************************************************/
#include "./pic16f1459/system.h"

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#include "../framework/usb/usb.h"

#include "app_led_usb_status.h"
#include "app_device_cdc_basic.h"
#include "usb_config.h"


extern bool ready_scan;

#define N_ADC          39  
extern uint8_t adc_levels[N_ADC];

static USB_ADC_EVENT_PACKET midiData;

/** VARIABLES ******************************************************/

static bool buttonPressed;
static char buttonMessage[] = "Button pressed.\r\n";
static uint8_t readBuffer[CDC_DATA_OUT_EP_SIZE];
static uint8_t writeBuffer[CDC_DATA_IN_EP_SIZE];

/*********************************************************************
* Function: void APP_DeviceCDCBasicDemoInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCDCBasicDemoInitialize()
{
    CDCInitEP();

    
    line_coding.bCharFormat = 0;
    line_coding.bDataBits = 8;
    line_coding.bParityType = 0;
    line_coding.dwDTERate = 9600;

    buttonPressed = false;
}

/*********************************************************************
* Function: void APP_DeviceCDCBasicDemoTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceCDCBasicDemoInitialize() and APP_DeviceCDCBasicDemoStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCDCBasicDemoTasks()
{
    uint8_t numBytesRead;
    uint8_t temp_level;
    static uint8_t numberChADC = 0;
    
    if (ready_scan){
        temp_level = adc_levels[numberChADC];
        numBytesRead = sprintf(writeBuffer, "ADC_NN=%u, LEVEL=%u \n\r", numberChADC, temp_level);
         if( USBUSARTIsTxTrfReady() == true)
            {
             putUSBUSART(writeBuffer,numBytesRead);
             numberChADC ++;
             if (numberChADC >= N_ADC) numberChADC = 0;
            }
      
           
    }    
    CDCTxService();
    
          //  USBTxOnePacket(USB_MAX_EP_NUMBER,(uint8_t*)&midiData,2);
          
   //     numBytesRead = sprintf(writeBuffer, "ADC_NN=%u, LEVEL=%u", midiData.IDX, midiData.ADC);
   //     putUSBUSART(writeBuffer,numBytesRead);
    
    
    
    
    /* If the user has pressed the button associated with this demo, then we
     * are going to send a "Button Pressed" message to the terminal.
     */
   // if(BUTTON_IsPressed(BUTTON_DEVICE_CDC_BASIC_DEMO) == true)
   // {
        /* Make sure that we only send the message once per button press and
         * not continuously as the button is held.
         */
   //     if(buttonPressed == false)
   //     {
            /* Make sure that the CDC driver is ready for a transmission.
             */
   //         if(mUSBUSARTIsTxTrfReady() == true)
   //         {
   //             putrsUSBUSART(buttonMessage);
   //             buttonPressed = true;
   //         }
   //     }
   // }
   // else
   // {
        /* If the button is released, we can then allow a new message to be
         * sent the next time the button is pressed.
         */
   //     buttonPressed = false;
   // }

    /* Check to see if there is a transmission in progress, if there isn't, then
     * we can see about performing an echo response to data received.
     */
  //  if( USBUSARTIsTxTrfReady() == true)
  //  {
   //     uint8_t i;
   //     uint8_t numBytesRead;

    //    numBytesRead = getsUSBUSART(readBuffer, sizeof(readBuffer));

        /* For every byte that was read... */
   //     for(i=0; i<numBytesRead; i++)
   //    {
   //         switch(readBuffer[i])
   //         {
                /* If we receive new line or line feed commands, just echo
                 * them direct.
                 */
   //             case 0x0A:
   //             case 0x0D:
   //                 writeBuffer[i] = readBuffer[i];
   //                 break;

                /* If we receive something else, then echo it plus one
                 * so that if we receive 'a', we echo 'b' so that the
                 * user knows that it isn't the echo enabled on their
                 * terminal program.
                 */
   //             default:
   //                 writeBuffer[i] = readBuffer[i] + 1;
   //                 break;
   //         }
   //     }

   //     if(numBytesRead > 0)
   //     {
            /* After processing all of the received data, we need to send out
             * the "echo" data now.
             */
          //  putUSBUSART(writeBuffer,numBytesRead);
    //    }
   // }

  //  CDCTxService();
}