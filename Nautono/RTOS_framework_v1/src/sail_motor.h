/* sail_motor.h
 * Header file for the motor controller for the autonomous sailboat project.
 * Created on Thomas Gwynne-Timothy.
 * Created by August 16, 2016.
 */

#ifndef SAIL_MOTOR_H_
#define SAIL_MOTOR_H_

#include <asf.h>

typedef enum MOTOR_ChannelIDs {
	MOTOR_SAIL,
	MOTOR_RUDDER,
	MOTOR_NUM_CHANNELS
} MOTOR_ChannelID;


/* MOTOR_Init
 * Initialize motor
 * Status:
 *   STATUS_OK - Motor initialization was successful
 */
enum status_code MOTOR_Init(void);


/* MOTOR_SetSail
 * Set sail degree
 * Input:
 *	 angle - original sail angle value
 * Status:
 *	 STATUS_OK - Successfully set sail angle
 */
enum status_code MOTOR_SetSail(double angle);


/* MOTOR_SetRudder
 * Set rudder degree
 * Input:
 *	 angle - original rudder angle value
 * Status:
 *	 STATUS_OK - Successfully set rudder angle
 */
enum status_code MOTOR_SetRudder(double angle);

void Test_Rudder(void);
uint8_t set_pos(double pos, uint16_t timeout);
void TurnOff(MOTOR_ChannelID id);
#endif // SAIL_MOTOR_H_