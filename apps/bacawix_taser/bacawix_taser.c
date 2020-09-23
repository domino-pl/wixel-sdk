// Copyright 2020, Dominik Przybysz, All rights reserved.
// dominik@przybysz.dev

#include <wixel.h>
#include <usb.h>
#include <usb_com.h>
#include <radio_queue.h>

int32 CODE param_poweron_key = 0x17; // T = 23

void updateLeds(){
    usbShowStatusWithGreenLed();
}

uint8 usb_initialized = 0;
uint32 start_time = 0;
uint32 usb_init_delay_ms = 1000;
uint8 XDATA * packet = 0;

void main(){
    systemInit();
    radioQueueInit();

    packet = radioQueueTxCurrentPacket();
    if (packet != 0){
        packet[0] = 1;
        packet[1] = (uint8)param_poweron_key;
        radioQueueTxSendPacket();
    }

    start_time = getMs();

    while(1){
        boardService();
        if(getMs() - start_time > usb_init_delay_ms){
            if(!usb_initialized)
                usbInit();
            usbComService();
            updateLeds();
        }
    }
}
