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
#ifndef OPENXCOM__DOGFIGHTSTATE_H
#define OPENXCOM__DOGFIGHTSTATE_H

#include "State.h"
#include "LangString.h"

class ImageButton;
class Text;
class Surface;
class InteractiveSurface;
class Timer;
class Craft;
class Ufo;

using namespace std;

/**
 * Shows a dogfight (interception) between a
 * player craft and an UFO.
 */
class DogfightState : public State
{
private:
	Timer *_animTimer, *_moveTimer, *_w1Timer, *_w2Timer;
	Surface *_window, *_battle, *_weapon1, *_range1, *_weapon2, *_range2, *_damage;
	InteractiveSurface *_btnMinimize;
	ImageButton *_btnStandoff, *_btnCautious, *_btnStandard, *_btnAgressive, *_btnDisengage, *_btnUfo;
	ImageButton *_mode;
	Text *_txtStatus, *_txtAmmo1, *_txtAmmo2, *_txtDistance;
	Craft *_craft;
	Ufo *_ufo;
	int _timeout, _currentDist, _targetDist, _w1Dist, _w2Dist;
public:
	/// Creates the Dogfight state.
	DogfightState(Game *game, Craft *craft, Ufo *ufo);
	/// Cleans up the Dogfight state.
	~DogfightState();
	/// Runs the timers.
	void think();
	/// Animates the window.
	void animate();
	/// Moves the craft.
	void move();

	void fireWeapon1();

	void fireWeapon2();
	/// Changes the status text.
	void setStatus(LangString status);
	/// Handler for clicking the Standoff button.
	void btnStandoffClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Standoff button.
	void btnCautiousClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Standoff button.
	void btnStandardClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Standoff button.
	void btnAgressiveClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Disengage button.
	void btnDisengageClick(SDL_Event *ev, int scale);
};

#endif
