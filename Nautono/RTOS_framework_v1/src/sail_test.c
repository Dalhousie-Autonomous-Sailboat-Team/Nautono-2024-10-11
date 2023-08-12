/*
 * sail_test.c
 *
 * Created: 2023-08-11 7:57:41 PM
 *  Author: Matthew Cockburn
 */ 

/* Headers */

#include "sail_radio.h"
#include "sail_gps.h"
#include "sail_wind.h"
#include "sail_imu.h"
#include "sail_anglesensor.h"

// Local Prototypes
static void Radio_Receive(void);
static void GPS_Receive(void);
static void WIND_Receive(void);
static void IMU_Receive(void);
static void Angle_Receive(void);
static void Radio_Transmit(void);

static GPS_Reading gps;
static WIND_Reading wind;
static COMP_Reading comp;

void Remote_Controller(void * params){
	
	while(1)
	{
		
		/* Parse Incoming Radio Commands */
		RadioReceive();		
		
		
		/* Get GPS Data */
		GPS_Receive();
		
		/* Get Wind Vane Data */
		WIND_Receive();
				
		/* Get IMU Data */
		IMU_Receive();
		
		/* Get Angle Sensor Data */
		Angle_Receive();
		
		/* Write Collected Data to Radio */
		Radio_Transmit();
		
		/* end of loop */
		// Blink LED
		
			
	}	
}



/* Private Implementation */

static void RadioReceive(void){
	
	RADIO_GenericMsg rx_msg;
	
	switch (RADIO_RxMsg(&rx_msg)) {
		case STATUS_OK:
		DEBUG_Write("Received a message!\r\n");
		HandleMessage(&rx_msg);
		break;
		
		case STATUS_ERR_BAD_DATA:
		DEBUG_Write("Received a corrupt message!\r\n");
		RADIO_Ack(RADIO_STATUS_ERROR);
		break;
		
		default:
		break;
	}
}

static void GPS_Receive(void){
	
	NMEA_GenericMsg  msg;
	
	if (GPS_RxMsg(&msg) == STATUS_OK) {
		if(msg.type == eNMEA_GPGGA)
			gps.lat =  msg.fields.gpgga.lat.lat;
			gps.lon =  msg.fields.gpgga.lon.lon;
	}
	return;
	
}

static void WIND_Receive(void){
	
	NMEA_GenericMsg  msg;
	
	if(STATUS_VALID_DATA == WIND_RxMsg(&msg)){
		if(msg.type == eIIMWV){
			wind.speed = msg.fields.wimwv.wind_dir_rel;
			wind.angle = msg.fields.wimwv.wind_speed_ms;
		}
	}
	return;
}

static void IMU_Receive(void){
	
	
	
}

static void Angle_Receive(void){
	
}

static void Radio_Transmit(void){
	
}
