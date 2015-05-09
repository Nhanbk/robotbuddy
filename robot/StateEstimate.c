//
//  StateEstimate.c
//  robot
//
//  Created by Tarun Chaudhry on 5/6/15.
//  Copyright (c) 2015 Tarun Chaudhry. All rights reserved.
//

#include <stdint.h>
#include <math.h>

#include "StateEstimate.h"
#include "UART.h"
#include "Config.h"
#include "MSP430_MPU6050.h"


// Holds the raw accelerometer data.
int16_t ax=0;
int16_t ay=0;
int16_t az=0;


// Holds the raw gyroscope data.
int16_t gx, gy, gz;

float ax_bias=0;
float az_bias=0;
float gy_bias=0;
float atan2_bias=0;

float accel_x, accel_z;

float angleA, angleG;

float angleEstimate = 0;

#define BURNIN 1000
#define BURNIN_DATA 1.0/BURNIN

float getAngularAcceleration();

void burnin(){
	int i=BURNIN;
    P1DIR |= BIT0;
    P1OUT |= BIT0;
	while(i--){
		if (i%100==0) {
			P1OUT^=BIT0;
		}
		getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
		//ax_bias += (float)ax*BURNIN_DATA;
		//az_bias += (float)az*BURNIN_DATA;
		gy_bias += (float)gy*BURNIN_DATA;
		atan2_bias += atan2(ax, az) * BURNIN_DATA;
	}
	P1OUT &= ~BIT0;
}
float getAngleEstimate() {
    getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
//    UART_out_bytes((char*)&ax,2);
//    UART_out_bytes((char*)&ay,2);
//    UART_out_bytes((char*)&az,2);
//    UART_out_bytes((char*)&gy,2);
    // Get Gyro Value
    angleG = getAngularVelocity();
//    UART_out_bytes((char*)&angleG,8);

    angleA = getAngularAcceleration();
//    UART_out_bytes((char*)&angleA,8);

    // State estimation
    angleEstimate = HPF * (angleEstimate + angleG * DT) + LPF * angleA;
//    UART_out_bytes((char*)&angleEstimate,8);

//    UART_out_float(accel_x);
//    UART_out_float(accel_y);
//    UART_out_float(accel_z);
//    UART_out_float(pitch);
//    UART_out_float(gyro);

    return angleEstimate;
}

// Fixing Gyro values
float getAngularVelocity() {
    return ((float)(gy)-gy_bias)*GYRO_RANGE;
}



float getAngularAcceleration() {
    // Fixing Accel values
    accel_x = (((float)(ax) - ax_bias));
    accel_z = (((float)(az) - az_bias));
    return atan2(accel_x, accel_z) - atan2_bias;
    //return accel_x;
}
