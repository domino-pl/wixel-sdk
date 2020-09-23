// Copyright 2020, Dominik Przybysz, All rights reserved.
// dominik@przybysz.dev

#include <wixel.h>
#include <usb.h>
#include <usb_hid.h>
#include <radio_queue.h>

void updateLeds(){
    usbShowStatusWithGreenLed();
    LED_YELLOW(radioQueueRxCurrentPacket());
}

uint8 key_sent = 0;
uint8 * packet = 0;

void receiverService(){
    if(key_sent){
        usbHidKeyboardInput.keyCodes[0] = 0;
        usbHidKeyboardInput.keyCodes[1] = 0;
        usbHidKeyboardInput.keyCodes[2] = 0;
        usbHidKeyboardInput.keyCodes[3] = 0;
        usbHidKeyboardInput.keyCodes[4] = 0;
        usbHidKeyboardInput.keyCodes[5] = 0;
        usbHidKeyboardInputUpdated = 1;
        key_sent = 0;
        return;
    }
    
    packet = radioQueueRxCurrentPacket();
    if(packet){
        usbHidKeyboardInput.keyCodes[0] = packet[1];
        usbHidKeyboardInputUpdated = 1;
        key_sent = 1;
        radioQueueRxDoneWithPacket();
    }
}

void main(){
    systemInit();
    usbInit();
    radioQueueInit();

    while(1){
        boardService();
        updateLeds();
        receiverService();
        usbHidService();
    }
}
