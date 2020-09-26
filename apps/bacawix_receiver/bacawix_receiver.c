// Copyright 2020, Dominik Przybysz, All rights reserved.
// dominik@przybysz.dev

#define MODE_HID 1
#define MODE_SERIAL 2

#define MODE MODE_HID // HID or SERIAL

#define BUFFER_SIZE (6)

#include <wixel.h>
#include <usb.h>

#if MODE == MODE_SERIAL
#include <usb_com.h>
#include <stdio.h>
#endif

#if MODE == MODE_HID
#include <usb_hid.h>
#endif

#include <radio_queue.h>

/* packet format: 
 * PACKAT_ID,
 * WIXEL_SERIAL[0],WIXEL_SERIAL[1],WIXEL_SERIAL[2],WIXEL_SERIAL[3],
 * HID_KEY_CODE
 * 
 * packet store format:
 * MAX_PACKET_ID, // set 254 after print in serial mode
 * WIXEL_SERIAL[0],WIXEL_SERIAL[1],WIXEL_SERIAL[2],WIXEL_SERIAL[3],
 * HID_KEY_CODE,
 * PACKET_IN_SERIES_COUNTER,
 * FIRST_TIME[0],FIRST_TIME[1],FIRST_TIME[2],FIRST_TIME[3]
 * 
**/

#if MODE == MODE_SERIAL
int32 CODE param_serial_show_raw_packages = 0;

uint8 com_message_lenght = 0;
uint8 XDATA com_buffer[81];
#endif

uint8 buffer[11*BUFFER_SIZE] = {0};

uint8 * packet = 0;

uint8 key_to_send = 0;
uint8 key_clear = 0;

uint32 led_red_last_on_time = 0;
uint32 led_red_on_time = 1000;

uint8 compare_bytes(uint8 * a, uint8 * b, uint8 n){
    uint8 i;
    for(i = 0; i<n; i++)
        if(a[i] != b[i])
            return 0;
    return 1;
}

void copy_bytes(uint8 * from, uint8 * to, uint8 n){
    uint8 i;
    for(i = 0; i<n; i++)
        to[i] = from[i];
}

uint8 massage_position_in_buffor(){
    uint8 i;
    for(i = 0; i < BUFFER_SIZE; i++)
        if(compare_bytes(packet+1, buffer+11*i+1, 5))
            return i;
    return 254;
}

uint32 * first_time(uint8 position){
    return (uint32 *)(buffer+11*position+7);
}

uint8 oldest_position_in_buffer(){
    uint8 oldest_position = 0;
    uint8 i;
    for(i = 1; i < BUFFER_SIZE; i++)
        if(*first_time(oldest_position) > *first_time(i))
            oldest_position = i;
    return oldest_position;
}

void overwrite_message(uint8 position){
    copy_bytes(packet, buffer+11*position, 6);
    buffer[11*position+6] = 1;
    *first_time(position) = getMs();
}

void add_massage(){
    overwrite_message(oldest_position_in_buffer());
}

void update_message(uint8 position){
    buffer[position*11] = packet[0];
    buffer[position*11+6] += 1;
}

uint8 process_packet(){
    uint8 position = massage_position_in_buffor();
    
    
    if(position == 254){
        add_massage();
        return packet[5];
    }else{
        if(packet[0] < buffer[position*11]){
            overwrite_message(position);
            return packet[5];
        }else{
            update_message(position);
        }
    }
        
    return 0;
}

void updateLeds(){
    usbShowStatusWithGreenLed();
    LED_YELLOW(radioQueueRxCurrentPacket());
    LED_RED(getMs() - led_red_last_on_time < led_red_on_time);
}

void bufferSevice(){
    uint8 i;
    for(i = 0; i < BUFFER_SIZE; i++){
        if(buffer[i*11] == 254 || buffer[i*11+6] == 0)
            continue;
        
        if(getMs() - *first_time(i) > 500){
            
            #if MODE == MODE_SERIAL
            while(!usbComTxAvailable());
            com_message_lenght = sprintf(
                com_buffer, 
                "From %2X-%2X-%2X-%2X got %2d (%2d/%2d)\r\n", 
                buffer[i*11+1], buffer[i*11+2], buffer[i*11+3],
                buffer[i*11+4], buffer[i*11+5], buffer[i*11+6],
                buffer[i*11]+1
            );
            usbComTxSend(com_buffer, com_message_lenght);
            #endif
            
            if(3*buffer[i*11+6] < buffer[i*11]+1)
                led_red_last_on_time = getMs();
            
            buffer[i*11] = 254;
        } 
    }
}

void receiverService(){
    if(key_clear){
        #if MODE == MODE_HID
        usbHidKeyboardInput.keyCodes[0] = 0;
        usbHidKeyboardInput.keyCodes[1] = 0;
        usbHidKeyboardInput.keyCodes[2] = 0;
        usbHidKeyboardInput.keyCodes[3] = 0;
        usbHidKeyboardInput.keyCodes[4] = 0;
        usbHidKeyboardInput.keyCodes[5] = 0;
        usbHidKeyboardInputUpdated = 1;
        #endif
        
        key_clear = 0;
        return;
    }
    
    packet = radioQueueRxCurrentPacket();
    if(packet){
        #if MODE == MODE_SERIAL
        if(param_serial_show_raw_packages){
            
            while(!usbComTxAvailable());
            com_message_lenght = sprintf(
                com_buffer, 
                "%2d-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X-%2X\r\n", 
                packet[0], packet[1], packet[2], packet[3],
                packet[4], packet[5], packet[6], packet[7],
                packet[8], packet[9], packet[10], packet[11],
                packet[12], packet[13], packet[14], packet[15],
                packet[16], packet[17],    packet[18], packet[19],
                packet[20]
            );
            usbComTxSend(com_buffer, com_message_lenght);
        
        }
        #endif
        
        packet += 1;
        
        key_to_send = process_packet();
        if(key_to_send){
            #if MODE == MODE_HID
            usbHidKeyboardInput.keyCodes[0] = key_to_send;
            usbHidKeyboardInputUpdated = 1;
            #endif
            
            key_clear = 1;
        }
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
        
        #if MODE == MODE_HID
        usbHidService();
        #endif
        
        bufferSevice();
        
        #if MODE == MODE_SERIAL
        usbComService();
        #endif
    }
}
