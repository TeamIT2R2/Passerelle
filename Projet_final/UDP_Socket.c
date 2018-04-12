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
#include "LPC17xx.h"                    // Device header
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO

#define MAILQUEUE_OBJECTS      16                               // number of Message Queue Objects

extern   ARM_DRIVER_CAN         Driver_CAN1;
extern   ARM_DRIVER_CAN         Driver_CAN2;

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

		osThreadId id_CAN_R;
		osThreadId id_CAN_T1;
		osThreadId id_CAN_T2;
		osThreadId id_CAN_T;

		//osMutexId mid_Thread_Mutex;                                     // mutex id
		//osMutexDef (SampleMutex);                                       // mutex name definition

typedef struct {                                                // object data type
  uint8_t ID[2];
	uint8_t data[4];
	uint32_t len;
	
} valeur_ID;

		osMailQId ID_BAL;
		osMailQDef(BAL1,MAILQUEUE_OBJECTS,valeur_ID);


void InitCan1 (void);
void InitCan2(void);
void envoi_CAN( short ID[], char data[], uint32_t len);
void recep_CAN(void /*const *argument*/);
void myCAN1_callback1(uint32_t obj_idx, uint32_t event);
void myCAN2_callback(uint32_t obj_idx, uint32_t event);
void CAN_R(void const *argument);
void CAN_T(void const *argument);
void send_udp_data (char taille, char DATANET[],char NETIP[] );

osThreadDef(CAN_R,osPriorityNormal, 1,0);
osThreadDef(CAN_T,osPriorityNormal, 1,0);

int32_t udp_sock;      // UDP socket handle

// Fonction Callback, CAN2 utilisé pour réception
void myCAN2_callback(uint32_t obj_idx, uint32_t event)
{
    switch (event)
    {
    case ARM_CAN_EVENT_RECEIVE:
        /*  Message was received successfully by the obj_idx object. */
       osSignalSet(id_CAN_R, 0x01);// Envoie 0x01 à ID_CAN_R
        break;
    }
}


// Fonction Callback, CAN1 utilisé pour émission
void myCAN1_callback(uint32_t obj_idx, uint32_t event)
{
    switch (event)
    {
    case ARM_CAN_EVENT_SEND_COMPLETE:
        /* 	Message was sent successfully by the obj_idx object.  */
     osSignalSet(id_CAN_T, 0x02);// Envoie 0x02 à ID_CAN_T
     break;
   }
}


// Notify the user application about UDP socket events.
uint32_t udp_cb_func (int32_t socket, const  NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
  
  // Data received
  char data[20],chaine_data[20],chaine_data1[20]; // déclaration deux chaines de caractère
	char ID[2]; // déclaration tableau ID
	int i;
	valeur_ID *pMail = 0; 
	 
	LED_On(7);
	

	for (i=0; i<4;i++)
	{
    data[i]=buf[2+i];	// mise en place de la data Ethernet dans data

	sprintf(chaine_data,"%02x",data[i]); // affichage de la valeur de la data

	GLCD_DrawString (40*i,30, chaine_data); 
	}
		for (i=0; i<2;i++)
	{
    ID[i]=buf[i];	// Mise en place de la valeur de l'ID
	}
	
	pMail = osMailAlloc (ID_BAL, osWaitForever);         // Allocate memory
    if (pMail) {
      pMail->ID[0] = ID[0];                                  // Set the mail content
      pMail->ID[1] = ID[1];
			pMail->data[0] = data[0];
			pMail->data[1] = data[1]; 
			pMail->data[2] = data[2]; 
			pMail->data[3] = data[3];
			pMail-> len = len;
      osMailPut (ID_BAL, pMail);                         // Send Mail
    }
			LED_Off(7);
			LED_Off(6);
		
  return (0);
}
 
// Allocate and initialize the socket.
int main (void) 
{
	osKernelInitialize ();                    // initialize CMSIS-RTOS
  LED_Initialize();                  // Initialisation des LED	
  netInitialize ();                  // Initialisation de l'ethernet
  InitCan1();                   // Initialisation CAN1
	InitCan2();                   // Initialisation CAN2

	GLCD_Initialize();            // Initialisation GLCD
	GLCD_ClearScreen();
	GLCD_SetFont (&GLCD_Font_16x24);
	GLCD_DrawString (1,3,"Recu");       // affichage de "Reçu"
	
  // Initialize UDP socket and open port 2000
  udp_sock = netUDP_GetSocket (udp_cb_func);     // ouverture socket
	
  if (udp_sock > 0) 
	{
    netUDP_Open (udp_sock, 5555);   // utilisation port 5555
  }
	
  id_CAN_R = osThreadCreate (osThread(CAN_R), NULL);          // Creation tache Reception CAN
	id_CAN_T = osThreadCreate (osThread(CAN_T), NULL);          // Creation tache Transmission CAN
	
	
	ID_BAL = osMailCreate(osMailQ(BAL1),NULL);
	
	
	osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);

}


void CAN_R(void const *argument)     // Tache Reception CAN
{
	ARM_CAN_MSG_INFO   rx_msg_info;         
	uint8_t data_buf[8];   // Declaration tableau data_buf en char correspondant à la data reçu du CAN_T
	char NETIP[4];    //  Declaration tableau NETIP correspondant à l'IP de la trame Ethernet récupérée
	char DATANET[4];  //  Declaration tableau DATANET correspondant à la DATA de la trame Ethernet récupérée
	char texte1[30],texte2[30];  //  Declaration chaine de caractère texte1 et texte2
	int identifiant,i,j;
	char retour;
	char taille;

		while(1)           // boucle car tâche
{			GLCD_DrawString (1,3,"Recu");  // Affichage de "reçu" pour vérification du bon fonctionnement de la tache

	osSignalWait(0x01, osWaitForever);  // Mise en sommeil de la tache jusqu'à verification que la tache ait bien reçu une trame

	Driver_CAN1.MessageRead(0,&rx_msg_info, data_buf,8);  // Lecture valeur de cette trame 
   retour = data_buf[0];     // mise de la valeur du premier octet dans " retour"
		
		taille= rx_msg_info.dlc;   // taille en octet de la data   
		
		if ( rx_msg_info.id == 0x161) // verification si identifiant est bien 161
		 {
			for (i=0; i<4 ;i++) // adresse IP trame ethernet
		 {
			NETIP[i]=data_buf[i];
		 }
		 sprintf(texte1,"recu IP %x,%x,%x,%x",NETIP[0],NETIP[1],NETIP[2],NETIP[3]);
		 GLCD_DrawString (1, 1, texte1);   // affichage du tableau NETIP pour vérification des bonnes valeurs
		 
for (j=4; j<8;j++) // data trame ethernet
		 {
			DATANET[j-4]=data_buf[j];
	   }
		 		 sprintf(texte2,"recu DATA %x,%x,%x,%x",DATANET[0],DATANET[1],DATANET[2],DATANET[3]);
		 GLCD_DrawString (1, 200, texte2);    // affichage du tableau DATANET pour vérification des bonnes valeurs
		
		send_udp_data(taille,DATANET,NETIP); // appele fonction envoie Ethernet avec comme argument la taille, la data ethernet et l'IP
	   
	 	osDelay(100); // attente 100 ms
     }
	 	osDelay(5000); // attente 5 secondes
}
}


void CAN_T(void const *argument)
{
	ARM_CAN_MSG_INFO                tx_msg_info;

	uint8_t data_buf[4];   // Declaration tableau data_buf en char correspondant à la data à envoyer au CAN de reception
	char chaineID[20];     //  Declaration chaine de caractere ID 
	char chaineDATA[20];   //  Declaration chaine de caractere DATA
	char i; 
	short id;

	valeur_ID  *pMail = 0; 
  osEvent           evt;
	
	
while (1)
{     
	
//osMutexWait(mid_Thread_Mutex, osWaitForever);
	
		LED_On(2);  // allumage LED 2
	
	evt = osMailGet (ID_BAL, osWaitForever);             // wait for mail
    if (evt.status == osEventMail) {    
      pMail = evt.value.p;
      // process data
			for ( i=0;i<4;i++)
			{
				data_buf[i] = pMail->data[i];   // stocker la data utile ethernet dans data_buf
			}
			id = (short)(pMail->ID[0]<<8|pMail->ID[1]);  // stocker l'ID CAN dans id
        
			}
		osMailFree (ID_BAL, pMail); // libérer la BAL
			
		tx_msg_info.id = ARM_CAN_STANDARD_ID(id); 
		tx_msg_info.rtr = 0;
		LED_On(3);
	
	
	
		sprintf(chaineDATA,"%02x,%02x,%02x,%02x",data_buf[0],data_buf[1],data_buf[2],data_buf[3]);
		GLCD_DrawString(0,100, chaineDATA);
									 // osMutexRelease(mid_Thread_Mutex);
			LED_On(4);	

	sprintf(chaineID,"ID = %04x ",id);
	GLCD_DrawString(0,0, chaineID);
	
	//	sprintf(chaineID," identifiant : %04x",ID);
//	GLCD_DrawString (0,0, chaineID);
	
	
			LED_Off(2);
			LED_Off(3);

	Driver_CAN1.MessageSend(1,&tx_msg_info,data_buf,4);  // envoi CAN de 4 octets
  osSignalWait(0x02, osWaitForever);                   // mis en sommeil jusqu'à réveil, par callback CAN
	LED_Off(4);

		 		osDelay(1000);

}
}


void InitCan1 (void) {
	Driver_CAN1.Initialize(NULL,myCAN1_callback);
	Driver_CAN1.PowerControl(ARM_POWER_FULL);
	
	Driver_CAN1.SetMode(ARM_CAN_MODE_INITIALIZATION);
	Driver_CAN1.SetBitrate( ARM_CAN_BITRATE_NOMINAL,
													125000,
													ARM_CAN_BIT_PROP_SEG(5U)   |         // Set propagation segment to 5 time quanta
                          ARM_CAN_BIT_PHASE_SEG1(1U) |         // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
                          ARM_CAN_BIT_PHASE_SEG2(1U) |         // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
                          ARM_CAN_BIT_SJW(1U));                // Resynchronization jump width is same as phase segment 2
                          
	// Mettre ici les filtres ID de réception sur objet 0
	//....................................................
	Driver_CAN1.ObjectSetFilter(0,
		                           ARM_CAN_FILTER_ID_EXACT_ADD ,
		                           ARM_CAN_STANDARD_ID(0x161),
		                           0);
	//Driver_CAN1.ObjectConfigure(0,ARM_CAN_OBJ_RX);				// Objet 0 du CAN1 pour réception
	Driver_CAN1.ObjectConfigure(1,ARM_CAN_OBJ_TX);				// Objet 0 du CAN1 pour réception
	
	Driver_CAN1.SetMode(ARM_CAN_MODE_NORMAL);					// fin init
}

void InitCan2 (void) {
	Driver_CAN2.Initialize(NULL,myCAN2_callback);
	Driver_CAN2.PowerControl(ARM_POWER_FULL);
	
	Driver_CAN2.SetMode(ARM_CAN_MODE_INITIALIZATION);
	Driver_CAN2.SetBitrate( ARM_CAN_BITRATE_NOMINAL,
													125000,
													ARM_CAN_BIT_PROP_SEG(5U)   |         // Set propagation segment to 5 time quanta
                          ARM_CAN_BIT_PHASE_SEG1(1U) |         // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
                          ARM_CAN_BIT_PHASE_SEG2(1U) |         // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
                          ARM_CAN_BIT_SJW(1U));                // Resynchronization jump width is same as phase segment 2
                          
	// Mettre ici les filtres ID de réception sur objet 0
	//....................................................
		
	//Driver_CAN2.ObjectConfigure(1,ARM_CAN_OBJ_TX);				// Objet 0 du CAN1 pour réception
	Driver_CAN2.ObjectConfigure(0,ARM_CAN_OBJ_RX);				// Objet 0 du CAN1 pour réception
	
	Driver_CAN2.SetMode(ARM_CAN_MODE_NORMAL);					// fin init
}


// Send UDP data to destination client.
void send_udp_data (char taille, char DATANET[],char NETIP[] ) {
 
		int i;
	int taille_data;  
 char chainetest[30],chainetest1[30]; // declaration chaine de caractere
	
			
	
  if (udp_sock > 0) {
    //IPv4 address: 192.168.0.194
NET_ADDR addr = { NET_ADDR_IP4, 2098, 0, 0, 0, 0};  	
	
		 
		uint8_t *sendbuf; // declaration pointeur type char sendbuff
		addr.addr[0] = NETIP[0];  // affectation dans la structure de l'adresse IP
		addr.addr[1] = NETIP[1];
		addr.addr[2] = NETIP[2];
		addr.addr[3] = NETIP[3];


		sprintf(chainetest,"recu %d,%d,%d,%d",addr.addr[0],addr.addr[1],addr.addr[2],addr.addr[3]);
    GLCD_DrawString (1, 50, chainetest); // affichage valeur de l'adresse IP en avec la structure

		taille_data = taille - 4; // definition taille data en fonction de la taille de la trame CAN
   
		
		sendbuf = netUDP_GetBuffer (taille_data); //
    for (i=0;i<taille_data;i++) //  sendbuf prend la valeur de la DATA de l'ethernet
		{
		sendbuf[i] = DATANET[i];

		}			

				sprintf(chainetest1,"recu %x,%x,%x,%x",sendbuf[0],sendbuf[1],sendbuf[2],sendbuf[3]); 
		
				GLCD_DrawString (1, 125, chainetest1); // affichage chaintest1

				netUDP_Send (udp_sock, &addr, sendbuf, 4); // envoi de 4 octet de type UDP
}
}


	// code_it2r;