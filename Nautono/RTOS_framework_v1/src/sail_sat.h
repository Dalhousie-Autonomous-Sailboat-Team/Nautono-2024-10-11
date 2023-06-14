/* 
 * @file sail_sat.h
 * @author Matthew Cockburn 
 * @brief Header file for the satellite communication module.
 * @version 0.1
 * @date 2023-01-30
 */

#include <stdarg.h>

#ifndef SAIL_SAT_H_
#define SAIL_SAT_H_
/**
 * @brief Function to initialize the Beacon interface
 *
 * @param
 *
 * @return returns a status code
 */
extern enum status_code Satellite_Init(void);

extern void TestSatellite(void);

extern uint8_t ConstructSBD_MSG(uint8_t * msg, uint8_t size, const char * format, ...);


#endif /* SAIL_SAT_H_ */