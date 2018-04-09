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
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Driver_CAN.h"                 // ::CMSIS Driver:CAN
#include "stdio.h"

extern   ARM_DRIVER_CAN         Driver_CAN1;
extern   ARM_DRIVER_CAN         Driver_CAN2;

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;



void InitCan1 (void);
void InitCan2(void);
void envoi_CAN( short ID[], char data[], uint32_t len);
void recep_CAN(void /*const *argument*/);
void myCAN1_callback(uint32_t obj_idx, uint32_t event);
void myCAN2_callback(uint32_t obj_idx, uint32_t event);


int32_t udp_sock;                       // UDP socket handle




// Notify the user application about UDP socket events.
uint32_t udp_cb_func (int32_t socket, const  NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
  
  // Data received
  char data[20],chaine_data[20];
	short ID[1];
	int i,j;

	for (i=0; i<len-2;i++)
	{
    data[i]=buf[2+i];	

	sprintf(chaine_data,"%02x",data[i]);
	GLCD_DrawString (20*i,150, chaine_data);
	}

		for (i=0; i<2;i++)
	{
    ID[i]=buf[i];	
	}
	//ID[0] = (((unsigned short)buf[0]) << 8) | buf[1];
	envoi_CAN(ID,data,len);
	
	
  return (0);
}
 


// Send UDP data to destination client.
void send_udp_data (char taille, char DATANET[],char NETIP[] ) {
 
		int i;
	int taille_data;
 char chainetest[30],chainetest1[30];
	
			
	
  if (udp_sock > 0) {
    //IPv4 address: 192.168.0.194
NET_ADDR addr = { NET_ADDR_IP4, 2098, 192, 168, 0, 125};  	
	
		 
		uint8_t *sendbuf;
		addr.addr[0] = NETIP[0];
		addr.addr[1] = NETIP[1];
		addr.addr[2] = NETIP[2];
		addr.addr[3] = NETIP[3];
    //NET_ADDR addr = { NET_ADDR_IP4, 2098, 0, NETIP[1], NETIP[2], NETIP[3]};
    // IPv6 address: [fe80::1c30:6cff:fea2:455e]
		//	NET_ADDR addr = { NET_ADDR_IP6, 2000,0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x1c, 0x30, 0x6C, 0xff, 0xfe, 0xa2, 0x45, 0x5e };

		sprintf(chainetest,"recu %d,%d,%d,%d",addr.addr[0],addr.addr[1],addr.addr[2],addr.addr[3]);
    GLCD_DrawString (1, 50, chainetest);

		taille_data = taille - 4;
    
		//sprintf(chainetest1,"taille = %d",taille_data );
     //GLCD_DrawString (1, 125, chainetest1);
		
		sendbuf = netUDP_GetBuffer (taille_data);
    for (i=0;i<taille_data;i++)
		{
		sendbuf[i] = DATANET[i];

		}			
	//sprintf(chainetest1,"data= %d,%d,%d,%d",DATANET[0],DATANET[1],DATANET[2],DATANET[3]);
     
     sprintf(chainetest1,"recu %x,%x,%x,%x",sendbuf[0],sendbuf[1],sendbuf[2],sendbuf[3]);
GLCD_DrawString (1, 125, chainetest1);
		
    netUDP_Send (udp_sock, &addr, sendbuf, 4);
		
	}
}


 
// Allocate and initialize the socket.
int main (void) 
{
  LED_Initialize();
  netInitialize ();
  InitCan1();
	InitCan2();

	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont (&GLCD_Font_16x24);
  // Initialize UDP socket and open port 2000
  udp_sock = netUDP_GetSocket (udp_cb_func);
  if (udp_sock > 0) 
	{
    netUDP_Open (udp_sock, 5555);
  }
	while(1){
		
	recep_CAN();
	}
}




void envoi_CAN(short ID[], char data[], uint32_t len)
{
	char data_buf[4];
	char chaineID[20];
	char chaineDATA[20];
	char i;

	ARM_CAN_MSG_INFO                tx_msg_info;
	tx_msg_info.id = ARM_CAN_STANDARD_ID(ID[0]<<8|ID[1]);
	tx_msg_info.rtr = 0;
	
	for ( i=0;i<len-2;i++)
	{
		data_buf[i] = data[i];
		
		sprintf(chaineDATA,"%x",data_buf[i]);
	GLCD_DrawString (35*i,80, chaineDATA);
	}
			
	sprintf(chaineID,"ID = %02x ",(short)(ID[0]<<8|ID[1]));
	GLCD_DrawString(0,0, (unsigned char*) chaineID);
	
	//	sprintf(chaineID," identifiant : %04x",ID);
//	GLCD_DrawString (0,0, chaineID);
	
	Driver_CAN2.MessageSend(1,&tx_msg_info,data_buf,4);
}

void recep_CAN(void/* const *argument*/){
			ARM_CAN_MSG_INFO   rx_msg_info;
	uint8_t data_buf[8];
	char NETIP[4];
	char DATANET[4];
	
	
	char texte1[30],texte2[30];
	int identifiant,i,j;
	char retour;
	char taille;
	

			
	Driver_CAN1.MessageRead(0,&rx_msg_info, data_buf,8);
   retour = data_buf[0];
		
		
	
	
		taille= rx_msg_info.dlc;
		
		if ( rx_msg_info.id == 0x161)
		  {
				
					
		for (i=0; i<4 ;i++) // adresse IP trame ethernet
		 {
			NETIP[i]=data_buf[i];
		 }

		 sprintf(texte1,"recu IP %x,%x,%x,%x",NETIP[0],NETIP[1],NETIP[2],NETIP[3]);
		 GLCD_DrawString (1, 1, texte1);
		 
	 for (j=4; j<8;j++) // data trame ethernet
		 {
			DATANET[j-4]=data_buf[j];
	 }
		 		 sprintf(texte2,"recu DATA %x,%x,%x,%x",DATANET[0],DATANET[1],DATANET[2],DATANET[3]);
		 GLCD_DrawString (1, 200, texte2);
		 while(1){
		send_udp_data(taille,DATANET,NETIP);
		 }
	 		osDelay(50);
      }
}

	// code_it2r;