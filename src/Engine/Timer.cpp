/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "Game.h"
#include "Options.h"
#include <assert.h>

namespace OpenXcom
{
	
const Uint32 accurate = 4;
Uint32 slowness = 1;
Uint32 slowTick()
{
	static Uint32 old_time = SDL_GetTicks();
	static Uint64 false_time = old_time << accurate;
	Uint64 new_time = ((Uint64)SDL_GetTicks()) << accurate;
	false_time += (new_time - old_time) / slowness;
	old_time = new_time;
	return false_time >> accurate;
}

int Timer::maxFrameSkip = 8; // this is a pretty good default at 60FPS. 
	
	
/**
 * Initializes a new timer with a set interval.
 * @param interval Time interval in milliseconds.
 */
Timer::Timer(Uint32 interval, bool frameSkipping) : _start(0), _interval(interval), _running(false), _state(0), _surface(0), _frameSkipping(frameSkipping)
{
	Timer::maxFrameSkip = Options::getInt("maxFrameSkip");
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
	_frameSkipStart = _start = slowTick();
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
 * @return Time in milliseconds.
 */
Uint32 Timer::getTime() const
{
	if (_running)
	{
		return slowTick() - _start;
	}
	return 0;
}

/**
 * Returns if the timer has been started.
 * @return Running state.
 */
bool Timer::isRunning() const
{
	return _running;
}

/**
 * The timer keeps calculating the passed time while it's running,
 * calling the respective action handler whenever the set interval passes.
 * @param state State that the action handler belongs to.
 * @param surface Surface that the action handler belongs to.
 */
void Timer::think(State* state, Surface* surface)
{
	Sint64 now = slowTick(); // must be signed to permit negative numbers
	Game *game = state ? state->_game : 0; // this is used to make sure we stop calling *_state on *state in the loop once *state has been popped and deallocated
	//assert(!game || game->isState(state));

	if (_running)
	{
		if ((now - _frameSkipStart) >= _interval)
		{
			for (int i = 0; i < maxFrameSkip && isRunning() && (now - _frameSkipStart) >= _interval; ++i)
			{
				if (state != 0 && _state != 0)
				{
					(state->*_state)();
				}
				_frameSkipStart += _interval;
				// breaking here after one iteration effectively returns this function to its old functionality:
				if (!game || !_frameSkipping || !game->isState(state)) break; // if game isn't set, we can't verify *state
			}
			
			if (_running && surface != 0 && _surface != 0)
			{
				(surface->*_surface)();
			}
			_start = slowTick();
			if (_start > _frameSkipStart) _frameSkipStart = _start; // don't play animations in ffwd to catch up :P
		}
	}
}

/**
 * Changes the timer's interval to a new value.
 * @param interval Interval in milliseconds.
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


/// Sets frame skipping on or off
void Timer::setFrameSkipping(bool skip)
{
	_frameSkipping = skip;
}

}
