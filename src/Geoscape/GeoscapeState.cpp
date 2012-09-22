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
#define _USE_MATH_DEFINES
#include "GeoscapeState.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include "../Engine/RNG.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Screen.h"
#include "../Engine/Surface.h"
#include "../Engine/Options.h"
#include "Globe.h"
#include "../Interface/Text.h"
#include "../Interface/ImageButton.h"
#include "../Engine/Timer.h"
#include "../Savegame/GameTime.h"
#include "../Engine/Music.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/Ufo.h"
#include "../Ruleset/RuleUfo.h"
#include "../Savegame/Waypoint.h"
#include "../Savegame/Transfer.h"
#include "../Savegame/Soldier.h"
#include "GeoscapeOptionsState.h"
#include "InterceptState.h"
#include "../Basescape/BasescapeState.h"
#include "GraphsState.h"
#include "FundingState.h"
#include "MonthlyReportState.h"
#include "ProductionCompleteState.h"
#include "UfoDetectedState.h"
#include "GeoscapeCraftState.h"
#include "DogfightState.h"
#include "UfoLostState.h"
#include "CraftPatrolState.h"
#include "LowFuelState.h"
#include "MultipleTargetsState.h"
#include "ConfirmLandingState.h"
#include "ItemsArrivingState.h"
#include "CraftErrorState.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Savegame/ResearchProject.h"
#include "ResearchCompleteState.h"
#include "../Ruleset/RuleResearch.h"
#include "NewPossibleResearchState.h"
#include "../Savegame/Production.h"
#include "../Ruleset/RuleManufacture.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/TerrorSite.h"
#include "../Ruleset/RuleRegion.h"
#include "../Ruleset/City.h"
#include "AlienTerrorState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Geoscape screen.
 * @param game Pointer to the core game.
 */
GeoscapeState::GeoscapeState(Game *game) : State(game), _pause(false), _music(false), _popups()
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_globe = new Globe(_game, 130, 100, 256, 200, 0, 0);

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

	_txtHour = new Text(20, 16, 259, 74);
	_txtHourSep = new Text(4, 16, 279, 74);
	_txtMin = new Text(20, 16, 283, 74);
	_txtMinSep = new Text(4, 16, 303, 74);
	_txtSec = new Text(11, 8, 307, 80);
	_txtWeekday = new Text(59, 8, 259, 87);
	_txtDay = new Text(29, 8, 259, 94);
	_txtMonth = new Text(29, 8, 288, 94);
	_txtYear = new Text(59, 8, 259, 101);

	_timeSpeed = _btn5Secs;
	_timer = new Timer(100);

	_txtDebug = new Text(100, 8, 0, 0);

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

	add(_txtDebug);

	// Set up objects
	_game->getResourcePack()->getSurface("GEOBORD.SCR")->blit(_bg);

	_btnIntercept->copy(_bg);
	_btnIntercept->setColor(Palette::blockOffset(15)+5);
	_btnIntercept->onMouseClick((ActionHandler)&GeoscapeState::btnInterceptClick);

	_btnBases->copy(_bg);
	_btnBases->setColor(Palette::blockOffset(15)+5);
	_btnBases->onMouseClick((ActionHandler)&GeoscapeState::btnBasesClick);

	_btnGraphs->copy(_bg);
	_btnGraphs->setColor(Palette::blockOffset(15)+5);
	_btnGraphs->onMouseClick((ActionHandler)&GeoscapeState::btnGraphsClick);

	_btnUfopaedia->copy(_bg);
	_btnUfopaedia->setColor(Palette::blockOffset(15)+5);
	_btnUfopaedia->onMouseClick((ActionHandler)&GeoscapeState::btnUfopaediaClick);

	_btnOptions->copy(_bg);
	_btnOptions->setColor(Palette::blockOffset(15)+5);
	_btnOptions->onMouseClick((ActionHandler)&GeoscapeState::btnOptionsClick);

	_btnFunding->copy(_bg);
	_btnFunding->setColor(Palette::blockOffset(15)+5);
	_btnFunding->onMouseClick((ActionHandler)&GeoscapeState::btnFundingClick);

	_btn5Secs->copy(_bg);
	_btn5Secs->setColor(Palette::blockOffset(15)+5);
	_btn5Secs->setGroup(&_timeSpeed);

	_btn1Min->copy(_bg);
	_btn1Min->setColor(Palette::blockOffset(15)+5);
	_btn1Min->setGroup(&_timeSpeed);

	_btn5Mins->copy(_bg);
	_btn5Mins->setColor(Palette::blockOffset(15)+5);
	_btn5Mins->setGroup(&_timeSpeed);

	_btn30Mins->copy(_bg);
	_btn30Mins->setColor(Palette::blockOffset(15)+5);
	_btn30Mins->setGroup(&_timeSpeed);

	_btn1Hour->copy(_bg);
	_btn1Hour->setColor(Palette::blockOffset(15)+5);
	_btn1Hour->setGroup(&_timeSpeed);

	_btn1Day->copy(_bg);
	_btn1Day->setColor(Palette::blockOffset(15)+5);
	_btn1Day->setGroup(&_timeSpeed);

	_btnRotateLeft->onMousePress((ActionHandler)&GeoscapeState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateLeftRelease);

	_btnRotateRight->onMousePress((ActionHandler)&GeoscapeState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateRightRelease);

	_btnRotateUp->onMousePress((ActionHandler)&GeoscapeState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateUpRelease);

	_btnRotateDown->onMousePress((ActionHandler)&GeoscapeState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateDownRelease);

	_btnZoomIn->onMouseClick((ActionHandler)&GeoscapeState::btnZoomInLeftClick, SDL_BUTTON_LEFT);
	_btnZoomIn->onMouseClick((ActionHandler)&GeoscapeState::btnZoomInRightClick, SDL_BUTTON_RIGHT);

	_btnZoomOut->onMouseClick((ActionHandler)&GeoscapeState::btnZoomOutLeftClick, SDL_BUTTON_LEFT);
	_btnZoomOut->onMouseClick((ActionHandler)&GeoscapeState::btnZoomOutRightClick, SDL_BUTTON_RIGHT);

	_txtHour->setBig();
	_txtHour->setColor(Palette::blockOffset(15)+4);
	_txtHour->setAlign(ALIGN_RIGHT);
	_txtHour->setText(L"");

	_txtHourSep->setBig();
	_txtHourSep->setColor(Palette::blockOffset(15)+4);
	_txtHourSep->setText(L":");

	_txtMin->setBig();
	_txtMin->setColor(Palette::blockOffset(15)+4);
	_txtMin->setText(L"");

	_txtMinSep->setBig();
	_txtMinSep->setColor(Palette::blockOffset(15)+4);
	_txtMinSep->setText(L":");

	_txtSec->setSmall();
	_txtSec->setColor(Palette::blockOffset(15)+4);
	_txtSec->setText(L"");

	_txtWeekday->setSmall();
	_txtWeekday->setColor(Palette::blockOffset(15)+4);
	_txtWeekday->setText(L"");
	_txtWeekday->setAlign(ALIGN_CENTER);

	_txtDay->setSmall();
	_txtDay->setColor(Palette::blockOffset(15)+4);
	_txtDay->setText(L"");
	_txtDay->setAlign(ALIGN_CENTER);

	_txtMonth->setSmall();
	_txtMonth->setColor(Palette::blockOffset(15)+4);
	_txtMonth->setText(L"");
	_txtMonth->setAlign(ALIGN_CENTER);

	_txtYear->setSmall();
	_txtYear->setColor(Palette::blockOffset(15)+4);
	_txtYear->setText(L"");
	_txtYear->setAlign(ALIGN_CENTER);

	_txtDebug->setColor(Palette::blockOffset(15)+4);

	_timer->onTimer((StateHandler)&GeoscapeState::timeAdvance);
	_timer->start();

	timeDisplay();
}

/**
 * Deletes timers.
 */
GeoscapeState::~GeoscapeState()
{
	delete _timer;
}

/**
 * Handle key shortcuts.
 * @param action Pointer to an action.
 */
void GeoscapeState::handle(Action *action)
{
	State::handle(action);

	if (action->getDetails()->type == SDL_KEYDOWN)
	{
		// "d" - enable debug mode
		if (Options::getBool("debug") && action->getDetails()->key.keysym.sym == SDLK_d)
		{
			_game->getSavedGame()->setDebugMode();
			if (_game->getSavedGame()->getDebugMode())
			{
				_txtDebug->setText(L"DEBUG MODE");
			}
			else
			{
				_txtDebug->setText(L"");
			}
		}
	}
}

/**
 * Updates the timer display and resets the palette
 * since it's bound to change on other screens.
 */
void GeoscapeState::init()
{
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());

	timeDisplay();

	_globe->onMouseClick((ActionHandler)&GeoscapeState::globeClick);
	_globe->focus();
	_globe->draw();

	// Set music if it's not already playing
	if (!_music)
	{
		std::stringstream ss;
		ss << "GMGEO" << RNG::generate(1, 2);
		_game->getResourcePack()->getMusic(ss.str())->play();
		_music = true;
	}
}

/**
 * Runs the game timer and handles popups.
 */
void GeoscapeState::think()
{
	State::think();

	if (_popups.empty())
	{
		// Handle timers
		_timer->think(this, 0);
	}
	else
	{
		// Handle popups
		_globe->rotateStop();
		_game->pushState(*_popups.begin());
		_popups.erase(_popups.begin());
	}

}

/**
 * Updates the Geoscape clock with the latest
 * game time and date in human-readable format.
 */
void GeoscapeState::timeDisplay()
{
	std::stringstream ss, ss2;
	std::wstringstream ss3, ss4, ss5;

	ss << std::setfill('0') << std::setw(2) << _game->getSavedGame()->getTime()->getSecond();
	_txtSec->setText(Language::utf8ToWstr(ss.str()));

	ss2 << std::setfill('0') << std::setw(2) << _game->getSavedGame()->getTime()->getMinute();
	_txtMin->setText(Language::utf8ToWstr(ss2.str()));

	ss3 << _game->getSavedGame()->getTime()->getHour();
	_txtHour->setText(ss3.str());

	ss4 << _game->getSavedGame()->getTime()->getDay() << _game->getLanguage()->getString(_game->getSavedGame()->getTime()->getDayString());
	_txtDay->setText(ss4.str());

	_txtWeekday->setText(_game->getLanguage()->getString(_game->getSavedGame()->getTime()->getWeekdayString()));

	_txtMonth->setText(_game->getLanguage()->getString(_game->getSavedGame()->getTime()->getMonthString()));

	ss5 << _game->getSavedGame()->getTime()->getYear();
	_txtYear->setText(ss5.str());
}

/**
 * Advances the game timer according to
 * the timer speed set, and calls the respective
 * triggers. The timer always advances in "5 secs"
 * cycles, regardless of the speed, otherwise it might
 * skip important steps. Instead, it just keeps advancing
 * the timer until the next speed step (eg. the next day
 * on 1 Day speed) or until an event occurs, since updating
 * the screen on each step would become cumbersomely slow.
 */
void GeoscapeState::timeAdvance()
{
	int timeSpan = 0;
	if (_timeSpeed == _btn5Secs)
	{
		timeSpan = 1;
	}
	else if (_timeSpeed == _btn1Min)
	{
		timeSpan = 12;
	}
	else if (_timeSpeed == _btn5Mins)
	{
		timeSpan = 12 * 5;
	}
	else if (_timeSpeed == _btn30Mins)
	{
		timeSpan = 12 * 5 * 6;
	}
	else if (_timeSpeed == _btn1Hour)
	{
		timeSpan = 12 * 5 * 6 * 2;
	}
	else if (_timeSpeed == _btn1Day)
	{
		timeSpan = 12 * 5 * 6 * 2 * 24;
	}

	for (int i = 0; i < timeSpan && !_pause; ++i)
	{
		TimeTrigger trigger;
		trigger = _game->getSavedGame()->getTime()->advance();
		switch (trigger)
		{
		case TIME_1MONTH:
			time1Month();
		case TIME_1DAY:
			time1Day();
		case TIME_1HOUR:
			time1Hour();
		case TIME_30MIN:
			time30Minutes();
		case TIME_10MIN:
			time10Minutes();
		case TIME_5SEC:
			time5Seconds();
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
void GeoscapeState::time5Seconds()
{
	// Handle UFO logic
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end(); ++i)
	{
		(*i)->think();
		if ((*i)->reachedDestination() || (*i)->getHoursCrashed() == 0)
		{
			(*i)->setDetected(false);
			if (!(*i)->getFollowers()->empty())
			{
				popup(new UfoLostState(_game, (*i)->getName(_game->getLanguage())));
			}
		}
	}

	// Handle craft logic
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			if ((*j)->getDestination() != 0)
			{
				Ufo* u = dynamic_cast<Ufo*>((*j)->getDestination());
				if (u != 0 && !u->getDetected())
				{
					(*j)->setDestination(0);
					Waypoint *w = new Waypoint();
					w->setLongitude(u->getLongitude());
					w->setLatitude(u->getLatitude());
					w->setId(u->getId());
					popup(new GeoscapeCraftState(_game, (*j), _globe, w));
				}
			}
			(*j)->think();
			if ((*j)->reachedDestination())
			{
				Ufo* u = dynamic_cast<Ufo*>((*j)->getDestination());
				Waypoint *w = dynamic_cast<Waypoint*>((*j)->getDestination());
				TerrorSite* t = dynamic_cast<TerrorSite*>((*j)->getDestination());
				if (u != 0)
				{
					if (!u->isCrashed())
					{
						timerReset();
						_music = false;
						popup(new DogfightState(_game, _globe, (*j), u));
					}
					else
					{
						if ((*j)->getNumSoldiers() > 0)
						{
							// look up polygons texture
							int texture, shade;
							_globe->getPolygonTextureAndShade(u->getLongitude(), u->getLatitude(), &texture, &shade);
							_music = false;
							timerReset();
							popup(new ConfirmLandingState(_game, *j, texture, shade));
						}
						else
						{
							(*j)->returnToBase();
						}
					}
				}
				else if (w != 0)
				{
					popup(new CraftPatrolState(_game, (*j), _globe));
					(*j)->setDestination(0);
				}
				else if (t != 0)
				{
					if ((*j)->getNumSoldiers() > 0)
					{
						// look up polygons texture
						int texture, shade;
						_globe->getPolygonTextureAndShade(t->getLongitude(), t->getLatitude(), &texture, &shade);
						_music = false;
						timerReset();
						popup(new ConfirmLandingState(_game, *j, texture, shade));
					}
					else
					{
						(*j)->returnToBase();
					}
				}
			}
		}
	}

	// Clean up dead UFOs
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end();)
	{
		if ((*i)->reachedDestination() || (*i)->getHoursCrashed() == 0)
		{
			delete *i;
			i = _game->getSavedGame()->getUfos()->erase(i);
		}
		else
		{
			++i;
		}
	}

	// Clean up terror sites
	for (std::vector<TerrorSite*>::iterator i = _game->getSavedGame()->getTerrorSites()->begin(); i != _game->getSavedGame()->getTerrorSites()->end();)
	{
		if ((*i)->getHoursActive() == 0 && (*i)->getFollowers()->empty()) // CHEEKY EXPLOIT
		{
			delete *i;
			i = _game->getSavedGame()->getTerrorSites()->erase(i);
		}
		else
		{
			++i;
		}
	}

	// Clean up unused waypoints
	for (std::vector<Waypoint*>::iterator i = _game->getSavedGame()->getWaypoints()->begin(); i != _game->getSavedGame()->getWaypoints()->end();)
	{
		if ((*i)->getFollowers()->empty())
		{
			delete *i;
			i = _game->getSavedGame()->getWaypoints()->erase(i);
		}
		else
		{
			++i;
		}
	}
}

/**
 * Takes care of any game logic that has to
 * run every game ten minutes, like fuel consumption.
 */
void GeoscapeState::time10Minutes()
{
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			if ((*j)->getStatus() == "STR_OUT")
			{
				(*j)->consumeFuel();
				if (!(*j)->getLowFuel() && (*j)->getFuel() <= (*j)->getFuelLimit())
				{
					(*j)->setLowFuel(true);
					(*j)->returnToBase();
					popup(new LowFuelState(_game, (*j), this));
				}
			}
		}
	}
}

/**
 * Takes care of any game logic that has to
 * run every game half hour, like UFO detection.
 */
void GeoscapeState::time30Minutes()
{
	// Spawn UFOs
	std::vector<std::string> ufos = _game->getRuleset()->getUfosList();
	int chance = RNG::generate(1, 100);
	if (chance <= 40)
	{
		// Makes smallest UFO the more likely, biggest UFO the least likely
		// eg. 0 - 0..6, 1 - 6..10, etc.
		unsigned int range = RNG::generate(1, (ufos.size()*(ufos.size()+1))/2);
		unsigned int type = 0;
		for (unsigned int i = 0, j = 1; i < ufos.size(); ++i, j += ufos.size()-i)
		{
			if (j <= range && range < j + ufos.size()-i)
			{
				type = i;
				break;
			}
		}
		Ufo *u = new Ufo(_game->getRuleset()->getUfo(ufos[type]));
		u->setLongitude(RNG::generate(0.0, 2*M_PI));
		u->setLatitude(RNG::generate(-M_PI_2, M_PI_2));
		Waypoint *w = new Waypoint();
		w->setLongitude(RNG::generate(0.0, 2*M_PI));
		w->setLatitude(RNG::generate(-M_PI_2, M_PI_2));
		u->setDestination(w);
		u->setSpeed(RNG::generate(u->getRules()->getMaxSpeed() / 4, u->getRules()->getMaxSpeed() / 2));
		int race = RNG::generate(1, 2);
		if (race == 1)
			u->setAlienRace("STR_SECTOID");
		else
			u->setAlienRace("STR_FLOATER");
		_game->getSavedGame()->getUfos()->push_back(u);
	}

	// Handle craft maintenance
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			if ((*j)->getStatus() == "STR_REFUELLING")
			{
				std::string item = (*j)->getRules()->getRefuelItem();
				if (item == "")
				{
					(*j)->refuel();
				}
				else
				{
					if ((*i)->getItems()->getItem(item) > 0)
					{
						(*i)->getItems()->removeItem(item);
						(*j)->refuel();
					}
					else
					{
						std::wstringstream ss;
						ss << _game->getLanguage()->getString("STR_NOT_ENOUGH");
						ss << _game->getLanguage()->getString(item);
						ss << _game->getLanguage()->getString("STR_TO_REFUEL");
						ss << (*j)->getName(_game->getLanguage());
						ss << _game->getLanguage()->getString("STR_AT_");
						ss << (*i)->getName();
						popup(new CraftErrorState(_game, this, ss.str()));
						(*j)->setStatus("STR_READY");
					}
				}
			}
		}
	}

	// Handle UFO detection
	for (std::vector<Ufo*>::iterator u = _game->getSavedGame()->getUfos()->begin(); u != _game->getSavedGame()->getUfos()->end(); ++u)
	{
		if ((*u)->isCrashed())
			continue;
		if (!(*u)->getDetected())
		{
			bool detected = false;
			for (std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end() && !detected; ++b)
			{
				if ((*b)->detect(*u))
				{
					detected = true;
				}
				for (std::vector<Craft*>::iterator c = (*b)->getCrafts()->begin(); c != (*b)->getCrafts()->end() && !detected; ++c)
				{
					if ((*c)->getLongitude() == (*b)->getLongitude() && (*c)->getLatitude() == (*b)->getLatitude() && (*c)->getDestination() == 0)
						continue;
					if ((*c)->detect(*u))
					{
						detected = true;
					}
				}
			}
			if (detected)
			{
				(*u)->setDetected(detected);
				popup(new UfoDetectedState(_game, (*u), this, true));
			}
		}
		else
		{
			bool detected = false;
			for (std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end() && !detected; ++b)
			{
				detected = detected || (*b)->insideRadarRange(*u);
				for (std::vector<Craft*>::iterator c = (*b)->getCrafts()->begin(); c != (*b)->getCrafts()->end() && !detected; ++c)
				{
					detected = detected || (*c)->detect(*u);
				}
			}
			(*u)->setDetected(detected);
			if (!detected && !(*u)->getFollowers()->empty())
			{
				popup(new UfoLostState(_game, (*u)->getName(_game->getLanguage())));
			}
		}
	}
}

/**
 * Takes care of any game logic that has to
 * run every game hour, like transfers.
 */
void GeoscapeState::time1Hour()
{
	// Handle craft maintenance
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			if ((*j)->getStatus() == "STR_REPAIRS")
			{
				(*j)->repair();
			}
			else if ((*j)->getStatus() == "STR_REARMING")
			{
				std::string s = (*j)->rearm();
				if (s != "")
				{
					std::wstringstream ss;
					ss << _game->getLanguage()->getString("STR_NOT_ENOUGH");
					ss << _game->getLanguage()->getString(s);
					ss << _game->getLanguage()->getString("STR_TO_REARM");
					ss << (*j)->getName(_game->getLanguage());
					ss << _game->getLanguage()->getString("STR_AT_");
					ss << (*i)->getName();
					popup(new CraftErrorState(_game, this, ss.str()));
				}
			}
		}
	}

	// Handle crashed UFOs expiring
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end(); ++i)
	{
		if ((*i)->isCrashed() && (*i)->getHoursCrashed() > 0)
		{
			(*i)->setHoursCrashed((*i)->getHoursCrashed() - 1);
		}
	}

	// Handle terror sites expiring
	for (std::vector<TerrorSite*>::iterator i = _game->getSavedGame()->getTerrorSites()->begin(); i != _game->getSavedGame()->getTerrorSites()->end(); ++i)
	{
		if ((*i)->getHoursActive() > 0)
		{
			(*i)->setHoursActive((*i)->getHoursActive() - 1);
		}
	}

	// Handle transfers
	bool window = false;
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Transfer*>::iterator j = (*i)->getTransfers()->begin(); j != (*i)->getTransfers()->end(); ++j)
		{
			(*j)->advance(*i);
			if (!window && (*j)->getHours() == 0)
			{
				window = true;
			}
		}
	}
	if (window)
	{
		popup(new ItemsArrivingState(_game, this));
	}
	// Handle Production
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		std::map<Production*, productionProgress_e> toRemove;
		for (std::vector<Production*>::const_iterator j = (*i)->getProductions().begin(); j != (*i)->getProductions().end(); ++j)
		{
			toRemove[(*j)] = (*j)->step((*i), _game->getSavedGame(), _game->getRuleset());
		}
		for (std::map<Production*, productionProgress_e>::iterator j = toRemove.begin(); j != toRemove.end(); ++j)
		{
			if (j->second > PROGRESS_NOT_COMPLETE)
			{
				(*i)->removeProduction (j->first);
				popup(new ProductionCompleteState(_game, _game->getLanguage()->getString(j->first->getRules()->getName()), (*i)->getName(), j->second));
				timerReset();
			}
		}
	}
}

/**
 * Takes care of any game logic that has to
 * run every game day, like constructions.
 */
void GeoscapeState::time1Day()
{
	// Spawn terror sites
	int chance = RNG::generate(1, 20);
	if (chance <= 2)
	{
		// Pick a city
		RuleRegion* region = 0;
		std::vector<std::string> regions = _game->getRuleset()->getRegionsList();
		do
		{
			region = _game->getRuleset()->getRegion(regions[RNG::generate(0, regions.size()-1)]);
		}
		while (region->getCities()->empty());
		City *city = (*region->getCities())[RNG::generate(0, region->getCities()->size()-1)];

		TerrorSite *t = new TerrorSite();
		t->setLongitude(city->getLongitude());
		t->setLatitude(city->getLatitude());
		t->setId(_game->getSavedGame()->getId("STR_TERROR_SITE"));
		t->setHoursActive(24 + RNG::generate(0, 24));
		int race = RNG::generate(1, 2);
		if (race == 1)
			t->setAlienRace("STR_SECTOID");
		else
			t->setAlienRace("STR_FLOATER");
		_game->getSavedGame()->getTerrorSites()->push_back(t);
		popup(new AlienTerrorState(_game, city, this));
	}

	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		// Handle facility construction
		for (std::vector<BaseFacility*>::iterator j = (*i)->getFacilities()->begin(); j != (*i)->getFacilities()->end(); ++j)
		{
			if ((*j)->getBuildTime() > 0)
			{
				(*j)->build();
				if ((*j)->getBuildTime() == 0)
				{
					timerReset();
					popup(new ProductionCompleteState(_game, _game->getLanguage()->getString((*j)->getRules()->getType()), (*i)->getName()));
				}
			}
		}
		// Handle science project
		std::vector<ResearchProject*> finished;
		for(std::vector<ResearchProject*>::const_iterator iter = (*i)->getResearch().begin (); iter != (*i)->getResearch().end (); ++iter)
		{
			if((*iter)->step())
			{
				finished.push_back(*iter);
			}
		}
		for(std::vector<ResearchProject*>::const_iterator iter = finished.begin (); iter != finished.end (); ++iter)
		{
			(*i)->removeResearch(*iter);
			const RuleResearch * research = (*iter)->getRules ();
			_game->getSavedGame()->addFinishedResearch(research, _game->getRuleset ());
			std::vector<RuleResearch *> newPossibleResearch;
			_game->getSavedGame()->getDependableResearch (newPossibleResearch, (*iter)->getRules(), _game->getRuleset(), *i);
			timerReset();
			popup(new ResearchCompleteState (_game, research));
			popup(new NewPossibleResearchState(_game, *i, newPossibleResearch));
			delete(*iter);
		}
		// Handle soldier wounds
		for (std::vector<Soldier*>::iterator j = (*i)->getSoldiers()->begin(); j != (*i)->getSoldiers()->end(); ++j)
		{
			if ((*j)->getWoundRecovery() > 0)
			{
				(*j)->heal();
			}
		}
	}
}

/**
 * Takes care of any game logic that has to
 * run every game month, like funding.
 */
void GeoscapeState::time1Month()
{
	// Handle funding
	timerReset();
	_game->getSavedGame()->monthlyFunding();
	popup(new MonthlyReportState(_game));
}

/**
 * Slows down the timer back to minimum speed,
 * for when important events occur.
 */
void GeoscapeState::timerReset()
{
	SDL_Event ev;
	ev.button.button = SDL_BUTTON_LEFT;
	Action act(&ev, _game->getScreen()->getXScale(), _game->getScreen()->getYScale());
	_btn5Secs->mousePress(&act, this);
}

/**
 * Adds a new popup window to the queue
 * (this prevents popups from overlapping)
 * and pauses the game timer respectively.
 * @param state Pointer to popup state.
 */
void GeoscapeState::popup(State *state)
{
	_pause = true;
	_popups.push_back(state);
}

/**
 * Returns a pointer to the Geoscape globe for
 * access by other substates.
 * @return Pointer to globe.
 */
Globe *const GeoscapeState::getGlobe() const
{
	return _globe;
}

/**
 * Processes any left-clicks on globe markers,
 * or right-clicks to scroll the globe.
 * @param action Pointer to an action.
 */

void GeoscapeState::globeClick(Action *action)
{
	int mouseX = (int)floor(action->getAbsoluteXMouse()), mouseY = (int)floor(action->getAbsoluteYMouse());

	// Clicking markers on the globe
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		std::vector<Target*> v = _globe->getTargets(mouseX, mouseY, false);
		if (!v.empty())
		{
			_game->pushState(new MultipleTargetsState(_game, v, 0, this));
		}
	}
}

/**
 * Opens the Intercept window.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnInterceptClick(Action *action)
{
	_game->pushState(new InterceptState(_game, _globe));
}

/**
 * Goes to the Basescape screen.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnBasesClick(Action *action)
{
	if (_game->getSavedGame()->getBases()->size() > 0)
	{
		_game->pushState(new BasescapeState(_game, _game->getSavedGame()->getBases()->front(), _globe));
	}
	else
	{
		_game->pushState(new BasescapeState(_game, 0, _globe));
	}
}

/**
 * Goes to the Graphs screen.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnGraphsClick(Action *action)
{
	_game->pushState(new GraphsState(_game));
}

/**
 * Goes to the Ufopaedia window.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnUfopaediaClick(Action *action)
{
	Ufopaedia::open(_game);
}

/**
 * Opens the Options window.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnOptionsClick(Action *action)
{
	_game->pushState(new GeoscapeOptionsState(_game));
}

/**
 * Goes to the Funding screen.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnFundingClick(Action *action)
{
	_game->pushState(new FundingState(_game));
}

/**
 * Starts rotating the globe to the left.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateLeftPress(Action *action)
{
	_globe->rotateLeft();
}

/**
 * Stops rotating the globe to the left.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateLeftRelease(Action *action)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe to the right.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateRightPress(Action *action)
{
	_globe->rotateRight();
}

/**
 * Stops rotating the globe to the right.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateRightRelease(Action *action)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe upwards.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateUpPress(Action *action)
{
	_globe->rotateUp();
}

/**
 * Stops rotating the globe upwards.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateUpRelease(Action *action)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe downwards.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateDownPress(Action *action)
{
	_globe->rotateDown();
}

/**
 * Stops rotating the globe downwards.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateDownRelease(Action *action)
{
	_globe->rotateStop();
}

/**
 * Zooms into the globe.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnZoomInLeftClick(Action *action)
{
	_globe->zoomIn();
}

/**
 * Zooms the globe maximum.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnZoomInRightClick(Action *action)
{
	_globe->zoomMax();
}

/**
 * Zooms out of the globe.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnZoomOutLeftClick(Action *action)
{
	_globe->zoomOut();
}

/**
 * Zooms the globe minimum.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnZoomOutRightClick(Action *action)
{
	_globe->zoomMin();
}

}