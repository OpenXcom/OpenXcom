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
#include "DogfightState.h"
#include <sstream>
#include "SDL_gfxPrimitives.h"
#include "Game.h"
#include "ResourcePack.h"
#include "Palette.h"
#include "Screen.h"
#include "Language.h"
#include "LangString.h"
#include "Font.h"
#include "SurfaceSet.h"
#include "Surface.h"
#include "ImageButton.h"
#include "Text.h"
#include "Timer.h"
#include "Craft.h"
#include "RuleCraft.h"
#include "CraftWeapon.h"
#include "RuleCraftWeapon.h"
#include "Ufo.h"
#include "Music.h"
#include "RNG.h"

/**
 * Initializes all the elements in the Dogfight window.
 * @param game Pointer to the core game.
 */
DogfightState::DogfightState(Game *game, Craft *craft, Ufo *ufo) : State(game), _craft(craft), _ufo(ufo), _timeout(40), _currentDist(640), _targetDist(560)
{
	_screen = false;

	// Create objects
	_window = new Surface(160, 96, 80, 52);
	_battle = new Surface(77, 74, 83, 55);
	_weapon1 = new Surface(15, 17, 84, 104);
	_range1 = new Surface(21, 74, 99, 55);
	_weapon2 = new Surface(15, 17, 144, 104);
	_range2 = new Surface(21, 74, 123, 55);
	_damage = new Surface(22, 25, 173, 92);
	_btnMinimize = new InteractiveSurface(12, 12, 80, 52);
	_btnStandoff = new ImageButton(36, 15, 163, 56);
	_btnCautious = new ImageButton(36, 15, 200, 56);
	_btnStandard = new ImageButton(36, 15, 163, 72);
	_btnAgressive = new ImageButton(36, 15, 200, 72);
	_btnDisengage = new ImageButton(36, 15, 200, 88);
	_btnUfo = new ImageButton(36, 17, 200, 104);
	_txtStatus = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 150, 9, 84, 137);
	_txtAmmo1 = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 16, 9, 84, 122);
	_txtAmmo2 = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 16, 9, 144, 122);
	_txtDistance = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 196, 124);

	_animTimer = new Timer(30);
	_moveTimer = new Timer(20);
	_mode = _btnStandoff;

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
	add(_btnAgressive);
	add(_btnDisengage);
	add(_btnUfo);
	add(_txtStatus);
	add(_txtAmmo1);
	add(_txtAmmo2);
	add(_txtDistance);
	
	// Set up objects
	Surface *graphic = _game->getResourcePack()->getSurface("INTERWIN.DAT");
	SDL_Rect crop;
	crop.x = 0;
	crop.y = 0;
	crop.w = 160;
	crop.h = 96;
	SDL_FillRect(_window->getSurface(), &crop, 15);
	graphic->setCrop(&crop);
	graphic->blit(_window);

	_btnStandoff->copy(_window);
	_btnStandoff->setColor(Palette::blockOffset(5)+4);
	_btnStandoff->setGroup(&_mode);
	_btnStandoff->onMouseClick((EventHandler)&DogfightState::btnStandoffClick);

	_btnCautious->copy(_window);
	_btnCautious->setColor(Palette::blockOffset(5)+4);
	_btnCautious->setGroup(&_mode);
	_btnCautious->onMouseClick((EventHandler)&DogfightState::btnCautiousClick);

	_btnStandard->copy(_window);
	_btnStandard->setColor(Palette::blockOffset(5)+4);
	_btnStandard->setGroup(&_mode);
	_btnStandard->onMouseClick((EventHandler)&DogfightState::btnStandardClick);

	_btnAgressive->copy(_window);
	_btnAgressive->setColor(Palette::blockOffset(5)+4);
	_btnAgressive->setGroup(&_mode);
	_btnAgressive->onMouseClick((EventHandler)&DogfightState::btnAgressiveClick);

	_btnDisengage->copy(_window);
	_btnDisengage->setColor(Palette::blockOffset(5)+4);
	_btnDisengage->onMouseClick((EventHandler)&DogfightState::btnDisengageClick);
	_btnDisengage->setGroup(&_mode);

	_btnUfo->copy(_window);
	_btnUfo->setColor(Palette::blockOffset(5)+4);

	_txtStatus->setColor(Palette::blockOffset(5)+9);
	_txtStatus->setText(_game->getResourcePack()->getLanguage()->getString(STR_STANDOFF));

	_txtAmmo1->setColor(Palette::blockOffset(5)+9);

	_txtAmmo2->setColor(Palette::blockOffset(5)+9);

	_txtDistance->setColor(Palette::blockOffset(5)+9);
	_txtDistance->setText("640");

	SurfaceSet *set = _game->getResourcePack()->getSurfaceSet("INTICON.PCK");

	if (_craft->getRules()->getWeapons() > 0 && _craft->getWeapons()->at(0) != 0)
	{
		// Draw weapon icon
		CraftWeapon *w1 = _craft->getWeapons()->at(0);

		Surface *frame = set->getFrame(w1->getRules()->getSprite() + 5);
		frame->setX(0);
		frame->setY(0);
		frame->blit(_weapon1);

		// Draw ammo
		stringstream ss;
		ss << w1->getAmmo();
		_txtAmmo1->setText(ss.str());

		// Draw range (1 km = 1 pixel)
		Uint8 color = Palette::blockOffset(7) - 1;
		_range1->lock();

		int rangeY = _range1->getHeight() - w1->getRules()->getRange(), connectY = 57;
		for (int x = 2; x <= 20; x += 2)
		{
			_range1->setPixel(x, rangeY, color);
		}

		int minY, maxY;
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
		for (int y = minY; y <= maxY; y++)
		{
			_range1->setPixel(2, y, color);
		}
		for (int x = 0; x <= 2; x++)
		{
			_range1->setPixel(x, connectY, color);
		}
		_range1->unlock();
	}
	else
	{
		_weapon1->setVisible(false);
		_range1->setVisible(false);
		_txtAmmo1->setVisible(false);
	}

	if (_craft->getRules()->getWeapons() > 1 && _craft->getWeapons()->at(1) != 0)
	{
		CraftWeapon *w2 = _craft->getWeapons()->at(1);

		// Draw weapon icon
		Surface *frame = set->getFrame(w2->getRules()->getSprite() + 5);
		frame->setX(0);
		frame->setY(0);
		frame->blit(_weapon2);

		// Draw ammo
		stringstream ss;
		ss << w2->getAmmo();
		_txtAmmo2->setText(ss.str());

		// Draw range (1 km = 1 pixel)
		Uint8 color = Palette::blockOffset(7) - 1;
		_range2->lock();

		int rangeY = _range2->getHeight() - w2->getRules()->getRange(), connectY = 57;
		for (int x = 0; x <= 18; x += 2)
		{
			_range2->setPixel(x, rangeY, color);
		}

		int minY, maxY;
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
		for (int y = minY; y <= maxY; y++)
		{
			_range2->setPixel(18, y, color);
		}
		for (int x = 18; x <= 20; x++)
		{
			_range2->setPixel(x, connectY, color);
		}
		_range2->unlock();
	}
	else
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

	// Set music
	_game->getResourcePack()->getMusic("GMINTER")->play();
}

/**
 *
 */
DogfightState::~DogfightState()
{
	
}

/**
 * Runs the dogfighter timers.
 */
void DogfightState::think()
{
	_animTimer->think(this, 0);
	_moveTimer->think(this, 0);
}

/**
 * Animates the window with a palette effect.
 */
void DogfightState::animate()
{
	SDL_Color* pal = _window->getPalette();
	SDL_Color newpal[16];
	for (int i = 0; i < 15; i++)
	{
		newpal[i] = pal[Palette::blockOffset(7) + i + 1];
	}
	newpal[15] = pal[Palette::blockOffset(7)];
	_window->setPalette(newpal, Palette::blockOffset(7), 16);

	if (_timeout == 0)
		_txtStatus->setText("");
	else
		_timeout--;
}

/**
 * Moves the craft towards the UFO according to
 * the current interception mode.
 */
void DogfightState::move()
{
	if (_currentDist < _targetDist)
	{
		_currentDist += 4;
	}
	else if (_currentDist > _targetDist)
	{
		_currentDist -= 2;
	}
	stringstream ss;
	ss << _currentDist;
	_txtDistance->setText(ss.str());

	if (_currentDist > 640 && _mode == _btnDisengage)
	{
		_game->popState();
		stringstream ss;
		ss << "GMGEO" << RNG::generate(1, 2);
		_game->getResourcePack()->getMusic(ss.str())->play();
	}

	_battle->clear();
	// Draw UFO
	for (int r = 4; r >= 1; r--)
	{
		filledCircleColor(_battle->getSurface(), _battle->getWidth() / 2, _battle->getHeight() - _currentDist / 8, r, Palette::getRGBA(_window->getPalette(), Palette::blockOffset(7) + 4 + r));
	}
}

/**
 * Updates the status text and restarts
 * the text timeout counter.
 */
void DogfightState::setStatus(string status)
{
	_txtStatus->setText(status);
	_timeout = 40;
}


/**
 * Switches to Standoff mode (maximum range).
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnStandoffClick(SDL_Event *ev, int scale)
{
	setStatus(_game->getResourcePack()->getLanguage()->getString(STR_STANDOFF));
	_targetDist = 560;
}

/**
 * Switches to Cautious mode (maximum weapon range).
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnCautiousClick(SDL_Event *ev, int scale)
{
	setStatus(_game->getResourcePack()->getLanguage()->getString(STR_CAUTIOUS_ATTACK));
	int max = 0;
	for (vector<CraftWeapon*>::iterator i = _craft->getWeapons()->begin(); i < _craft->getWeapons()->end(); i++)
	{
		if (*i == 0)
			continue;
		if ((*i)->getRules()->getRange() > max)
		{
			max = (*i)->getRules()->getRange();
		}
	}

	_targetDist = max * 8;
}

/**
 * Switches to Standard mode (minimum weapon range).
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnStandardClick(SDL_Event *ev, int scale)
{
	setStatus(_game->getResourcePack()->getLanguage()->getString(STR_STANDARD_ATTACK));
	int min = 1000;
	for (vector<CraftWeapon*>::iterator i = _craft->getWeapons()->begin(); i < _craft->getWeapons()->end(); i++)
	{
		if (*i == 0)
			continue;
		if ((*i)->getRules()->getRange() < min)
		{
			min = (*i)->getRules()->getRange();
		}
	}

	_targetDist = min * 8;
}

/**
 * Switches to Agressive mode (minimum range).
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnAgressiveClick(SDL_Event *ev, int scale)
{
	setStatus(_game->getResourcePack()->getLanguage()->getString(STR_AGRESSIVE_ATTACK));
	_targetDist = 64;
}

/**
 * Disengages from the UFO.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnDisengageClick(SDL_Event *ev, int scale)
{
	setStatus(_game->getResourcePack()->getLanguage()->getString(STR_DISENGAGING));
	_targetDist = 800;
}
