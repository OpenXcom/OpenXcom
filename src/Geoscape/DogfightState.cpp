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
#include "DogfightState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Engine/Screen.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Surface.h"
#include "../Interface/ImageButton.h"
#include "../Interface/Text.h"
#include "../Engine/Timer.h"
#include "Globe.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Savegame/Ufo.h"
#include "../Ruleset/RuleUfo.h"
#include "../Engine/Music.h"
#include "../Engine/RNG.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Savegame/Base.h"
#include "../Savegame/CraftWeaponProjectile.h"
#include <cstdlib>

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
		/*2 STR_MEDIUM */
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
 * @param globe Pointer to the Geoscape globe.
 * @param craft Pointer to the craft intercepting.
 * @param ufo Pointer to the UFO being intercepted.
 */
DogfightState::DogfightState(Game *game, Globe *globe, Craft *craft, Ufo *ufo) : State(game), _globe(globe), _craft(craft), _ufo(ufo), _timeout(50), _currentDist(640), _targetDist(560), _end(false), _destroyUfo(false), _destroyCraft(false), _ufoBreakingOff(false), _ufoSize(0), _craftHeight(0), _currentCraftDamageColor(13), _weapon1Enabled(true), _weapon2Enabled(true)
{
	_screen = false;

	// Create objects
	_window = new Surface(160, 96, 80, 52);
	_battle = new Surface(77, 74, 83, 55);
	_weapon1 = new InteractiveSurface(15, 17, 84, 104);
	_range1 = new Surface(21, 74, 99, 55);
	_weapon2 = new InteractiveSurface(15, 17, 144, 104);
	_range2 = new Surface(21, 74, 123, 55);
	_damage = new Surface(22, 25, 173, 92);

	_btnMinimize = new InteractiveSurface(12, 12, 80, 52);
	_preview = new InteractiveSurface(160, 96, 80, 52);
	_btnStandoff = new ImageButton(36, 15, 163, 56);
	_btnCautious = new ImageButton(36, 15, 200, 56);
	_btnStandard = new ImageButton(36, 15, 163, 72);
	_btnAggressive = new ImageButton(36, 15, 200, 72);
	_btnDisengage = new ImageButton(36, 15, 200, 88);
	_btnUfo = new ImageButton(36, 17, 200, 104);
	_txtAmmo1 = new Text(16, 9, 84, 122);
	_txtAmmo2 = new Text(16, 9, 144, 122);
	_txtDistance = new Text(40, 9, 196, 124);
	_txtStatus = new Text(150, 9, 84, 137);

	_animTimer = new Timer(30);
	_moveTimer = new Timer(20);
	_w1Timer = new Timer(0);
	_w2Timer = new Timer(0);
	_mode = _btnStandoff;
	_ufoWtimer = new Timer(0);
	_ufoEscapeTimer = new Timer(0);
	_craftDamageAnimTimer = new Timer(500);

	add(_window);
	add(_battle);
	add(_weapon1);
	add(_range1);
	add(_weapon2);
	add(_range2);
	add(_damage);
	add(_btnMinimize);
	add(_btnStandoff);
	add(_btnCautious);
	add(_btnStandard);
	add(_btnAggressive);
	add(_btnDisengage);
	add(_btnUfo);
	add(_txtAmmo1);
	add(_txtAmmo2);
	add(_txtDistance);
	add(_preview);
	add(_txtStatus);

	// Set up objects
	Surface *graphic = _game->getResourcePack()->getSurface("INTERWIN.DAT");
	graphic->setX(0);
	graphic->setY(0);
	graphic->getCrop()->x = 0;
	graphic->getCrop()->y = 0;
	graphic->getCrop()->w = 160;
	graphic->getCrop()->h = 96;
	_window->drawRect(graphic->getCrop(), 15);
	graphic->blit(_window);

	_preview->drawRect(graphic->getCrop(), 15);
	graphic->getCrop()->y = 96;
	graphic->getCrop()->h = 15;
	graphic->blit(_preview);
	graphic->setY(67);
	graphic->getCrop()->y = 111;
	graphic->getCrop()->h = 29;
	graphic->blit(_preview);
	graphic->setY(15);
	graphic->getCrop()->y = 140 + 52 * _ufo->getRules()->getSprite();
	graphic->getCrop()->h = 52;
	graphic->blit(_preview);
	_preview->setVisible(false);
	_preview->onMouseClick((ActionHandler)&DogfightState::previewClick);

	_btnMinimize->onMouseClick((ActionHandler)&DogfightState::btnMinimizeClick);

	_btnStandoff->copy(_window);
	_btnStandoff->setColor(Palette::blockOffset(5)+1);
	_btnStandoff->setGroup(&_mode);
	_btnStandoff->onMouseClick((ActionHandler)&DogfightState::btnStandoffClick);

	_btnCautious->copy(_window);
	_btnCautious->setColor(Palette::blockOffset(5)+1);
	_btnCautious->setGroup(&_mode);
	_btnCautious->onMouseClick((ActionHandler)&DogfightState::btnCautiousClick);

	_btnStandard->copy(_window);
	_btnStandard->setColor(Palette::blockOffset(5)+1);
	_btnStandard->setGroup(&_mode);
	_btnStandard->onMouseClick((ActionHandler)&DogfightState::btnStandardClick);

	_btnAggressive->copy(_window);
	_btnAggressive->setColor(Palette::blockOffset(5)+1);
	_btnAggressive->setGroup(&_mode);
	_btnAggressive->onMouseClick((ActionHandler)&DogfightState::btnAggressiveClick);

	_btnDisengage->copy(_window);
	_btnDisengage->setColor(Palette::blockOffset(5)+1);
	_btnDisengage->onMouseClick((ActionHandler)&DogfightState::btnDisengageClick);
	_btnDisengage->setGroup(&_mode);

	_btnUfo->copy(_window);
	_btnUfo->setColor(Palette::blockOffset(5)+1);
	_btnUfo->onMouseClick((ActionHandler)&DogfightState::btnUfoClick);

	_txtAmmo1->setColor(Palette::blockOffset(5)+9);

	_txtAmmo2->setColor(Palette::blockOffset(5)+9);

	_txtDistance->setColor(Palette::blockOffset(5)+9);
	_txtDistance->setText(L"640");

	_txtStatus->setColor(Palette::blockOffset(5)+9);
	_txtStatus->setText(_game->getLanguage()->getString("STR_STANDOFF"));

	SurfaceSet *set = _game->getResourcePack()->getSurfaceSet("INTICON.PCK");

	for (int i = 0; i < _craft->getRules()->getWeapons(); ++i)
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
		Surface *frame = set->getFrame(w->getRules()->getSprite() + 5);
		
		frame->setX(0);
		frame->setY(0);
		frame->blit(weapon);

		// Draw ammo
		std::wstringstream ss;
		ss << w->getAmmo();
		ammo->setText(ss.str());

		// Draw range (1 km = 1 pixel)
		Uint8 color = Palette::blockOffset(7) - 1;
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

	Surface *frame = set->getFrame(_craft->getRules()->getSprite() + 11);
	frame->setX(0);
	frame->setY(0);
	frame->blit(_damage);

	_animTimer->onTimer((StateHandler)&DogfightState::animate);
	_animTimer->start();

	_moveTimer->onTimer((StateHandler)&DogfightState::move);
	_moveTimer->start();

	_w1Timer->onTimer((StateHandler)&DogfightState::fireWeapon1);

	_w2Timer->onTimer((StateHandler)&DogfightState::fireWeapon2);

	_ufoWtimer->onTimer((StateHandler)&DogfightState::ufoFireWeapon);
	_ufoFireInterval = (_ufo->getRules()->getWeaponReload() - 2 * _game->getSavedGame()->getDifficulty()) * 75;
	_ufoFireInterval = RNG::generate(0, _ufoFireInterval) + _ufoFireInterval;
	_ufoWtimer->setInterval(_ufoFireInterval);

	_ufoEscapeTimer->onTimer((StateHandler)&DogfightState::ufoBreakOff);
	int ufoBreakOffInterval = (_ufo->getRules()->getBreakOffTime() + RNG::generate(1, _ufo->getRules()->getBreakOffTime()) - 30 * _game->getSavedGame()->getDifficulty()) * 20;
	_ufoEscapeTimer->setInterval(ufoBreakOffInterval);

	_craftDamageAnimTimer->onTimer((StateHandler)&DogfightState::animateCraftDamage);

	// Set UFO size - going to be moved to Ufo class to implement simultanous dogfights.
	std::string ufoSize = _ufo->getRules()->getSize();
	if(ufoSize.compare("STR_VERY_SMALL") == 0)
	{
		_ufoSize = 0;
	}
	else if(ufoSize.compare("STR_SMALL") == 0)
	{
		_ufoSize = 1;
	}
	else if(ufoSize.compare("STR_MEDIUM") == 0)
	{
		_ufoSize = 2;
	}
	else if(ufoSize.compare("STR_LARGE") == 0)
	{
		_ufoSize = 3;
	}
	else
	{
		_ufoSize = 4;
	}

	// Get crafts height. Used for damage indication.
	int x =_damage->getWidth() / 2;
	for(int y = 0; y < _damage->getHeight(); ++y)
	{
		Uint8 pixelColor = _damage->getPixel(x, y);
		if(pixelColor >= Palette::blockOffset(10) || pixelColor < Palette::blockOffset(11))
		{
			++_craftHeight;
		}
	}

	// Used for weapon toggling.
	_weapon1->onMouseClick((ActionHandler)&DogfightState::weapon1Click);
	_weapon2->onMouseClick((ActionHandler)&DogfightState::weapon2Click);

	// Set music
	_game->getResourcePack()->getMusic("GMINTER")->play();
}

/**
 * Deletes timers.
 */
DogfightState::~DogfightState()
{
	delete _animTimer;
	delete _moveTimer;
	delete _w1Timer;
	delete _w2Timer;
	delete _ufoWtimer;
	delete _ufoEscapeTimer;
	while(!_projectiles.empty())
	{
		delete _projectiles.back();
		_projectiles.pop_back();
	}
}

/**
 * Runs the dogfighter timers.
 */
void DogfightState::think()
{
	_animTimer->think(this, 0);
	_moveTimer->think(this, 0);
	_w1Timer->think(this, 0);
	_w2Timer->think(this, 0);
	_ufoWtimer->think(this, 0);
	_ufoEscapeTimer->think(this, 0);
	_craftDamageAnimTimer->think(this, 0);
}

/**
 * Animates interceptor damage by changing the color and redrawing the image.
 */
void DogfightState::animateCraftDamage()
{
	--_currentCraftDamageColor;
	if(_currentCraftDamageColor < 13)
	{
		_currentCraftDamageColor = 14;
	}
	drawCraftDamage();
}

/**
 * Draws interceptor damage according to percentage of HP's left.
 */
void DogfightState::drawCraftDamage()
{
	if(_craft->getDamagePercentage() != 0)
	{
		int damagePercentage = _craft->getDamagePercentage();
		int rowsToColor = (int)floor((double)_craftHeight * (double)(damagePercentage / 100.));
		if(rowsToColor == 0)
		{
			return;
		}
		int rowsColored = 0;
		bool rowColored = false;
		for(int y = 0; y < _damage->getHeight(); ++y)
		{
			rowColored = false;
			for(int x = 0; x < _damage->getWidth(); ++x)
			{
				int pixelColor = _damage->getPixel(x, y);
				if(pixelColor == 13 || pixelColor == 14)
				{
					_damage->setPixel(x, y, _currentCraftDamageColor);
					rowColored = true;
				}
				if(pixelColor >= Palette::blockOffset(10) && pixelColor < Palette::blockOffset(11))
				{
					_damage->setPixel(x, y, _currentCraftDamageColor);
					rowColored = true;
				}
			}
			if(rowColored)
			{
				++rowsColored;
			}
			if(rowsColored == rowsToColor)
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
	for(int x = 0; x < _window->getWidth(); ++x)
	{
		for(int y = 0; y < _window->getHeight(); ++y)
		{
			Uint8 radarPixelColor = _window->getPixel(x, y);
			if(radarPixelColor >= Palette::blockOffset(7) && radarPixelColor < Palette::blockOffset(7) + 16)
			{
				++radarPixelColor;
				if(radarPixelColor >= Palette::blockOffset(7) + 16)
				{
					radarPixelColor = Palette::blockOffset(7);
				}
				_window->setPixel(x, y, radarPixelColor);
			}
		}
	}

	_battle->clear();

	// Draw UFO.
	if(!_ufo->isDestroyed())
	{
		drawUfo();
	}

	// Draw projectiles.
	for(std::vector<CraftWeaponProjectile*>::iterator it = _projectiles.begin(); it != _projectiles.end(); ++it)
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
	if(_ufo->getHit() > 0)
	{
		_ufo->setHit(_ufo->getHit() - 1);
		if(_ufo->getHit() == 0)
		{
			lastHitAnimFrame = true;
		}
	}

	// Animate UFO crash landing.
	if(_ufo->isCrashed() && _ufo->getHit() == 0 && !lastHitAnimFrame)
	{
		--_ufoSize;
	}
}

/**
 * Moves the craft towards the UFO according to
 * the current interception mode.
 */
void DogfightState::move()
{
	// Check if UFO is not breaking off.
	if(_ufo->getSpeed() == _ufo->getRules()->getMaxSpeed())
	{
		_craft->setSpeed(_craft->getRules()->getMaxSpeed());
		// Crappy craft is chasing UFO.
		if(_ufo->getSpeed() > _craft->getSpeed())
		{
			_ufoBreakingOff = true;
			setStatus("STR_UFO_OUTRUNNING_INTERCEPTOR");
		}
	}

	// Update distance
	if(!_ufoBreakingOff)
	{
		if (_currentDist < _targetDist && !_ufo->isCrashed() && !_craft->isDestroyed())
		{
			_currentDist += 4;
		}
		else if (_currentDist > _targetDist && !_ufo->isCrashed() && !_craft->isDestroyed())
		{
			_currentDist -= 2;
		}
	}
	else
	{
		_currentDist += 4;
	}

	std::wstringstream ss;
	ss << _currentDist;
	_txtDistance->setText(ss.str());

	// Move projectiles and check for hits.
	for(std::vector<CraftWeaponProjectile*>::iterator it = _projectiles.begin(); it != _projectiles.end(); ++it)
	{
		CraftWeaponProjectile *p = (*it);
		p->move();
		// Projectiles fired by interceptor.
		if(p->getDirection() == D_UP)
		{
			// Projectile reached the UFO - determine if it's been hit.
			if((p->getPosition() >= _currentDist) || (p->getGlobalType() == CWPGT_BEAM && p->toBeRemoved()) && !_ufo->isCrashed())
			{
				int acc = RNG::generate(1, 100);
				// UFO hit.
				if (acc <= p->getAccuracy())
				{
					// Formula delivered by Volutar
					int damage = RNG::generate(p->getDamage() / 2, p->getDamage());
					_ufo->setDamage(_ufo->getDamage() + damage);
					if(_ufo->getHit() == 0)
					{
						_ufo->setHit(3);
					}
					setStatus("STR_UFO_HIT");
					_currentRadius += 4;
					_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(12)->play(); //12
					p->remove();
				}
				// Missed.
				else
				{
					if(p->getGlobalType() == CWPGT_BEAM)
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
			if(p->getMissed() && ((p->getPosition() / 8) >= p->getRange()))
			{
				p->remove();
			}
		}
		// Projectiles fired by UFO.
		else if(p->getDirection() == D_DOWN)
		{
			if(p->getGlobalType() == CWPGT_MISSILE || (p->getGlobalType() == CWPGT_BEAM && p->toBeRemoved()))
			{
				int acc = RNG::generate(1, 100);
				if(acc <= p->getAccuracy())
				{
					// Formula delivered by Volutar
					int damage = RNG::generate(0, _ufo->getRules()->getWeaponPower());
					_craft->setDamage(_craft->getDamage() + damage);
					if(!_craftDamageAnimTimer->isRunning())
					{
						_craftDamageAnimTimer->start();
					}
					drawCraftDamage();
					setStatus("STR_INTERCEPTOR_DAMAGED");
					_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(10)->play(); //10
				}
				p->remove();
			}
		}
	}
		
	// Remove projectiles that hit or missed their target.
	for(std::vector<CraftWeaponProjectile*>::iterator it = _projectiles.begin(); it != _projectiles.end();)
	{
		if((*it)->toBeRemoved() == true || ((*it)->getMissed() == true && (*it)->getPosition() <= 0))
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
	for (int i = 0; i < _craft->getRules()->getWeapons(); ++i)
	{
		CraftWeapon *w = _craft->getWeapons()->at(i);
		if (w == 0)
		{
			continue;
		}
		Timer *wTimer = 0;
		if (i == 0)
		{
			wTimer = _w1Timer;
		}
		else
		{
			wTimer = _w2Timer;
		}

		// Handle weapon firing
		if (!wTimer->isRunning() && _currentDist <= w->getRules()->getRange() * 8 && w->getAmmo() > 0 && _mode != _btnStandoff 
			&& _mode != _btnDisengage && !_ufo->isCrashed() && !_craft->isDestroyed())
		{
			wTimer->start();
			if (i == 0)
			{
				fireWeapon1();
			}
			else
			{
				fireWeapon2();
			}
		}
		else if (wTimer->isRunning() && (_currentDist > w->getRules()->getRange() * 8 || w->getAmmo() == 0 || _mode == _btnStandoff
			|| _mode == _btnDisengage || _ufo->isCrashed() || _craft->isDestroyed()))
		{
			wTimer->stop();
			// Handle craft distance according to option set by user and available ammo.
			if (w->getAmmo() == 0 && !_craft->isDestroyed())
			{
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
	}

	// Handle UFO firing.
	if(!_ufoWtimer->isRunning() && _currentDist <= _ufo->getRules()->getWeaponRange() * 8 && !_ufo->isCrashed() && !_craft->isDestroyed())
	{
		_ufoWtimer->start();
		ufoFireWeapon();
	}
	else if(_ufoWtimer->isRunning() && (_currentDist > _ufo->getRules()->getWeaponRange() * 8 || _ufo->isCrashed() || _craft->isDestroyed()))
	{
		_ufoWtimer->stop();
	}

	// Check when battle is over.
	if ((_currentDist > 640 && (_mode == _btnDisengage || _ufoBreakingOff == true)) || (_timeout == 0 && (_ufo->isCrashed() || _craft->isDestroyed())))
	{
		if(_ufoBreakingOff)
		{
			_ufo->move();
			_craft->setDestination(_ufo);
		}
		if(_destroyCraft)
		{
			// Remove the craft.
			for(std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end(); ++b)
			{
				for(std::vector<Craft*>::iterator c = (*b)->getCrafts()->begin(); c != (*b)->getCrafts()->end(); ++c)
				{
					if(*c == _craft)
					{
						delete *c;
						(*b)->getCrafts()->erase(c);
						break;
					}
				}
			}
		}

		if (_destroyUfo)
		{
			_craft->returnToBase();
			// Disengage any other craft.
			while (_ufo->getFollowers()->size() > 0)
			{
				for (std::vector<Target*>::iterator i = _ufo->getFollowers()->begin(); i != _ufo->getFollowers()->end(); ++i)
				{
					Craft *c = dynamic_cast<Craft*>(*i);
					if (c)
					{
						c->returnToBase();
						break;
					}
				}
			}

			// Clear UFO
			for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end(); ++i)
			{
				if (*i == _ufo)
				{
					delete *i;
					_game->getSavedGame()->getUfos()->erase(i);
					break;
				}
			}
		}
		_game->popState();
	}

	// End dogfight if craft is destroyed.
	if(!_end && _craft->isDestroyed())
	{
		setStatus("STR_INTERCEPTOR_DESTROYED");
		_timeout += 30;
		_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(13)->play();
		_end = true;
		_destroyCraft = true;
		_ufoWtimer->stop();
		_w1Timer->stop();
		_w2Timer->stop();
	}
	
	// End dogfight if UFO is crashed or destroyed.
	if (!_end && _ufo->isCrashed())
	{
		if (_ufo->isDestroyed())
		{
			setStatus("STR_UFO_DESTROYED");
			_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(10)->play(); //11
			_destroyUfo = true;
		}
		else
		{
			setStatus("STR_UFO_CRASH_LANDS");
			_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(10)->play(); //10
			if (!_globe->insideLand(_ufo->getLongitude(), _ufo->getLatitude()))
			{
				_destroyUfo = true;
			}
			else
			{
				_ufo->setHoursCrashed(24 + RNG::generate(0, 72));
			}
		}
		_timeout += 30;
		_end = true;
		_ufo->setSpeed(0);
		_ufo->setAltitude("STR_GROUND");
	}
}

/**
 * Fires a shot from the first weapon
 * equipped on the craft.
 */
void DogfightState::fireWeapon1()
{
	if(_weapon1Enabled)
	{
		CraftWeapon *w1 = _craft->getWeapons()->at(0);
		w1->setAmmo(w1->getAmmo() - 1);

		std::wstringstream ss;
		ss << w1->getAmmo();
		_txtAmmo1->setText(ss.str());

		CraftWeaponProjectile *p = w1->fire();
		p->setDirection(D_UP);
		p->setHorizontalPosition(HP_LEFT);
		_projectiles.push_back(p);

		_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(w1->getRules()->getSound())->play();
	}
}

/**
 * Fires a shot from the second weapon
 * equipped on the craft.
 */
void DogfightState::fireWeapon2()
{
	if(_weapon2Enabled)
	{
		CraftWeapon *w2 = _craft->getWeapons()->at(1);
		w2->setAmmo(w2->getAmmo() - 1);

		std::wstringstream ss;
		ss << w2->getAmmo();
		_txtAmmo2->setText(ss.str());

		CraftWeaponProjectile *p = w2->fire();
		p->setDirection(D_UP);
		p->setHorizontalPosition(HP_RIGHT);
		_projectiles.push_back(p);

		_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(w2->getRules()->getSound())->play();
	}
}

/// Fires a shot from UFO's weapon. 
/**
 *	Each time a UFO will try to fire it's cannons
 *	a calculation is made. There's only 10% chance
 *	that it will actually fire.
 */
void DogfightState::ufoFireWeapon()
{
	_ufoFireInterval = (_ufo->getRules()->getWeaponReload() - 2 * _game->getSavedGame()->getDifficulty()) * 75;
	_ufoFireInterval = RNG::generate(0, _ufoFireInterval) + _ufoFireInterval;
	_ufoWtimer->setInterval(_ufoFireInterval);

	setStatus("STR_UFO_RETURN_FIRE");
	CraftWeaponProjectile *p = new CraftWeaponProjectile();
	p->setType("STR_PLASMA_BEAM_UC");
	p->setAccuracy(40);
	p->setDamage(_ufo->getRules()->getWeaponPower());
	p->setDirection(D_DOWN);
	p->setHorizontalPosition(HP_CENTER);
	p->setPosition(_currentDist - (_currentRadius / 2));
	_projectiles.push_back(p);
	_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(8)->play();
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
		_targetDist = 560;
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
		_targetDist = 560;
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
	_txtStatus->setText(_game->getLanguage()->getString(status));
	_timeout = 50;
}

/**
 * Minimizes the dogfight window.
 * @param action Pointer to an action.
 */
void DogfightState::btnMinimizeClick(Action *action)
{
}

/**
 * Switches to Standoff mode (maximum range).
 * @param action Pointer to an action.
 */
void DogfightState::btnStandoffClick(Action *action)
{
	if (!_ufo->isCrashed())
	{
		setStatus("STR_STANDOFF");
		_targetDist = 560;
	}
}

/**
 * Switches to Cautious mode (maximum weapon range).
 * @param action Pointer to an action.
 */
void DogfightState::btnCautiousClick(Action *action)
{
	if (!_ufo->isCrashed())
	{
		setStatus("STR_CAUTIOUS_ATTACK");
		if (_craft->getNumWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
		{
			_w1Timer->setInterval(_craft->getWeapons()->at(0)->getRules()->getCautiousReload() * 75);
		}
		if (_craft->getNumWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
		{
			_w2Timer->setInterval(_craft->getWeapons()->at(1)->getRules()->getCautiousReload() * 75);
		}
		minimumDistance();
		_ufoEscapeTimer->start();
	}
}

/**
 * Switches to Standard mode (minimum weapon range).
 * @param action Pointer to an action.
 */
void DogfightState::btnStandardClick(Action *action)
{
	if (!_ufo->isCrashed())
	{
		setStatus("STR_STANDARD_ATTACK");
		if (_craft->getNumWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
		{
			_w1Timer->setInterval(_craft->getWeapons()->at(0)->getRules()->getStandardReload() * 75);
		}
		if (_craft->getNumWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
		{
			_w2Timer->setInterval(_craft->getWeapons()->at(1)->getRules()->getStandardReload() * 75);
		}
		maximumDistance();
		_ufoEscapeTimer->start();
	}
}

/**
 * Switches to Aggressive mode (minimum range).
 * @param action Pointer to an action.
 */
void DogfightState::btnAggressiveClick(Action *action)
{
	if (!_ufo->isCrashed())
	{
		setStatus("STR_AGGRESSIVE_ATTACK");
		if (_craft->getNumWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
		{
			_w1Timer->setInterval(_craft->getWeapons()->at(0)->getRules()->getAggressiveReload() * 75);
		}
		if (_craft->getNumWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
		{
			_w2Timer->setInterval(_craft->getWeapons()->at(1)->getRules()->getAggressiveReload() * 75);
		}
		_targetDist = 64;
		_ufoEscapeTimer->start();
	}
}

/**
 * Disengages from the UFO.
 * @param action Pointer to an action.
 */
void DogfightState::btnDisengageClick(Action *action)
{
	if (!_ufo->isCrashed())
	{
		setStatus("STR_DISENGAGING");
		_targetDist = 800;
		_ufoEscapeTimer->stop();
		_craft->returnToBase();
	}
}

/**
 * Shows a front view of the UFO.
 * @param action Pointer to an action.
 */
void DogfightState::btnUfoClick(Action *action)
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
}

/**
 * Shows a front view of the UFO.
 * @param action Pointer to an action.
 */
void DogfightState::previewClick(Action *action)
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
}

/*
 * Sets UFO to break off mode. Started via timer.
 */
void DogfightState::ufoBreakOff()
{
	if(!_ufo->isCrashed() && !_ufo->isDestroyed())
	{
		_ufo->setSpeed(_ufo->getRules()->getMaxSpeed());
	}
}

/*
 * Draws the UFO blob on the radar screen.
 * Currently works only for original sized blobs
 * 13 x 13 pixels.
 */
void DogfightState::drawUfo()
{
	if(_ufoSize < 0 || _ufo->isDestroyed())
	{
		return;
	}
	int currentUfoXposition =  _battle->getWidth() / 2 - 6;
	int currentUfoYposition = _battle->getHeight() - (_currentDist / 8) - 6;
	int hitFrame = _ufo->getHit();
	for(int y = 0; y < 13; ++y)
	{
		for(int x = 0; x < 13; ++x)
		{
			Uint8 pixelOffset = _ufoBlobs[_ufoSize + hitFrame][y][x];
			if(pixelOffset == 0)
			{
				continue;
			}
			else
			{
				if(_ufo->isCrashed() || hitFrame > 0)
				{
					pixelOffset *= 2;
				}
				Uint8 radarPixelColor = _window->getPixel(currentUfoXposition + x + 3, currentUfoYposition + y + 3); // + 3 cause of the window frame
				Uint8 color = radarPixelColor - pixelOffset;
				if(color < 108)
				{
					color = 108;
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
void DogfightState::drawProjectile(const CraftWeaponProjectile* p) {
	int xPos = _battle->getWidth() / 2 + p->getHorizontalPosition();
	// Draw missiles.
	if(p->getGlobalType() == CWPGT_MISSILE)
	{
		xPos -= 1;
		int yPos = _battle->getHeight() - p->getPosition() / 8;
		for(int x = 0; x < 3; ++x)
		{
			for(int y = 0; y < 6; ++y)
			{
				int pixelOffset = _projectileBlobs[p->getType()][y][x];
				if(pixelOffset == 0)
				{
					continue;
				}
				else
				{
					Uint8 radarPixelColor = _window->getPixel(xPos + x + 3, yPos + y + 3); // + 3 cause of the window frame
					Uint8 color = radarPixelColor - pixelOffset;
					if(color < 108)
					{
						color = 108;
					}
					_battle->setPixel(xPos + x, yPos + y, color);
				}
			}
		}
	}
	// Draw beams.
	else if(p->getGlobalType() == CWPGT_BEAM)
	{
		int yStart = _battle->getHeight() - 2;
		int yEnd = _battle->getHeight() - (_currentDist / 8);
		Uint8 pixelOffset = p->getState();
		for(int y = yStart; y > yEnd; --y)
		{
			Uint8 radarPixelColor = _window->getPixel(xPos + 3, y + 3);
			Uint8 color = radarPixelColor - pixelOffset;
			if(color < 108)
			{
				color = 108;
			}
			_battle->setPixel(xPos, y, color);
		}
	}
}

/**
 * Toggles usage of weapon number 1.
 */
void DogfightState::weapon1Click(Action *action)
{
	_weapon1Enabled = !_weapon1Enabled;
	recolor(0, _weapon1Enabled);
}

/**
 * Toggles usage of weapon number 2.
 */
void DogfightState::weapon2Click(Action *action)
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
	int weaponAndAmmoOffset = 24, rangeOffset = 7;
	if(weaponNo == 0)
	{
		weapon = _weapon1;
		ammo = _txtAmmo1;
		range = _range1;
	}
	else if(weaponNo == 1)
	{
		weapon = _weapon2;
		ammo = _txtAmmo2;
		range = _range2;
	}
	else
	{
		return;
	}

	if(currentState)
	{
		weapon->offset(-weaponAndAmmoOffset);
		ammo->offset(-weaponAndAmmoOffset);
		range->offset(-rangeOffset);
	}
	else
	{
		weapon->offset(weaponAndAmmoOffset);
		ammo->offset(weaponAndAmmoOffset);
		range->offset(rangeOffset);
	}
}

}
