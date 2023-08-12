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
#include "sail_debug.h"
#include "sail_nmea.h"
#include "port.h"
#include "sail_actuator.h"
#include "sail_motor.h"

// Local Prototypes
uint8_t Radio_Receive(void);
void GPS_Receive(void);
void WIND_Receive(void);
void IMU_Receive(void);
void Angle_Receive(void);
void Radio_Transmit(void);


static GPS_Reading gps;
static WIND_Reading wind;
static COMP_Reading imu;
uint16_t Sail_Angle;

uint8_t imu_calib_flag;

uint8_t gps_data_flag;
uint8_t imu_data_flag;
uint8_t wind_data_flag;

#define TEST_CONFIG_2 

const float angle_conversion_factor = 360.0 / 4096;

void Remote_Controller(void * params){
	
	Sail_Angle = 0;
	
	imu_calib_flag = 0;
	
	gps_data_flag = 0;
	imu_data_flag = 0;
	wind_data_flag = 0;
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);

	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;

	port_pin_set_config(PIN_PA25, &config_port_pin);
	//GPS_On();
	
	// Run the Actuator test
	setActuator(45);
	vTaskDelay(pdMS_TO_TICKS(5000));
	
		
	DEBUG_Write("\n\r***** Starting Remote_Controller Task *****\n\r");
	while(1)
	{
		
		/* Parse Incoming Radio Commands */
		if(Radio_Receive()){
			TurnOff(MOTOR_RUDDER);
			vTaskDelay(1000);
			
		}else{
			TurnOff(MOTOR_RUDDER);
			vTaskDelay(100);
		}
	    #ifndef TEST_CONFIG_2	
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
		#endif
		/* end of loop */
		// Blink LED
		//port_pin_toggle_output_level(PIN_PA25);
		
	}	
}



/* Private Implementation */

uint8_t Radio_Receive(void){
	
	RADIO_GenericMsg rx_msg;
	uint8_t ret = 0;
	switch (RADIO_RxMsg(&rx_msg)) {
		case STATUS_OK:
		DEBUG_Write_Unprotected("Received a message!\r\n");
		ret = HandleMessage(&rx_msg);
		break;
		
		case STATUS_ERR_BAD_DATA:
		DEBUG_Write("Received a corrupt message!\r\n");
		RADIO_Ack(RADIO_STATUS_ERROR);
		break;
		
		default:
		break;
	}
	return ret;
}

void GPS_Receive(void){
	
	NMEA_GenericMsg  msg;
	
	if (GPS_RxMsg(&msg) == STATUS_VALID_DATA) {
		if(msg.type == eGPGGA)
			gps.lat =  msg.fields.gpgga.lat.lat;
			gps.lon =  msg.fields.gpgga.lon.lon;
			gps_data_flag = 1;
			DEBUG_Write("LAT: %x | LON: %x \n\r",gps.lat, gps.lon);
	}
	return;
}

void WIND_Receive(void){
	
	NMEA_GenericMsg  msg;
	
	if(STATUS_VALID_DATA == WIND_RxMsg(&msg)){
		if(msg.type == eIIMWV){
			wind.speed = msg.fields.wimwv.wind_dir_rel;
			wind.angle = msg.fields.wimwv.wind_speed_ms;
			wind_data_flag = 1;
			DEBUG_Write("Wind Speed: %x | Wind Dir: %x \n\r",wind.speed, wind.angle);
		}
	}
	return;
}

void IMU_Receive(void){
		
	if(!imu_calib_flag){
		setMode(OPERATION_MODE_NDOF);
		if(isFullyCalibrated()){
			imu_calib_flag = 1;
			return;
		}
	}else{
		getHeading(&imu);
		imu_data_flag = 1;
	}
	return;
}

void Angle_Receive(void){
	
	uint16_t raw_angle = 0;
	
	rawAngle(&raw_angle);
	Sail_Angle = raw_angle * angle_conversion_factor;
	
	return;
}

void Radio_Transmit(void){
	
	RADIO_GenericMsg tx_msg_log;
			
	// Log the GPS coordinates
	if(gps_data_flag){
		tx_msg_log.type = RADIO_GPS;
		tx_msg_log.fields.gps.data = gps;
		RADIO_TxMsg(&tx_msg_log);
		gps_data_flag = 0;
	}
	
	
	// Log the wind speed and direction
	if(wind_data_flag){
		tx_msg_log.type = RADIO_WIND;
		tx_msg_log.fields.wind.data = wind;
		RADIO_TxMsg(&tx_msg_log);
		wind_data_flag = 0;	
	}

	if(imu_data_flag){
		// Log the compass data
		tx_msg_log.type = RADIO_COMP;
		tx_msg_log.fields.comp.data = imu;
		RADIO_TxMsg(&tx_msg_log);
		imu_data_flag = 0;
	}
}
