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

#include "sail_sat.h"
#include "sail_uart.h"

/* Global Variables */
int init_flag = 0;

/* Local Prototypes*/


/* Exported Implemetations */

enum status_code Satellite_Init(void){

    if(init_flag) return STATUS_ERR_ALREADY_INITIALIZED;
    return UART_Init(UART_SATELLITE);

}

 