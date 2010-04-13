/*
 * Copyright 2010 Daniel Albano
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Timer.h"

Timer::Timer(Uint32 interval) : _interval(interval), _running(false)
{
}

Timer::~Timer()
{
}

void Timer::start()
{
	_start = SDL_GetTicks();
	_running = true;
}

void Timer::stop()
{
	_start = 0;
	_running = false;
}

Uint32 Timer::getTime()
{
	if (_running)
		return SDL_GetTicks() - _start;
	return 0;
}

void Timer::think(State* state)
{
	if (_running)
	{
		if (getTime() >= _interval)
		{
			_start = SDL_GetTicks();
			if (_timer != 0)
				(state->*_timer)();
		}
	}
}

void Timer::setInterval(Uint32 interval)
{
	_interval = interval;
}

void Timer::onTimer(TimerHandler handler)
{
	_timer = handler;
}