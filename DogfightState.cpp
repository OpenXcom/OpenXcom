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
#include "Globe.h"
#include "Craft.h"
#include "RuleCraft.h"
#include "CraftWeapon.h"
#include "RuleCraftWeapon.h"
#include "Ufo.h"
#include "RuleUfo.h"
#include "Music.h"
#include "RNG.h"
#include "SoundSet.h"
#include "Sound.h"

/**
 * Initializes all the elements in the Dogfight window.
 * @param game Pointer to the core game.
 * @param globe Pointer to the Geoscape globe.
 * @param craft Pointer to the craft intercepting.
 * @param ufo Pointer to the UFO being intercepted.
 */
DogfightState::DogfightState(Game *game, Globe *globe, Craft *craft, Ufo *ufo) : State(game), _globe(globe), _craft(craft), _ufo(ufo), _timeout(50), _currentDist(640), _targetDist(560), _w1Dist(), _w2Dist(), _end(false)
{
	_targetRadius = _currentRadius = STR_VERY_SMALL - _ufo->getRules()->getSize() + 2;
	
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
	_preview = new InteractiveSurface(160, 96, 80, 52);
	_btnStandoff = new ImageButton(36, 15, 163, 56);
	_btnCautious = new ImageButton(36, 15, 200, 56);
	_btnStandard = new ImageButton(36, 15, 163, 72);
	_btnAggressive = new ImageButton(36, 15, 200, 72);
	_btnDisengage = new ImageButton(36, 15, 200, 88);
	_btnUfo = new ImageButton(36, 17, 200, 104);
	_txtAmmo1 = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 16, 9, 84, 122);
	_txtAmmo2 = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 16, 9, 144, 122);
	_txtDistance = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 196, 124);
	_txtStatus = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 150, 9, 84, 137);

	_animTimer = new Timer(30);
	_moveTimer = new Timer(20);
	_w1Timer = new Timer(0);
	_w2Timer = new Timer(0);
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
	SDL_Rect crop;
	crop.x = 0;
	crop.y = 0;
	crop.w = 160;
	crop.h = 96;
	SDL_FillRect(_window->getSurface(), &crop, 15);
	graphic->setCrop(&crop);
	graphic->blit(_window);
	
	SDL_FillRect(_preview->getSurface(), &crop, 15);
	crop.y = 96;
	crop.h = 15;
	graphic->setCrop(&crop);
	graphic->blit(_preview);
	graphic->setY(67);
	crop.y = 111;
	crop.h = 29;
	graphic->setCrop(&crop);
	graphic->blit(_preview);
	graphic->setY(15);
	crop.y = 140 + 52 * _ufo->getRules()->getSprite();
	crop.h = 52;
	graphic->setCrop(&crop);
	graphic->blit(_preview);
	_preview->setVisible(false);
	_preview->onMouseClick((EventHandler)&DogfightState::previewClick);

	_btnMinimize->onMouseClick((EventHandler)&DogfightState::btnMinimizeClick);

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

	_btnAggressive->copy(_window);
	_btnAggressive->setColor(Palette::blockOffset(5)+4);
	_btnAggressive->setGroup(&_mode);
	_btnAggressive->onMouseClick((EventHandler)&DogfightState::btnAggressiveClick);

	_btnDisengage->copy(_window);
	_btnDisengage->setColor(Palette::blockOffset(5)+4);
	_btnDisengage->onMouseClick((EventHandler)&DogfightState::btnDisengageClick);
	_btnDisengage->setGroup(&_mode);

	_btnUfo->copy(_window);
	_btnUfo->setColor(Palette::blockOffset(5)+4);
	_btnUfo->onMouseClick((EventHandler)&DogfightState::btnUfoClick);

	_btnUfo->copy(_window);
	_btnUfo->setColor(Palette::blockOffset(5)+4);

	_txtAmmo1->setColor(Palette::blockOffset(5)+9);

	_txtAmmo2->setColor(Palette::blockOffset(5)+9);

	_txtDistance->setColor(Palette::blockOffset(5)+9);
	_txtDistance->setText("640");

	_txtStatus->setColor(Palette::blockOffset(5)+9);
	_txtStatus->setText(_game->getResourcePack()->getLanguage()->getString(STR_STANDOFF));

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

	_w1Timer->onTimer((StateHandler)&DogfightState::fireWeapon1);

	_w2Timer->onTimer((StateHandler)&DogfightState::fireWeapon2);

	// Set music
	_game->getResourcePack()->getMusic("GMINTER")->play();
}

/**
 *
 */
DogfightState::~DogfightState()
{
	delete _animTimer;
	delete _moveTimer;
	delete _w1Timer;
	delete _w2Timer;
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

	// Clears text after a while
	if (_timeout == 0)
	{
		_txtStatus->setText("");
	}
	else
	{
		_timeout--;
	}
}

/**
 * Moves the craft towards the UFO according to
 * the current interception mode.
 */
void DogfightState::move()
{
	// Update distance
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

	// Update weapons
	for (vector<int>::iterator w = _w1Dist.begin(); w != _w1Dist.end(); w++)
	{
		(*w) += 8;
	}
	for (vector<int>::iterator w = _w2Dist.begin(); w != _w2Dist.end(); w++)
	{
		(*w) += 8;
	}

	CraftWeapon *w1 = 0, *w2 = 0;
	if (_craft->getRules()->getWeapons() > 0)
	{
		w1 = _craft->getWeapons()->at(0);
	}
	if (_craft->getRules()->getWeapons() > 1)
	{
		w2 = _craft->getWeapons()->at(1);
	}
	// Handle weapon damage
	for (vector<int>::iterator w = _w1Dist.begin(); w != _w1Dist.end(); w++)
	{
		if ((*w) >= _currentDist)
		{
			int acc = RNG::generate(1, 100);
			if (acc <= w1->getRules()->getAccuracy() && !_ufo->isCrashed())
			{
				int damage = RNG::generate(w1->getRules()->getDamage() / 2, w1->getRules()->getDamage());
				_ufo->setDamage(_ufo->getDamage() + damage);
				setStatus(STR_UFO_HIT);
				_currentRadius += 4;
				_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(12)->play();
			}
			_w1Dist.erase(w);
			break;
		}
	}
	for (vector<int>::iterator w = _w2Dist.begin(); w != _w2Dist.end(); w++)
	{
		if ((*w) >= _currentDist)
		{
			int acc = RNG::generate(1, 100);
			if (acc <= w2->getRules()->getAccuracy() && !_ufo->isCrashed())
			{
				int damage = RNG::generate(w2->getRules()->getDamage() / 2, w2->getRules()->getDamage());
				_ufo->setDamage(_ufo->getDamage() + damage);
				setStatus(STR_UFO_HIT);
				_currentRadius += 4;
				_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(12)->play();
			}
			_w2Dist.erase(w);
			break;
		}
	}

	// Handle weapon firing
	if (w1 != 0)
	{
		if (!_w1Timer->isRunning() && _currentDist <= w1->getRules()->getRange() * 8 && w1->getAmmo() > 0 && _mode != _btnStandoff && _mode != _btnDisengage && !_ufo->isCrashed())
		{
			_w1Timer->start();
			fireWeapon1();
		}
		else if (_w1Timer->isRunning() && (_currentDist > w1->getRules()->getRange() * 8 || w1->getAmmo() == 0 || _mode == _btnStandoff || _mode == _btnDisengage || _ufo->isCrashed()))
		{
			_w1Timer->stop();
			if (w1->getAmmo() == 0)
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
	if (w2 != 0)
	{
		if (!_w2Timer->isRunning() && _currentDist <= w2->getRules()->getRange() * 8 && w2->getAmmo() > 0 && _mode != _btnStandoff && _mode != _btnDisengage && !_ufo->isCrashed())
		{
			_w2Timer->start();
			fireWeapon2();
		}
		else if (_w2Timer->isRunning() && (_currentDist > w2->getRules()->getRange() * 8 || w2->getAmmo() == 0 || _mode == _btnStandoff || _mode == _btnDisengage || _ufo->isCrashed()))
		{
			_w2Timer->stop();
			if (w2->getAmmo() == 0)
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

	_battle->clear();

	// Draw UFO
	if (_targetRadius < _currentRadius)
	{
		_currentRadius--;
	}
	if (_currentRadius > 0)
	{
		for (int r = _currentRadius; r >= 0; r--)
		{
			filledCircleColor(_battle->getSurface(), _battle->getWidth() / 2, _battle->getHeight() - _currentDist / 8, r, Palette::getRGBA(_window->getPalette(), Palette::blockOffset(7) + 4 + r));
		}
	}

	// Draw weapon shots
	for (vector<int>::iterator w = _w1Dist.begin(); w != _w1Dist.end(); w++)
	{
		for (int i = -2; i <= 0; i++)
		{
			_battle->setPixel(_battle->getWidth() / 2 - 1, _battle->getHeight() - (*w) / 8 + i, Palette::blockOffset(7));
		}
	}
	for (vector<int>::iterator w = _w2Dist.begin(); w != _w2Dist.end(); w++)
	{
		for (int i = -2; i <= 0; i++)
		{
			_battle->setPixel(_battle->getWidth() / 2 + 1, _battle->getHeight() - (*w) / 8 + i, Palette::blockOffset(7));
		}
	}

	// Check when battle is over
	if ((_currentDist > 640 && _mode == _btnDisengage) || (_timeout == 0 && _ufo->getDamage() >= _ufo->getRules()->getMaxDamage() / 2))
	{
		_craft->returnToBase();
		_game->popState();
		stringstream ss;
		ss << "GMGEO" << RNG::generate(1, 2);
		_game->getResourcePack()->getMusic(ss.str())->play();
	}

	if (!_end && _ufo->isCrashed())
	{
		if (_ufo->isDestroyed())
		{
			setStatus(STR_UFO_DESTROYED);
			_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(11)->play();
		}
		else
		{
			setStatus(STR_UFO_CRASH_LANDS);
			_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(10)->play();
			if (!_globe->insideLand(_ufo->getLongitude(), _ufo->getLatitude()))
			{
				_ufo->setDaysCrashed(5);
			}
		}
		_targetRadius = 0;
		_end = true;
		_ufo->setSpeed(0);
	}
}

/**
 * Fires a shot from the first weapon
 * equipped on the craft.
 */
void DogfightState::fireWeapon1()
{
	_w1Dist.push_back(8);
	CraftWeapon *w1 = _craft->getWeapons()->at(0);
	w1->setAmmo(w1->getAmmo() - 1);

	stringstream ss;
	ss << w1->getAmmo();
	_txtAmmo1->setText(ss.str());

	_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(w1->getRules()->getSound())->play();
}

/**
 * Fires a shot from the second weapon
 * equipped on the craft.
 */
void DogfightState::fireWeapon2()
{
	_w2Dist.push_back(8);
	CraftWeapon *w2= _craft->getWeapons()->at(1);
	w2->setAmmo(w2->getAmmo() - 1);

	stringstream ss;
	ss << w2->getAmmo();
	_txtAmmo2->setText(ss.str());

	_game->getResourcePack()->getSoundSet("GEO.CAT")->getSound(w2->getRules()->getSound())->play();
}

/**
 * Sets the craft to the minimum distance
 * required to fire a weapon.
 */
void DogfightState::minimumDistance()
{
	int max = 0;
	for (vector<CraftWeapon*>::iterator i = _craft->getWeapons()->begin(); i < _craft->getWeapons()->end(); i++)
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
	for (vector<CraftWeapon*>::iterator i = _craft->getWeapons()->begin(); i < _craft->getWeapons()->end(); i++)
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
 */
void DogfightState::setStatus(LangString status)
{
	_txtStatus->setText(_game->getResourcePack()->getLanguage()->getString(status));
	_timeout = 50;
}

/**
 * Minimizes the dogfight window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnMinimizeClick(SDL_Event *ev, int scale)
{
}

/**
 * Switches to Standoff mode (maximum range).
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnStandoffClick(SDL_Event *ev, int scale)
{
	if (!_ufo->isCrashed())
	{
		setStatus(STR_STANDOFF);
		_targetDist = 560;
	}
}

/**
 * Switches to Cautious mode (maximum weapon range).
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnCautiousClick(SDL_Event *ev, int scale)
{
	if (!_ufo->isCrashed())
	{
		setStatus(STR_CAUTIOUS_ATTACK);
		if (_craft->getWeapons()->at(0) != 0)
		{
			_w1Timer->setInterval(_craft->getWeapons()->at(0)->getRules()->getCautiousReload() * 75);
		}
		if (_craft->getWeapons()->at(1) != 0)
		{
			_w2Timer->setInterval(_craft->getWeapons()->at(1)->getRules()->getCautiousReload() * 75);
		}
		minimumDistance();
	}
}

/**
 * Switches to Standard mode (minimum weapon range).
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnStandardClick(SDL_Event *ev, int scale)
{
	if (!_ufo->isCrashed())
	{
		setStatus(STR_STANDARD_ATTACK);
		if (_craft->getWeapons()->at(0) != 0)
		{
			_w1Timer->setInterval(_craft->getWeapons()->at(0)->getRules()->getStandardReload() * 75);
		}
		if (_craft->getWeapons()->at(1) != 0)
		{
			_w2Timer->setInterval(_craft->getWeapons()->at(1)->getRules()->getStandardReload() * 75);
		}
		maximumDistance();
	}
}

/**
 * Switches to Aggressive mode (minimum range).
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnAggressiveClick(SDL_Event *ev, int scale)
{
	if (!_ufo->isCrashed())
	{
		setStatus(STR_AGGRESSIVE_ATTACK);
		if (_craft->getWeapons()->at(0) != 0)
		{
			_w1Timer->setInterval(_craft->getWeapons()->at(0)->getRules()->getAggressiveReload() * 75);
		}
		if (_craft->getWeapons()->at(1) != 0)
		{
			_w2Timer->setInterval(_craft->getWeapons()->at(1)->getRules()->getAggressiveReload() * 75);
		}
		_targetDist = 64;
	}
}

/**
 * Disengages from the UFO.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnDisengageClick(SDL_Event *ev, int scale)
{
	if (!_ufo->isCrashed())
	{
		setStatus(STR_DISENGAGING);
		_targetDist = 800;
	}
}

/**
 * Shows a front view of the UFO.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::btnUfoClick(SDL_Event *ev, int scale)
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
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DogfightState::previewClick(SDL_Event *ev, int scale)
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
