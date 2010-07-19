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

/**
 * Initializes a new timer with a set interval.
 * @param interval Time interval in miliseconds.
 */
Timer::Timer(Uint32 interval) : _interval(interval), _running(false)
{
}

/**
 *
 */
Timer::~Timer()
{
}

/**
 * Starts the timer running and counting time.
 */
void Timer::start()
{
	_start = SDL_GetTicks();
	_running = true;
}

/**
 * Stops the timer from running.
 */
void Timer::stop()
{
	_start = 0;
	_running = false;
}

/**
 * Returns the time passed since the last interval.
 * @return Time in miliseconds.
 */
Uint32 Timer::getTime()
{
	if (_running)
		return SDL_GetTicks() - _start;
	return 0;
}

/**
 * The timer keeps calculating the passed time while it's running,
 * calling the respective event handler whenever the set interval passes.
 * @param state State that the event handler belongs to.
 * @param surface Surface that the event handler belongs to.
 */
void Timer::think(State* state, Surface* surface)
{
	if (_running)
	{
		if (getTime() >= _interval)
		{
			_start = SDL_GetTicks();
			if (state != 0 && _state != 0)
				(state->*_state)();
			if (surface != 0 && _surface != 0)
				(surface->*_surface)();
		}
	}
}

/**
 * Changes the timer's interval to a new value.
 * @param interval Interval in miliseconds.
 */
void Timer::setInterval(Uint32 interval)
{
	_interval = interval;
}

/**
 * Sets a state function for the timer to call every interval.
 * @param handler Event handler.
 */
void Timer::onTimer(StateHandler handler)
{
	_state = handler;
}

/**
 * Sets a surface function for the timer to call every interval.
 * @param handler Event handler.
 */
void Timer::onTimer(SurfaceHandler handler)
{
	_surface = handler;
}
