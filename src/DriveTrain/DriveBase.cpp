/*
 * DriveBase.cpp
 *
 *  Created on: Jul 18, 2015
 *      Author: nlele
 */

#include <DriveTrain/DriveBase.h>

#include <iterator>

DriveBase::~DriveBase() = default;

void DriveBase::setAll(double speed)
{
	for (auto& it: all_motors)
	{
		if(it.second._side == RIGHT)
		{
			speed = -speed;
		}
		it.second->setSpeed(speed);
	}
}

void DriveBase::setIndividual(int port, double speed)
{
	all_motors[port]->setSpeed(speed);
}

void DriveBase::setSide(Side side, double speed)
{
	for(auto& it : all_motors)
	{
		if(it.second._side == RIGHT)
		{
			speed = -speed;
		}
		if(it.second._side == side)
		{
			it.second->setSpeed(speed);
		}
	}
}

void DriveBase::kill()
{
	//never starts again
	//like when you kill something
	for(auto& it : all_motors)
	{
		it.second->disable();
	}
}
