/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2015 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    UDP_Socket.c
 * Purpose: UDP Socket Code Template
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_UDP_Socket]
#include "rl_net.h"
#include "Board_LED.h"                  // ::Board Support:LED
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include <stdint.h>
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "LPC17xx.h"                    // Device header

int32_t udp_sock;                       // UDP socket handle
 
// Notify the user application about UDP socket events.
uint32_t udp_cb_func (int32_t socket, const  NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
  
  // Data received
  if ((buf[1] == 0x01) && (len == 2)) {
    // Switch LEDs on and off
     LED_On(buf[0]);
  }
	else if ((buf[1] == 0x00) && (len == 2)){
		LED_Off(buf[0]);
	}
  return (0);
}
 
// Send UDP data to destination client.
void send_udp_data (a,b) {
 
  if (udp_sock > 0) {
    //IPv4 address: 192.168.0.194
    NET_ADDR addr = { NET_ADDR_IP4, 2098, 192, 168, 0, 104};
    // IPv6 address: [fe80::1c30:6cff:fea2:455e]
		//	NET_ADDR addr = { NET_ADDR_IP6, 2000,0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x1c, 0x30, 0x6C, 0xff, 0xfe, 0xa2, 0x45, 0x5e };
    uint8_t *sendbuf;

    sendbuf = netUDP_GetBuffer (2);
    sendbuf[0] = b;
    sendbuf[1] = a;
 
    netUDP_Send (udp_sock, &addr, sendbuf, 2);
		
	}
}
 
// Allocate and initialize the socket.
int main (void) 
{
  LED_Initialize();
  netInitialize ();
  LPc
  // Initialize UDP socket and open port 2000
  udp_sock = netUDP_GetSocket (udp_cb_func);
  if (udp_sock > 0) {
    netUDP_Open (udp_sock, 2017);
  }
	
	while(1)
	{
		
		send_udp_data(0x01,0x00);
		osDelay(50);		
		send_udp_data(0x01,0x01);
		osDelay(50);
		send_udp_data(0x01,0x02);
		osDelay(50);
		send_udp_data(0x01,0x03);
		osDelay(50);
		send_udp_data(0x01,0x04);
		osDelay(50);
		send_udp_data(0x01,0x05);
		osDelay(50);
		send_udp_data(0x01,0x06);
		osDelay(50);
		send_udp_data(0x01,0x07);
		osDelay(50);
		
		send_udp_data(0x00,0x00);
		osDelay(50);
		send_udp_data(0x00,0x01);
		osDelay(50);
		send_udp_data(0x00,0x02);
		osDelay(50);
		send_udp_data(0x00,0x03);
		osDelay(50);
		send_udp_data(0x00,0x04);
		osDelay(50);
		send_udp_data(0x00,0x05);
		osDelay(50);
		send_udp_data(0x00,0x06);
		osDelay(50);
		send_udp_data(0x00,0x07);
		osDelay(50);
	}
	
}
//! [code_UDP_Socket]
