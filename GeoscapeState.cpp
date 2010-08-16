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
#include "GeoscapeState.h"
#include <string>
#include <sstream>
#include "RNG.h"
#include "Game.h"
#include "ResourcePack.h"
#include "Language.h"
#include "LangString.h"
#include "Font.h"
#include "Palette.h"
#include "Screen.h"
#include "Surface.h"
#include "Globe.h"
#include "Text.h"
#include "ImageButton.h"
#include "Timer.h"
#include "GameTime.h"
#include "Music.h"
#include "SavedGame.h"
#include "Base.h"
#include "BaseFacility.h"
#include "RuleBaseFacility.h"
#include "Craft.h"
#include "RuleCraft.h"
#include "CraftWeapon.h"
#include "RuleCraftWeapon.h"
#include "OptionsState.h"
#include "InterceptState.h"
#include "BasescapeState.h"
#include "GraphsState.h"
#include "FundingState.h"
#include "MonthlyReportState.h"
#include "GeoscapeMessageState.h"

using namespace std;

#define LONGITUDE_SPEED 0.2
#define LATITUDE_SPEED 0.1

/**
 * Initializes all the elements in the Geoscape screen.
 * @param game Pointer to the core game.
 */
GeoscapeState::GeoscapeState(Game *game) : State(game), _rotLon(0), _rotLat(0), _pause(false)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_globe = new Globe(130, 100, 256, 200, 0, 0);

	_btnIntercept = new ImageButton(63, 11, 257, 0);
	_btnBases = new ImageButton(63, 11, 257, 12);
	_btnGraphs = new ImageButton(63, 11, 257, 24);
	_btnUfopaedia = new ImageButton(63, 11, 257, 36);
	_btnOptions = new ImageButton(63, 11, 257, 48);
	_btnFunding = new ImageButton(63, 11, 257, 60);

	_btn5Secs = new ImageButton(31, 13, 257, 112);
	_btn1Min = new ImageButton(31, 13, 289, 112);
	_btn5Mins = new ImageButton(31, 13, 257, 126);
	_btn30Mins = new ImageButton(31, 13, 289, 126);
	_btn1Hour = new ImageButton(31, 13, 257, 140);
	_btn1Day = new ImageButton(31, 13, 289, 140);

	_btnRotateLeft = new InteractiveSurface(12, 10, 259, 176);
	_btnRotateRight = new InteractiveSurface(12, 10, 283, 176);
	_btnRotateUp = new InteractiveSurface(13, 12, 271, 162);
	_btnRotateDown = new InteractiveSurface(13, 12, 271, 187);
	_btnZoomIn = new InteractiveSurface(23, 23, 295, 156);
	_btnZoomOut = new InteractiveSurface(13, 17, 300, 182);

	_txtHour = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 20, 16, 259, 74);
	_txtHourSep = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 4, 16, 279, 74);
	_txtMin = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 20, 16, 283, 74);
	_txtMinSep = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 4, 16, 303, 74);
	_txtSec = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 11, 8, 307, 80);
	_txtWeekday = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 59, 8, 259, 87);
	_txtDay = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 29, 8, 259, 94);
	_txtMonth = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 29, 8, 288, 94);
	_txtYear = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 59, 8, 259, 101);

	_timer = _btn5Secs;

	_rotTimer = new Timer(25);
	_gameTimer = new Timer(100);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	
	add(_bg);
	add(_globe);
	
	add(_btnIntercept);
	add(_btnBases);
	add(_btnGraphs);
	add(_btnUfopaedia);
	add(_btnOptions);
	add(_btnFunding);
	
	add(_btn5Secs);
	add(_btn1Min);
	add(_btn5Mins);
	add(_btn30Mins);
	add(_btn1Hour);
	add(_btn1Day);
	
	add(_btnRotateLeft);
	add(_btnRotateRight);
	add(_btnRotateUp);
	add(_btnRotateDown);
	add(_btnZoomIn);
	add(_btnZoomOut);
	
	add(_txtHour);
	add(_txtHourSep);
	add(_txtMin);
	add(_txtMinSep);
	add(_txtSec);
	add(_txtWeekday);
	add(_txtDay);
	add(_txtMonth);
	add(_txtYear);
	
	// Set up objects
	_game->getResourcePack()->getSurface("GEOBORD.SCR")->blit(_bg);
	if (_game->getResourcePack()->getLanguageName() != "ENGLISH.DAT")
	{
		Surface* sidebar = 0;
		if (_game->getResourcePack()->getLanguageName() == "GERMAN.DAT")
			sidebar = _game->getResourcePack()->getSurface("LANG1.DAT");
		else if (_game->getResourcePack()->getLanguageName() == "FRENCH.DAT")
			sidebar = _game->getResourcePack()->getSurface("LANG2.DAT");
		sidebar->setX(320 - sidebar->getWidth());
		sidebar->setY(0);
		sidebar->blit(_bg);
	}
	
	_globe->setSavedGame(game->getSavedGame());
	_globe->setResourcePack(_game->getResourcePack());

	_btnIntercept->copy(_bg);
	_btnIntercept->setColor(Palette::blockOffset(15)+8);
	_btnIntercept->onMouseClick((EventHandler)&GeoscapeState::btnInterceptClick);

	_btnBases->copy(_bg);
	_btnBases->setColor(Palette::blockOffset(15)+8);
	_btnBases->onMouseClick((EventHandler)&GeoscapeState::btnBasesClick);

	_btnGraphs->copy(_bg);
	_btnGraphs->setColor(Palette::blockOffset(15)+8);
	_btnGraphs->onMouseClick((EventHandler)&GeoscapeState::btnGraphsClick);

	_btnUfopaedia->copy(_bg);
	_btnUfopaedia->setColor(Palette::blockOffset(15)+8);
	_btnUfopaedia->onMouseClick((EventHandler)&GeoscapeState::btnUfopaediaClick);

	_btnOptions->copy(_bg);
	_btnOptions->setColor(Palette::blockOffset(15)+8);
	_btnOptions->onMouseClick((EventHandler)&GeoscapeState::btnOptionsClick);

	_btnFunding->copy(_bg);
	_btnFunding->setColor(Palette::blockOffset(15)+8);
	_btnFunding->onMouseClick((EventHandler)&GeoscapeState::btnFundingClick);

	_btn5Secs->copy(_bg);
	_btn5Secs->setColor(Palette::blockOffset(15)+8);
	_btn5Secs->setGroup(&_timer);

	_btn1Min->copy(_bg);
	_btn1Min->setColor(Palette::blockOffset(15)+8);
	_btn1Min->setGroup(&_timer);

	_btn5Mins->copy(_bg);
	_btn5Mins->setColor(Palette::blockOffset(15)+8);
	_btn5Mins->setGroup(&_timer);

	_btn30Mins->copy(_bg);
	_btn30Mins->setColor(Palette::blockOffset(15)+8);
	_btn30Mins->setGroup(&_timer);

	_btn1Hour->copy(_bg);
	_btn1Hour->setColor(Palette::blockOffset(15)+8);
	_btn1Hour->setGroup(&_timer);

	_btn1Day->copy(_bg);
	_btn1Day->setColor(Palette::blockOffset(15)+8);
	_btn1Day->setGroup(&_timer);
	
	_btnRotateLeft->onMousePress((EventHandler)&GeoscapeState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((EventHandler)&GeoscapeState::btnRotateLeftRelease);

	_btnRotateRight->onMousePress((EventHandler)&GeoscapeState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((EventHandler)&GeoscapeState::btnRotateRightRelease);

	_btnRotateUp->onMousePress((EventHandler)&GeoscapeState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((EventHandler)&GeoscapeState::btnRotateUpRelease);

	_btnRotateDown->onMousePress((EventHandler)&GeoscapeState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((EventHandler)&GeoscapeState::btnRotateDownRelease);

	_btnZoomIn->onMouseClick((EventHandler)&GeoscapeState::btnZoomInClick);

	_btnZoomOut->onMouseClick((EventHandler)&GeoscapeState::btnZoomOutClick);

	_txtHour->setBig();
	_txtHour->setColor(Palette::blockOffset(15)+4);
	_txtHour->setAlign(ALIGN_RIGHT);
	_txtHour->setText("");

	_txtHourSep->setBig();
	_txtHourSep->setColor(Palette::blockOffset(15)+4);
	_txtHourSep->setText(":");

	_txtMin->setBig();
	_txtMin->setColor(Palette::blockOffset(15)+4);
	_txtMin->setText("");

	_txtMinSep->setBig();
	_txtMinSep->setColor(Palette::blockOffset(15)+4);
	_txtMinSep->setText(":");

	_txtSec->setSmall();
	_txtSec->setColor(Palette::blockOffset(15)+4);
	_txtSec->setText("");

	_txtWeekday->setSmall();
	_txtWeekday->setColor(Palette::blockOffset(15)+4);
	_txtWeekday->setText("");
	_txtWeekday->setAlign(ALIGN_CENTER);

	_txtDay->setSmall();
	_txtDay->setColor(Palette::blockOffset(15)+4);
	_txtDay->setText("");
	_txtDay->setAlign(ALIGN_CENTER);

	_txtMonth->setSmall();
	_txtMonth->setColor(Palette::blockOffset(15)+4);
	_txtMonth->setText("");
	_txtMonth->setAlign(ALIGN_CENTER);

	_txtYear->setSmall();
	_txtYear->setColor(Palette::blockOffset(15)+4);
	_txtYear->setText("");
	_txtYear->setAlign(ALIGN_CENTER);

	_rotTimer->onTimer((StateHandler)&GeoscapeState::globeRotate);

	_gameTimer->onTimer((StateHandler)&GeoscapeState::timeAdvance);
	_gameTimer->start();

	// Set music
	stringstream ss;
	ss << "GMGEO" << RNG::generate(1, 2);
	_game->getResourcePack()->getMusic(ss.str())->play();
}

/**
 *
 */
GeoscapeState::~GeoscapeState()
{
	
}

/**
 * Updates the timer display and resets the palette
 * since it's bound to change on other screens.
 */
void GeoscapeState::init()
{
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());

	_globe->onMouseClick((EventHandler)&GeoscapeState::globeClick);
	_globe->onKeyboardPress((EventHandler)&GeoscapeState::globeKeyPress);
	_globe->focus();
	_globe->draw();

	timeDisplay();
}

/**
 * Runs the game timer and globe rotation timer.
 */
void GeoscapeState::think()
{
	State::think();

	_rotTimer->think(this, 0);
	_gameTimer->think(this, 0);
}

/**
 * Rotates the globe by a set amount. Necessary
 * since the globe keeps rotating while the button
 * is pressed down.
 */
void GeoscapeState::globeRotate()
{
	_globe->rotate(_rotLon, _rotLat);
}

/**
 * Updates the Geoscape clock with the latest
 * game time and date in human-readable format.
 */
void GeoscapeState::timeDisplay()
{
	stringstream ss, ss2, ss3, ss4, ss5;
	
	if (_game->getSavedGame()->getTime()->getSecond() < 10)
		ss << "0" << _game->getSavedGame()->getTime()->getSecond();
	else
		ss << _game->getSavedGame()->getTime()->getSecond();
	_txtSec->setText(ss.str());

	if (_game->getSavedGame()->getTime()->getMinute() < 10)
		ss2 << "0" << _game->getSavedGame()->getTime()->getMinute();
	else
		ss2 << _game->getSavedGame()->getTime()->getMinute();
	_txtMin->setText(ss2.str());

	ss3 << _game->getSavedGame()->getTime()->getHour();
	_txtHour->setText(ss3.str());

	ss4 << _game->getSavedGame()->getTime()->getDay() << _game->getResourcePack()->getLanguage()->getString(_game->getSavedGame()->getTime()->getDayString());
	_txtDay->setText(ss4.str());

	_txtWeekday->setText(_game->getResourcePack()->getLanguage()->getString(_game->getSavedGame()->getTime()->getWeekdayString()));

	_txtMonth->setText(_game->getResourcePack()->getLanguage()->getString(_game->getSavedGame()->getTime()->getMonthString()));

	ss5 << _game->getSavedGame()->getTime()->getYear();
	_txtYear->setText(ss5.str());
}

/**
 * Advances the game timer according to
 * the timer speed set, and calls the respective
 * triggers. The timer always advances in "5 secs"
 * cycles regardless of the speed otherwise it might
 * skip important steps. Instead, it just keeps advancing
 * the timer until the next speed step (eg. the next day
 * on 1 Day speed) or until an event occurs, since updating
 * the screen on each step would become cumbersomely slow.
 */
void GeoscapeState::timeAdvance()
{
	int timeSpan = 0;
	if (_timer == _btn5Secs)
		timeSpan = 1;
	else if (_timer == _btn1Min)
		timeSpan = 12;
	else if (_timer == _btn5Mins)
		timeSpan = 12 * 5;
	else if (_timer == _btn30Mins)
		timeSpan = 12 * 5 * 6;
	else if (_timer == _btn1Hour)
		timeSpan = 12 * 5 * 6 * 2;
	else if (_timer == _btn1Day)
		timeSpan = 12 * 5 * 6 * 2 * 24;
		
	for (int i = 0; i < timeSpan && !_pause; i++)
	{
		TimeTrigger trigger;
		trigger = _game->getSavedGame()->getTime()->advance();
		switch (trigger)
		{
		case TIME_1MONTH:
			timeMonth();
		case TIME_1DAY:
			timeDay();
		case TIME_1HOUR:
			timeHour();
		case TIME_30MIN:
			timeMinute();
		case TIME_5SEC:
			timeSecond();
		}

	}

	_pause = false;

	timeDisplay();
	_globe->draw();
}

/**
 * Takes care of any game logic that has to
 * run every game second, like craft movement.
 */
void GeoscapeState::timeSecond()
{
}

/**
 * Takes care of any game logic that has to
 * run every game half hour, like reloading.
 */
void GeoscapeState::timeMinute()
{
	for (vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); i++)
	{
		for (vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); j++)
		{
			switch ((*j)->getStatus())
			{
				case STR_REFUELLING:
					(*j)->setFuel((*j)->getFuel() + (*j)->getRules()->getRefuelRate());
					if ((*j)->getFuel() == (*j)->getRules()->getMaxFuel())
						(*j)->setStatus(STR_REARMING);
					break;
			}
		}
	}
}

/**
 * Takes care of any game logic that has to
 * run every game second, like transfers.
 */
void GeoscapeState::timeHour()
{
	for (vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); i++)
	{
		for (vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); j++)
		{
			switch ((*j)->getStatus())
			{
				case STR_REPAIRS:
					(*j)->setDamage((*j)->getDamage() - (*j)->getRules()->getRepairRate());
					if ((*j)->getDamage() == 0)
						(*j)->setStatus(STR_REFUELLING);
					break;
				case STR_REARMING:
					int available = 0, full = 0;
					for (vector<CraftWeapon*>::iterator k = (*j)->getWeapons()->begin(); k != (*j)->getWeapons()->end(); k++)
					{
						if ((*k) == 0)
							continue;

						available++;
						(*k)->setAmmo((*k)->getAmmo() + (*k)->getRules()->getRearmRate());

						if ((*k)->getAmmo() == (*k)->getRules()->getAmmoMax())
							full++;
					}

					if (full == available)
						(*j)->setStatus(STR_READY);
					break;
			}
		}
	}
}

/**
 * Takes care of any game logic that has to
 * run every game day, like constructions.
 */
void GeoscapeState::timeDay()
{
	for (vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); i++)
	{
		for (vector<BaseFacility*>::iterator j = (*i)->getFacilities()->begin(); j != (*i)->getFacilities()->end(); j++)
		{
			if ((*j)->getBuildTime() > 0)
			{
				(*j)->setBuildTime((*j)->getBuildTime() - 1);
				if ((*j)->getBuildTime() == 0)
				{
					_pause = true;
					timerReset();
					string s = _game->getResourcePack()->getLanguage()->getString(STR_PRODUCTION_OF);
					s += _game->getResourcePack()->getLanguage()->getString((*j)->getRules()->getType());
					s += _game->getResourcePack()->getLanguage()->getString(STR_AT);
					s += (*i)->getName();
					s += _game->getResourcePack()->getLanguage()->getString(STR_IS_COMPLETE);
					_game->pushState(new GeoscapeMessageState(_game, s));
				}
			}
		}
	}
}

/**
 * Takes care of any game logic that has to
 * run every game month, like funding.
 */
void GeoscapeState::timeMonth()
{
	_pause = true;
	timerReset();
	_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() + _game->getSavedGame()->getCountryFunding() - _game->getSavedGame()->getBaseMaintenance());
	_game->pushState(new MonthlyReportState(_game));
}

/**
 * Slows down the timer back to minimum speed,
 * for when important events occur.
 */
void GeoscapeState::timerReset()
{
	SDL_Event ev;
	ev.button.button = SDL_BUTTON_LEFT;
	_btn5Secs->mousePress(&ev, _game->getScreen()->getScale(), this);
}

/**
 * Returns a pointer to the Geoscape globe for
 * access by other substates.
 */
Globe *GeoscapeState::getGlobe()
{
	return _globe;
}

/**
 * Processes any left-clicks on globe markers,
 * or right-clicks to scroll the globe.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::globeClick(SDL_Event *ev, int scale)
{
	double lon, lat;
	_globe->cartToPolar(ev->button.x / scale, ev->button.y / scale, &lon, &lat);
	
	if (ev->button.button == SDL_BUTTON_LEFT)
	{

	}
	else if (ev->button.button == SDL_BUTTON_RIGHT)
	{
		_globe->center(lon, lat);
	}
	else if (ev->button.button == SDL_BUTTON_WHEELUP)
	{
		_globe->zoom(1);
	}
	else if (ev->button.button == SDL_BUTTON_WHEELDOWN)
	{
		_globe->zoom(-1);
	}
}

/**
 * Processes any key presses on the globe.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::globeKeyPress(SDL_Event *ev, int scale)
{
	if (ev->key.keysym.sym == SDLK_TAB)
		_globe->switchDetail();
}

/**
 * Opens the Intercept window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnInterceptClick(SDL_Event *ev, int scale)
{
	_game->pushState(new InterceptState(_game));
}

/**
 * Goes to the Basescape screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnBasesClick(SDL_Event *ev, int scale)
{
	_game->pushState(new BasescapeState(_game, _game->getSavedGame()->getBases()->front(), _globe));
}

/**
 * Goes to the Graphs screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnGraphsClick(SDL_Event *ev, int scale)
{
	_game->pushState(new GraphsState(_game));
}

void GeoscapeState::btnUfopaediaClick(SDL_Event *ev, int scale)
{

}

/**
 * Opens the Options window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnOptionsClick(SDL_Event *ev, int scale)
{
	_game->pushState(new OptionsState(_game));
}

/**
 * Goes to the Funding screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnFundingClick(SDL_Event *ev, int scale)
{
	_game->pushState(new FundingState(_game));
}

/**
 * Starts rotating the globe to the left.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnRotateLeftPress(SDL_Event *ev, int scale)
{
	_rotLon += LONGITUDE_SPEED;
	_rotTimer->start();
	globeRotate();
}

/**
 * Stops rotating the globe to the left.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnRotateLeftRelease(SDL_Event *ev, int scale)
{
	_rotLon -= LONGITUDE_SPEED;
	_rotTimer->stop();
}

/**
 * Starts rotating the globe to the right.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnRotateRightPress(SDL_Event *ev, int scale)
{
	_rotLon += -LONGITUDE_SPEED;
	_rotTimer->start();
	globeRotate();
}

/**
 * Stops rotating the globe to the right.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnRotateRightRelease(SDL_Event *ev, int scale)
{
	_rotLon -= -LONGITUDE_SPEED;
	_rotTimer->stop();
}

/**
 * Starts rotating the globe upwards.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnRotateUpPress(SDL_Event *ev, int scale)
{
	_rotLat += LATITUDE_SPEED;
	_rotTimer->start();
	globeRotate();
}

/**
 * Stops rotating the globe upwards.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnRotateUpRelease(SDL_Event *ev, int scale)
{
	_rotLat -= LATITUDE_SPEED;
	_rotTimer->stop();
}

/**
 * Starts rotating the globe downwards.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnRotateDownPress(SDL_Event *ev, int scale)
{
	_rotLat += -LATITUDE_SPEED;
	_rotTimer->start();
	globeRotate();
}

/**
 * Stops rotating the globe downwards.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnRotateDownRelease(SDL_Event *ev, int scale)
{
	_rotLat -= -LATITUDE_SPEED;
	_rotTimer->stop();
}

/**
 * Zooms into the globe.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnZoomInClick(SDL_Event *ev, int scale)
{
	_globe->zoom(1);
}

/**
 * Zooms out of the globe.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void GeoscapeState::btnZoomOutClick(SDL_Event *ev, int scale)
{
	_globe->zoom(-1);
}
