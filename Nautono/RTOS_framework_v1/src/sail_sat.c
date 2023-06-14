/**
 * @file sail_sat.c
 * @author Matthew Cockburn
 * @brief This file contains the source code to send and receive data from
 * MetOcean Stream 211 Beacon.
 * @version 0.1
 * @date 2023-01-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/* Standard Headers */
#include <stdarg.h>
#include <stdio.h>

/* FreeRTOS Headers */
#include "FreeRTOS.h"
#include "task.h"

/* Sailboat Headers */
#include "sail_sat.h"
#include "sail_uart.h"
#include "sail_debug.h"

#include "status_codes.h"
#include <delay.h>

#define UART_SAT UART_RADIO 


//bool init_flag = false;

enum status_code Satellite_Init(void){

   // if(init_flag) return STATUS_ERR_ALREADY_INITIALIZED;
	
    return UART_Init(UART_SAT);
}

void TestSatellite(void){
	
	Satellite_Init();
	UART_Enable(UART_SAT);
	
	
	uint8_t counter = 0;
	uint8_t msg[100];
	
	msg[0] = '\0';
	
	//memset(msg, '\0', 100);
	
	while(1){
		DEBUG_Write("<<<<<<<<<< Test Beacon Task >>>>>>>>>>\n\r");
		//ConstructSBD_MSG(msg,100,"%X",counter++);
		
		UART_TxString(UART_SAT, "AT");
		
		delay_s(1);
		
		UART_RxString(UART_SAT, msg, 100);
		
		
		DEBUG_Write(msg);
		//SatelliteWrite(msg);
		
		vTaskDelay(1000);
	}
	
}

uint8_t ConstructSBD_MSG(uint8_t * msg, uint8_t size, const char * format, ...){
	
	uint8_t buffer[50];
	
	va_list args;
	va_start(args,format);
	vsnprintf(buffer,size,format,args);
	va_end(args);
	
	/*
	 * snprintf args: 
		* char* string (buffer to write to)
		* size
		* format
		* ...: optional arguments
	*/
	
	snprintf(msg,"AT+SWGT%s/n",buffer, size + 20);
	
	return 0;
}

 