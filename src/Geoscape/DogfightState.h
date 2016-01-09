#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/State.h"
#include <vector>
#include <string>

namespace OpenXcom
{

const int STANDOFF_DIST = 560;
enum ColorNames { CRAFT_MIN, CRAFT_MAX, RADAR_MIN, RADAR_MAX, DAMAGE_MIN, DAMAGE_MAX, BLOB_MIN, RANGE_METER, DISABLED_WEAPON, DISABLED_AMMO, DISABLED_RANGE };

class ImageButton;
class Text;
class Surface;
class InteractiveSurface;
class Timer;
class GeoscapeState;
class Craft;
class Ufo;
class CraftWeaponProjectile;

/**
 * Shows a dogfight (interception) between a
 * player craft and an UFO.
 */
class DogfightState : public State
{
private:
	GeoscapeState *_state;
	Timer *_craftDamageAnimTimer;
	Surface *_window, *_battle, *_range1, *_range2, *_damage;
	InteractiveSurface *_btnMinimize, *_preview, *_weapon1, *_weapon2;
	ImageButton *_btnStandoff, *_btnCautious, *_btnStandard, *_btnAggressive, *_btnDisengage, *_btnUfo;
	ImageButton *_mode;
	InteractiveSurface *_btnMinimizedIcon;
	Text *_txtAmmo1, *_txtAmmo2, *_txtDistance, *_txtStatus, *_txtInterceptionNumber;
	Craft *_craft;
	Ufo *_ufo;
	int _timeout, _currentDist, _targetDist, _w1FireInterval, _w2FireInterval, _w1FireCountdown, _w2FireCountdown;
	bool _end, _destroyUfo, _destroyCraft, _ufoBreakingOff, _weapon1Enabled, _weapon2Enabled, _minimized, _endDogfight, _animatingHit, _waitForPoly;
	std::vector<CraftWeaponProjectile*> _projectiles;
	static const int _ufoBlobs[8][13][13];
	static const int _projectileBlobs[4][6][3];
	int _ufoSize, _craftHeight, _currentCraftDamageColor, _interceptionNumber;
	size_t _interceptionsCount;
	int _x, _y, _minimizedIconX, _minimizedIconY;
	int _colors[11];
	// Ends the dogfight.
	void endDogfight();

public:
	/// Creates the Dogfight state.
	DogfightState(GeoscapeState *state, Craft *craft, Ufo *ufo);
	/// Cleans up the Dogfight state.
	~DogfightState();
	/// Runs the timers.
	void think();
	/// Animates the window.
	void animate();
	/// Moves the craft.
	void update();
	// Fires the first weapon.
	void fireWeapon1();
	// Fires the second weapon.
	void fireWeapon2();
	// Fires UFO weapon.
	void ufoFireWeapon();
	// Sets the craft to minimum distance.
	void minimumDistance();
	// Sets the craft to maximum distance.
	void maximumDistance();
	/// Changes the status text.
	void setStatus(const std::string &status);
	/// Handler for clicking the Minimize button.
	void btnMinimizeClick(Action *action);
	/// Handler for pressing the Standoff button.
	void btnStandoffPress(Action *action);
	/// Handler for pressing the Cautious Attack button.
	void btnCautiousPress(Action *action);
	/// Handler for pressing the Standard Attack button.
	void btnStandardPress(Action *action);
	/// Handler for pressing the Aggressive Attack button.
	void btnAggressivePress(Action *action);
	/// Handler for pressing the Disengage button.
	void btnDisengagePress(Action *action);
	/// Handler for clicking the Ufo button.
	void btnUfoClick(Action *action);
	/// Handler for clicking the Preview graphic.
	void previewClick(Action *action);
	/// Draws UFO.
	void drawUfo();
	/// Draws projectiles.
	void drawProjectile(const CraftWeaponProjectile* p);
	/// Animates craft damage.
	void animateCraftDamage();
	/// Updates craft damage.
	void drawCraftDamage();
	/// Toggles usage of weapon 1.
	void weapon1Click(Action *action);
	/// Toggles usage of weapon 2.
	void weapon2Click(Action *action);
	/// Changes colors of weapon icons, range indicators and ammo texts base on current weapon state.
	void recolor(const int weaponNo, const bool currentState);
	/// Returns true if state is minimized.
	bool isMinimized() const;
	/// Sets state minimized or maximized.
	void setMinimized(const bool minimized);
	/// Handler for clicking the minimized interception window icon.
	void btnMinimizedIconClick(Action *action);
	/// Gets interception number.
	int getInterceptionNumber() const;
	/// Sets interception number.
	void setInterceptionNumber(const int number);
	/// Sets interceptions count.
	void setInterceptionsCount(const size_t count);
	/// Calculates window position according to opened interception windows.
	void calculateWindowPosition();
	/// Moves window to new position.
	void moveWindow();
	/// Checks if the dogfight should be ended.
	bool dogfightEnded() const;
	/// Gets pointer to the UFO in this dogfight.
	Ufo* getUfo() const;
	void setWaitForPoly(bool wait);
	bool getWaitForPoly();
};

}
