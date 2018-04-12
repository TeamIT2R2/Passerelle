/*---------------------------------------------------
* CAN 2 uniquement en TX 
* + réception CAN1 
* avec RTOS et utilisation des fonction CB
* pour test sur 1 carte -> relier CAN1 et CAN2
* 2017-04-02 - XM
---------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module


//#include "LPC17xx.h"                    // Device header
//#include "Driver_CAN.h"                 // ::CMSIS Driver:CAN
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
//#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
//#include "stdio.h"
//#include "cmsis_os.h"

//extern GLCD_FONT GLCD_Font_6x8;
//extern GLCD_FONT GLCD_Font_16x24;

//osThreadId id_CANthreadR;
//osThreadId id_CANthreadT;
//osThreadId id_CAN_R;
//osThreadId id_CAN_T;

//extern   ARM_DRIVER_CAN         Driver_CAN1;
//extern   ARM_DRIVER_CAN         Driver_CAN2;
///*
//// CAN1 utilisé pour réception
//void myCAN1_callback(uint32_t obj_idx, uint32_t event)
//{
//    switch (event)
//    {
//    case ARM_CAN_EVENT_RECEIVE:
//        /*  Message was received successfully by the obj_idx object. */
//       osSignalSet(id_CAN_R, 0x01);
//        break;
//    }
//}

//// CAN2 utilisé pour émission
//void myCAN2_callback(uint32_t obj_idx, uint32_t event)
//{
//    switch (event)
//    {
//    case ARM_CAN_EVENT_SEND_COMPLETE:
//        /* 	Message was sent successfully by the obj_idx object.  */
//     osSignalSet(id_CAN_T, 0x02);
//     break;
//   }
//}

//// CAN1 utilisé pour réception
//void InitCan1 (void) {
//	Driver_CAN1.Initialize(NULL,myCAN1_callback);
//	Driver_CAN1.PowerControl(ARM_POWER_FULL);
//	
//	Driver_CAN1.SetMode(ARM_CAN_MODE_INITIALIZATION);
//	Driver_CAN1.SetBitrate( ARM_CAN_BITRATE_NOMINAL,
//													125000,
//													ARM_CAN_BIT_PROP_SEG(5U)   |         // Set propagation segment to 5 time quanta
//                          ARM_CAN_BIT_PHASE_SEG1(1U) |         // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
//                          ARM_CAN_BIT_PHASE_SEG2(1U) |         // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
//                          ARM_CAN_BIT_SJW(1U));                // Resynchronization jump width is same as phase segment 2
//                          
//	// Mettre ici les filtres ID de réception sur objet 0
//	//....................................................
//	Driver_CAN1.ObjectSetFilter(0,
//		                           ARM_CAN_FILTER_ID_EXACT_ADD ,
//		                           ARM_CAN_STANDARD_ID(0x161),
//		                           0);
//	Driver_CAN1.ObjectConfigure(0,ARM_CAN_OBJ_RX);				// Objet 0 du CAN1 pour réception
//	
//	Driver_CAN1.SetMode(ARM_CAN_MODE_NORMAL);					// fin init
//}

//void InitCan2 (void) {
//	Driver_CAN2.Initialize(NULL,myCAN2_callback);
//	Driver_CAN2.PowerControl(ARM_POWER_FULL);
//	
//	Driver_CAN2.SetMode(ARM_CAN_MODE_INITIALIZATION);
//	Driver_CAN2.SetBitrate( ARM_CAN_BITRATE_NOMINAL,
//													125000,
//													ARM_CAN_BIT_PROP_SEG(5U)   |         // Set propagation segment to 5 time quanta
//                          ARM_CAN_BIT_PHASE_SEG1(1U) |         // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
//                          ARM_CAN_BIT_PHASE_SEG2(1U) |         // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
//                          ARM_CAN_BIT_SJW(1U));                // Resynchronization jump width is same as phase segment 2
//                          
//	// Mettre ici les filtres ID de réception sur objet 0
//	//....................................................
//		
//	Driver_CAN2.ObjectConfigure(1,ARM_CAN_OBJ_TX);				// Objet 0 du CAN1 pour réception
//	
//	Driver_CAN2.SetMode(ARM_CAN_MODE_NORMAL);					// fin init
//}
///*
// * main: initialize and start the system
// */
//int main1 (void) {
//  osKernelInitialize ();                    // initialize CMSIS-RTOS

//  // initialize peripherals here
//	GLCD_Initialize();
//	GLCD_ClearScreen();
//	GLCD_SetFont(&GLCD_Font_16x24);
//	
//	// Initialisation des 2 périphériques CAN
//	InitCan1();
//	InitCan2();

//  // create 'thread' functions that start executing,
//  // example: tid_name = osThreadCreate (osThread(name), NULL);
//	
///*	id_CANthreadR = osThreadCreate (osThread(CANthreadR), NULL);
//	id_CANthreadT = osThreadCreate (osThread(CANthreadT), NULL);

//  osKernelStart ();                         // start thread execution 
//	osDelay(osWaitForever);*/
//}





