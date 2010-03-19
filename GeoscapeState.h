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
#ifndef OPENXCOM__GEOSCAPESTATE_H
#define OPENXCOM__GEOSCAPESTATE_H

#include "State_Interactive.h"
#include "Game.h"
#include "Palette.h"
#include "Surface.h"
#include "Globe.h"
#include "Text.h"
#include "CustomButton.h"
#include "Timer.h"
#include "OptionsState.h"
#include "InterceptState.h"
#include "BasescapeState.h"
#include "GraphsState.h"
#include "FundingState.h"

#define LONGITUDE_SPEED 0.2
#define LATITUDE_SPEED 0.1

class GeoscapeState : public State
{
private:
	Surface *_bg;
	Globe *_globe;
	CustomButton *_btnIntercept, *_btnBases, *_btnGraphs, *_btnUfopaedia, *_btnOptions, *_btnFunding;
	CustomButton *_timer;
	CustomButton *_btn5Secs, *_btn1Min, *_btn5Mins, *_btn30Mins, *_btn1Hour, *_btn1Day;
	InteractiveSurface *_btnRotateLeft, *_btnRotateRight, *_btnRotateUp, *_btnRotateDown, *_btnZoomIn, *_btnZoomOut;
	Text *_txtHour, *_txtHourSep, *_txtMin, *_txtMinSep, *_txtSec, *_txtWeekday, *_txtDay, *_txtMonth, *_txtYear;
	Timer *_rotTimer;
	double _rotLon, _rotLat;
public:
	GeoscapeState(Game *game);
	~GeoscapeState();
	void think();
	void globeRotate();
	void globeClick(SDL_Event *ev, int scale);
	void btnInterceptClick(SDL_Event *ev, int scale);
	void btnBasesClick(SDL_Event *ev, int scale);
	void btnGraphsClick(SDL_Event *ev, int scale);
	void btnUfopaediaClick(SDL_Event *ev, int scale);
	void btnOptionsClick(SDL_Event *ev, int scale);
	void btnFundingClick(SDL_Event *ev, int scale);
	void btnRotateLeftPress(SDL_Event *ev, int scale);
	void btnRotateLeftRelease(SDL_Event *ev, int scale);
	void btnRotateRightPress(SDL_Event *ev, int scale);
	void btnRotateRightRelease(SDL_Event *ev, int scale);
	void btnRotateUpPress(SDL_Event *ev, int scale);
	void btnRotateUpRelease(SDL_Event *ev, int scale);
	void btnRotateDownPress(SDL_Event *ev, int scale);
	void btnRotateDownRelease(SDL_Event *ev, int scale);
	void btnZoomInClick(SDL_Event *ev, int scale);
	void btnZoomOutClick(SDL_Event *ev, int scale);
};

#endif
