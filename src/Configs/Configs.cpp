/*
 * Configs.cpp
 *
 *  Created on: Sep 5, 2015
 *      Author: norayu
 */

#include <Configs/Configs.h>

//milli seconds
const double Configs::ACCEL_TIME = 175.0;

//in feet
const double Configs::ZERO_THROTTLE_THRESHOLD = 0.0;

//from 0 - 1
const double Configs::THROTTLE_CUTOFF = .01;

//it's a scalar
const double Configs::THROTTLE_MULTIPLIER = 0.5;

//in ft
const double Configs::MAX_TURN_RADIUS = 1.0;

//in ft
const double Configs::ROBOT_WIDTH = 2.0;

//CONTROLLER PORTS
const double Configs::DRIVE_CONTROLLER_PORT = 0;

const int Configs::PANIC_BIND = 11;
