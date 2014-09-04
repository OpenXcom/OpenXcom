/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include <ctime>
#include <algorithm>
#include <functional>
#include <assert.h>
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
#include "../Interface/TextButton.h"
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
#include "../Ruleset/RuleScriptedEvent.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/Ufo.h"
#include "../Ruleset/RuleUfo.h"
#include "../Savegame/Waypoint.h"
#include "../Savegame/Transfer.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDeath.h"
#include "../Menu/PauseState.h"
#include "InterceptState.h"
#include "../Basescape/BasescapeState.h"
#include "../Basescape/SellState.h"
#include "../Menu/ErrorMessageState.h"
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
#include "ResearchRequiredState.h"
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
#include "../Menu/LoadGameState.h"
#include "../Menu/SaveGameState.h"
#include "../Menu/ListSaveState.h"
#include "../Ruleset/AlienRace.h"
#include "../Savegame/ScriptedEventLocation.h"
#include "ScriptedEventState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Geoscape screen.
 * @param game Pointer to the core game.
 */
GeoscapeState::GeoscapeState() : _pause(false), _zoomInEffectDone(false), _zoomOutEffectDone(false), _popups(), _dogfights(), _dogfightsToBeStarted(), _minimizedDogfights(0)
{
	int screenWidth = Options::baseXGeoscape;
	int screenHeight = Options::baseYGeoscape;

	// Create objects
	Surface *hd = _game->getResourcePack()->getSurface("ALTGEOBORD.SCR");
	_bg = new Surface(hd->getWidth(), hd->getHeight(), 0, 0);
	_sideLine = new Surface(64, screenHeight, screenWidth - 64, 0);
	_sidebar = new Surface(64, 200, screenWidth - 64, screenHeight / 2 - 100);

	_globe = new Globe(_game, (screenWidth-64)/2, screenHeight/2, screenWidth-64, screenHeight, 0, 0);
	_bg->setX((_globe->getWidth() - _bg->getWidth()) / 2);
	_bg->setY((_globe->getHeight() - _bg->getHeight()) / 2);

	_btnIntercept = new TextButton(63, 11, screenWidth-63, screenHeight/2-100);
	_btnBases = new TextButton(63, 11, screenWidth-63, screenHeight/2-88);
	_btnGraphs = new TextButton(63, 11, screenWidth-63, screenHeight/2-76);
	_btnUfopaedia = new TextButton(63, 11, screenWidth-63, screenHeight/2-64);
	_btnOptions = new TextButton(63, 11, screenWidth-63, screenHeight/2-52);
	_btnFunding = new TextButton(63, 11, screenWidth-63, screenHeight/2-40);

	_btn5Secs = new TextButton(31, 13, screenWidth-63, screenHeight/2+12);
	_btn1Min = new TextButton(31, 13, screenWidth-31, screenHeight/2+12);
	_btn5Mins = new TextButton(31, 13, screenWidth-63, screenHeight/2+26);
	_btn30Mins = new TextButton(31, 13, screenWidth-31, screenHeight/2+26);
	_btn1Hour = new TextButton(31, 13, screenWidth-63, screenHeight/2+40);
	_btn1Day = new TextButton(31, 13, screenWidth-31, screenHeight/2+40);

	_btnRotateLeft = new InteractiveSurface(12, 10, screenWidth-61, screenHeight/2+76);
	_btnRotateRight = new InteractiveSurface(12, 10, screenWidth-37, screenHeight/2+76);
	_btnRotateUp = new InteractiveSurface(13, 12, screenWidth-49, screenHeight/2+62);
	_btnRotateDown = new InteractiveSurface(13, 12, screenWidth-49, screenHeight/2+87);
	_btnZoomIn = new InteractiveSurface(23, 23, screenWidth-25, screenHeight/2+56);
	_btnZoomOut = new InteractiveSurface(13, 17, screenWidth-20, screenHeight/2+82);

	int height = (screenHeight - Screen::ORIGINAL_HEIGHT) / 2 + 10;
	_sideTop = new TextButton(63, height, screenWidth-63, _sidebar->getY() - height - 1);
	_sideBottom = new TextButton(63, height, screenWidth-63, _sidebar->getY() + _sidebar->getHeight() + 1);

	_txtHour = new Text(20, 16, screenWidth-61, screenHeight/2-26);
	_txtHourSep = new Text(4, 16, screenWidth-41, screenHeight/2-26);
	_txtMin = new Text(20, 16, screenWidth-37, screenHeight/2-26);
	_txtMinSep = new Text(4, 16, screenWidth-17, screenHeight/2-26);
	_txtSec = new Text(11, 8, screenWidth-13, screenHeight/2-20);
	_txtWeekday = new Text(59, 8, screenWidth-61, screenHeight/2-13);
	_txtDay = new Text(29, 8, screenWidth-61, screenHeight/2-6);
	_txtMonth = new Text(29, 8, screenWidth-32, screenHeight/2-6);
	_txtYear = new Text(59, 8, screenWidth-61, screenHeight/2+1);
	_txtFunds = new Text(59, 8, screenWidth-61, screenHeight/2-27);

	_timeSpeed = _btn5Secs;
	_gameTimer = new Timer(Options::geoClockSpeed);

	_zoomInEffectTimer = new Timer(Options::dogfightSpeed+10);
	_zoomOutEffectTimer = new Timer(Options::dogfightSpeed+10);
	_dogfightStartTimer = new Timer(Options::dogfightSpeed+10);

	_txtDebug = new Text(200, 18, 0, 0);

	// Set palette
	setPalette("PAL_GEOSCAPE");

	// Fix system colors
	_game->getCursor()->setColor(Palette::blockOffset(15)+12);
	_game->getFpsCounter()->setColor(Palette::blockOffset(15)+12);

	add(_bg);
	add(_sideLine);
	add(_sidebar);
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

	add(_sideTop);
	add(_sideBottom);

	add(_txtFunds);
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
	Surface *geobord = _game->getResourcePack()->getSurface("GEOBORD.SCR");
	geobord->setX(_sidebar->getX() - geobord->getWidth() + _sidebar->getWidth());
	geobord->setY(_sidebar->getY());
	_sidebar->copy(geobord);
	_game->getResourcePack()->getSurface("ALTGEOBORD.SCR")->blit(_bg);

	_sideLine->drawRect(0, 0, _sideLine->getWidth(), _sideLine->getHeight(), 15);

	_btnIntercept->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnIntercept->setColor(Palette::blockOffset(15)+6);
	_btnIntercept->setTextColor(Palette::blockOffset(15)+5);
	_btnIntercept->setText(tr("STR_INTERCEPT"));
	_btnIntercept->onMouseClick((ActionHandler)&GeoscapeState::btnInterceptClick);
	_btnIntercept->onKeyboardPress((ActionHandler)&GeoscapeState::btnInterceptClick, Options::keyGeoIntercept);

	_btnBases->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnBases->setColor(Palette::blockOffset(15)+6);
	_btnBases->setTextColor(Palette::blockOffset(15)+5);
	_btnBases->setText(tr("STR_BASES"));
	_btnBases->onMouseClick((ActionHandler)&GeoscapeState::btnBasesClick);
	_btnBases->onKeyboardPress((ActionHandler)&GeoscapeState::btnBasesClick, Options::keyGeoBases);

	_btnGraphs->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnGraphs->setColor(Palette::blockOffset(15)+6);
	_btnGraphs->setTextColor(Palette::blockOffset(15)+5);
	_btnGraphs->setText(tr("STR_GRAPHS"));
	_btnGraphs->onMouseClick((ActionHandler)&GeoscapeState::btnGraphsClick);
	_btnGraphs->onKeyboardPress((ActionHandler)&GeoscapeState::btnGraphsClick, Options::keyGeoGraphs);

	_btnUfopaedia->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnUfopaedia->setColor(Palette::blockOffset(15)+6);
	_btnUfopaedia->setTextColor(Palette::blockOffset(15)+5);
	_btnUfopaedia->setText(tr("STR_UFOPAEDIA_UC"));
	_btnUfopaedia->onMouseClick((ActionHandler)&GeoscapeState::btnUfopaediaClick);
	_btnUfopaedia->onKeyboardPress((ActionHandler)&GeoscapeState::btnUfopaediaClick, Options::keyGeoUfopedia);

	_btnOptions->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnOptions->setColor(Palette::blockOffset(15)+6);
	_btnOptions->setTextColor(Palette::blockOffset(15)+5);
	_btnOptions->setText(tr("STR_OPTIONS_UC"));
	_btnOptions->onMouseClick((ActionHandler)&GeoscapeState::btnOptionsClick);
	_btnOptions->onKeyboardPress((ActionHandler)&GeoscapeState::btnOptionsClick, Options::keyGeoOptions);

	_btnFunding->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnFunding->setColor(Palette::blockOffset(15)+6);
	_btnFunding->setTextColor(Palette::blockOffset(15)+5);
	_btnFunding->setText(tr("STR_FUNDING_UC"));
	_btnFunding->onMouseClick((ActionHandler)&GeoscapeState::btnFundingClick);
	_btnFunding->onKeyboardPress((ActionHandler)&GeoscapeState::btnFundingClick, Options::keyGeoFunding);

	_btn5Secs->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn5Secs->setBig();
	_btn5Secs->setColor(Palette::blockOffset(15)+6);
	_btn5Secs->setTextColor(Palette::blockOffset(15)+5);
	_btn5Secs->setText(tr("STR_5_SECONDS"));
	_btn5Secs->setGroup(&_timeSpeed);
	_btn5Secs->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed1);

	_btn1Min->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn1Min->setBig();
	_btn1Min->setColor(Palette::blockOffset(15)+6);
	_btn1Min->setTextColor(Palette::blockOffset(15)+5);
	_btn1Min->setText(tr("STR_1_MINUTE"));
	_btn1Min->setGroup(&_timeSpeed);
	_btn1Min->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed2);

	_btn5Mins->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn5Mins->setBig();
	_btn5Mins->setColor(Palette::blockOffset(15)+6);
	_btn5Mins->setTextColor(Palette::blockOffset(15)+5);
	_btn5Mins->setText(tr("STR_5_MINUTES"));
	_btn5Mins->setGroup(&_timeSpeed);
	_btn5Mins->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed3);

	_btn30Mins->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn30Mins->setBig();
	_btn30Mins->setColor(Palette::blockOffset(15)+6);
	_btn30Mins->setTextColor(Palette::blockOffset(15)+5);
	_btn30Mins->setText(tr("STR_30_MINUTES"));
	_btn30Mins->setGroup(&_timeSpeed);
	_btn30Mins->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed4);

	_btn1Hour->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn1Hour->setBig();
	_btn1Hour->setColor(Palette::blockOffset(15)+6);
	_btn1Hour->setTextColor(Palette::blockOffset(15)+5);
	_btn1Hour->setText(tr("STR_1_HOUR"));
	_btn1Hour->setGroup(&_timeSpeed);
	_btn1Hour->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed5);

	_btn1Day->initText(_game->getResourcePack()->getFont("FONT_GEO_BIG"), _game->getResourcePack()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn1Day->setBig();
	_btn1Day->setColor(Palette::blockOffset(15)+6);
	_btn1Day->setTextColor(Palette::blockOffset(15)+5);
	_btn1Day->setText(tr("STR_1_DAY"));
	_btn1Day->setGroup(&_timeSpeed);
	_btn1Day->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed6);

	_btnRotateLeft->onMousePress((ActionHandler)&GeoscapeState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateLeftRelease);
	_btnRotateLeft->onKeyboardPress((ActionHandler)&GeoscapeState::btnRotateLeftPress, Options::keyGeoLeft);
	_btnRotateLeft->onKeyboardRelease((ActionHandler)&GeoscapeState::btnRotateLeftRelease, Options::keyGeoLeft);

	_btnRotateRight->onMousePress((ActionHandler)&GeoscapeState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateRightRelease);
	_btnRotateRight->onKeyboardPress((ActionHandler)&GeoscapeState::btnRotateRightPress, Options::keyGeoRight);
	_btnRotateRight->onKeyboardRelease((ActionHandler)&GeoscapeState::btnRotateRightRelease, Options::keyGeoRight);

	_btnRotateUp->onMousePress((ActionHandler)&GeoscapeState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateUpRelease);
	_btnRotateUp->onKeyboardPress((ActionHandler)&GeoscapeState::btnRotateUpPress, Options::keyGeoUp);
	_btnRotateUp->onKeyboardRelease((ActionHandler)&GeoscapeState::btnRotateUpRelease, Options::keyGeoUp);

	_btnRotateDown->onMousePress((ActionHandler)&GeoscapeState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((ActionHandler)&GeoscapeState::btnRotateDownRelease);
	_btnRotateDown->onKeyboardPress((ActionHandler)&GeoscapeState::btnRotateDownPress, Options::keyGeoDown);
	_btnRotateDown->onKeyboardRelease((ActionHandler)&GeoscapeState::btnRotateDownRelease, Options::keyGeoDown);

	_btnZoomIn->onMouseClick((ActionHandler)&GeoscapeState::btnZoomInLeftClick, SDL_BUTTON_LEFT);
	_btnZoomIn->onMouseClick((ActionHandler)&GeoscapeState::btnZoomInRightClick, SDL_BUTTON_RIGHT);
	_btnZoomIn->onKeyboardPress((ActionHandler)&GeoscapeState::btnZoomInLeftClick, Options::keyGeoZoomIn);

	_btnZoomOut->onMouseClick((ActionHandler)&GeoscapeState::btnZoomOutLeftClick, SDL_BUTTON_LEFT);
	_btnZoomOut->onMouseClick((ActionHandler)&GeoscapeState::btnZoomOutRightClick, SDL_BUTTON_RIGHT);
	_btnZoomOut->onKeyboardPress((ActionHandler)&GeoscapeState::btnZoomOutLeftClick, Options::keyGeoZoomOut);

	_sideTop->setColor(Palette::blockOffset(15)+6);
	_sideBottom->setColor(Palette::blockOffset(15)+6);
	
	_txtFunds->setColor(Palette::blockOffset(15)+4);
	_txtFunds->setAlign(ALIGN_CENTER);
	_txtFunds->setVisible(Options::showFundsOnGeoscape);

	_txtHour->setBig();
	_txtHour->setColor(Palette::blockOffset(15)+4);
	_txtHour->setAlign(ALIGN_RIGHT);

	_txtHourSep->setBig();
	_txtHourSep->setColor(Palette::blockOffset(15)+4);
	_txtHourSep->setText(L":");

	_txtMin->setBig();
	_txtMin->setColor(Palette::blockOffset(15)+4);

	_txtMinSep->setBig();
	_txtMinSep->setColor(Palette::blockOffset(15)+4);
	_txtMinSep->setText(L":");

	_txtSec->setColor(Palette::blockOffset(15)+4);

	_txtWeekday->setColor(Palette::blockOffset(15)+4);
	_txtWeekday->setAlign(ALIGN_CENTER);

	_txtDay->setColor(Palette::blockOffset(15)+4);
	_txtDay->setAlign(ALIGN_CENTER);

	_txtMonth->setColor(Palette::blockOffset(15)+4);
	_txtMonth->setAlign(ALIGN_CENTER);

	_txtYear->setColor(Palette::blockOffset(15)+4);
	_txtYear->setAlign(ALIGN_CENTER);

	_txtDebug->setColor(Palette::blockOffset(15)+4);
	
	if (Options::showFundsOnGeoscape)
	{
		_txtHour->setY(_txtHour->getY()+6);
		_txtHour->setSmall();
		_txtHourSep->setY(_txtHourSep->getY()+6);
		_txtHourSep->setSmall();
		_txtMin->setY(_txtMin->getY()+6);
		_txtMin->setSmall();
		_txtMinSep->setX(_txtMinSep->getX()-10);
		_txtMinSep->setY(_txtMinSep->getY()+6);
		_txtMinSep->setSmall();
		_txtSec->setX(_txtSec->getX()-10);
	}

	_gameTimer->onTimer((StateHandler)&GeoscapeState::timeAdvance);
	_gameTimer->start();

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
	delete _gameTimer;
	delete _zoomInEffectTimer;
	delete _zoomOutEffectTimer;
	delete _dogfightStartTimer;
	
	std::list<DogfightState*>::iterator it = _dogfights.begin();
	for(; it != _dogfights.end();)
	{
		delete *it;
		it = _dogfights.erase(it);
	}
	for(it = _dogfightsToBeStarted.begin(); it != _dogfightsToBeStarted.end();)
	{
		delete *it;
		it = _dogfightsToBeStarted.erase(it);
	}
}

/**
 * Handle blitting of Geoscape and Dogfights.
 */
void GeoscapeState::blit()
{
	State::blit();
	for(std::list<DogfightState*>::iterator it = _dogfights.begin(); it != _dogfights.end(); ++it)
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
	if (_dogfights.size() == _minimizedDogfights)
	{
		State::handle(action);
	}

	if (action->getDetails()->type == SDL_KEYDOWN)
	{
		// "ctrl-d" - enable debug mode
		if (Options::debug && action->getDetails()->key.keysym.sym == SDLK_d && (SDL_GetModState() & KMOD_CTRL) != 0)
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
		else if (!_game->getSavedGame()->isIronman())
		{
			if (action->getDetails()->key.keysym.sym == Options::keyQuickSave)
			{
				popup(new SaveGameState(OPT_GEOSCAPE, SAVE_QUICK, _palette));
			}
			else if (action->getDetails()->key.keysym.sym == Options::keyQuickLoad)
			{
				popup(new LoadGameState(OPT_GEOSCAPE, SAVE_QUICK, _palette));
			}
		}
	}
	if (!_dogfights.empty())
	{
		for(std::list<DogfightState*>::iterator it = _dogfights.begin(); it != _dogfights.end(); ++it)
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
	State::init();
	timeDisplay();

	_globe->onMouseClick((ActionHandler)&GeoscapeState::globeClick);
	_globe->onMouseOver(0);
	_globe->rotateStop();
	_globe->setFocus(true);
	_globe->draw();

	// Pop up save screen if it's a new ironman game
	if (_game->getSavedGame()->isIronman() && _game->getSavedGame()->getName().empty())
	{
		popup(new ListSaveState(OPT_GEOSCAPE));
	}

	// Set music if it's not already playing
	if (_dogfights.empty() && !_dogfightStartTimer->isRunning())
	{
		if (_game->getSavedGame()->getMonthsPassed() == -1)
		{
			_game->getResourcePack()->playMusic("GMGEO1");
		}
		else
		{
			_game->getResourcePack()->playMusic("GMGEO", true);
		}
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
		setupTerrorMission();
		_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - (_game->getSavedGame()->getBaseMaintenance() - _game->getSavedGame()->getBases()->front()->getPersonnelMaintenance()));
	}
	if (_popups.empty() && _dogfights.empty() && (!_zoomInEffectTimer->isRunning() || _zoomInEffectDone) && (!_zoomOutEffectTimer->isRunning() || _zoomOutEffectDone))
	{
		// Handle timers
		_gameTimer->think(this, 0);
	}
	else
	{
		if (!_dogfights.empty() || _minimizedDogfights != 0)
		{
			handleDogfights();
		}
		if (!_popups.empty())
		{
			// Handle popups
			_globe->rotateStop();
			_game->pushState(_popups.front());
			_popups.erase(_popups.begin());
		}
	}
}

/**
 * Updates the Geoscape clock with the latest
 * game time and date in human-readable format. (+Funds)
 */
void GeoscapeState::timeDisplay()
{
	if (Options::showFundsOnGeoscape)
	{
		_txtFunds->setText(Text::formatFunding(_game->getSavedGame()->getFunds()));
	}

	std::wostringstream ss;
	ss << std::setfill(L'0') << std::setw(2) << _game->getSavedGame()->getTime()->getSecond();
	_txtSec->setText(ss.str());

	std::wostringstream ss2;
	ss2 << std::setfill(L'0') << std::setw(2) << _game->getSavedGame()->getTime()->getMinute();
	_txtMin->setText(ss2.str());

	std::wostringstream ss3;
	ss3 << _game->getSavedGame()->getTime()->getHour();
	_txtHour->setText(ss3.str());

	std::wostringstream ss4;
	ss4 << _game->getSavedGame()->getTime()->getDayString(_game->getLanguage());
	_txtDay->setText(ss4.str());

	_txtWeekday->setText(tr(_game->getSavedGame()->getTime()->getWeekdayString()));

	_txtMonth->setText(tr(_game->getSavedGame()->getTime()->getMonthString()));

	std::wostringstream ss5;
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

	_pause = !_dogfightsToBeStarted.empty();

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
	if (_game->getSavedGame()->getBases()->empty())
	{
		popup(new DefeatState);
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
					if ((*i)->getMission())
					{
						size_t terrorSiteCount = _game->getSavedGame()->getTerrorSites()->size();
						AlienMission *mission = (*i)->getMission();
						bool detected = (*i)->getDetected();
						mission->ufoReachedWaypoint(**i, *_game, *_globe);
						if (detected != (*i)->getDetected() && !(*i)->getFollowers()->empty())
						{
							if (!((*i)->getTrajectory().getID() == "__RETALIATION_ASSAULT_RUN" && (*i)->getStatus() ==  Ufo::LANDED))
								popup(new UfoLostState((*i)->getName(_game->getLanguage())));
						}
						if (terrorSiteCount < _game->getSavedGame()->getTerrorSites()->size())
						{
							TerrorSite *ts = _game->getSavedGame()->getTerrorSites()->back();
							const City *city = _game->getRuleset()->locateCity(ts->getLongitude(), ts->getLatitude());
							assert(city);
							popup(new AlienTerrorState(ts, city->getName(), this));
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
							if (!base->getDefenses()->empty())
							{
								popup(new BaseDefenseState(base, *i, this));
							}
							else
							{
								handleBaseDefense(base, *i);
								return;
							}
						}
					}
					else if ((*i)->getScriptedEvent())
					{
						// Scripted Event UFOs simply move around between a series of 
						// locations set in the rules.
						(*i)->getScriptedEvent()->newUfoDestination();
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
					popup(new UfoLostState((*i)->getName(_game->getLanguage())));
				}
			}
			break;
		case Ufo::CRASHED:
			(*i)->think();
			if (!(*i)->getScriptedEvent() && (*i)->getSecondsRemaining() == 0)
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
					if ((*country)->getRules()->insideCountry((*j)->getLongitude(), (*j)->getLatitude()))
					{
						(*country)->addActivityXcom(-(*j)->getRules()->getScore());
						break;
					}
				}
				for(std::vector<Region*>::iterator region = _game->getSavedGame()->getRegions()->begin(); region != _game->getSavedGame()->getRegions()->end(); ++region)
				{
					if ((*region)->getRules()->insideRegion((*j)->getLongitude(), (*j)->getLatitude()))
					{
						(*region)->addActivityXcom(-(*j)->getRules()->getScore());
						break;
					}
				}
				// if a transport craft has been shot down, kill all the soldiers on board.
				if ((*j)->getRules()->getSoldiers() > 0)
				{
					for (std::vector<Soldier*>::iterator k = (*i)->getSoldiers()->begin(); k != (*i)->getSoldiers()->end();)
					{
						if ((*k)->getCraft() == (*j))
						{
							SoldierDeath *death = new SoldierDeath();
							death->setTime(*_game->getSavedGame()->getTime());
							(*k)->die(death);
							_game->getSavedGame()->getDeadSoldiers()->push_back((*k));
							k = (*i)->getSoldiers()->erase(k);
						}
						else
						{
							++k;
						}
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
						popup(new GeoscapeCraftState((*j), _globe, w));
					}
				}
				if (u != 0 && u->getStatus() == Ufo::DESTROYED)
				{
					(*j)->returnToBase();
				}
			}
			if (!_zoomInEffectTimer->isRunning() && !_zoomOutEffectTimer->isRunning())
			{
				(*j)->think();
			}
			if ((*j)->reachedDestination())
			{
				Ufo* u = dynamic_cast<Ufo*>((*j)->getDestination());
				Waypoint *w = dynamic_cast<Waypoint*>((*j)->getDestination());
				TerrorSite* t = dynamic_cast<TerrorSite*>((*j)->getDestination());
				AlienBase* b = dynamic_cast<AlienBase*>((*j)->getDestination());
				ScriptedEventLocation* se = dynamic_cast<ScriptedEventLocation*>((*j)->getDestination());
				if (u != 0)
				{
					switch (u->getStatus())
					{
					case Ufo::FLYING:
						// Check scripted event requirements
						if (u->getScriptedEvent() && !u->getScriptedEvent()->checkEngagementRestrictions(_game->getSavedGame(), (*j)->getBase(), (*j)))
						{
							// did not meet requirements, so pop up a message and limp home to base
							popup(new ScriptedEventState(u, u->getScriptedEvent(), SS_FAILED_ENGAGE, this));
							(*j)->returnToBase();
							continue;
						}
						// Not more than 4 interceptions at a time.
						if (_dogfights.size() + _dogfightsToBeStarted.size() >= 4)
						{
							++j;
							continue;
						}
						if (!(*j)->isInDogfight() && !(*j)->getDistance(u))
						{
							_dogfightsToBeStarted.push_back(new DogfightState(_globe, (*j), u));

							if (!_dogfightStartTimer->isRunning())
							{
								_pause = true;
								timerReset();
								_globe->center((*j)->getLongitude(), (*j)->getLatitude());
								startDogfight();
								_dogfightStartTimer->start();
							}
							_game->getResourcePack()->playMusic("GMINTER");
						}
						break;
					case Ufo::LANDED:
					case Ufo::CRASHED:
					case Ufo::DESTROYED: // Just before expiration
						if ((*j)->getNumSoldiers() > 0 || (*j)->getNumVehicles() > 0)
						{
							if (!(*j)->isInDogfight())
							{
								// look up polygons texture
								int texture, shade;
								_globe->getPolygonTextureAndShade(u->getLongitude(), u->getLatitude(), &texture, &shade);
								timerReset();
								popup(new ConfirmLandingState(*j, texture, shade));
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
					popup(new CraftPatrolState((*j), _globe));
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
						popup(new ConfirmLandingState(*j, texture, shade));
					}
					else
					{
						(*j)->returnToBase();
					}
				}
				else if (se != 0)
				{
					if (se->getDetected())
					{
						if ((*j)->getNumSoldiers() > 0)
						{
							// Check whether the craft meets the engagement requirements for this event
							if (se->getScriptedEvent()->checkEngagementRestrictions(_game->getSavedGame(), (*j)->getBase(), (*j)))
							{
								// look up polygons texture
								int texture, shade;
								_globe->getPolygonTextureAndShade(se->getLongitude(), se->getLatitude(), &texture, &shade);
								timerReset();
								popup(new ConfirmLandingState(*j, texture, shade));
							}
							else
							{
								// did not meet requirements, so pop up a message and limp home to base
								popup(new ScriptedEventState(se, se->getScriptedEvent(), SS_FAILED_ENGAGE, this));
								(*j)->returnToBase();
							}
						}
						else
						{
							(*j)->returnToBase();
						}
					}
				}
				else if (b != 0)
				{
					if (b->isDiscovered())
					{
						if ((*j)->getNumSoldiers() > 0)
						{
							int texture, shade;
							_globe->getPolygonTextureAndShade(b->getLongitude(), b->getLatitude(), &texture, &shade);
							timerReset();
							popup(new ConfirmLandingState(*j, texture, shade));
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
			if (!(*i)->getFollowers()->empty())
			{
				// Remove all dogfights with this UFO.
				for(std::list<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end();)
				{
					if ((*d)->getUfo() == (*i))
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
	DetectXCOMBase(const Base &base, int difficulty) : _base(base), _difficulty(difficulty) { /* Empty by design.  */ }
	/// Attempt detection
	bool operator()(const Ufo *ufo) const;
private:
	const Base &_base;	//!< The target base.
	const int _difficulty;
};

/**
 * Only UFOs within detection range of the base have a chance to detect it.
 * @param ufo Pointer to the UFO attempting detection.
 * @return If the base is detected by @a ufo.
 */
bool DetectXCOMBase::operator()(const Ufo *ufo) const
{
	if ((ufo->getMissionType() != "STR_ALIEN_RETALIATION" && !Options::aggressiveRetaliation) || // only UFOs on retaliation missions actively scan for bases
		ufo->getTrajectory().getID() == "__RETALIATION_ASSAULT_RUN" || 										// UFOs attacking a base don't detect!
		ufo->isCrashed() ||																				 // Crashed UFOs don't detect!
		_base.getDistance(ufo) >= ufo->getRules()->getSightRange() * (1 / 60.0) * (M_PI / 180.0))		 // UFOs have a detection range of 80 XCOM units. - we use a great circle fomrula and nautical miles.
	{
		return false;
	}
	return RNG::percent(_base.getDetectionChance(_difficulty));
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
					popup(new LowFuelState((*j), this));
				}

				if ((*j)->getDestination() == 0)
				{
					double range = ((*j)->getRules()->getSightRange() * (1 / 60.0) * (M_PI / 180));
					for(std::vector<AlienBase*>::iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); b++)
					{
						if ((*j)->getDistance(*b) <= range)
						{
							if (RNG::percent(50-((*j)->getDistance(*b) / range) * 50) && !(*b)->isDiscovered())
							{
								(*b)->setDiscovered(true);
							}
						}
					}
				}
			}
		}
	}
	int diff = (int)(_game->getSavedGame()->getDifficulty());
	if (Options::aggressiveRetaliation)
	{
		// Detect as many bases as possible.
		for (std::vector<Base*>::iterator iBase = _game->getSavedGame()->getBases()->begin(); iBase != _game->getSavedGame()->getBases()->end(); ++iBase)
		{
			// Find a UFO that detected this base, if any.
			std::vector<Ufo*>::const_iterator uu = std::find_if (_game->getSavedGame()->getUfos()->begin(), _game->getSavedGame()->getUfos()->end(), DetectXCOMBase(**iBase, diff));
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
			std::vector<Ufo*>::const_iterator uu = std::find_if (_game->getSavedGame()->getUfos()->begin(), _game->getSavedGame()->getUfos()->end(), DetectXCOMBase(**iBase, diff));
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
	 * @param globe The globe object.
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
 * @param ts Pointer to terror site.
 * @return Has terror site expired?
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
		region->addActivityAlien(_game->getRuleset()->getAlienMission("STR_ALIEN_TERROR")->getPoints() * 100);
		//kids, tell your folks... don't ignore terror sites.
	}
	for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
	{
		if ((*k)->getRules()->insideCountry(ts->getLongitude(), ts->getLatitude()))
		{
			(*k)->addActivityAlien(_game->getRuleset()->getAlienMission("STR_ALIEN_TERROR")->getPoints() * 100);
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
	// Check to see if any new scripted events should be spawned
	_game->getSavedGame()->checkEventsToSpawn(_game->getRuleset(), SE_NONE, _globe);

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
						(*j)->setLowFuel(false);
					}
					else if (!(*j)->getLowFuel())
					{
						std::wstring msg = tr("STR_NOT_ENOUGH_ITEM_TO_REFUEL_CRAFT_AT_BASE")
										   .arg(tr(item))
										   .arg((*j)->getName(_game->getLanguage()))
										   .arg((*i)->getName());
						popup(new CraftErrorState(this, msg));
						if ((*j)->getFuel() > 0)
						{
							(*j)->setStatus("STR_READY");
						}
						else
						{
							(*j)->setLowFuel(true);
						}
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

			// UFO detection - scripted event UFOs are handled differently (they are spotted immediately if 
			// a base meets the requirements), and are never 'lost'.
			if (!(*u)->getDetected())
			{
				bool detected = false, hyperdetected = false;

				if ((*u)->getScriptedEvent())
				{
					bool detected = false;

					if ((*u)->getScriptedEvent()->getRules().getLocationEvent().automaticDetection) detected = true;

					for (std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); !detected && b != _game->getSavedGame()->getBases()->end(); ++b)
					{
						detected = (*u)->getScriptedEvent()->checkDetectionRestrictions(_game->getSavedGame(), (*b), 0);
					}
					if (detected)
					{
						(*u)->setDetected(true);
						popup(new ScriptedEventState((*u), (*u)->getScriptedEvent(), SS_DETECTED, this));
					}
				}
				else
				{
					for (std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); !hyperdetected && b != _game->getSavedGame()->getBases()->end(); ++b)
					{
						switch ((*b)->detect(*u))
						{
						case 2:	// hyper-wave decoder
							(*u)->setHyperDetected(true);
							hyperdetected = true;
						case 1: // conventional radar
							detected = true;
						}
						for (std::vector<Craft*>::iterator c = (*b)->getCrafts()->begin(); !detected && c != (*b)->getCrafts()->end(); ++c)
						{
							if ((*c)->getStatus() == "STR_OUT" && (*c)->detect(*u))
							{
								detected = true;
								break;
							}
						}
					}
					if (detected)
					{
						(*u)->setDetected(true);
						popup(new UfoDetectedState((*u), this, true, (*u)->getHyperDetected()));
					}
				}
			}
			else if (!(*u)->getScriptedEvent())
			{
				bool detected = false, hyperdetected = false;
				for (std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); !hyperdetected && b != _game->getSavedGame()->getBases()->end(); ++b)
				{
					switch ((*b)->insideRadarRange(*u))
					{
					case 2:	// hyper-wave decoder
						detected = true;
						hyperdetected = true;
						(*u)->setHyperDetected(true);
						break;
					case 1: // conventional radar
						detected = true;
						hyperdetected = (*u)->getHyperDetected();
					}
					for (std::vector<Craft*>::iterator c = (*b)->getCrafts()->begin(); !detected && c != (*b)->getCrafts()->end(); ++c)
					{
						if ((*c)->getStatus() == "STR_OUT" && (*c)->detect(*u))
						{
							detected = true;
							hyperdetected = (*u)->getHyperDetected();
							break;
						}
					}
				}
				if (!detected)
				{
					(*u)->setDetected(false);
					(*u)->setHyperDetected(false);
					if (!(*u)->getFollowers()->empty())
					{
						popup(new UfoLostState((*u)->getName(_game->getLanguage())));
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

	// Processes Scripted Event Locations.  These are automatically detected if there
	// is a base (anywhere) meeting the requirements to detect it.
	for (std::vector<ScriptedEventLocation*>::iterator se = _game->getSavedGame()->getScriptedEventLocations()->begin();
		se != _game->getSavedGame()->getScriptedEventLocations()->end();++se)
	{
		if (!(*se)->getDetected())
		{
			bool detected = false;

			if ((*se)->getScriptedEvent()->getRules().getLocationEvent().automaticDetection) detected = true;

			for (std::vector<Base*>::iterator b = _game->getSavedGame()->getBases()->begin(); !detected && b != _game->getSavedGame()->getBases()->end(); ++b)
			{
				detected = (*se)->getScriptedEvent()->checkDetectionRestrictions(_game->getSavedGame(), (*b), 0);
			}
			if (detected)
			{
				(*se)->setDetected(true);
				popup(new ScriptedEventState((*se), (*se)->getScriptedEvent(), SS_DETECTED, this));
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
				std::string s = (*j)->rearm(_game->getRuleset());
				if (s != "")
				{
					std::wstring msg = tr("STR_NOT_ENOUGH_ITEM_TO_REARM_CRAFT_AT_BASE")
									   .arg(tr(s))
									   .arg((*j)->getName(_game->getLanguage()))
									   .arg((*i)->getName());
					popup(new CraftErrorState(this, msg));
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
		popup(new ItemsArrivingState(this));
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
				popup(new ProductionCompleteState((*i),  tr(j->first->getRules()->getName()), this, j->second));
			}
		}

		if (Options::storageLimitsEnforced && (*i)->storesOverfull())
		{
			popup(new ErrorMessageState(tr("STR_STORAGE_EXCEEDED").arg((*i)->getName()).c_str(), _palette, Palette::blockOffset(15) + 1, "BACK13.SCR", 6));
			popup(new SellState((*i)));
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
	if (RNG::percent(6))
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
					popup(new ProductionCompleteState((*i),  tr((*j)->getRules()->getType()), this, PROGRESS_CONSTRUCTION));
				}
			}
		}
		// Handle science project
		std::vector<ResearchProject*> finished;
		for(std::vector<ResearchProject*>::const_iterator iter = (*i)->getResearch().begin (); iter != (*i)->getResearch().end (); ++iter)
		{
			if ((*iter)->step())
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
			if (Options::spendResearchedItems && research->needItem() && _game->getRuleset()->getUnit(research->getName()))
			{
				(*i)->getItems()->addItem(
					_game->getRuleset()->getArmor(
						_game->getRuleset()->getUnit(
							research->getName()
						)->getArmor()
					)->getCorpseGeoscape()
				); // ;)
			}
			if ((*iter)->getRules()->getGetOneFree().size() != 0)
			{
				std::vector<std::string> possibilities;
				for(std::vector<std::string>::const_iterator f = (*iter)->getRules()->getGetOneFree().begin(); f != (*iter)->getRules()->getGetOneFree().end(); ++f)
				{
					bool newFound = true;
					for(std::vector<const RuleResearch*>::const_iterator discovered = _game->getSavedGame()->getDiscoveredResearch().begin(); discovered != _game->getSavedGame()->getDiscoveredResearch().end(); ++discovered)
					{
						if (*f == (*discovered)->getName())
						{
							newFound = false;
						}
					}
					if (newFound)
					{
						possibilities.push_back(*f);
					}
				}
				if (possibilities.size() != 0)
				{
					size_t pick = RNG::generate(0, possibilities.size()-1);
					std::string sel = possibilities.at(pick);
					bonus = _game->getRuleset()->getResearch(sel);
					_game->getSavedGame()->addFinishedResearch(bonus, _game->getRuleset ());
					if (bonus->getLookup() != "")
					{
						_game->getSavedGame()->addFinishedResearch(_game->getRuleset()->getResearch(bonus->getLookup()), _game->getRuleset ());
					}
				}
			}
			const RuleResearch * newResearch = research;
			std::string name = research->getLookup() == "" ? research->getName() : research->getLookup();
			if (_game->getSavedGame()->isResearched(name))
			{
				newResearch = 0;
			}
			_game->getSavedGame()->addFinishedResearch(research, _game->getRuleset ());
			if (research->getLookup() != "")
			{
				_game->getSavedGame()->addFinishedResearch(_game->getRuleset()->getResearch(research->getLookup()), _game->getRuleset ());
			}
			popup(new ResearchCompleteState(newResearch, bonus));
			std::vector<RuleResearch *> newPossibleResearch;
			_game->getSavedGame()->getDependableResearch (newPossibleResearch, (*iter)->getRules(), _game->getRuleset(), *i);
			std::vector<RuleManufacture *> newPossibleManufacture;
			_game->getSavedGame()->getDependableManufacture (newPossibleManufacture, (*iter)->getRules(), _game->getRuleset(), *i);
			timerReset();
			// check for possible researching weapon before clip
			if (newResearch)
			{
				RuleItem *item = _game->getRuleset()->getItem(newResearch->getName());
				if (item && item->getBattleType() == BT_FIREARM && !item->getCompatibleAmmo()->empty())
				{
					RuleManufacture *man = _game->getRuleset()->getManufacture(item->getType());
					if (man && !man->getRequirements().empty())
					{
						const std::vector<std::string> &req = man->getRequirements();
						RuleItem *ammo = _game->getRuleset()->getItem(item->getCompatibleAmmo()->front());
						if (ammo && std::find(req.begin(), req.end(), ammo->getType()) != req.end() && !_game->getSavedGame()->isResearched(req))
						{
							popup(new ResearchRequiredState(item));
						}
					}
				}
			}

			popup(new NewPossibleResearchState(*i, newPossibleResearch));
			if (!newPossibleManufacture.empty())
			{
				popup(new NewPossibleManufactureState(*i, newPossibleManufacture));
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
		if ((*i)->getAvailablePsiLabs() > 0 && Options::anytimePsiTraining)
		{
			for (std::vector<Soldier*>::const_iterator s = (*i)->getSoldiers()->begin(); s != (*i)->getSoldiers()->end(); ++s)
			{
				(*s)->trainPsi1Day();
				(*s)->calcStatString(_game->getRuleset()->getStatStrings(), (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getRuleset()->getPsiRequirements())));
			}
		}
	}
	// handle regional and country points for alien bases
	for(std::vector<AlienBase*>::const_iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); ++b)
	{
		for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
		{
			if ((*k)->getRules()->insideRegion((*b)->getLongitude(), (*b)->getLatitude()))
			{
				(*k)->addActivityAlien(_game->getRuleset()->getAlienMission("STR_ALIEN_BASE")->getPoints() / 10);
				break;
			}
		}
		for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
		{
			if ((*k)->getRules()->insideCountry((*b)->getLongitude(), (*b)->getLatitude()))
			{
				(*k)->addActivityAlien(_game->getRuleset()->getAlienMission("STR_ALIEN_BASE")->getPoints() / 10);
				break;
			}
		}
	}

	// Handle resupply of alien bases.
	std::for_each(_game->getSavedGame()->getAlienBases()->begin(), _game->getSavedGame()->getAlienBases()->end(),
			  GenerateSupplyMission(*_game->getRuleset(), *_game->getSavedGame()));

	// Autosave 3 times a month
	int day = _game->getSavedGame()->getTime()->getDay();
	if (day == 10 || day == 20)
	{
		if (_game->getSavedGame()->isIronman())
		{
			popup(new SaveGameState(OPT_GEOSCAPE, SAVE_IRONMAN, _palette));
		}
		else if (Options::autosave)
		{
			popup(new SaveGameState(OPT_GEOSCAPE, SAVE_AUTO_GEOSCAPE, _palette));
		}
	}
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

	setupTerrorMission();

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
						// get races for retaliation missions
						std::vector<std::string> races = _game->getRuleset()->getAlienRacesList();
						for (std::vector<std::string>::iterator i = races.begin(); i != races.end();)
						{
							if (_game->getRuleset()->getAlienRace(*i)->canRetaliate())
							{
								i++;
							}
							else
							{
								i = races.erase(i);
							}
						}
						size_t race = RNG::generate(0, races.size()-1);
						mission->setRace(races[race]);
						mission->start(150);
						_game->getSavedGame()->getAlienMissions().push_back(mission);
						newRetaliation = false;
					}
					break;
				}
			}
		}
		if ((*b)->getAvailablePsiLabs() > 0 && !Options::anytimePsiTraining)
		{
			psi = true;
			for(std::vector<Soldier*>::const_iterator s = (*b)->getSoldiers()->begin(); s != (*b)->getSoldiers()->end(); ++s)
			{
				if ((*s)->isInPsiTraining())
				{
					(*s)->trainPsi();
					(*s)->calcStatString(_game->getRuleset()->getStatStrings(), (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getRuleset()->getPsiRequirements())));
				}
			}
		}
	}

	// Handle funding
	timerReset();
	_game->getSavedGame()->monthlyFunding();
	popup(new MonthlyReportState(psi, _globe));

	// Handle Xcom Operatives discovering bases
	if (!_game->getSavedGame()->getAlienBases()->empty() && RNG::percent(20))
	{
		for(std::vector<AlienBase*>::const_iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); ++b)
		{
			if (!(*b)->isDiscovered())
			{
				(*b)->setDiscovered(true);
				popup(new AlienBaseState(*b, this));
				break;
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
	Action act(&ev, _game->getScreen()->getXScale(), _game->getScreen()->getYScale(), _game->getScreen()->getCursorTopBlackBand(), _game->getScreen()->getCursorLeftBlackBand());
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
			_game->pushState(new MultipleTargetsState(v, 0, this));
		}
	}

	if (_game->getSavedGame()->getDebugMode())
	{
		double lon, lat;
		_globe->cartToPolar(mouseX, mouseY, &lon, &lat);
		double lonDeg = lon / M_PI * 180, latDeg = lat / M_PI * 180;
		std::wostringstream ss;
		ss << "rad: " << lon << " , " << lat << std::endl;
		ss << "deg: " << lonDeg << " , " << latDeg << std::endl;
		_txtDebug->setText(ss.str());
	}
}

/**
 * Opens the Intercept window.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnInterceptClick(Action *)
{
	_game->pushState(new InterceptState(_globe));
}

/**
 * Goes to the Basescape screen.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnBasesClick(Action *)
{
	timerReset();
	if (!_game->getSavedGame()->getBases()->empty())
	{
		_game->pushState(new BasescapeState(_game->getSavedGame()->getSelectedBase(), _globe));
	}
	else
	{
		_game->pushState(new BasescapeState(0, _globe));
	}
}

/**
 * Goes to the Graphs screen.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnGraphsClick(Action *)
{
	_game->pushState(new GraphsState);
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
	_game->pushState(new PauseState(OPT_GEOSCAPE));
}

/**
 * Goes to the Funding screen.
 * @param action Pointer to an action.
 */
void GeoscapeState::btnFundingClick(Action *)
{
	_game->pushState(new FundingState);
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
	_globe->rotateStopLon();
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
	_globe->rotateStopLon();
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
	_globe->rotateStopLat();
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
	_globe->rotateStopLat();
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
	if (_globe->zoomDogfightIn())
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
	if (_globe->zoomDogfightOut())
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
	if (_dogfights.size() == _minimizedDogfights)
	{
		_pause = false;
		_gameTimer->think(this, 0);
	}
	// Handle dogfights logic.
	_minimizedDogfights = 0;
	std::list<DogfightState*>::iterator d = _dogfights.begin();
	while(d != _dogfights.end())
	{
		if ((*d)->isMinimized())
		{
			_minimizedDogfights++;
		}
		else
		{
			_globe->rotateStop();
		}
		(*d)->think();
		if ((*d)->dogfightEnded())
		{
			if ((*d)->isMinimized())
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
	if (_dogfights.empty())
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
	for(std::list<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
	{
		if ((*d)->isMinimized())
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
	if (_globe->getZoom() < 3)
	{
		if (!_zoomInEffectTimer->isRunning())
		{
			_globe->saveZoomDogfight();
			_globe->rotateStop();
			_zoomInEffectTimer->start();
		}
	}
	else
	{
		_dogfightStartTimer->stop();
		_zoomInEffectTimer->stop();
		timerReset();
		while(!_dogfightsToBeStarted.empty())
		{
			_dogfights.push_back(_dogfightsToBeStarted.back());
			_dogfightsToBeStarted.pop_back();
			_dogfights.back()->setInterceptionNumber(getFirstFreeDogfightSlot());
			_dogfights.back()->setInterceptionsCount(_dogfights.size() + _dogfightsToBeStarted.size());
		}
		// Set correct number of interceptions for every dogfight.
		for(std::list<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
		{
			(*d)->setInterceptionsCount(_dogfights.size());
		}
	}
}

/**
 * Returns the first free dogfight slot.
 * @return free slot
 */
int GeoscapeState::getFirstFreeDogfightSlot()
{
	int slotNo = 1;
	for(std::list<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
	{
		if ((*d)->getInterceptionNumber() == slotNo)
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

	if (base->getAvailableSoldiers(true) > 0 || !base->getVehicles()->empty())
	{
		SavedBattleGame *bgame = new SavedBattleGame();
		_game->getSavedGame()->setBattleGame(bgame);
		bgame->setMissionType("STR_BASE_DEFENSE");
		BattlescapeGenerator bgen = BattlescapeGenerator(_game);
		bgen.setBase(base);
		bgen.setAlienRace(ufo->getAlienRace());
		bgen.run();
		_pause = true;
		_game->pushState(new BriefingState(0, base));
	}
	else
	{
		// Please garrison your bases in future
		popup(new BaseDestroyedState(base));
	}
}

/**
 * Determine the alien missions to start this month.
 * In the vanilla game each month a terror mission and one other are started in
 * random regions.
 */
void GeoscapeState::determineAlienMissions(bool atGameStart)
{
	if (!atGameStart)
	{
		//
		// One randomly selected mission.
		//
		AlienStrategy &strategy = _game->getSavedGame()->getAlienStrategy();
		const std::string &targetRegion = strategy.chooseRandomRegion(_game->getRuleset());
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
		std::string research = _game->getRuleset()->getAlienMissionList().front();
		const RuleAlienMission &missionRules = *_game->getRuleset()->getAlienMission(research);
		AlienMission *otherMission = new AlienMission(missionRules);
		otherMission->setId(_game->getSavedGame()->getId("ALIEN_MISSIONS"));
		otherMission->setRegion(targetRegion, *_game->getRuleset());
		std::string sectoid = missionRules.getTopRace(_game->getSavedGame()->getMonthsPassed());
		otherMission->setRace(sectoid);
		otherMission->start(150);
		_game->getSavedGame()->getAlienMissions().push_back(otherMission);
		// Make sure this combination never comes up again.
		strategy.removeMission(targetRegion, research);
	}
}

void GeoscapeState::setupTerrorMission()
{
	//Determine a random region with at least one city and no currently running terror mission.
	RuleRegion* region = 0;
	int counter = 0;
	std::vector<std::string> regions = _game->getRuleset()->getRegionsList();
	do
	{
		// we try 40 times to pick a valid zone for a terror mission
		if (counter == 40) return;
		region = _game->getRuleset()->getRegion(regions[RNG::generate(0, regions.size()-1)]);
		counter++;
	}
	while (region->getCities()->empty() || _game->getSavedGame()->getAlienMission(region->getType(), "STR_ALIEN_TERROR") != 0);
	// Choose race for terror mission.
	const RuleAlienMission &terrorRules = *_game->getRuleset()->getAlienMission("STR_ALIEN_TERROR");
	const std::string &terrorRace = terrorRules.generateRace(_game->getSavedGame()->getMonthsPassed());
	AlienMission *terrorMission = new AlienMission(terrorRules);
	terrorMission->setId(_game->getSavedGame()->getId("ALIEN_MISSIONS"));
	terrorMission->setRegion(region->getType(), *_game->getRuleset());
	terrorMission->setRace(terrorRace);
	terrorMission->start(150);
	_game->getSavedGame()->getAlienMissions().push_back(terrorMission);
}
/**
 * Handler for clicking on a timer button.
 * @param action pointer to the mouse action.
 */
void GeoscapeState::btnTimerClick(Action *action)
{
	SDL_Event ev;
	ev.type = SDL_MOUSEBUTTONDOWN;
	ev.button.button = SDL_BUTTON_LEFT;
	Action a = Action(&ev, 0.0, 0.0, 0, 0);
	action->getSender()->mousePress(&a, this);
}

/**
 * Updates the scale.
 * @param dX delta of X;
 * @param dY delta of Y;
 */
void GeoscapeState::resize(int &dX, int &dY)
{
	if (_game->getSavedGame()->getSavedBattle())
		return;
	dX = Options::baseXResolution;
	dY = Options::baseYResolution;
	int divisor = 1;
	double pixelRatioY = 1.0;

	if (Options::nonSquarePixelRatio)
	{
		pixelRatioY = 1.2;
	}
	switch (Options::geoscapeScale)
	{
	case SCALE_SCREEN_DIV_3:
		divisor = 3;
		break;
	case SCALE_SCREEN_DIV_2:
		divisor = 2;
		break;
	case SCALE_SCREEN:
		break;
	default:
		dX = 0;
		dY = 0;
		return;
	}
	
	Options::baseXResolution = std::max(Screen::ORIGINAL_WIDTH, Options::displayWidth / divisor);
	Options::baseYResolution = std::max(Screen::ORIGINAL_HEIGHT, (int)(Options::displayHeight / pixelRatioY / divisor));

	dX = Options::baseXResolution - dX;
	dY = Options::baseYResolution - dY;

	_globe->resize();

	for (std::vector<Surface*>::const_iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		if (*i != _globe)
		{
			(*i)->setX((*i)->getX() + dX);
			(*i)->setY((*i)->getY() + dY/2);
		}
	}

	_bg->setX((_globe->getWidth() - _bg->getWidth()) / 2);
	_bg->setY((_globe->getHeight() - _bg->getHeight()) / 2);

	int height = (Options::baseYResolution - Screen::ORIGINAL_HEIGHT) / 2 + 10;
	_sideTop->setHeight(height);
	_sideTop->setY(_sidebar->getY() - height - 1);
	_sideBottom->setHeight(height);
	_sideBottom->setY(_sidebar->getY() + _sidebar->getHeight() + 1);

	_sideLine->setHeight(Options::baseYResolution);
	_sideLine->setY(0);
	_sideLine->drawRect(0, 0, _sideLine->getWidth(), _sideLine->getHeight(), 15);
}

}
