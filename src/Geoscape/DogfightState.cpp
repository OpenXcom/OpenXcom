	/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "DogfightState.h"
#include <sstream>
#include "GeoscapeState.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/Screen.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Surface.h"
#include "../Interface/ImageButton.h"
#include "../Interface/Text.h"
#include "../Engine/Timer.h"
#include "Globe.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Craft.h"
#include "../Mod/RuleCraft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Mod/RuleCraftWeapon.h"
#include "../Savegame/Ufo.h"
#include "../Mod/RuleUfo.h"
#include "../Engine/RNG.h"
#include "../Engine/Sound.h"
#include "../Savegame/Base.h"
#include "../Savegame/CraftWeaponProjectile.h"
#include "../Savegame/Country.h"
#include "../Mod/RuleCountry.h"
#include "../Savegame/Region.h"
#include "../Mod/RuleRegion.h"
#include "../Savegame/AlienMission.h"
#include "DogfightErrorState.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleInterface.h"

namespace OpenXcom
{

// UFO blobs graphics ...
const int DogfightState::_ufoBlobs[8][13][13] =
{
		/*0 STR_VERY_SMALL */
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 3, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 3, 5, 3, 1, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 3, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
		/*1 STR_SMALL */
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 4, 5, 4, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
		/*2 STR_MEDIUM_UC */
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0, 0},
		{0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 0},
		{0, 0, 1, 2, 3, 5, 5, 5, 3, 2, 1, 0, 0},
		{0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 0},
		{0, 0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
		/*3 STR_LARGE */
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0},
		{0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0, 0},
		{0, 0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0, 0},
		{0, 1, 2, 3, 4, 5, 5, 5, 4, 3, 2, 1, 0},
		{0, 1, 2, 3, 4, 5, 5, 5, 4, 3, 2, 1, 0},
		{0, 1, 2, 3, 4, 5, 5, 5, 4, 3, 2, 1, 0},
		{0, 0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0, 0},
		{0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0, 0},
		{0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
		/*4 STR_VERY_LARGE */
	{
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0},
		{0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0, 0},
		{0, 1, 2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 0},
		{0, 1, 2, 3, 4, 5, 5, 5, 4, 3, 2, 1, 0},
		{1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1},
		{1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1},
		{1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1},
		{0, 1, 2, 3, 4, 5, 5, 5, 4, 3, 2, 1, 0},
		{0, 1, 2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 0},
		{0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0, 0},
		{0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0}
	},
		/*5 STR_HUGE */
	{
		{0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0},
		{0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0, 0},
		{0, 1, 2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 0},
		{1, 2, 3, 4, 4, 5, 5, 5, 4, 4, 3, 2, 1},
		{1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1},
		{2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2},
		{2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2},
		{2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2},
		{1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1},
		{1, 2, 3, 4, 4, 5, 5, 5, 4, 4, 3, 2, 1},
		{0, 1, 2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 0},
		{0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0, 0},
		{0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0}
	},
		/*6 STR_VERY_HUGE :p */
	{
		{0, 0, 0, 2, 2, 3, 3, 3, 2, 2, 0, 0, 0},
		{0, 0, 2, 3, 3, 4, 4, 4, 3, 3, 2, 0, 0},
		{0, 2, 3, 4, 4, 5, 5, 5, 4, 4, 3, 2, 0},
		{2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2},
		{2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2},
		{3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3},
		{3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3},
		{3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3},
		{2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2},
		{2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 2},
		{0, 2, 3, 4, 4, 5, 5, 5, 4, 4, 3, 2, 0},
		{0, 0, 2, 3, 3, 4, 4, 4, 3, 3, 2, 0, 0},
		{0, 0, 0, 2, 2, 3, 3, 3, 2, 2, 0, 0, 0}
	},
		/*7 STR_ENOURMOUS */
	{
		{0, 0, 0, 3, 3, 4, 4, 4, 3, 3, 0, 0, 0},
		{0, 0, 3, 4, 4, 5, 5, 5, 4, 4, 3, 0, 0},
		{0, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 0},
		{3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3},
		{3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3},
		{4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4},
		{4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4},
		{4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4},
		{3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3},
		{3, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 3},
		{0, 3, 4, 5, 5, 5, 5, 5, 5, 5, 4, 3, 0},
		{0, 0, 3, 4, 4, 5, 5, 5, 4, 4, 3, 0, 0},
		{0, 0, 0, 3, 3, 4, 4, 4, 3, 3, 0, 0, 0}
	}
};

// Projectile blobs
const int DogfightState::_projectileBlobs[4][6][3] =
{
		/*0 STR_STINGRAY_MISSILE ?*/
	{
		{0, 1, 0},
		{1, 9, 1},
		{1, 4, 1},
		{0, 3, 0},
		{0, 2, 0},
		{0, 1, 0}
	},
		/*1 STR_AVALANCHE_MISSILE ?*/
	{
		{1, 2, 1},
		{2, 9, 2},
		{2, 5, 2},
		{1, 3, 1},
		{0, 2, 0},
		{0, 1, 0}
	},
		/*2 STR_CANNON_ROUND ?*/
	{
		{0, 0, 0},
		{0, 7, 0},
		{0, 2, 0},
		{0, 1, 0},
		{0, 0, 0},
		{0, 0, 0}
	},
		/*3 STR_FUSION_BALL ?*/
	{
		{2, 4, 2},
		{4, 9, 4},
		{2, 4, 2},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	}
};
/**
 * Initializes all the elements in the Dogfight window.
 * @param game Pointer to the core game.
 * @param state Pointer to the Geoscape.
 * @param craft Pointer to the craft intercepting.
 * @param ufo Pointer to the UFO being intercepted.
 */
DogfightState::DogfightState(GeoscapeState *state, Craft *craft, Ufo *ufo) : _state(state), _craft(craft), _ufo(ufo), _timeout(50), _currentDist(640), _targetDist(560), _w1FireCountdown(0), _w2FireCountdown(0), _end(false), _destroyUfo(false), _destroyCraft(false), _ufoBreakingOff(false), _weapon1Enabled(true), _weapon2Enabled(true), _minimized(false), _endDogfight(false), _animatingHit(false), _ufoSize(0), _craftHeight(0), _currentCraftDamageColor(0), _interceptionNumber(0), _interceptionsCount(0), _x(0), _y(0), _minimizedIconX(0), _minimizedIconY(0)
{
	_screen = false;

	_craft->setInDogfight(true);

	// Create objects
	_window = new Surface(160, 96, _x, _y);
	_battle = new Surface(77, 74, _x + 3, _y + 3);
	_weapon1 = new InteractiveSurface(15, 17, _x + 4, _y + 52);
	_range1 = new Surface(21, 74, _x + 19, _y + 3);
	_weapon2 = new InteractiveSurface(15, 17, _x + 64, _y + 52);
	_range2 = new Surface(21, 74, _x + 43, _y + 3);
	_damage = new Surface(22, 25, _x + 93, _y + 40);

	_btnMinimize = new InteractiveSurface(12, 12, _x, _y);
	_preview = new InteractiveSurface(160, 96, _x, _y);
	_btnStandoff = new ImageButton(36, 15, _x + 83, _y + 4);
	_btnCautious = new ImageButton(36, 15, _x + 120, _y + 4);
	_btnStandard = new ImageButton(36, 15, _x + 83, _y + 20);
	_btnAggressive = new ImageButton(36, 15, _x + 120, _y + 20);
	_btnDisengage = new ImageButton(36, 15, _x + 120, _y + 36);
	_btnUfo = new ImageButton(36, 17, _x + 120, _y + 52);
	_txtAmmo1 = new Text(16, 9, _x + 4, _y + 70);
	_txtAmmo2 = new Text(16, 9, _x + 64, _y + 70);
	_txtDistance = new Text(40, 9, _x + 116, _y + 72);
	_txtStatus = new Text(150, 9, _x + 4, _y + 85);
	_btnMinimizedIcon = new InteractiveSurface(32, 20, _minimizedIconX, _minimizedIconY);
	_txtInterceptionNumber = new Text(16, 9, _minimizedIconX + 18, _minimizedIconY + 6);

	_mode = _btnStandoff;
	_craftDamageAnimTimer = new Timer(500);

	// Set palette
	setInterface("dogfight");

	add(_window);
	add(_battle);
	add(_weapon1);
	add(_range1);
	add(_weapon2);
	add(_range2);
	add(_damage);
	add(_btnMinimize);
	add(_btnStandoff, "standoffButton", "dogfight", _window);
	add(_btnCautious, "cautiousButton", "dogfight", _window);
	add(_btnStandard, "standardButton", "dogfight", _window);
	add(_btnAggressive, "aggressiveButton", "dogfight", _window);
	add(_btnDisengage, "disengageButton", "dogfight", _window);
	add(_btnUfo, "ufoButton", "dogfight", _window);
	add(_txtAmmo1, "numbers", "dogfight", _window);
	add(_txtAmmo2, "numbers", "dogfight", _window);
	add(_txtDistance, "distance", "dogfight", _window);
	add(_preview);
	add(_txtStatus, "text", "dogfight", _window);
	add(_btnMinimizedIcon);
	add(_txtInterceptionNumber, "minimizedNumber", "dogfight");

	_btnStandoff->invalidate(false);
	_btnCautious->invalidate(false);
	_btnStandard->invalidate(false);
	_btnAggressive->invalidate(false);
	_btnDisengage->invalidate(false);
	_btnUfo->invalidate(false);

	// Set up objects
	RuleInterface *dogfightInterface = _game->getMod()->getInterface("dogfight");

	Surface *graphic;
	graphic = _game->getMod()->getSurface("INTERWIN.DAT");
	graphic->setX(0);
	graphic->setY(0);
	graphic->getCrop()->x = 0;
	graphic->getCrop()->y = 0;
	graphic->getCrop()->w = _window->getWidth();
	graphic->getCrop()->h = _window->getHeight();
	_window->drawRect(graphic->getCrop(), 15);
	graphic->blit(_window);

	_preview->drawRect(graphic->getCrop(), 15);
	graphic->getCrop()->y = dogfightInterface->getElement("previewTop")->y;
	graphic->getCrop()->h = dogfightInterface->getElement("previewTop")->h;
	graphic->blit(_preview);
	graphic->setY(_window->getHeight() - dogfightInterface->getElement("previewBot")->h);
	graphic->getCrop()->y = dogfightInterface->getElement("previewBot")->y;
	graphic->getCrop()->h = dogfightInterface->getElement("previewBot")->h;
	graphic->blit(_preview);
	if (ufo->getRules()->getModSprite().empty())
	{
		graphic->getCrop()->y = dogfightInterface->getElement("previewMid")->y + dogfightInterface->getElement("previewMid")->h * _ufo->getRules()->getSprite();
		graphic->getCrop()->h = dogfightInterface->getElement("previewMid")->h;
	}
	else
	{
		graphic = _game->getMod()->getSurface(ufo->getRules()->getModSprite());
	}
	graphic->setX(dogfightInterface->getElement("previewTop")->x);
	graphic->setY(dogfightInterface->getElement("previewTop")->h);
	graphic->blit(_preview);
	_preview->setVisible(false);
	_preview->onMouseClick((ActionHandler)&DogfightState::previewClick);

	_btnMinimize->onMouseClick((ActionHandler)&DogfightState::btnMinimizeClick);

	_btnStandoff->copy(_window);
	_btnStandoff->setGroup(&_mode);
	_btnStandoff->onMousePress((ActionHandler)&DogfightState::btnStandoffPress);

	_btnCautious->copy(_window);
	_btnCautious->setGroup(&_mode);
	_btnCautious->onMousePress((ActionHandler)&DogfightState::btnCautiousPress);

	_btnStandard->copy(_window);
	_btnStandard->setGroup(&_mode);
	_btnStandard->onMousePress((ActionHandler)&DogfightState::btnStandardPress);

	_btnAggressive->copy(_window);
	_btnAggressive->setGroup(&_mode);
	_btnAggressive->onMousePress((ActionHandler)&DogfightState::btnAggressivePress);

	_btnDisengage->copy(_window);
	_btnDisengage->onMousePress((ActionHandler)&DogfightState::btnDisengagePress);
	_btnDisengage->setGroup(&_mode);

	_btnUfo->copy(_window);
	_btnUfo->onMouseClick((ActionHandler)&DogfightState::btnUfoClick);

	_txtDistance->setText(L"640");

	_txtStatus->setText(tr("STR_STANDOFF"));

	SurfaceSet *set = _game->getMod()->getSurfaceSet("INTICON.PCK");

	// Create the minimized dogfight icon.
	Surface *frame = set->getFrame(_craft->getRules()->getSprite());
	frame->setX(0);
	frame->setY(0);
	frame->blit(_btnMinimizedIcon);
	_btnMinimizedIcon->onMouseClick((ActionHandler)&DogfightState::btnMinimizedIconClick);
	_btnMinimizedIcon->setVisible(false);

	// Draw correct number on the minimized dogfight icon.
	std::wostringstream ss1;
	ss1 << _craft->getInterceptionOrder();
	_txtInterceptionNumber->setText(ss1.str());
	_txtInterceptionNumber->setVisible(false);

	// define the colors to be used
	_colors[CRAFT_MIN] = dogfightInterface->getElement("craftRange")->color;
	_colors[CRAFT_MAX] = dogfightInterface->getElement("craftRange")->color2;
	_colors[RADAR_MIN] = dogfightInterface->getElement("radarRange")->color;
	_colors[RADAR_MAX] = dogfightInterface->getElement("radarRange")->color2;
	_colors[DAMAGE_MIN] = dogfightInterface->getElement("damageRange")->color;
	_colors[DAMAGE_MAX] = dogfightInterface->getElement("damageRange")->color2;
	_colors[BLOB_MIN] = dogfightInterface->getElement("radarDetail")->color;
	_colors[RANGE_METER] = dogfightInterface->getElement("radarDetail")->color2;
	_colors[DISABLED_WEAPON] = dogfightInterface->getElement("disabledWeapon")->color;
	_colors[DISABLED_RANGE] = dogfightInterface->getElement("disabledWeapon")->color2;
	_colors[DISABLED_AMMO] = dogfightInterface->getElement("disabledAmmo")->color;

	for (unsigned int i = 0; i < _craft->getRules()->getWeapons(); ++i)
	{
		CraftWeapon *w = _craft->getWeapons()->at(i);
		if (w == 0)
			continue;

		Surface *weapon = 0, *range = 0;
		Text *ammo = 0;
		int x1, x2;
		if (i == 0)
		{
			weapon = _weapon1;
			range = _range1;
			ammo = _txtAmmo1;
			x1 = 2;
			x2 = 0;
		}
		else
		{
			weapon = _weapon2;
			range = _range2;
			ammo = _txtAmmo2;
			x1 = 0;
			x2 = 18;
		}

		// Draw weapon icon
		frame = set->getFrame(w->getRules()->getSprite() + 5);

		frame->setX(0);
		frame->setY(0);
		frame->blit(weapon);

		// Draw ammo
		std::wostringstream ss;
		ss << w->getAmmo();
		ammo->setText(ss.str());

		// Draw range (1 km = 1 pixel)
		Uint8 color = _colors[RANGE_METER];
		range->lock();

		int rangeY = range->getHeight() - w->getRules()->getRange(), connectY = 57;
		for (int x = x1; x <= x1 + 18; x += 2)
		{
			range->setPixel(x, rangeY, color);
		}

		int minY = 0, maxY = 0;
		if (rangeY < connectY)
		{
			minY = rangeY;
			maxY = connectY;
		}
		else if (rangeY > connectY)
		{
			minY = connectY;
			maxY = rangeY;
		}
		for (int y = minY; y <= maxY; ++y)
		{
			range->setPixel(x1 + x2, y, color);
		}
		for (int x = x2; x <= x2 + 2; ++x)
		{
			range->setPixel(x, connectY, color);
		}
		range->unlock();
	}

	if (!(_craft->getRules()->getWeapons() > 0 && _craft->getWeapons()->at(0) != 0))
	{
		_weapon1->setVisible(false);
		_range1->setVisible(false);
		_txtAmmo1->setVisible(false);
	}
	if (!(_craft->getRules()->getWeapons() > 1 && _craft->getWeapons()->at(1) != 0))
	{
		_weapon2->setVisible(false);
		_range2->setVisible(false);
		_txtAmmo2->setVisible(false);
	}

	// Draw damage indicator.
	frame = set->getFrame(_craft->getRules()->getSprite() + 11);
	frame->setX(0);
	frame->setY(0);
	frame->blit(_damage);

	_craftDamageAnimTimer->onTimer((StateHandler)&DogfightState::animateCraftDamage);

	// don't set these variables if the ufo is already engaged in a dogfight
	if (!_ufo->getEscapeCountdown())
	{
		_ufo->setFireCountdown(0);
		_ufo->setEscapeCountdown(_ufo->getRules()->getBreakOffTime() + RNG::generate(0, _ufo->getRules()->getBreakOffTime()) - 30 * _game->getSavedGame()->getDifficultyCoefficient());
	}

	// technically this block is redundant, but i figure better to initialize the variables as SOMETHING
	if (_craft->getRules()->getWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
	{
		_w1FireInterval = _craft->getWeapons()->at(0)->getRules()->getStandardReload();
	}
	if (_craft->getRules()->getWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
	{
		_w2FireInterval = _craft->getWeapons()->at(1)->getRules()->getStandardReload();
	}

	// Set UFO size - going to be moved to Ufo class to implement simultaneous dogfights.
	std::string ufoSize = _ufo->getRules()->getSize();
	if (ufoSize.compare("STR_VERY_SMALL") == 0)
	{
		_ufoSize = 0;
	}
	else if (ufoSize.compare("STR_SMALL") == 0)
	{
		_ufoSize = 1;
	}
	else if (ufoSize.compare("STR_MEDIUM_UC") == 0)
	{
		_ufoSize = 2;
	}
	else if (ufoSize.compare("STR_LARGE") == 0)
	{
		_ufoSize = 3;
	}
	else
	{
		_ufoSize = 4;
	}

	// Get crafts height. Used for damage indication.
	int x =_damage->getWidth() / 2;
	for (int y = 0; y < _damage->getHeight(); ++y)
	{
		Uint8 pixelColor = _damage->getPixel(x, y);
		if (pixelColor >= _colors[CRAFT_MIN] && pixelColor < _colors[CRAFT_MAX])
		{
			++_craftHeight;
		}
	}

	drawCraftDamage();

	// Used for weapon toggling.
	_weapon1->onMouseClick((ActionHandler)&DogfightState::weapon1Click);
	_weapon2->onMouseClick((ActionHandler)&DogfightState::weapon2Click);
}

/**
 * Cleans up the dogfight state.
 */
DogfightState::~DogfightState()
{
	delete _craftDamageAnimTimer;
	while (!_projectiles.empty())
	{
		delete _projectiles.back();
		_projectiles.pop_back();
	}
	if (_craft)
		_craft->setInDogfight(false);
	// set the ufo as "free" for the next engagement (as applicable)
	if (_ufo)
		_ufo->setInterceptionProcessed(false);
}

/**
 * Runs the higher level dogfight functionality.
 */
void DogfightState::think()
{
	if (!_endDogfight)
	{
		update();
		_craftDamageAnimTimer->think(this, 0);
	}
	if (!_craft->isInDogfight() || _craft->getDestination() != _ufo || _ufo->getStatus() == Ufo::LANDED)
	{
		endDogfight();
	}
}

/**
 * Animates interceptor damage by changing the color and redrawing the image.
 */
void DogfightState::animateCraftDamage()
{
	if (_minimized)
	{
		return;
	}
	--_currentCraftDamageColor;
	if (_currentCraftDamageColor < _colors[DAMAGE_MIN])
	{
		_currentCraftDamageColor = _colors[DAMAGE_MAX];
	}
	drawCraftDamage();
}

/**
 * Draws interceptor damage according to percentage of HP's left.
 */
void DogfightState::drawCraftDamage()
{
	if (_craft->getDamagePercentage() != 0)
	{
		if (!_craftDamageAnimTimer->isRunning())
		{
			_craftDamageAnimTimer->start();
			if (_currentCraftDamageColor < _colors[DAMAGE_MIN])
			{
				_currentCraftDamageColor = _colors[DAMAGE_MIN];
			}
		}
		int damagePercentage = _craft->getDamagePercentage();
		int rowsToColor = (int)floor((double)_craftHeight * (double)(damagePercentage / 100.));
		if (rowsToColor == 0)
		{
			return;
		}
		int rowsColored = 0;
		bool rowColored = false;
		for (int y = 0; y < _damage->getHeight(); ++y)
		{
			rowColored = false;
			for (int x = 0; x < _damage->getWidth(); ++x)
			{
				int pixelColor = _damage->getPixel(x, y);
				if (pixelColor >= _colors[DAMAGE_MIN] && pixelColor <= _colors[DAMAGE_MAX])
				{
					_damage->setPixel(x, y, _currentCraftDamageColor);
					rowColored = true;
				}
				if (pixelColor >= _colors[CRAFT_MIN] && pixelColor < _colors[CRAFT_MAX])
				{
					_damage->setPixel(x, y, _currentCraftDamageColor);
					rowColored = true;
				}
			}
			if (rowColored)
			{
				++rowsColored;
			}
			if (rowsColored == rowsToColor)
			{
				break;
			}
		}
	}
}

/**
 * Animates the window with a palette effect.
 */
void DogfightState::animate()
{
	// Animate radar waves and other stuff.
	for (int x = 0; x < _window->getWidth(); ++x)
	{
		for (int y = 0; y < _window->getHeight(); ++y)
		{
			Uint8 radarPixelColor = _window->getPixel(x, y);
			if (radarPixelColor >= _colors[RADAR_MIN] && radarPixelColor < _colors[RADAR_MAX])
			{
				++radarPixelColor;
				if (radarPixelColor >= _colors[RADAR_MAX])
				{
					radarPixelColor = _colors[RADAR_MIN];
				}
				_window->setPixel(x, y, radarPixelColor);
			}
		}
	}

	_battle->clear();

	// Draw UFO.
	if (!_ufo->isDestroyed())
	{
		drawUfo();
	}

	// Draw projectiles.
	for (std::vector<CraftWeaponProjectile*>::iterator it = _projectiles.begin(); it != _projectiles.end(); ++it)
	{
		drawProjectile((*it));
	}

	// Clears text after a while
	if (_timeout == 0)
	{
		_txtStatus->setText(L"");
	}
	else
	{
		_timeout--;
	}

	// Animate UFO hit.
	bool lastHitAnimFrame = false;
	if (_animatingHit && _ufo->getHitFrame() > 0)
	{
		_ufo->setHitFrame(_ufo->getHitFrame() - 1);
		if (_ufo->getHitFrame() == 0)
		{
			_animatingHit = false;
			lastHitAnimFrame = true;
		}
	}

	// Animate UFO crash landing.
	if (_ufo->isCrashed() && _ufo->getHitFrame() == 0 && !lastHitAnimFrame)
	{
		--_ufoSize;
	}
}

/**
 * Updates all the elements in the dogfight, including ufo movement,
 * weapons fire, projectile movement, ufo escape conditions,
 * craft and ufo destruction conditions, and retaliation mission generation, as applicable.
 */
void DogfightState::update()
{
	bool finalRun = false;
	// Check if craft is not low on fuel when window minimized, and
	// Check if crafts destination hasn't been changed when window minimized.
	Ufo* u = dynamic_cast<Ufo*>(_craft->getDestination());
	if (u != _ufo || !_craft->isInDogfight() || _craft->getLowFuel() || (_minimized && _ufo->isCrashed()))
	{
		endDogfight();
		return;
	}

	if (!_minimized)
	{
		animate();
		if (!_ufo->isCrashed() && !_ufo->isDestroyed() && !_craft->isDestroyed() && !_ufo->getInterceptionProcessed())
		{
			_ufo->setInterceptionProcessed(true);
			int escapeCounter = _ufo->getEscapeCountdown();

			if (escapeCounter > 0 )
			{
				escapeCounter--;
				_ufo->setEscapeCountdown(escapeCounter);
				// Check if UFO is breaking off.
				if (escapeCounter == 0)
				{
					_ufo->setSpeed(_ufo->getRules()->getMaxSpeed());
				}
			}
			if (_ufo->getFireCountdown() > 0)
			{
				_ufo->setFireCountdown(_ufo->getFireCountdown() - 1);
			}
		}
	}
	// Crappy craft is chasing UFO.
	if (_ufo->getSpeed() > _craft->getRules()->getMaxSpeed())
	{
		_ufoBreakingOff = true;
		finalRun = true;
		setStatus("STR_UFO_OUTRUNNING_INTERCEPTOR");
	}
	else
	{
		_ufoBreakingOff = false;
	}

	bool projectileInFlight = false;
	if (!_minimized)
	{
		int distanceChange = 0;

		// Update distance
		if (!_ufoBreakingOff)
		{
			if (_currentDist < _targetDist && !_ufo->isCrashed() && !_craft->isDestroyed())
			{
				distanceChange = 4;
				if (_currentDist + distanceChange >_targetDist)
				{
					distanceChange = _targetDist - _currentDist;
				}
			}
			else if (_currentDist > _targetDist && !_ufo->isCrashed() && !_craft->isDestroyed())
			{
				distanceChange = -2;
			}

			// don't let the interceptor mystically push or pull its fired projectiles
			for (std::vector<CraftWeaponProjectile*>::iterator it = _projectiles.begin(); it != _projectiles.end(); ++it)
			{
				if ((*it)->getGlobalType() != CWPGT_BEAM && (*it)->getDirection() == D_UP) (*it)->setPosition((*it)->getPosition() + distanceChange);
			}
		}
		else
		{
			distanceChange = 4;

			// UFOs can try to outrun our missiles, don't adjust projectile positions here
			// If UFOs ever fire anything but beams, those positions need to be adjust here though.
		}

		_currentDist += distanceChange;

		std::wostringstream ss;
		ss << _currentDist;
		_txtDistance->setText(ss.str());

		// Move projectiles and check for hits.
		for (std::vector<CraftWeaponProjectile*>::iterator it = _projectiles.begin(); it != _projectiles.end(); ++it)
		{
			CraftWeaponProjectile *p = (*it);
			p->move();
			// Projectiles fired by interceptor.
			if (p->getDirection() == D_UP)
			{
				// Projectile reached the UFO - determine if it's been hit.
				if (((p->getPosition() >= _currentDist) || (p->getGlobalType() == CWPGT_BEAM && p->toBeRemoved())) && !_ufo->isCrashed() && !p->getMissed())
				{
					// UFO hit.
					if (RNG::percent((p->getAccuracy() * (100 + 300 / (5 - _ufoSize)) + 100) / 200))
					{
						// Formula delivered by Volutar
						int damage = RNG::generate(p->getDamage() / 2, p->getDamage());
						_ufo->setDamage(_ufo->getDamage() + damage);
						if (_ufo->isCrashed())
						{
							_ufo->setShotDownByCraftId(_craft->getUniqueId());
							_ufo->setSpeed(0);
							// if the ufo got destroyed here, these no longer apply
							_ufoBreakingOff = false;
							finalRun = false;
							_end = false;
						}
						if (_ufo->getHitFrame() == 0)
						{
							_animatingHit = true;
							_ufo->setHitFrame(3);
						}

						setStatus("STR_UFO_HIT");
						_game->getMod()->getSound("GEO.CAT", Mod::UFO_HIT)->play();
						p->remove();
					}
					// Missed.
					else
					{
						if (p->getGlobalType() == CWPGT_BEAM)
						{
							p->remove();
						}
						else
						{
							p->setMissed(true);
						}
					}
				}
				// Check if projectile passed it's maximum range.
				if (p->getGlobalType() == CWPGT_MISSILE)
				{
					if (p->getPosition() / 8 >= p->getRange())
					{
						p->remove();
					}
					else if (!_ufo->isCrashed())
					{
						projectileInFlight = true;
					}
				}
			}
			// Projectiles fired by UFO.
			else if (p->getDirection() == D_DOWN)
			{
				if (p->getGlobalType() == CWPGT_MISSILE || (p->getGlobalType() == CWPGT_BEAM && p->toBeRemoved()))
				{
					if (RNG::percent(p->getAccuracy()))
					{
						// Formula delivered by Volutar
						int damage = RNG::generate(0, _ufo->getRules()->getWeaponPower());
						if (damage)
						{
							_craft->setDamage(_craft->getDamage() + damage);
							drawCraftDamage();
							setStatus("STR_INTERCEPTOR_DAMAGED");
							_game->getMod()->getSound("GEO.CAT", Mod::INTERCEPTOR_HIT)->play(); //10
							if (_mode == _btnCautious && _craft->getDamagePercentage() >= 50)
							{
								_targetDist = STANDOFF_DIST;
							}
						}
					}
					p->remove();
				}
			}
		}

		// Remove projectiles that hit or missed their target.
		for (std::vector<CraftWeaponProjectile*>::iterator it = _projectiles.begin(); it != _projectiles.end();)
		{
			if ((*it)->toBeRemoved() == true || ((*it)->getMissed() == true && (*it)->getPosition() <= 0))
			{
				delete *it;
				it = _projectiles.erase(it);
			}
			else
			{
				++it;
			}
		}

		// Handle weapons and craft distance.
		for (unsigned int i = 0; i < _craft->getRules()->getWeapons(); ++i)
		{
			CraftWeapon *w = _craft->getWeapons()->at(i);
			if (w == 0)
			{
				continue;
			}
			int wTimer;
			if (i == 0)
			{
				wTimer = _w1FireCountdown;
			}
			else
			{
				wTimer = _w2FireCountdown;
			}

			// Handle weapon firing
			if (wTimer == 0 && _currentDist <= w->getRules()->getRange() * 8 && w->getAmmo() > 0 && _mode != _btnStandoff
				&& _mode != _btnDisengage && !_ufo->isCrashed() && !_craft->isDestroyed())
			{
				if (i == 0)
				{
					fireWeapon1();
				}
				else
				{
					fireWeapon2();
				}
			}
			else if (wTimer > 0)
			{
				if (i == 0)
				{
					_w1FireCountdown--;
				}
				else
				{
					_w2FireCountdown--;
				}
			}

			if (w->getAmmo() == 0 && !projectileInFlight && !_craft->isDestroyed())
			{
				// Handle craft distance according to option set by user and available ammo.
				if (_mode == _btnCautious)
				{
					minimumDistance();
				}
				else if (_mode == _btnStandard)
				{
					maximumDistance();
				}
			}
		}

		// Handle UFO firing.
		if (_currentDist <= _ufo->getRules()->getWeaponRange() * 8 && !_ufo->isCrashed() && !_craft->isDestroyed())
		{
			if (_ufo->getShootingAt() == 0)
			{
				_ufo->setShootingAt(_interceptionNumber);
			}
			if (_ufo->getShootingAt() == _interceptionNumber)
			{
				if (_ufo->getFireCountdown() == 0)
				{
					ufoFireWeapon();
				}
			}
		}
		else if (_ufo->getShootingAt() == _interceptionNumber)
		{
			_ufo->setShootingAt(0);
		}
	}

	// Check when battle is over.
	if (_end == true && (((_currentDist > 640 || _minimized) && (_mode == _btnDisengage || _ufoBreakingOff == true)) || (_timeout == 0 && (_ufo->isCrashed() || _craft->isDestroyed()))))
	{
		if (_ufoBreakingOff)
		{
			_ufo->move();
			_craft->setDestination(_ufo);
		}
		if (!_destroyCraft && (_destroyUfo || _mode == _btnDisengage))
		{
			_craft->returnToBase();
		}
		endDogfight();
	}

	if (_currentDist > 640 && _ufoBreakingOff)
	{
		finalRun = true;
	}

	// End dogfight if craft is destroyed.
	if (!_end)
	{
		if (_craft->isDestroyed())
		{
			setStatus("STR_INTERCEPTOR_DESTROYED");
			_timeout += 30;
			_game->getMod()->getSound("GEO.CAT", Mod::INTERCEPTOR_EXPLODE)->play();
			finalRun = true;
			_destroyCraft = true;
			_ufo->setShootingAt(0);
		}

		// End dogfight if UFO is crashed or destroyed.
		if (_ufo->isCrashed())
		{
			AlienMission *mission = _ufo->getMission();
			mission->ufoShotDown(*_ufo);
			// Check for retaliation trigger.
			int retaliationOdds = mission->getRules().getRetaliationOdds();
			if (retaliationOdds == -1)
			{
				retaliationOdds = 100 - (4 * (24 - _game->getSavedGame()->getDifficultyCoefficient()));
			}

			if (RNG::percent(retaliationOdds))
			{
				// Spawn retaliation mission.
				std::string targetRegion;
				if (RNG::percent(50 - 6 * _game->getSavedGame()->getDifficultyCoefficient()))
				{
					// Attack on UFO's mission region
					targetRegion = _ufo->getMission()->getRegion();
				}
				else
				{
					// Try to find and attack the originating base.
					targetRegion = _game->getSavedGame()->locateRegion(*_craft->getBase())->getRules()->getType();
					// TODO: If the base is removed, the mission is canceled.
				}
				// Difference from original: No retaliation until final UFO lands (Original: Is spawned).
				if (!_game->getSavedGame()->findAlienMission(targetRegion, OBJECTIVE_RETALIATION))
				{
					const RuleAlienMission &rule = *_game->getMod()->getRandomMission(OBJECTIVE_RETALIATION, _game->getSavedGame()->getMonthsPassed());
					AlienMission *mission = new AlienMission(rule);
					mission->setId(_game->getSavedGame()->getId("ALIEN_MISSIONS"));
					mission->setRegion(targetRegion, *_game->getMod());
					mission->setRace(_ufo->getAlienRace());
					mission->start(mission->getRules().getWave(0).spawnTimer); // fixed delay for first scout
					_game->getSavedGame()->getAlienMissions().push_back(mission);
				}
			}

			if (_ufo->isDestroyed())
			{
				if (_ufo->getShotDownByCraftId() == _craft->getUniqueId())
				{
					for (std::vector<Country*>::iterator country = _game->getSavedGame()->getCountries()->begin(); country != _game->getSavedGame()->getCountries()->end(); ++country)
					{
						if ((*country)->getRules()->insideCountry(_ufo->getLongitude(), _ufo->getLatitude()))
						{
							(*country)->addActivityXcom(_ufo->getRules()->getScore()*2);
							break;
						}
					}
					for (std::vector<Region*>::iterator region = _game->getSavedGame()->getRegions()->begin(); region != _game->getSavedGame()->getRegions()->end(); ++region)
					{
						if ((*region)->getRules()->insideRegion(_ufo->getLongitude(), _ufo->getLatitude()))
						{
							(*region)->addActivityXcom(_ufo->getRules()->getScore()*2);
							break;
						}
					}
					setStatus("STR_UFO_DESTROYED");
					_game->getMod()->getSound("GEO.CAT", Mod::UFO_EXPLODE)->play(); //11
				}
				_destroyUfo = true;
			}
			else
			{
				if (_ufo->getShotDownByCraftId() == _craft->getUniqueId())
				{
					setStatus("STR_UFO_CRASH_LANDS");
					_game->getMod()->getSound("GEO.CAT", Mod::UFO_CRASH)->play(); //10
					for (std::vector<Country*>::iterator country = _game->getSavedGame()->getCountries()->begin(); country != _game->getSavedGame()->getCountries()->end(); ++country)
					{
						if ((*country)->getRules()->insideCountry(_ufo->getLongitude(), _ufo->getLatitude()))
						{
							(*country)->addActivityXcom(_ufo->getRules()->getScore());
							break;
						}
					}
					for (std::vector<Region*>::iterator region = _game->getSavedGame()->getRegions()->begin(); region != _game->getSavedGame()->getRegions()->end(); ++region)
					{
						if ((*region)->getRules()->insideRegion(_ufo->getLongitude(), _ufo->getLatitude()))
						{
							(*region)->addActivityXcom(_ufo->getRules()->getScore());
							break;
						}
					}
				}
				if (!_state->getGlobe()->insideLand(_ufo->getLongitude(), _ufo->getLatitude()))
				{
					_ufo->setStatus(Ufo::DESTROYED);
					_destroyUfo = true;
				}
				else
				{
					_ufo->setSecondsRemaining(RNG::generate(24, 96)*3600);
					_ufo->setAltitude("STR_GROUND");
					if (_ufo->getCrashId() == 0)
					{
						_ufo->setCrashId(_game->getSavedGame()->getId("STR_CRASH_SITE"));
					}
				}
			}
			_timeout += 30;
			if (_ufo->getShotDownByCraftId() != _craft->getUniqueId())
			{
				_timeout += 50;
				_ufo->setHitFrame(3);
			}
			finalRun = true;

			if (_ufo->getStatus() == Ufo::LANDED)
			{
				_timeout += 30;
				finalRun = true;
				_ufo->setShootingAt(0);
			}
		}
	}

	if (!projectileInFlight && finalRun)
	{
		_end = true;
	}
}

/**
 * Fires a shot from the first weapon
 * equipped on the craft.
 */
void DogfightState::fireWeapon1()
{
	if (_weapon1Enabled)
	{
		CraftWeapon *w1 = _craft->getWeapons()->at(0);
		if (w1->setAmmo(w1->getAmmo() - 1))
		{
			_w1FireCountdown = _w1FireInterval;

			std::wostringstream ss;
			ss << w1->getAmmo();
			_txtAmmo1->setText(ss.str());

			CraftWeaponProjectile *p = w1->fire();
			p->setDirection(D_UP);
			p->setHorizontalPosition(HP_LEFT);
			_projectiles.push_back(p);

			_game->getMod()->getSound("GEO.CAT", w1->getRules()->getSound())->play();
		}
	}
}

/**
 * Fires a shot from the second weapon
 * equipped on the craft.
 */
void DogfightState::fireWeapon2()
{
	if (_weapon2Enabled)
	{
		CraftWeapon *w2 = _craft->getWeapons()->at(1);
		if (w2->setAmmo(w2->getAmmo() - 1))
		{
			_w2FireCountdown = _w2FireInterval;

			std::wostringstream ss;
			ss << w2->getAmmo();
			_txtAmmo2->setText(ss.str());

			CraftWeaponProjectile *p = w2->fire();
			p->setDirection(D_UP);
			p->setHorizontalPosition(HP_RIGHT);
			_projectiles.push_back(p);

			_game->getMod()->getSound("GEO.CAT", w2->getRules()->getSound())->play();
		}
	}
}

/**
 *	Each time a UFO will try to fire it's cannons
 *	a calculation is made. There's only 10% chance
 *	that it will actually fire.
 */
void DogfightState::ufoFireWeapon()
{
	int fireCountdown = std::max(1, (_ufo->getRules()->getWeaponReload() - 2 * _game->getSavedGame()->getDifficultyCoefficient()));
	_ufo->setFireCountdown(RNG::generate(0, fireCountdown) + fireCountdown);

	setStatus("STR_UFO_RETURN_FIRE");
	CraftWeaponProjectile *p = new CraftWeaponProjectile();
	p->setType(CWPT_PLASMA_BEAM);
	p->setAccuracy(60);
	p->setDamage(_ufo->getRules()->getWeaponPower());
	p->setDirection(D_DOWN);
	p->setHorizontalPosition(HP_CENTER);
	p->setPosition(_currentDist - (_ufo->getRules()->getRadius() / 2));
	_projectiles.push_back(p);
	_game->getMod()->getSound("GEO.CAT", Mod::UFO_FIRE)->play();
}

/**
 * Sets the craft to the minimum distance
 * required to fire a weapon.
 */
void DogfightState::minimumDistance()
{
	int max = 0;
	for (std::vector<CraftWeapon*>::iterator i = _craft->getWeapons()->begin(); i < _craft->getWeapons()->end(); ++i)
	{
		if (*i == 0)
			continue;
		if ((*i)->getRules()->getRange() > max && (*i)->getAmmo() > 0)
		{
			max = (*i)->getRules()->getRange();
		}
	}
	if (max == 0)
	{
		_targetDist = STANDOFF_DIST;
	}
	else
	{
		_targetDist = max * 8;
	}
}

/**
 * Sets the craft to the maximum distance
 * required to fire a weapon.
 */
void DogfightState::maximumDistance()
{
	int min = 1000;
	for (std::vector<CraftWeapon*>::iterator i = _craft->getWeapons()->begin(); i < _craft->getWeapons()->end(); ++i)
	{
		if (*i == 0)
			continue;
		if ((*i)->getRules()->getRange() < min && (*i)->getAmmo() > 0)
		{
			min = (*i)->getRules()->getRange();
		}
	}
	if (min == 1000)
	{
		_targetDist = STANDOFF_DIST;
	}
	else
	{
		_targetDist = min * 8;
	}
}

/**
 * Updates the status text and restarts
 * the text timeout counter.
 * @param status New status text.
 */
void DogfightState::setStatus(const std::string &status)
{
	_txtStatus->setText(tr(status));
	_timeout = 50;
}

/**
 * Minimizes the dogfight window.
 * @param action Pointer to an action.
 */
void DogfightState::btnMinimizeClick(Action *)
{
	if (!_ufo->isCrashed() && !_craft->isDestroyed() && !_ufoBreakingOff)
	{
		if (_currentDist >= STANDOFF_DIST)
		{
			setMinimized(true);
			_window->setVisible(false);
			_preview->setVisible(false);
			_btnStandoff->setVisible(false);
			_btnCautious->setVisible(false);
			_btnStandard->setVisible(false);
			_btnAggressive->setVisible(false);
			_btnDisengage->setVisible(false);
			_btnUfo->setVisible(false);
			_btnMinimize->setVisible(false);
			_battle->setVisible(false);
			_weapon1->setVisible(false);
			_range1->setVisible(false);
			_weapon2->setVisible(false);
			_range2->setVisible(false);
			_damage->setVisible(false);
			_txtAmmo1->setVisible(false);
			_txtAmmo2->setVisible(false);
			_txtDistance->setVisible(false);
			_preview->setVisible(false);
			_txtStatus->setVisible(false);
			_btnMinimizedIcon->setVisible(true);
			_txtInterceptionNumber->setVisible(true);
		}
		else
		{
			setStatus("STR_MINIMISE_AT_STANDOFF_RANGE_ONLY");
		}
	}
}

/**
 * Switches to Standoff mode (maximum range).
 * @param action Pointer to an action.
 */
void DogfightState::btnStandoffPress(Action *)
{
	if (!_ufo->isCrashed() && !_craft->isDestroyed() && !_ufoBreakingOff)
	{
		_end = false;
		setStatus("STR_STANDOFF");
		_targetDist = STANDOFF_DIST;
	}
}

/**
 * Switches to Cautious mode (maximum weapon range).
 * @param action Pointer to an action.
 */
void DogfightState::btnCautiousPress(Action *)
{
	if (!_ufo->isCrashed() && !_craft->isDestroyed() && !_ufoBreakingOff)
	{
		_end = false;
		setStatus("STR_CAUTIOUS_ATTACK");
		if (_craft->getRules()->getWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
		{
			_w1FireInterval = _craft->getWeapons()->at(0)->getRules()->getCautiousReload();
		}
		if (_craft->getRules()->getWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
		{
			_w2FireInterval = _craft->getWeapons()->at(1)->getRules()->getCautiousReload();
		}
		minimumDistance();
	}
}

/**
 * Switches to Standard mode (minimum weapon range).
 * @param action Pointer to an action.
 */
void DogfightState::btnStandardPress(Action *)
{
	if (!_ufo->isCrashed() && !_craft->isDestroyed() && !_ufoBreakingOff)
	{
		_end = false;
		setStatus("STR_STANDARD_ATTACK");
		if (_craft->getRules()->getWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
		{
			_w1FireInterval = _craft->getWeapons()->at(0)->getRules()->getStandardReload();
		}
		if (_craft->getRules()->getWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
		{
			_w2FireInterval = _craft->getWeapons()->at(1)->getRules()->getStandardReload();
		}
		maximumDistance();
	}
}

/**
 * Switches to Aggressive mode (minimum range).
 * @param action Pointer to an action.
 */
void DogfightState::btnAggressivePress(Action *)
{
	if (!_ufo->isCrashed() && !_craft->isDestroyed() && !_ufoBreakingOff)
	{
		_end = false;
		setStatus("STR_AGGRESSIVE_ATTACK");
		if (_craft->getRules()->getWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
		{
			_w1FireInterval = _craft->getWeapons()->at(0)->getRules()->getAggressiveReload();
		}
		if (_craft->getRules()->getWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
		{
			_w2FireInterval = _craft->getWeapons()->at(1)->getRules()->getAggressiveReload();
		}
		_targetDist = 64;
	}
}

/**
 * Disengages from the UFO.
 * @param action Pointer to an action.
 */
void DogfightState::btnDisengagePress(Action *)
{
	if (!_ufo->isCrashed() && !_craft->isDestroyed() && !_ufoBreakingOff)
	{
		_end = true;
		setStatus("STR_DISENGAGING");
		_targetDist = 800;
	}
}

/**
 * Shows a front view of the UFO.
 * @param action Pointer to an action.
 */
void DogfightState::btnUfoClick(Action *)
{
	_preview->setVisible(true);
	// Disable all other buttons to prevent misclicks
	_btnStandoff->setVisible(false);
	_btnCautious->setVisible(false);
	_btnStandard->setVisible(false);
	_btnAggressive->setVisible(false);
	_btnDisengage->setVisible(false);
	_btnUfo->setVisible(false);
	_btnMinimize->setVisible(false);
	_weapon1->setVisible(false);
	_weapon2->setVisible(false);
}

/**
 * Hides the front view of the UFO.
 * @param action Pointer to an action.
 */
void DogfightState::previewClick(Action *)
{
	_preview->setVisible(false);
	// Reenable all other buttons to prevent misclicks
	_btnStandoff->setVisible(true);
	_btnCautious->setVisible(true);
	_btnStandard->setVisible(true);
	_btnAggressive->setVisible(true);
	_btnDisengage->setVisible(true);
	_btnUfo->setVisible(true);
	_btnMinimize->setVisible(true);
	_weapon1->setVisible(true);
	_weapon2->setVisible(true);
}

/*
 * Draws the UFO blob on the radar screen.
 * Currently works only for original sized blobs
 * 13 x 13 pixels.
 */
void DogfightState::drawUfo()
{
	if (_ufoSize < 0 || _ufo->isDestroyed())
	{
		return;
	}
	int currentUfoXposition =  _battle->getWidth() / 2 - 6;
	int currentUfoYposition = _battle->getHeight() - (_currentDist / 8) - 6;
	for (int y = 0; y < 13; ++y)
	{
		for (int x = 0; x < 13; ++x)
		{
			Uint8 pixelOffset = _ufoBlobs[_ufoSize + _ufo->getHitFrame()][y][x];
			if (pixelOffset == 0)
			{
				continue;
			}
			else
			{
				if (_ufo->isCrashed() || _ufo->getHitFrame() > 0)
				{
					pixelOffset *= 2;
				}
				Uint8 radarPixelColor = _window->getPixel(currentUfoXposition + x + 3, currentUfoYposition + y + 3); // + 3 cause of the window frame
				Uint8 color = radarPixelColor - pixelOffset;
				if (color < _colors[BLOB_MIN])
				{
					color = _colors[BLOB_MIN];
				}
				_battle->setPixel(currentUfoXposition + x, currentUfoYposition + y, color);
			}
		}
	}
}

/*
 * Draws projectiles on the radar screen.
 * Depending on what type of projectile it is, it's
 * shape will be different. Currently works for
 * original sized blobs 3 x 6 pixels.
 */
void DogfightState::drawProjectile(const CraftWeaponProjectile* p)
{
	int xPos = _battle->getWidth() / 2 + p->getHorizontalPosition();
	// Draw missiles.
	if (p->getGlobalType() == CWPGT_MISSILE)
	{
		xPos -= 1;
		int yPos = _battle->getHeight() - p->getPosition() / 8;
		for (int x = 0; x < 3; ++x)
		{
			for (int y = 0; y < 6; ++y)
			{
				int pixelOffset = _projectileBlobs[p->getType()][y][x];
				if (pixelOffset == 0)
				{
					continue;
				}
				else
				{
					Uint8 radarPixelColor = _window->getPixel(xPos + x + 3, yPos + y + 3); // + 3 cause of the window frame
					Uint8 color = radarPixelColor - pixelOffset;
					if (color < _colors[BLOB_MIN])
					{
						color = _colors[BLOB_MIN];
					}
					_battle->setPixel(xPos + x, yPos + y, color);
				}
			}
		}
	}
	// Draw beams.
	else if (p->getGlobalType() == CWPGT_BEAM)
	{
		int yStart = _battle->getHeight() - 2;
		int yEnd = _battle->getHeight() - (_currentDist / 8);
		Uint8 pixelOffset = p->getState();
		for (int y = yStart; y > yEnd; --y)
		{
			Uint8 radarPixelColor = _window->getPixel(xPos + 3, y + 3);
			Uint8 color = radarPixelColor - pixelOffset;
			if (color < _colors[BLOB_MIN])
			{
				color = _colors[BLOB_MIN];
			}
			_battle->setPixel(xPos, y, color);
		}
	}
}

/**
 * Toggles usage of weapon number 1.
 * @param action Pointer to an action.
 */
void DogfightState::weapon1Click(Action *)
{
	_weapon1Enabled = !_weapon1Enabled;
	recolor(0, _weapon1Enabled);
}

/**
 * Toggles usage of weapon number 2.
 * @param action Pointer to an action.
 */
void DogfightState::weapon2Click(Action *)
{
	_weapon2Enabled = !_weapon2Enabled;
	recolor(1, _weapon2Enabled);
}

/**
 * Changes colors of weapon icons, range indicators and ammo texts base on current weapon state.
 * @param weaponNo - number of weapon for which colors must be changed.
 * @param currentState - state of weapon (enabled = true, disabled = false).
 */
void DogfightState::recolor(const int weaponNo, const bool currentState)
{
	InteractiveSurface *weapon = 0;
	Text *ammo = 0;
	Surface *range = 0;
	if (weaponNo == 0)
	{
		weapon = _weapon1;
		ammo = _txtAmmo1;
		range = _range1;
	}
	else if (weaponNo == 1)
	{
		weapon = _weapon2;
		ammo = _txtAmmo2;
		range = _range2;
	}
	else
	{
		return;
	}

	if (currentState)
	{
		weapon->offset(-_colors[DISABLED_WEAPON]);
		ammo->offset(-_colors[DISABLED_AMMO]);
		range->offset(-_colors[DISABLED_RANGE]);
	}
	else
	{
		weapon->offset(_colors[DISABLED_WEAPON]);
		ammo->offset(_colors[DISABLED_AMMO]);
		range->offset(_colors[DISABLED_RANGE]);
	}
}

/**
 * Returns true if state is minimized. Otherwise returns false.
 * @return Is the dogfight minimized?
 */
bool DogfightState::isMinimized() const
{
	return _minimized;
}

/**
 * Sets the state to minimized/maximized status.
 * @param minimized Is the dogfight minimized?
 */
void DogfightState::setMinimized(const bool minimized)
{
	_minimized = minimized;
}

/**
 * Maximizes the interception window.
 * @param action Pointer to an action.
 */
void DogfightState::btnMinimizedIconClick(Action *)
{
	if (_craft->getDestination()->getSiteDepth() > _craft->getRules()->getMaxDepth())
	{
		_state->popup(new DogfightErrorState(_craft, tr("STR_UNABLE_TO_ENGAGE_DEPTH")));
	}
	else
	{
		bool underwater = !_craft->getWeapons()->empty();
		for (std::vector<CraftWeapon*>::iterator w = _craft->getWeapons()->begin(); w != _craft->getWeapons()->end(); ++w)
		{
			if ((*w) && !(*w)->getRules()->isWaterOnly())
			{
				underwater = false;
				break;
			}
		}
		if (underwater && !_state->getGlobe()->insideLand(_craft->getLongitude(), _craft->getLatitude()))
		{
			_state->popup(new DogfightErrorState(_craft, tr("STR_UNABLE_TO_ENGAGE_AIRBORNE")));
		}
		else
		{
			setMinimized(false);
			_window->setVisible(true);
			_btnStandoff->setVisible(true);
			_btnCautious->setVisible(true);
			_btnStandard->setVisible(true);
			_btnAggressive->setVisible(true);
			_btnDisengage->setVisible(true);
			_btnUfo->setVisible(true);
			_btnMinimize->setVisible(true);
			_battle->setVisible(true);
			_weapon1->setVisible(true);
			_range1->setVisible(true);
			_weapon2->setVisible(true);
			_range2->setVisible(true);
			_damage->setVisible(true);
			_txtAmmo1->setVisible(true);
			_txtAmmo2->setVisible(true);
			_txtDistance->setVisible(true);
			_txtStatus->setVisible(true);
			_btnMinimizedIcon->setVisible(false);
			_txtInterceptionNumber->setVisible(false);
			_preview->setVisible(false);			
		}
	}
}

/**
 * Sets interception number. Used to draw proper number when window minimized.
 * @param number ID number.
 */
void DogfightState::setInterceptionNumber(const int number)
{
	_interceptionNumber = number;
}

/**
 * Sets interceptions count. Used to properly position the window.
 * @param count Amount of interception windows.
 */
void DogfightState::setInterceptionsCount(const size_t count)
{
	_interceptionsCount = count;
	calculateWindowPosition();
	moveWindow();
}

/**
 * Calculates dogfight window position according to
 * number of active interceptions.
 */
void DogfightState::calculateWindowPosition()
{

	_minimizedIconX = 5;
	_minimizedIconY = (5 * _interceptionNumber) + (16 * (_interceptionNumber - 1));

	if (_interceptionsCount == 1)
	{
		_x = 80;
		_y = 52;
	}
	else if (_interceptionsCount == 2)
	{
		if (_interceptionNumber == 1)
		{
			_x = 80;
			_y = 0;
		}
		else // 2
		{
			_x = 80;
			//_y = (_game->getScreen()->getHeight() / 2) - 96;
			_y = 200 - _window->getHeight();//96;
		}
	}
	else if (_interceptionsCount == 3)
	{
		if (_interceptionNumber == 1)
		{
			_x = 80;
			_y = 0;
		}
		else if (_interceptionNumber == 2)
		{
			_x = 0;
			//_y = (_game->getScreen()->getHeight() / 2) - 96;
			_y = 200 - _window->getHeight();//96;
		}
		else // 3
		{
			//_x = (_game->getScreen()->getWidth() / 2) - 160;
			//_y = (_game->getScreen()->getHeight() / 2) - 96;
			_x = 320 - _window->getWidth();//160;
			_y = 200 - _window->getHeight();//96;
		}
	}
	else
	{
		if (_interceptionNumber == 1)
		{
			_x = 0;
			_y = 0;
		}
		else if (_interceptionNumber == 2)
		{
			//_x = (_game->getScreen()->getWidth() / 2) - 160;
			_x = 320 - _window->getWidth();//160;
			_y = 0;
		}
		else if (_interceptionNumber == 3)
		{
			_x = 0;
			//_y = (_game->getScreen()->getHeight() / 2) - 96;
			_y = 200 - _window->getHeight();//96;
		}
		else // 4
		{
			//_x = (_game->getScreen()->getWidth() / 2) - 160;
			//_y = (_game->getScreen()->getHeight() / 2) - 96;
			_x = 320 - _window->getWidth();//160;
			_y = 200 - _window->getHeight();//96;
		}
	}
	_x += _game->getScreen()->getDX();
	_y += _game->getScreen()->getDY();
}

/**
 * Relocates all dogfight window elements to
 * calculated position. This is used when multiple
 * interceptions are running.
 */
void DogfightState::moveWindow()
{
	int x = _window->getX() - _x;
	int y = _window->getY() - _y;
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		(*i)->setX((*i)->getX() - x);
		(*i)->setY((*i)->getY() - y);
	}
	_btnMinimizedIcon->setX(_minimizedIconX); _btnMinimizedIcon->setY(_minimizedIconY);
	_txtInterceptionNumber->setX(_minimizedIconX + 18); _txtInterceptionNumber->setY(_minimizedIconY + 6);
}

/**
 * Checks whether the dogfight should end.
 * @return Returns true if the dogfight should end, otherwise returns false.
 */
bool DogfightState::dogfightEnded() const
{
	return _endDogfight;
}

/**
 * Returns the UFO associated to this dogfight.
 * @return Returns pointer to UFO object associated to this dogfight.
 */
Ufo* DogfightState::getUfo() const
{
	return _ufo;
}

/**
 * Ends the dogfight.
 */
void DogfightState::endDogfight()
{
	if (_craft)
		_craft->setInDogfight(false);
	_endDogfight = true;
}

/**
 * Returns interception number.
 * @return interception number
 */
int DogfightState::getInterceptionNumber() const
{
	return _interceptionNumber;
}

}
