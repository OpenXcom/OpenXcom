/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "../Engine/Palette.h"
#include "../Engine/Screen.h"
#include "../Engine/Surface.h"
#include "../Engine/Options.h"
#include "Globe.h"
#include "../Interface/Text.h"
#include "../Interface/ImageButton.h"
#include "../Interface/Cursor.h"
#include "../Interface/FpsCounter.h"
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
#include "NewPossibleManufactureState.h"
#include "../Savegame/Production.h"
#include "../Ruleset/RuleManufacture.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/TerrorSite.h"
#include "../Savegame/AlienBase.h"
#include "../Ruleset/RuleRegion.h"
#include "../Ruleset/City.h"
#include "AlienTerrorState.h"
#include "AlienBaseState.h"
#include "../Savegame/Region.h"
#include "../Savegame/Country.h"
#include "../Ruleset/RuleCountry.h"
#include "../Ruleset/RuleAlienMission.h"
#include "../Savegame/AlienStrategy.h"
#include "../Savegame/AlienMission.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Battlescape/BriefingState.h"
#include "../Ruleset/UfoTrajectory.h"
#include "../Ruleset/Armor.h"
#include "BaseDefenseState.h"
#include "BaseDestroyedState.h"
#include "DefeatState.h"
#include <ctime>
#include <algorithm>
#include <functional>
#include <assert.h>
#include "../Menu/SaveState.h"
#include "../Menu/LoadState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Geoscape screen.
 * @param game Pointer to the core game.
 */
GeoscapeState::GeoscapeState(Game *game) : State(game), _pause(false), _music(false), _zoomInEffectDone(false), _zoomOutEffectDone(false), _battleMusic(false), _popups(), _dogfights(), _dogfightsToBeStarted(), _minimizedDogfights(0)
{
	int screenWidth = Options::getInt("baseXResolution");
	int screenHeight = Options::getInt("baseYResolution");

	_showFundsOnGeoscape = Options::getBool("showFundsOnGeoscape");

	// Create objects
	_bg = new Surface(320, 200, screenWidth-320, screenHeight/2-100);
	_globe = new Globe(_game, (screenWidth-64)/2, screenHeight/2, screenWidth-64, screenHeight, 0, 0);

	_btnIntercept = new ImageButton(63, 11, screenWidth-63, screenHeight/2-100);
	_btnBases = new ImageButton(63, 11, screenWidth-63, screenHeight/2-88);
	_btnGraphs = new ImageButton(63, 11, screenWidth-63, screenHeight/2-76);
	_btnUfopaedia = new ImageButton(63, 11, screenWidth-63, screenHeight/2-64);
	_btnOptions = new ImageButton(63, 11, screenWidth-63, screenHeight/2-52);
	_btnFunding = new ImageButton(63, 11, screenWidth-63, screenHeight/2-40);

	_btn5Secs = new ImageButton(31, 13, screenWidth-63, screenHeight/2+12);
	_btn1Min = new ImageButton(31, 13, screenWidth-31, screenHeight/2+12);
	_btn5Mins = new ImageButton(31, 13, screenWidth-63, screenHeight/2+26);
	_btn30Mins = new ImageButton(31, 13, screenWidth-31, screenHeight/2+26);
	_btn1Hour = new ImageButton(31, 13, screenWidth-63, screenHeight/2+40);
	_btn1Day = new ImageButton(31, 13, screenWidth-31, screenHeight/2+40);

	_btnRotateLeft = new InteractiveSurface(12, 10, screenWidth-61, screenHeight/2+76);
	_btnRotateRight = new InteractiveSurface(12, 10, screenWidth-37, screenHeight/2+76);
	_btnRotateUp = new InteractiveSurface(13, 12, screenWidth-49, screenHeight/2+62);
	_btnRotateDown = new InteractiveSurface(13, 12, screenWidth-49, screenHeight/2+87);
	_btnZoomIn = new InteractiveSurface(23, 23, screenWidth-25, screenHeight/2+56);
	_btnZoomOut = new InteractiveSurface(13, 17, screenWidth-20, screenHeight/2+82);

	_txtHour = new Text(20, 16, screenWidth-61, screenHeight/2-26);
	_txtHourSep = new Text(4, 16, screenWidth-41, screenHeight/2-26);
	_txtMin = new Text(20, 16, screenWidth-37, screenHeight/2-26);
	_txtMinSep = new Text(4, 16, screenWidth-17, screenHeight/2-26);
	_txtSec = new Text(11, 8, screenWidth-13, screenHeight/2-20);
	_txtWeekday = new Text(59, 8, screenWidth-61, screenHeight/2-13);
	_txtDay = new Text(29, 8, screenWidth-61, screenHeight/2-6);
	_txtMonth = new Text(29, 8, screenWidth-32, screenHeight/2-6);
	_txtYear = new Text(59, 8, screenWidth-61, screenHeight/2+1);
	if (_showFundsOnGeoscape)
	{
		_txtFunds = new Text(59, 8, screenWidth-61, screenHeight/2-27);
		_txtHour->setY(_txtHour->getY()+6);
		_txtHourSep->setY(_txtHourSep->getY()+6);
		_txtMin->setY(_txtMin->getY()+6);
		_txtMinSep->setY(_txtMinSep->getY()+6);
		_txtMinSep->setX(_txtMinSep->getX()-10);
		_txtSec->setX(_txtSec->getX()-10);
	}

	_timeSpeed = _btn5Secs;
	_timer = new Timer(100);

	_zoomInEffectTimer = new Timer(50);
	_zoomOutEffectTimer = new Timer(50);
	_dogfightStartTimer = new Timer(250);

	_txtDebug = new Text(100, 8, 0, 0);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());

	// Fix system colors
	_game->getCursor()->setColor(Palette::blockOffset(15)+12);
	_game->getFpsCounter()->setColor(Palette::blockOffset(15)+12);

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

	if (_showFundsOnGeoscape) add(_txtFunds);
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
	_btnIntercept->onKeyboardPress((ActionHandler)&GeoscapeState::btnInterceptClick, (SDLKey)Options::getInt("keyGeoIntercept"));

	_btnBases->copy(_bg);
	_btnBases->setColor(Palette::blockOffset(15)+5);
	_btnBases->onMouseClick((ActionHandler)&GeoscapeState::btnBasesClick);
	_btnBases->onKeyboardPress((ActionHandler)&GeoscapeState::btnBasesClick, (SDLKey)Options::getInt("keyGeoBases"));

	_btnGraphs->copy(_bg);
	_btnGraphs->setColor(Palette::blockOffset(15)+5);
	_btnGraphs->onMouseClick((ActionHandler)&GeoscapeState::btnGraphsClick);
	_btnGraphs->onKeyboardPress((ActionHandler)&GeoscapeState::btnGraphsClick, (SDLKey)Options::getInt("keyGeoGraphs"));

	_btnUfopaedia->copy(_bg);
	_btnUfopaedia->setColor(Palette::blockOffset(15)+5);
	_btnUfopaedia->onMouseClick((ActionHandler)&GeoscapeState::btnUfopaediaClick);
	_btnUfopaedia->onKeyboardPress((ActionHandler)&GeoscapeState::btnUfopaediaClick, (SDLKey)Options::getInt("keyGeoUfopedia"));

	_btnOptions->copy(_bg);
	_btnOptions->setColor(Palette::blockOffset(15)+5);
	_btnOptions->onMouseClick((ActionHandler)&GeoscapeState::btnOptionsClick);
	_btnOptions->onKeyboardPress((ActionHandler)&GeoscapeState::btnOptionsClick, (SDLKey)Options::getInt("keyGeoOptions"));

	_btnFunding->copy(_bg);
	_btnFunding->setColor(Palette::blockOffset(15)+5);
	_btnFunding->onMouseClick((ActionHandler)&GeoscapeState::btnFundingClick);
	_btnFunding->onKeyboardPress((ActionHandler)&GeoscapeState::btnFundingClick, (SDLKey)Options::getInt("keyGeoFunding"));

	_btn5Secs->copy(_bg);
	_btn5Secs->setColor(Palette::blockOffset(15)+5);
	_btn5Secs->setGroup(&_timeSpeed);
	_btn5Secs->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, (SDLKey)Options::getInt("keyGeoSpeed1"));

	_btn1Min->copy(_bg);
	_btn1Min->setColor(Palette::blockOffset(15)+5);
	_btn1Min->setGroup(&_timeSpeed);
	_btn1Min->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, (SDLKey)Options::getInt("keyGeoSpeed2"));

	_btn5Mins->copy(_bg);
	_btn5Mins->setColor(Palette::blockOffset(15)+5);
	_btn5Mins->setGroup(&_timeSpeed);
	_btn5Mins->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, (SDLKey)Options::getInt("keyGeoSpeed3"));

	_btn30Mins->copy(_bg);
	_btn30Mins->setColor(Palette::blockOffset(15)+5);
	_btn30Mins->setGroup(&_timeSpeed);
	_btn30Mins->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, (SDLKey)Options::getInt("keyGeoSpeed4"));

	_btn1Hour->copy(_bg);
	_btn1Hour->setColor(Palette::blockOffset(15)+5);
	_btn1Hour->setGroup(&_timeSpeed);
	_btn1Hour->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, (SDLKey)Options::getInt("keyGeoSpeed5"));

	_btn1Day->copy(_bg);
	_btn1Day->setColor(Palette::blockOffset(15)+5);
	_btn1Day->setGroup(&_timeSpeed);
	_btn1Day->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, (SDLKey)Options::getInt("keyGeoSpeed6"));

	_btnRotateLeft->onMousePress((ActionHandler)&GeoscapeState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateLeftRelease);
	_btnRotateLeft->onKeyboardPress((ActionHandler)&GeoscapeState::btnRotateLeftPress, (SDLKey)Options::getInt("keyGeoLeft"));
	_btnRotateLeft->onKeyboardRelease((ActionHandler)&GeoscapeState::btnRotateLeftRelease, (SDLKey)Options::getInt("keyGeoLeft"));

	_btnRotateRight->onMousePress((ActionHandler)&GeoscapeState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateRightRelease);
	_btnRotateRight->onKeyboardPress((ActionHandler)&GeoscapeState::btnRotateRightPress, (SDLKey)Options::getInt("keyGeoRight"));
	_btnRotateRight->onKeyboardRelease((ActionHandler)&GeoscapeState::btnRotateRightRelease, (SDLKey)Options::getInt("keyGeoRight"));

	_btnRotateUp->onMousePress((ActionHandler)&GeoscapeState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateUpRelease);
	_btnRotateUp->onKeyboardPress((ActionHandler)&GeoscapeState::btnRotateUpPress, (SDLKey)Options::getInt("keyGeoUp"));
	_btnRotateUp->onKeyboardRelease((ActionHandler)&GeoscapeState::btnRotateUpRelease, (SDLKey)Options::getInt("keyGeoUp"));

	_btnRotateDown->onMousePress((ActionHandler)&GeoscapeState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateDownRelease);
	_btnRotateDown->onKeyboardPress((ActionHandler)&GeoscapeState::btnRotateDownPress, (SDLKey)Options::getInt("keyGeoDown"));
	_btnRotateDown->onKeyboardRelease((ActionHandler)&GeoscapeState::btnRotateDownRelease, (SDLKey)Options::getInt("keyGeoDown"));

	_btnZoomIn->onMouseClick((ActionHandler)&GeoscapeState::btnZoomInLeftClick, SDL_BUTTON_LEFT);
	_btnZoomIn->onMouseClick((ActionHandler)&GeoscapeState::btnZoomInRightClick, SDL_BUTTON_RIGHT);
	_btnZoomIn->onKeyboardPress((ActionHandler)&GeoscapeState::btnZoomInLeftClick, (SDLKey)Options::getInt("keyGeoZoomIn"));

	_btnZoomOut->onMouseClick((ActionHandler)&GeoscapeState::btnZoomOutLeftClick, SDL_BUTTON_LEFT);
	_btnZoomOut->onMouseClick((ActionHandler)&GeoscapeState::btnZoomOutRightClick, SDL_BUTTON_RIGHT);
	_btnZoomOut->onKeyboardPress((ActionHandler)&GeoscapeState::btnZoomOutLeftClick, (SDLKey)Options::getInt("keyGeoZoomOut"));
	
	// dirty hacks to get the rotate buttons to work in "classic" style
	_btnRotateLeft->setListButton();
	_btnRotateRight->setListButton();
	_btnRotateUp->setListButton();
	_btnRotateDown->setListButton();

	if (_showFundsOnGeoscape)
	{
		_txtFunds->setSmall();
		_txtFunds->setColor(Palette::blockOffset(15)+4);
		_txtFunds->setText(L"");
		_txtFunds->setAlign(ALIGN_CENTER);
	}

	if (_showFundsOnGeoscape) _txtHour->setSmall(); else _txtHour->setBig();
	_txtHour->setColor(Palette::blockOffset(15)+4);
	_txtHour->setAlign(ALIGN_RIGHT);
	_txtHour->setText(L"");

	if (_showFundsOnGeoscape) _txtHourSep->setSmall(); else _txtHourSep->setBig();
	_txtHourSep->setColor(Palette::blockOffset(15)+4);
	_txtHourSep->setText(L":");

	if (_showFundsOnGeoscape) _txtMin->setSmall(); else _txtMin->setBig();
	_txtMin->setColor(Palette::blockOffset(15)+4);
	_txtMin->setText(L"");

	if (_showFundsOnGeoscape) _txtMinSep->setSmall(); else _txtMinSep->setBig();
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

	_zoomInEffectTimer->onTimer((StateHandler)&GeoscapeState::zoomInEffect);
	_zoomOutEffectTimer->onTimer((StateHandler)&GeoscapeState::zoomOutEffect);
	_dogfightStartTimer->onTimer((StateHandler)&GeoscapeState::startDogfight);

	timeDisplay();
}

/**
 * Deletes timers.
 */
GeoscapeState::~GeoscapeState()
{
	delete _timer;
	delete _zoomInEffectTimer;
	delete _zoomOutEffectTimer;
	delete _dogfightStartTimer;
}

/**
 * Handle blitting of Geoscape and Dogfights.
 */
void GeoscapeState::blit()
{
	State::blit();
	for(std::vector<DogfightState*>::iterator it = _dogfights.begin(); it != _dogfights.end(); ++it)
	{
		(*it)->blit();
	}
}
/**
 * Handle key shortcuts.
 * @param action Pointer to an action.
 */
void GeoscapeState::handle(Action *action)
{
	if(_dogfights.size() == _minimizedDogfights)
	{
		State::handle(action);
	}

	if (action->getDetails()->type == SDL_KEYDOWN)
	{
		// "ctrl-d" - enable debug mode
		if (Options::getBool("debug") && action->getDetails()->key.keysym.sym == SDLK_d && (SDL_GetModState() & KMOD_CTRL) != 0)
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
		// quick save and quick load
		else if (action->getDetails()->key.keysym.sym == Options::getInt("keyQuickSave") && Options::getInt("autosave") == 1)
			_game->pushState(new SaveState(_game, true, true));
		else if (action->getDetails()->key.keysym.sym == Options::getInt("keyQuickLoad") && Options::getInt("autosave") == 1)
			_game->pushState(new LoadState(_game, true, true));
	}
	if(!_dogfights.empty())
	{
		for(std::vector<DogfightState*>::iterator it = _dogfights.begin(); it != _dogfights.end(); ++it)
		{
			(*it)->handle(action);
		}
		_minimizedDogfights = minimizedDogfightsCount();
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
	_globe->onMouseOver(0);
	_globe->focus();
	_globe->draw();

	// Set music if it's not already playing
	if (!_music && !_battleMusic)
	{
		if (_game->getSavedGame()->getMonthsPassed() == -1)
		{
			_game->getResourcePack()->getMusic("GMGEO1")->play();
		}
		else
		{
			_game->getResourcePack()->getRandomMusic("GMGEO")->play();
		}
		_music = true;
	}
	_globe->unsetNewBaseHover();
}

/**
 * Runs the game timer and handles popups.
 */
void GeoscapeState::think()
{
	State::think();

	_zoomInEffectTimer->think(this, 0);
	_zoomOutEffectTimer->think(this, 0);
	_dogfightStartTimer->think(this, 0);

	if (_game->getSavedGame()->getMonthsPassed() == -1)
	{
		_game->getSavedGame()->addMonth();
		determineAlienMissions(true);
		_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - _game->getSavedGame()->getBaseMaintenance());
	}
	if(_popups.empty() && _dogfights.empty() && (!_zoomInEffectTimer->isRunning() || _zoomInEffectDone) && (!_zoomOutEffectTimer->isRunning() || _zoomOutEffectDone))
	{
		// Handle timers
		_timer->think(this, 0);
	}
	else
	{
		if(!_dogfights.empty() || _minimizedDogfights != 0)
		{
			handleDogfights();
		}
		if(!_popups.empty())
		{
			// Handle popups
			_globe->rotateStop();
			_game->pushState(*_popups.begin());
			_popups.erase(_popups.begin());
		}
	}
	if (_battleMusic && _dogfights.empty() && !_dogfightStartTimer->isRunning())
	{
		_battleMusic = false;
		musicStop();
	}
}

/**
 * Updates the Geoscape clock with the latest
 * game time and date in human-readable format. (+Funds)
 */
void GeoscapeState::timeDisplay()
{
	std::stringstream ss, ss2;
	std::wstringstream ss3, ss4, ss5;

	if (_showFundsOnGeoscape)
	{
		_txtFunds->setText(Text::formatFunding(_game->getSavedGame()->getFunds()));
	}

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
	// Game over if there are no more bases.
	if (_game->getSavedGame()->getBases()->size() == 0)
	{
		popup(new DefeatState(_game));
		return;
	}

	// Handle UFO logic
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end(); ++i)
	{
		switch ((*i)->getStatus())
		{
		case Ufo::FLYING:
			if (!_zoomInEffectTimer->isRunning() && !_zoomOutEffectTimer->isRunning())
			{
				(*i)->think();
				if ((*i)->reachedDestination())
				{
					unsigned terrorSiteCount = _game->getSavedGame()->getTerrorSites()->size();
					AlienMission *mission = (*i)->getMission();
					bool detected = (*i)->getDetected();
					mission->ufoReachedWaypoint(**i, *_game, *_globe);
					if (detected != (*i)->getDetected() && !(*i)->getFollowers()->empty())
					{
						if (!((*i)->getTrajectory().getID() == "__RETALIATION_ASSAULT_RUN" && (*i)->getStatus() ==  Ufo::LANDED))
							popup(new UfoLostState(_game, (*i)->getName(_game->getLanguage())));
					}
					if (terrorSiteCount < _game->getSavedGame()->getTerrorSites()->size())
					{
						const TerrorSite &ts = *_game->getSavedGame()->getTerrorSites()->back();
						const City *city = _game->getRuleset()->locateCity(ts.getLongitude(), ts.getLatitude());
						assert(city);
						popup(new AlienTerrorState(_game, city, this));
					}
					// If UFO was destroyed, don't spawn missions
					if ((*i)->getStatus() == Ufo::DESTROYED)
						return;
					if (Base *base = dynamic_cast<Base*>((*i)->getDestination()))
					{
						mission->setWaveCountdown(30 * (RNG::generate(0, 48) + 400));
						(*i)->setDestination(0);
						base->setupDefenses();
						timerReset();
						if (base->getDefenses()->size() > 0)
						{
							popup(new BaseDefenseState(_game, base, *i, this));
						}
						else
						{
                            handleBaseDefense(base, *i);
                            return;
						}
					}
				}
			}
			break;
		case Ufo::LANDED:
			(*i)->think();
			if ((*i)->getSecondsRemaining() == 0)
			{
				AlienMission *mission = (*i)->getMission();
				bool detected = (*i)->getDetected();
				mission->ufoLifting(**i, *_game, *_globe);
				if (detected != (*i)->getDetected() && !(*i)->getFollowers()->empty())
				{
					popup(new UfoLostState(_game, (*i)->getName(_game->getLanguage())));
				}
			}
			break;
		case Ufo::CRASHED:
			(*i)->think();
			if ((*i)->getSecondsRemaining() == 0)
			{
				(*i)->setDetected(false);
				(*i)->setStatus(Ufo::DESTROYED);
			}
			break;
		case Ufo::DESTROYED:
			// Nothing to do
			break;
		}
	}

	// Handle craft logic
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end();)
		{
			if ((*j)->isDestroyed())
			{
				for(std::vector<Country*>::iterator country = _game->getSavedGame()->getCountries()->begin(); country != _game->getSavedGame()->getCountries()->end(); ++country)
				{
					if((*country)->getRules()->insideCountry((*j)->getLongitude(), (*j)->getLatitude()))
					{
						(*country)->addActivityXcom(-(*j)->getRules()->getScore());
						break;
					}
				}
				for(std::vector<Region*>::iterator region = _game->getSavedGame()->getRegions()->begin(); region != _game->getSavedGame()->getRegions()->end(); ++region)
				{
					if((*region)->getRules()->insideRegion((*j)->getLongitude(), (*j)->getLatitude()))
					{
						(*region)->addActivityXcom(-(*j)->getRules()->getScore());
						break;
					}
				}

				delete *j;
				j = (*i)->getCrafts()->erase(j);
				continue;
			}
			if ((*j)->getDestination() != 0)
			{
				Ufo* u = dynamic_cast<Ufo*>((*j)->getDestination());
				if (u != 0 && !u->getDetected())
				{
					if (u->getTrajectory().getID() == "__RETALIATION_ASSAULT_RUN" && (u->getStatus() == Ufo::LANDED || u->getStatus() == Ufo::DESTROYED))
					{
						(*j)->returnToBase();
					}
					else
					{
						(*j)->setDestination(0);
						Waypoint *w = new Waypoint();
						w->setLongitude(u->getLongitude());
						w->setLatitude(u->getLatitude());
						w->setId(u->getId());
						popup(new GeoscapeCraftState(_game, (*j), _globe, w));
					}
				}
				if (u != 0 && u->getStatus() == Ufo::DESTROYED)
				{
					(*j)->returnToBase();
				}
			}
			if(!_zoomInEffectTimer->isRunning() && !_zoomOutEffectTimer->isRunning())
			{
				(*j)->think();
			}
			if((*j)->reachedDestination())
			{
				Ufo* u = dynamic_cast<Ufo*>((*j)->getDestination());
				Waypoint *w = dynamic_cast<Waypoint*>((*j)->getDestination());
				TerrorSite* t = dynamic_cast<TerrorSite*>((*j)->getDestination());
				AlienBase* b = dynamic_cast<AlienBase*>((*j)->getDestination());
				if (u != 0)
				{
					switch (u->getStatus())
					{
					case Ufo::FLYING:
						// Not more than 4 interceptions at a time.
						if(_dogfights.size() + _dogfightsToBeStarted.size() >= 4)
						{
							++j;
							continue;
						}
						if(!(*j)->isInDogfight() && !(*j)->getDistance(u))
						{
							_dogfightsToBeStarted.push_back(new DogfightState(_game, _globe, (*j), u));

							if(!_dogfightStartTimer->isRunning())
							{
								_pause = true;
								timerReset();
								_globe->center((*j)->getLongitude(), (*j)->getLatitude());
								startDogfight();
								_dogfightStartTimer->start();
							}
							if(!_battleMusic)
							{
								// Set music
								_game->getResourcePack()->getMusic("GMINTER")->play();
								_battleMusic = true;
							}
						}
						break;
					case Ufo::LANDED:
					case Ufo::CRASHED:
					case Ufo::DESTROYED: // Just before expiration
						if ((*j)->getNumSoldiers() > 0)
						{
							if(!(*j)->isInDogfight())
							{
								// look up polygons texture
								int texture, shade;
								_globe->getPolygonTextureAndShade(u->getLongitude(), u->getLatitude(), &texture, &shade);
								timerReset();
								popup(new ConfirmLandingState(_game, *j, texture, shade, this));
							}
						}
						else if (u->getStatus() != Ufo::LANDED)
						{
							(*j)->returnToBase();
						}
						break;
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
						timerReset();
						popup(new ConfirmLandingState(_game, *j, texture, shade, this));
					}
					else
					{
						(*j)->returnToBase();
					}
				}
				else if (b != 0)
				{
					if (b->isDiscovered())
					{
						if((*j)->getNumSoldiers() > 0)
						{
							int texture, shade;
							_globe->getPolygonTextureAndShade(b->getLongitude(), b->getLatitude(), &texture, &shade);
							timerReset();
							popup(new ConfirmLandingState(_game, *j, texture, shade, this));
						}
						else
						{
							(*j)->returnToBase();
						}
					}
				}
			}
			 ++j;
		}
	}

	// Clean up dead UFOs and end dogfights which were minimized.
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end();)
	{
		if ((*i)->getStatus() == Ufo::DESTROYED)
		{
			if(!(*i)->getFollowers()->empty())
			{
				// Remove all dogfights with this UFO.
				for(std::vector<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end();)
				{
					if((*d)->getUfo() == (*i))
					{
						delete *d;
						d = _dogfights.erase(d);
					}
					else
					{
						++d;
					}
				}
			}
			delete *i;
			i = _game->getSavedGame()->getUfos()->erase(i);
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
 * Functor that attempt to detect an XCOM base.
 */
class DetectXCOMBase: public std::unary_function<Ufo *, bool>
{
public:
	/// Create a detector for the given base.
	DetectXCOMBase(const Base &base) : _base(base) { /* Empty by design.  */ }
	/// Attempt detection
	bool operator()(const Ufo *ufo) const;
private:
	const Base &_base;	//!< The target base.
};

/**
 * Only UFOs within detection range of the base have a chance to detect it.
 * @param ufo Pointer to the UFO attempting detection.
 * @return If the base is detected by @a ufo.
 */
bool DetectXCOMBase::operator()(const Ufo *ufo) const
{
	// only UFOs on retaliation missions actively scan for bases
	if (ufo->getMissionType() != "STR_ALIEN_RETALIATION" && !Options::getBool("aggressiveRetaliation"))
		return false;

	// UFOs attacking a base don't detect!
	if (ufo->getTrajectory().getID() == "__RETALIATION_ASSAULT_RUN")
	{
		return false;
	}

	// UFOs have a detection range of 80 XCOM units.
	if (_base.getDistance(ufo) >= 80 * (1 / 60.0) * (M_PI / 180.0))
	{
		return false;
	}
	return ((int)_base.getDetectionChance() < RNG::generate(0, 100));
}

/**
 * Functor that marks an XCOM base for retaliation.
 * This is required because of the iterator type.
 */
struct SetRetaliationTarget: public std::unary_function<std::map<const Region *, Base *>::value_type, void>
{
	/// Mark as a valid retaliation target.
	void operator()(const argument_type &iter) const { iter.second->setRetaliationTarget(true); }
};

/**
 * Takes care of any game logic that has to
 * run every game ten minutes, like fuel consumption.
 */
void GeoscapeState::time10Minutes()
{
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		// Fuel consumption for XCOM craft.
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

				if ((*j)->getDestination() == 0)
				{
					for(std::vector<AlienBase*>::iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); b++)
					{
						if ((*j)->getDistance(*b) <= (1696 * (1 / 60.0) * (M_PI / 180) ))
						{
							// TODO: move the detection range to the ruleset, or use the pre-defined one (which is 600, but detection range should be 500).
							if ((50-((*j)->getDistance(*b) / (1696 * (1 / 60.0) * (M_PI / 180) )) * 50 >= RNG::generate(0, 100)) && !(*b)->isDiscovered())
							{
								(*b)->setDiscovered(true);
							}
						}
					}
				}
			}
		}
	}
	if (Options::getBool("aggressiveRetaliation"))
	{
		// Detect as many bases as possible.
		for (std::vector<Base*>::iterator iBase = _game->getSavedGame()->getBases()->begin(); iBase != _game->getSavedGame()->getBases()->end(); ++iBase)
		{
			// Find a UFO that detected this base, if any.
			std::vector<Ufo*>::const_iterator uu = std::find_if(_game->getSavedGame()->getUfos()->begin(), _game->getSavedGame()->getUfos()->end(), DetectXCOMBase(**iBase));
			if (uu != _game->getSavedGame()->getUfos()->end())
			{
				// Base found
				(*iBase)->setRetaliationTarget(true);
			}
		}
	}
	else
	{
		// Only remember last base in each region.
		std::map<const Region *, Base *> discovered;
		for (std::vector<Base*>::iterator iBase = _game->getSavedGame()->getBases()->begin(); iBase != _game->getSavedGame()->getBases()->end(); ++iBase)
		{
			// Find a UFO that detected this base, if any.
			std::vector<Ufo*>::const_iterator uu = std::find_if(_game->getSavedGame()->getUfos()->begin(), _game->getSavedGame()->getUfos()->end(), DetectXCOMBase(**iBase));
			if (uu != _game->getSavedGame()->getUfos()->end())
			{
				discovered[_game->getSavedGame()->locateRegion(**iBase)] = *iBase;
			}
		}
		// Now mark the bases as discovered.
		std::for_each(discovered.begin(), discovered.end(), SetRetaliationTarget());
	}
}

/** @brief Call AlienMission::think() with proper parameters.
 * This function object calls AlienMission::think() with the proper parameters.
 */
class callThink: public std::unary_function<AlienMission*, void>
{
public:
	/// Store the parameters.
	/**
	 * @param game The game engine.
	 * @param game The globe object.
	 */
	callThink(Game &game, const Globe &globe) : _game(game), _globe(globe) { /* Empty by design. */ }
	/// Call AlienMission::think() with stored parameters.
	void operator()(AlienMission *am) const { am->think(_game, _globe); }
private:
	Game &_game;
	const Globe &_globe;
};

/** @brief Process a TerrorSite.
 * This function object will count down towards expiring a TerrorSite, and handle expired TerrorSites.
 */
bool GeoscapeState::processTerrorSite(TerrorSite *ts) const
{
	if (ts->getSecondsRemaining() >= 30 * 60)
	{
		ts->setSecondsRemaining(ts->getSecondsRemaining() - 30 * 60);
		return false;
	}
	if (!ts->getFollowers()->empty()) // CHEEKY EXPLOIT
	{
		return false;
	}
	// Score and delete it.
	Region *region = _game->getSavedGame()->locateRegion(*ts);
	if (region)
	{
		//TODO: This should come from mission rules!
		region->addActivityAlien(1000);
		//kids, tell your folks... don't ignore terror sites.
	}
	for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
	{
		if ((*k)->getRules()->insideCountry(ts->getLongitude(), ts->getLatitude()))
		{
			(*k)->addActivityAlien(1000);
			break;
		}
	}
	delete ts;
	return true;
}

/** @brief Advance time for crashed UFOs.
 * This function object will decrease the expiration timer for crashed UFOs.
 */
struct expireCrashedUfo: public std::unary_function<Ufo*, void>
{
	/// Decrease UFO expiration timer.
	void operator()(Ufo *ufo) const
	{
		if (ufo->getStatus() == Ufo::CRASHED)
		{
			if (ufo->getSecondsRemaining() >= 30 * 60)
			{
				ufo->setSecondsRemaining(ufo->getSecondsRemaining() - 30 * 60);
				return;
			}
			// Marked expired UFOs for removal.
			ufo->setStatus(Ufo::DESTROYED);
		}
	}
};

/**
 * Takes care of any game logic that has to
 * run every game half hour, like UFO detection.
 */
void GeoscapeState::time30Minutes()
{
	// Decrease mission countdowns
	std::for_each(_game->getSavedGame()->getAlienMissions().begin(),
		      _game->getSavedGame()->getAlienMissions().end(),
		      callThink(*_game, *_globe));
	// Remove finished missions
	for (std::vector<AlienMission*>::iterator am = _game->getSavedGame()->getAlienMissions().begin();
		am != _game->getSavedGame()->getAlienMissions().end();)
	{
		if ((*am)->isOver())
		{
			delete *am;
			am = _game->getSavedGame()->getAlienMissions().erase(am);
		}
		else
		{
			++am;
		}
	}

	// Handle crashed UFOs expiration
	std::for_each(_game->getSavedGame()->getUfos()->begin(),
		      _game->getSavedGame()->getUfos()->end(),
		      expireCrashedUfo());


	// Handle craft maintenance and alien base detection
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

	// Handle UFO detection and give aliens points
	for (std::vector<Ufo*>::iterator u = _game->getSavedGame()->getUfos()->begin(); u != _game->getSavedGame()->getUfos()->end(); ++u)
	{
		int points = 0;
		switch ((*u)->getStatus())
		{
		case Ufo::LANDED:
			points++;
		case Ufo::FLYING:
			points++;
			// Get area
			for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
			{
				if ((*k)->getRules()->insideRegion((*u)->getLongitude(), (*u)->getLatitude()))
				{
					//one point per UFO in-flight per half hour
					(*k)->addActivityAlien(points);
					break;
				}
			}
			// Get country
			for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
			{
				if ((*k)->getRules()->insideCountry((*u)->getLongitude(), (*u)->getLatitude()))
				{
					//one point per UFO in-flight per half hour
					(*k)->addActivityAlien(points);
					break;
				}
			}
			if (!(*u)->getDetected())
			{
				bool detected = false;
				for (std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end() && !detected; ++b)
				{
					if ((*b)->detect(*u))
					{
						detected = true;
						if((*b)->getHyperDetection())
						{
							(*u)->setHyperDetected(true);
						}
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
					(*u)->setDetected(true);
					popup(new UfoDetectedState(_game, (*u), this, true, (*u)->getHyperDetected()));
				}
			}
			else
			{
				bool detected = false;
				for (std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end() && !detected; ++b)
				{
					detected = detected || (*b)->insideRadarRange(*u);
					if((*b)->getHyperDetection())
					{
						(*u)->setHyperDetected(true);
					}
					for (std::vector<Craft*>::iterator c = (*b)->getCrafts()->begin(); c != (*b)->getCrafts()->end() && !detected; ++c)
					{
						detected = detected || (*c)->detect(*u);
					}
				}
				if (!detected)
				{
					(*u)->setDetected(false);
					(*u)->setHyperDetected(false); // i'm not 100% sure this is correct, need verification.
					if (!(*u)->getFollowers()->empty())
					{
						popup(new UfoLostState(_game, (*u)->getName(_game->getLanguage())));
					}
				}
			}
			break;
		case Ufo::CRASHED:
		case Ufo::DESTROYED:
			break;
		}
	}

	// Processes TerrorSites
	for (std::vector<TerrorSite*>::iterator ts = _game->getSavedGame()->getTerrorSites()->begin();
		ts != _game->getSavedGame()->getTerrorSites()->end();)
	{
		if (processTerrorSite(*ts))
		{
			ts = _game->getSavedGame()->getTerrorSites()->erase(ts);
		}
		else
		{
			++ts;
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
 * This class will attempt to generate a supply mission for a base.
 * Each alien base has a 6/101 chance to generate a supply mission.
 */
class GenerateSupplyMission: public std::unary_function<const AlienBase *, void>
{
public:
	/// Store rules and game data references for later use.
	GenerateSupplyMission(const Ruleset &ruleset, SavedGame &save) : _ruleset(ruleset), _save(save) { /* Empty by design */ }
	/// Check and spawn mission.
	void operator()(const AlienBase *base) const;
private:
	const Ruleset &_ruleset;
	SavedGame &_save;
};

/**
 * Check and create supply mission for the given base.
 * There is a 6/101 chance of the mission spawning.
 * @param base A pointer to the alien base.
 */
void GenerateSupplyMission::operator()(const AlienBase *base) const
{
	if (RNG::generate(0, 100) < 6)
	{
		//Spawn supply mission for this base.
		const RuleAlienMission &rule = *_ruleset.getAlienMission("STR_ALIEN_SUPPLY");
		AlienMission *mission = new AlienMission(rule);
		mission->setRegion(_save.locateRegion(*base)->getRules()->getType(), _ruleset);
		mission->setId(_save.getId("ALIEN_MISSIONS"));
		mission->setRace(base->getAlienRace());
		mission->setAlienBase(base);
		mission->start();
		_save.getAlienMissions().push_back(mission);
	}
}

/**
 * Takes care of any game logic that has to
 * run every game day, like constructions.
 */
void GeoscapeState::time1Day()
{
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
			RuleResearch * bonus = 0;
			const RuleResearch * research = (*iter)->getRules ();
			// If "researched" the live alien, his body sent to the stores.
			if (Options::getBool("researchedItemsWillSpent") && research->needItem() && _game->getRuleset()->getUnit(research->getName()))
			{
				(*i)->getItems()->addItem(
					_game->getRuleset()->getArmor(
						_game->getRuleset()->getUnit(
							research->getName()
						)->getArmor()
					)->getCorpseItem()
				); // ;)
			}
			if((*iter)->getRules()->getGetOneFree().size() != 0)
			{
				std::vector<std::string> possibilities;
				for(std::vector<std::string>::const_iterator f = (*iter)->getRules()->getGetOneFree().begin(); f != (*iter)->getRules()->getGetOneFree().end(); ++f)
				{
					bool newFound = true;
					for(std::vector<const RuleResearch*>::const_iterator discovered = _game->getSavedGame()->getDiscoveredResearch().begin(); discovered != _game->getSavedGame()->getDiscoveredResearch().end(); ++discovered)
					{
						if(*f == (*discovered)->getName())
						{
							newFound = false;
						}
					}
					if(newFound)
					{
						possibilities.push_back(*f);
					}
				}
				if(possibilities.size() !=0)
				{
					int pick = RNG::generate(0, possibilities.size()-1);
					std::string sel = possibilities.at(pick);
					bonus = _game->getRuleset()->getResearch(sel);
					_game->getSavedGame()->addFinishedResearch(bonus, _game->getRuleset ());
					if(bonus->getLookup() != "")
					{
						_game->getSavedGame()->addFinishedResearch(_game->getRuleset()->getResearch(bonus->getLookup()), _game->getRuleset ());
					}
				}
			}
			const RuleResearch * newResearch = research;
			std::string name = research->getLookup() == "" ? research->getName() : research->getLookup();
			if(_game->getSavedGame()->isResearched(name))
			{
				newResearch = 0;
			}
			_game->getSavedGame()->addFinishedResearch(research, _game->getRuleset ());
			if(research->getLookup() != "")
			{
				_game->getSavedGame()->addFinishedResearch(_game->getRuleset()->getResearch(research->getLookup()), _game->getRuleset ());
			}
			popup(new ResearchCompleteState(_game, newResearch, bonus));
			std::vector<RuleResearch *> newPossibleResearch;
			_game->getSavedGame()->getDependableResearch (newPossibleResearch, (*iter)->getRules(), _game->getRuleset(), *i);
			std::vector<RuleManufacture *> newPossibleManufacture;
			_game->getSavedGame()->getDependableManufacture (newPossibleManufacture, (*iter)->getRules(), _game->getRuleset(), *i);
			timerReset();
			popup(new NewPossibleResearchState(_game, *i, newPossibleResearch));
			if (!newPossibleManufacture.empty())
			{
				popup(new NewPossibleManufactureState(_game, *i, newPossibleManufacture));
			}
			// now iterate through all the bases and remove this project from their labs
			for (std::vector<Base*>::iterator j = _game->getSavedGame()->getBases()->begin(); j != _game->getSavedGame()->getBases()->end(); ++j)
			{
				for (std::vector<ResearchProject*>::const_iterator iter2 = (*j)->getResearch().begin(); iter2 != (*j)->getResearch().end(); ++iter2)
				{
					if ((*iter)->getRules()->getName() == (*iter2)->getRules()->getName() && 
						_game->getRuleset()->getUnit((*iter2)->getRules()->getName()) == 0)
					{
						(*j)->removeResearch(*iter2);
						break;
					}
				}
			}
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
		// Handle psionic training
		if ((*i)->getAvailablePsiLabs() > 0 && Options::getBool("anytimePsiTraining"))
		{
			for(std::vector<Soldier*>::const_iterator s = (*i)->getSoldiers()->begin(); s != (*i)->getSoldiers()->end(); ++s)
				(*s)->trainPsi1Day();
		}
	}
	// handle regional and country points for alien bases
	for(std::vector<AlienBase*>::const_iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); ++b)
	{
		for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
		{
			if ((*k)->getRules()->insideRegion((*b)->getLongitude(), (*b)->getLatitude()))
			{
				(*k)->addActivityAlien(5);
				break;
			}
		}
		for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
		{
			if ((*k)->getRules()->insideCountry((*b)->getLongitude(), (*b)->getLatitude()))
			{
				(*k)->addActivityAlien(5);
				break;
			}
		}
	}

	// Handle resupply of alien bases.
	std::for_each(_game->getSavedGame()->getAlienBases()->begin(), _game->getSavedGame()->getAlienBases()->end(),
		      GenerateSupplyMission(*_game->getRuleset(), *_game->getSavedGame()));

	// Autosave
	if (Options::getInt("autosave") >= 2)
		_game->pushState(new SaveState(_game, true, false));
}

/**
 * Takes care of any game logic that has to
 * run every game month, like funding.
 */
void GeoscapeState::time1Month()
{
	_game->getSavedGame()->addMonth();

	int monthsPassed = _game->getSavedGame()->getMonthsPassed();
	bool newRetaliation = false;

	// Determine alien mission for this month.
	determineAlienMissions();
	if (monthsPassed > 5)
		determineAlienMissions();
	if (monthsPassed >= 14 - (int)(_game->getSavedGame()->getDifficulty())
		|| _game->getSavedGame()->isResearched("STR_THE_MARTIAN_SOLUTION"))
	{
		newRetaliation = true;
	}

	// Handle Psi-Training and initiate a new retaliation mission, if applicable
	bool psi = false;
	for(std::vector<Base*>::const_iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end(); ++b)
	{
		if (newRetaliation)
		{
			for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
			{
				if ((*i)->getRules()->insideRegion((*b)->getLongitude(), (*b)->getLatitude()))
				{
					if (!_game->getSavedGame()->getAlienMission((*i)->getRules()->getType(), "STR_ALIEN_RETALIATION"))
					{
						const RuleAlienMission &rule = *_game->getRuleset()->getAlienMission("STR_ALIEN_RETALIATION");
						AlienMission *mission = new AlienMission(rule);
						mission->setId(_game->getSavedGame()->getId("ALIEN_MISSIONS"));
						mission->setRegion((*i)->getRules()->getType(), *_game->getRuleset());
						int race = RNG::generate(0, _game->getRuleset()->getAlienRacesList().size()-2); // -2 to avoid "MIXED" race
						mission->setRace(_game->getRuleset()->getAlienRacesList().at(race));
						mission->start();
						_game->getSavedGame()->getAlienMissions().push_back(mission);
						newRetaliation = false;
					}
					break;
				}
			}
		}
		if ((*b)->getAvailablePsiLabs() > 0 && !Options::getBool("anytimePsiTraining"))
		{
			psi = true;
			for(std::vector<Soldier*>::const_iterator s = (*b)->getSoldiers()->begin(); s != (*b)->getSoldiers()->end(); ++s)
			{
				if ((*s)->isInPsiTraining())
				{
					(*s)->trainPsi();
				}
			}
		}
	}

	// Handle funding
	timerReset();
	_game->getSavedGame()->monthlyFunding();
	popup(new MonthlyReportState(_game, psi, _globe));

	// Handle Xcom Operatives discovering bases
	if(_game->getSavedGame()->getAlienBases()->size())
	{
		bool _baseDiscovered = false;
		for(std::vector<AlienBase*>::const_iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); ++b)
		{
			int number = RNG::generate(1, 100);
			if(!(*b)->isDiscovered() && number <= 5 && !_baseDiscovered)
			{
				(*b)->setDiscovered(true);
				_baseDiscovered = true;
				popup(new AlienBaseState(_game, *b, this));
			}
		}
	}
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
 * Stops the Geoscape music for when another
 * music is gonna take place, so it resumes
 * when we go back to the Geoscape.
 * @param pause True if we want to resume
 * from the same spot we left off.
 */
void GeoscapeState::musicStop(bool)
{
	_music = false;
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
Globe *GeoscapeState::getGlobe() const
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
void GeoscapeState::btnInterceptClick(Action *)
{
	_game->pushState(new InterceptState(_game, _globe));
}

/**
 * Goes to the Basescape screen.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnBasesClick(Action *)
{
	timerReset();
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
void GeoscapeState::btnGraphsClick(Action *)
{
	_game->pushState(new GraphsState(_game));
}

/**
 * Goes to the Ufopaedia window.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnUfopaediaClick(Action *)
{
	Ufopaedia::open(_game);
}

/**
 * Opens the Options window.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnOptionsClick(Action *)
{
	_game->pushState(new GeoscapeOptionsState(_game));
}

/**
 * Goes to the Funding screen.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnFundingClick(Action *)
{
	_game->pushState(new FundingState(_game));
}

/**
 * Starts rotating the globe to the left.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateLeftPress(Action *)
{
	_globe->rotateLeft();
}

/**
 * Stops rotating the globe to the left.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateLeftRelease(Action *)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe to the right.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateRightPress(Action *)
{
	_globe->rotateRight();
}

/**
 * Stops rotating the globe to the right.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateRightRelease(Action *)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe upwards.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateUpPress(Action *)
{
	_globe->rotateUp();
}

/**
 * Stops rotating the globe upwards.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateUpRelease(Action *)
{
	_globe->rotateStop();
}

/**
 * Starts rotating the globe downwards.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateDownPress(Action *)
{
	_globe->rotateDown();
}

/**
 * Stops rotating the globe downwards.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnRotateDownRelease(Action *)
{
	_globe->rotateStop();
}

/**
 * Zooms into the globe.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnZoomInLeftClick(Action *)
{
	_globe->zoomIn();
}

/**
 * Zooms the globe maximum.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnZoomInRightClick(Action *)
{
	_globe->zoomMax();
}

/**
 * Zooms out of the globe.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnZoomOutLeftClick(Action *)
{
	_globe->zoomOut();
}

/**
 * Zooms the globe minimum.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnZoomOutRightClick(Action *)
{
	_globe->zoomMin();
}

/**
 * Zoom in effect for dogfights.
 */
void GeoscapeState::zoomInEffect()
{
	_globe->zoomIn();
	if(_globe->isZoomedInToMax())
	{
		_zoomInEffectDone = true;
		_zoomInEffectTimer->stop();
	}
}

/**
 * Zoom out effect for dogfights.
 */
void GeoscapeState::zoomOutEffect()
{
	_globe->zoomOut();
	if(_globe->isZoomedOutToMax())
	{
		_zoomOutEffectDone = true;
		_zoomOutEffectTimer->stop();
		init();
	}
}

/**
 * Dogfight logic. Moved here to have the code clean.
 */
void GeoscapeState::handleDogfights()
{
	// If all dogfights are minimized rotate the globe, etc.
	if(_dogfights.size() == _minimizedDogfights)
	{
		_pause = false;
		_timer->think(this, 0);
	}
	// Handle dogfights logic.
	_minimizedDogfights = 0;
	std::vector<DogfightState*>::iterator d = _dogfights.begin();
	while(d != _dogfights.end())
	{
		if((*d)->isMinimized())
		{
			_minimizedDogfights++;
		}
		(*d)->think();
		if((*d)->dogfightEnded())
		{
			if((*d)->isMinimized())
			{
				_minimizedDogfights--;
			}
			delete *d;
			d = _dogfights.erase(d);
		}
		else
		{
			++d;
		}
	}
	if(_dogfights.empty())
	{
		_zoomOutEffectTimer->start();
	}
}

/**
 * Gets the number of minimized dogfights.
 * @return Number of minimized dogfights.
 */
int GeoscapeState::minimizedDogfightsCount()
{
	int minimizedDogfights = 0;
	for(std::vector<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
	{
		if((*d)->isMinimized())
		{
			++minimizedDogfights;
		}
	}
	return minimizedDogfights;
}

/**
 * Starts a new dogfight.
 */
void GeoscapeState::startDogfight()
{
	if(!_globe->isZoomedInToMax())
	{
		if(!_zoomInEffectTimer->isRunning())
		{
			_zoomInEffectTimer->start();
		}
	}
	else
	{
		_dogfightStartTimer->stop();
		_zoomInEffectTimer->stop();
		timerReset();
		musicStop();
		while(!_dogfightsToBeStarted.empty())
		{
			_dogfights.push_back(_dogfightsToBeStarted.back());
			_dogfightsToBeStarted.pop_back();
			_dogfights.back()->setInterceptionNumber(getFirstFreeDogfightSlot());
			_dogfights.back()->setInterceptionsCount(_dogfights.size() + _dogfightsToBeStarted.size());
		}
		// Set correct number of interceptions for every dogfight.
		for(std::vector<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
		{
			(*d)->setInterceptionsCount(_dogfights.size());
		}
	}
}

/**
 * Returns the first free dogfight slot.
 */
int GeoscapeState::getFirstFreeDogfightSlot()
{
	int slotNo = 1;
	for(std::vector<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
	{
		if((*d)->getInterceptionNumber() == slotNo)
		{
			++slotNo;
		}
	}
	return slotNo;
}

/**
 * Handle base defense
 * @param base Base to defend.
 * @param ufo Ufo attacking base.
 */
void GeoscapeState::handleBaseDefense(Base *base, Ufo *ufo)
{
    // Whatever happens in the base defense, the UFO has finished its duty
	ufo->setStatus(Ufo::DESTROYED);

	if (base->getAvailableSoldiers(true) > 0)
	{
		size_t month = _game->getSavedGame()->getMonthsPassed();
		if (month > _game->getRuleset()->getAlienItemLevels().size()-1)
			month = _game->getRuleset()->getAlienItemLevels().size()-1;
		SavedBattleGame *bgame = new SavedBattleGame();
		_game->getSavedGame()->setBattleGame(bgame);
		bgame->setMissionType("STR_BASE_DEFENSE");
		BattlescapeGenerator bgen = BattlescapeGenerator(_game);
		bgen.setBase(base);
		bgen.setAlienRace(ufo->getAlienRace());
		bgen.setAlienItemlevel(_game->getRuleset()->getAlienItemLevels().at(month).at(RNG::generate(0,9)));
		bgen.run();
		musicStop();
		popup(new BriefingState(_game, 0, base));
	}
	else
	{
	    // Please garrison your bases in future
		popup(new BaseDestroyedState(_game, base));
	}
}

/**
 * Determine the alien missions to start this month.
 * In the vanilla game each month a terror mission and one other are started in
 * random regions.
 */
void GeoscapeState::determineAlienMissions(bool atGameStart)
{
	//
	// One terror mission per month
	//

	//Determine a random region with at least one city.
	RuleRegion* region = 0;
	std::vector<std::string> regions = _game->getRuleset()->getRegionsList();
	do
	{
		region = _game->getRuleset()->getRegion(regions[RNG::generate(0, regions.size()-1)]);
	}
	while (region->getCities()->empty());
	// Choose race for terror mission.
	const RuleAlienMission &terrorRules = *_game->getRuleset()->getAlienMission("STR_ALIEN_TERROR");
	const std::string &terrorRace = terrorRules.generateRace(_game->getSavedGame()->getMonthsPassed());
	AlienMission *terrorMission = new AlienMission(terrorRules);
	terrorMission->setId(_game->getSavedGame()->getId("ALIEN_MISSIONS"));
	terrorMission->setRegion(region->getType(), *_game->getRuleset());
	terrorMission->setRace(terrorRace);
	terrorMission->start();
	_game->getSavedGame()->getAlienMissions().push_back(terrorMission);

	if (!atGameStart)
	{
		//
		// One randomly selected mission.
		//
		AlienStrategy &strategy = _game->getSavedGame()->getAlienStrategy();
		const std::string &targetRegion = strategy.chooseRandomRegion();
		const std::string &targetMission = strategy.chooseRandomMission(targetRegion);
		// Choose race for this mission.
		const RuleAlienMission &missionRules = *_game->getRuleset()->getAlienMission(targetMission);
		const std::string &missionRace = missionRules.generateRace(_game->getSavedGame()->getMonthsPassed());
		AlienMission *otherMission = new AlienMission(missionRules);
		otherMission->setId(_game->getSavedGame()->getId("ALIEN_MISSIONS"));
		otherMission->setRegion(targetRegion, *_game->getRuleset());
		otherMission->setRace(missionRace);
		otherMission->start();
		_game->getSavedGame()->getAlienMissions().push_back(otherMission);
		// Make sure this combination never comes up again.
		strategy.removeMission(targetRegion, targetMission);
	}
	else
	{
		//
		// Sectoid Research at base's region.
		//
		AlienStrategy &strategy = _game->getSavedGame()->getAlienStrategy();
		std::string targetRegion =
		_game->getSavedGame()->locateRegion(*_game->getSavedGame()->getBases()->front())->getRules()->getType();
		// Choose race for this mission.
		const RuleAlienMission &missionRules = *_game->getRuleset()->getAlienMission("STR_ALIEN_RESEARCH");
		AlienMission *otherMission = new AlienMission(missionRules);
		otherMission->setId(_game->getSavedGame()->getId("ALIEN_MISSIONS"));
		otherMission->setRegion(targetRegion, *_game->getRuleset());
		otherMission->setRace("STR_SECTOID");
		otherMission->start(150);
		_game->getSavedGame()->getAlienMissions().push_back(otherMission);
		// Make sure this combination never comes up again.
		strategy.removeMission(targetRegion, "STR_ALIEN_RESEARCH");
	}
}

void GeoscapeState::btnTimerClick(Action *action)
{
	SDL_Event ev;
	ev.type = SDL_MOUSEBUTTONDOWN;
	ev.button.button = SDL_BUTTON_LEFT;
	Action a = Action(&ev, 0.0, 0.0);
	action->getSender()->mousePress(&a, this);
}

}
