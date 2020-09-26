// Copyright 2020, Dominik Przybysz, All rights reserved.
// dominik@przybysz.dev

/* packet format: 
 * WIXEL_SERIAL[0],WIXEL_SERIAL[1],WIXEL_SERIAL[2],WIXEL_SERIAL[3],
 * PACKAT_ID,
 * HID_KEY_CODE
 * 
 * program sends via radio_queue PACKET_SERIES_SIZE same packets 
 * with interval PACKET_SERIAS_INTERVAL_MS PACKET_SERIAS_COUNT times
 * wait MAIN_START_DELAY_MS and start usbComService
 * 
**/

#define PACKET_SERIES_INTERVAL_MS (5)
#define PACKET_SERIES_SIZE (4)
#define PACKET_SERIES_COUNT (5)
#define MAIN_START_DELAY_MS (500)

#include <wixel.h>
#include <usb.h>
#include <usb_com.h>
#include <radio_queue.h>

// https://pololu.github.io/wixel-sdk/usb__hid__constants_8h_source.html
int32 CODE param_poweron_key = 0x17; // T = 23 

uint32 time = 0;
uint8 packetCounter = 0;
uint8 XDATA * packet = 0;

void send_packet(){
    while(!(packet = radioQueueTxCurrentPacket())){
        LED_RED(1);
    }
    LED_RED(0);
    
    packet[0] = 6;
    packet[1] = packetCounter++;
    packet[2] = serialNumber[0];
    packet[3] = serialNumber[1];
    packet[4] = serialNumber[2];
    packet[5] = serialNumber[3];
    packet[6] = (uint8)param_poweron_key;
    radioQueueTxSendPacket();
}

void main(){
	uint8 i;
	uint8 j;
	
    systemInit();
    radioQueueInit();

    for(i=0; i<PACKET_SERIES_COUNT; i++){
        for(j=0; j<PACKET_SERIES_SIZE; j++)
            send_packet();
        
        time = getMs();
        while(getMs() - time < PACKET_SERIES_INTERVAL_MS);
   }
    
    time = getMs();
    while(getMs() - time < MAIN_START_DELAY_MS);
    
    usbInit();

    while(1){
        boardService();
        usbComService();
        usbShowStatusWithGreenLed();
    }
}
