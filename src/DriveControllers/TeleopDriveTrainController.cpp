/*
 * TeleopDriveTrainController.cpp
 *
 *  Created on: Jul 18, 2015
 *      Author: nlele
 */

#include <DriveControllers/TeleopDriveTrainController.h>

#include "User Controls/UserController.h"
#include "Configs/Configs.h"
#include "Logging/Logger.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <cassert>

namespace
{
	class FindThrottle
	{
	private:
		typedef std::chrono::high_resolution_clock clock;
		float _prev_throttle = 0;
		auto _prev_time = clock::now;
	public:
		double findThrottle(double throttle)
		{
			//stores last throttle!

			//clock returns clock ticks since epoch
			//in seconds
			auto current_time = clock::now;

			if(Configs::ACCEL_TIME > 0)
			{
				double accel = (throttle - _prev_throttle) / Configs::ACCEL_TIME;
				double delta_time = std::chrono::duration_cast<std::chrono::duration<double> >(current_time - _prev_time);

				constexpr double MILLI_TO_SEC = 1000;

				delta_time *= MILLI_TO_SEC;

				std::cout << "Delta time is " << delta_time << " ms";

				double delta_throttle = delta_time * accel;

				throttle = _prev_throttle + delta_throttle;
			}

			//sets prev time to current one
			_prev_time = current_time;

			//sets prev throttle to current one
			_prev_throttle = throttle;

			return (std::abs(throttle) <= Configs::THROTTLE_CUTOFF) ? 0.0 : throttle;
		}
	};
}

TeleopDriveTrainController::TeleopDriveTrainController(UserController* controller, DriveBase* drivebase, DriveType drivetype) :
		p_user_controller(controller), p_drive_base(drivebase), current_type(drivetype)
{
}

TeleopDriveTrainController::~TeleopDriveTrainController() = default;

void TeleopDriveTrainController::update()
{
	double controller_turn = p_user_controller->getRightXAxis();
	double controller_throttle = -p_user_controller->getLeftYAxis();
	//                           ^: For some reason, the Y axis is inverted


	//moved it out of joystick into here
	if(std::abs(controller_turn) <= Configs::ZERO_THROTTLE_THRESHOLD)
	{
		controller_turn = 0;
	}

	if(std::abs(controller_throttle) <= Configs::ZERO_THROTTLE_THRESHOLD)
	{
		controller_throttle = 0;
	}

	std::cout << "CONTROLLER TURN: " << controller_turn << std::endl;
	std::cout << "CONTROLLER THROT: " << controller_throttle << std::endl;

	static FindThrottle ft_turn{};
	static FindThrottle ft_throt{};

	//is l side?
	double throttle = ft_throt.findThrottle(controller_throttle * Configs::THROTTLE_MULTIPLIER);
	double turn = ft_turn.findThrottle(controller_turn * Configs::THROTTLE_MULTIPLIER);

	std::cout << "CALC THROT: " << throttle << std::endl;
	std::cout << "CALC TURN: " << turn << std::endl;

	assert(throttle >= -1 && throttle <= 1);
	assert(turn >= -1 && turn <= 1);

	if(throttle != 0 && turn != 0)
	{
		setTurn(throttle, turn);
	}
	else if(throttle != 0 && turn == 0)
	{
		setStraight(throttle);
	}
	else if(throttle == 0 && turn != 0)
	{
		turnInPlace(turn);
	}
	else
	{
		const double STOP = 0.0;
		setStraight(STOP);
	}

	auto getTime = []()
		{
		time_t t;
		struct tm * timeinfo;
		time(&t);
		timeinfo = localtime(&t);
		return timeinfo;
		};

	Logger::addToQueue(asctime(getTime() ) );
	Logger::addToQueue("Left Side: " + std::to_string(throttle) );
	Logger::addToQueue("Right Side: " + std::to_string(turn) );
}

void TeleopDriveTrainController::setStraight(double speed)
{
	if(current_type == DriveType::TANK)
	{
		p_drive_base->setAll(speed);
	}
}

void TeleopDriveTrainController::turnInPlace(double speed)
{
	if(current_type == DriveType::TANK)
	{
		p_drive_base->setSide(LEFT, speed);
		p_drive_base->setSide(RIGHT, -speed);
	}
}

void TeleopDriveTrainController::setTurn(double throttle, double turn)
{
	auto calcInsideWheelSpeed = [](double outside_speed, double turn_steepness)
		{
			double turn_radius = Configs::MAX_TURN_RADIUS - (Configs::MAX_TURN_RADIUS * turn_steepness);

			return outside_speed * (turn_radius / (turn_radius + Configs::ROBOT_WIDTH) );
		};
	if(current_type == DriveType::TANK)
	{
		double l_side;
		double r_side;

		if(turn > 0)
		{
			l_side = throttle;
			r_side = calcInsideWheelSpeed(throttle, turn);
		}
		else
		{
			l_side = calcInsideWheelSpeed(throttle, turn);
			r_side = throttle;
		}

		//TODO: IMPLEMENT REVERSE TURNING

		std::cout << r_side << std::endl;

		assert(l_side >= -1 && l_side <= 1);
		assert(r_side >= -1 && r_side <= 1);

		p_drive_base->setSide(LEFT, l_side);
		p_drive_base->setSide(RIGHT, r_side);
	}
}
