// Copyright 2020, Dominik Przybysz, All rights reserved.
// dominik@przybysz.dev

/* Aplication use to spy all queue packets on selected channel 
 * via Serial Port with baud 9600 */

#include <radio_queue.h>
#include <wixel.h>
#include <usb.h>
#include <usb_com.h>
#include <stdio.h>

uint8 XDATA response[81];
uint8 responseLength;
uint8 * packet = 0;

uint32 ledYellowOnTime = 0;

void updateLeds(){
    usbShowStatusWithGreenLed();
    
    LED_YELLOW(getMs() - ledYellowOnTime < 20);
}

void listenerService(){
    packet = radioQueueRxCurrentPacket();
    if(packet){
        ledYellowOnTime = getMs();
        
        responseLength = sprintf(
            response, 
            "%2d-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X\r\n", 
            packet[0], packet[1], packet[2], packet[3], packet[4],
            packet[5], packet[6], packet[7], packet[8], packet[9],
            packet[10], packet[11], packet[12], packet[13],
            packet[14], packet[15], packet[16], packet[17],
            packet[18], packet[19], packet[20]
        );
        usbComTxSend(response, responseLength);
        radioQueueRxDoneWithPacket();
    }
}

void main()
{
    systemInit();
    radioQueueInit();
    usbInit();

    while(1)
    {
        boardService();
        updateLeds();
        listenerService();
        usbComService();
    }
    
}
