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
#include "GeoscapeState.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include "../Engine/RNG.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Screen.h"
#include "../Engine/Surface.h"
#include "../Engine/Options.h"
#include "Globe.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Engine/Timer.h"
#include "../Savegame/GameTime.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Mod/RuleBaseFacility.h"
#include "../Savegame/Craft.h"
#include "../Mod/RuleCraft.h"
#include "../Savegame/Ufo.h"
#include "../Mod/RuleUfo.h"
#include "../Mod/RuleMissionScript.h"
#include "../Savegame/Waypoint.h"
#include "../Savegame/Transfer.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDiary.h"
#include "../Menu/PauseState.h"
#include "InterceptState.h"
#include "../Basescape/BasescapeState.h"
#include "../Basescape/SellState.h"
#include "../Menu/CutsceneState.h"
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
#include "DogfightErrorState.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Savegame/ResearchProject.h"
#include "ResearchCompleteState.h"
#include "../Mod/RuleResearch.h"
#include "ResearchRequiredState.h"
#include "NewPossibleResearchState.h"
#include "NewPossibleManufactureState.h"
#include "../Savegame/Production.h"
#include "../Mod/RuleManufacture.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/MissionSite.h"
#include "../Savegame/AlienBase.h"
#include "../Mod/RuleRegion.h"
#include "MissionDetectedState.h"
#include "AlienBaseState.h"
#include "../Savegame/Region.h"
#include "../Savegame/Country.h"
#include "../Mod/RuleCountry.h"
#include "../Mod/RuleAlienMission.h"
#include "../Savegame/AlienStrategy.h"
#include "../Savegame/AlienMission.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Battlescape/BriefingState.h"
#include "../Mod/UfoTrajectory.h"
#include "../Mod/Armor.h"
#include "BaseDefenseState.h"
#include "BaseDestroyedState.h"
#include "../Menu/LoadGameState.h"
#include "../Menu/SaveGameState.h"
#include "../Menu/ListSaveState.h"
#include "../Mod/RuleGlobe.h"
#include "../Engine/Exception.h"
#include "../Mod/AlienDeployment.h"
#include "../Mod/RuleInterface.h"
#include "../fmath.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Geoscape screen.
 * @param game Pointer to the core game.
 */
GeoscapeState::GeoscapeState() : _pause(false), _zoomInEffectDone(false), _zoomOutEffectDone(false), _minimizedDogfights(0)
{
	int screenWidth = Options::baseXGeoscape;
	int screenHeight = Options::baseYGeoscape;

	// Create objects
	Surface *hd = _game->getMod()->getSurface("ALTGEOBORD.SCR");
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

	_zoomInEffectTimer = new Timer(Options::dogfightSpeed);
	_zoomOutEffectTimer = new Timer(Options::dogfightSpeed);
	_dogfightStartTimer = new Timer(Options::dogfightSpeed);
	_dogfightTimer = new Timer(Options::dogfightSpeed);

	_txtDebug = new Text(200, 18, 0, 0);

	// Set palette
	setInterface("geoscape");

	add(_bg);
	add(_sideLine);
	add(_sidebar);
	add(_globe);

	add(_btnIntercept, "button", "geoscape");
	add(_btnBases, "button", "geoscape");
	add(_btnGraphs, "button", "geoscape");
	add(_btnUfopaedia, "button", "geoscape");
	add(_btnOptions, "button", "geoscape");
	add(_btnFunding, "button", "geoscape");

	add(_btn5Secs, "button", "geoscape");
	add(_btn1Min, "button", "geoscape");
	add(_btn5Mins, "button", "geoscape");
	add(_btn30Mins, "button", "geoscape");
	add(_btn1Hour, "button", "geoscape");
	add(_btn1Day, "button", "geoscape");

	add(_btnRotateLeft);
	add(_btnRotateRight);
	add(_btnRotateUp);
	add(_btnRotateDown);
	add(_btnZoomIn);
	add(_btnZoomOut);

	add(_sideTop, "button", "geoscape");
	add(_sideBottom, "button", "geoscape");

	add(_txtFunds, "text", "geoscape");
	add(_txtHour, "text", "geoscape");
	add(_txtHourSep, "text", "geoscape");
	add(_txtMin, "text", "geoscape");
	add(_txtMinSep, "text", "geoscape");
	add(_txtSec, "text", "geoscape");
	add(_txtWeekday, "text", "geoscape");
	add(_txtDay, "text", "geoscape");
	add(_txtMonth, "text", "geoscape");
	add(_txtYear, "text", "geoscape");

	add(_txtDebug, "text", "geoscape");

	// Set up objects
	Surface *geobord = _game->getMod()->getSurface("GEOBORD.SCR");
	geobord->setX(_sidebar->getX() - geobord->getWidth() + _sidebar->getWidth());
	geobord->setY(_sidebar->getY());
	_sidebar->copy(geobord);
	_game->getMod()->getSurface("ALTGEOBORD.SCR")->blit(_bg);

	_sideLine->drawRect(0, 0, _sideLine->getWidth(), _sideLine->getHeight(), 15);

	_btnIntercept->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnIntercept->setText(tr("STR_INTERCEPT"));
	_btnIntercept->onMouseClick((ActionHandler)&GeoscapeState::btnInterceptClick);
	_btnIntercept->onKeyboardPress((ActionHandler)&GeoscapeState::btnInterceptClick, Options::keyGeoIntercept);
	_btnIntercept->setGeoscapeButton(true);

	_btnBases->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnBases->setText(tr("STR_BASES"));
	_btnBases->onMouseClick((ActionHandler)&GeoscapeState::btnBasesClick);
	_btnBases->onKeyboardPress((ActionHandler)&GeoscapeState::btnBasesClick, Options::keyGeoBases);
	_btnBases->setGeoscapeButton(true);

	_btnGraphs->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnGraphs->setText(tr("STR_GRAPHS"));
	_btnGraphs->onMouseClick((ActionHandler)&GeoscapeState::btnGraphsClick);
	_btnGraphs->onKeyboardPress((ActionHandler)&GeoscapeState::btnGraphsClick, Options::keyGeoGraphs);
	_btnGraphs->setGeoscapeButton(true);

	_btnUfopaedia->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnUfopaedia->setText(tr("STR_UFOPAEDIA_UC"));
	_btnUfopaedia->onMouseClick((ActionHandler)&GeoscapeState::btnUfopaediaClick);
	_btnUfopaedia->onKeyboardPress((ActionHandler)&GeoscapeState::btnUfopaediaClick, Options::keyGeoUfopedia);
	_btnUfopaedia->setGeoscapeButton(true);

	_btnOptions->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnOptions->setText(tr("STR_OPTIONS_UC"));
	_btnOptions->onMouseClick((ActionHandler)&GeoscapeState::btnOptionsClick);
	_btnOptions->onKeyboardPress((ActionHandler)&GeoscapeState::btnOptionsClick, Options::keyGeoOptions);
	_btnOptions->setGeoscapeButton(true);

	_btnFunding->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btnFunding->setText(tr("STR_FUNDING_UC"));
	_btnFunding->onMouseClick((ActionHandler)&GeoscapeState::btnFundingClick);
	_btnFunding->onKeyboardPress((ActionHandler)&GeoscapeState::btnFundingClick, Options::keyGeoFunding);
	_btnFunding->setGeoscapeButton(true);

	_btn5Secs->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn5Secs->setBig();
	_btn5Secs->setText(tr("STR_5_SECONDS"));
	_btn5Secs->setGroup(&_timeSpeed);
	_btn5Secs->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed1);
	_btn5Secs->setGeoscapeButton(true);

	_btn1Min->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn1Min->setBig();
	_btn1Min->setText(tr("STR_1_MINUTE"));
	_btn1Min->setGroup(&_timeSpeed);
	_btn1Min->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed2);
	_btn1Min->setGeoscapeButton(true);

	_btn5Mins->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn5Mins->setBig();
	_btn5Mins->setText(tr("STR_5_MINUTES"));
	_btn5Mins->setGroup(&_timeSpeed);
	_btn5Mins->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed3);
	_btn5Mins->setGeoscapeButton(true);

	_btn30Mins->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn30Mins->setBig();
	_btn30Mins->setText(tr("STR_30_MINUTES"));
	_btn30Mins->setGroup(&_timeSpeed);
	_btn30Mins->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed4);
	_btn30Mins->setGeoscapeButton(true);

	_btn1Hour->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn1Hour->setBig();
	_btn1Hour->setText(tr("STR_1_HOUR"));
	_btn1Hour->setGroup(&_timeSpeed);
	_btn1Hour->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed5);
	_btn1Hour->setGeoscapeButton(true);

	_btn1Day->initText(_game->getMod()->getFont("FONT_GEO_BIG"), _game->getMod()->getFont("FONT_GEO_SMALL"), _game->getLanguage());
	_btn1Day->setBig();
	_btn1Day->setText(tr("STR_1_DAY"));
	_btn1Day->setGroup(&_timeSpeed);
	_btn1Day->onKeyboardPress((ActionHandler)&GeoscapeState::btnTimerClick, Options::keyGeoSpeed6);
	_btn1Day->setGeoscapeButton(true);

	_sideBottom->setGeoscapeButton(true);
	_sideTop->setGeoscapeButton(true);

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

	_txtFunds->setAlign(ALIGN_CENTER);
	_txtFunds->setVisible(Options::showFundsOnGeoscape);

	_txtHour->setBig();
	_txtHour->setAlign(ALIGN_RIGHT);

	_txtHourSep->setBig();
	_txtHourSep->setText(L":");

	_txtMin->setBig();

	_txtMinSep->setBig();
	_txtMinSep->setText(L":");

	_txtWeekday->setAlign(ALIGN_CENTER);

	_txtDay->setAlign(ALIGN_CENTER);

	_txtMonth->setAlign(ALIGN_CENTER);

	_txtYear->setAlign(ALIGN_CENTER);

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
	_dogfightTimer->onTimer((StateHandler)&GeoscapeState::handleDogfights);

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
	delete _dogfightTimer;

	std::list<DogfightState*>::iterator it = _dogfights.begin();
	for (; it != _dogfights.end();)
	{
		delete *it;
		it = _dogfights.erase(it);
	}
	for (it = _dogfightsToBeStarted.begin(); it != _dogfightsToBeStarted.end();)
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
	for (std::list<DogfightState*>::iterator it = _dogfights.begin(); it != _dogfights.end(); ++it)
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
		// "ctrl-c" - delete all soldier commendations
		if (Options::debug && action->getDetails()->key.keysym.sym == SDLK_c && (SDL_GetModState() & KMOD_CTRL) != 0)
		{
			if (_game->getSavedGame()->getDebugMode())
			{
				_txtDebug->setText(L"SOLDIER COMMENDATIONS DELETED");
				for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
				{
					for (std::vector<Soldier*>::iterator j = (*i)->getSoldiers()->begin(); j != (*i)->getSoldiers()->end(); ++j)
					{
						for (std::vector<SoldierCommendations*>::iterator k = (*j)->getDiary()->getSoldierCommendations()->begin(); k != (*j)->getDiary()->getSoldierCommendations()->end(); ++k)
						{
							delete *k;
						}
						(*j)->getDiary()->getSoldierCommendations()->clear();
					}
				}
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
		for (std::list<DogfightState*>::iterator it = _dogfights.begin(); it != _dogfights.end(); ++it)
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
			_game->getMod()->playMusic("GMGEO", 1);
		}
		else
		{
			_game->getMod()->playMusic("GMGEO");
		}
	}
	else
	{
		_game->getMod()->playMusic("GMINTER");
	}
	_globe->unsetNewBaseHover();

		// run once
	if (_game->getSavedGame()->getMonthsPassed() == -1 &&
		// as long as there's a base
		!_game->getSavedGame()->getBases()->empty() &&
		// and it has a name (THIS prevents it from running prior to the base being placed.)
		_game->getSavedGame()->getBases()->front()->getName() != L"")
	{
		_game->getSavedGame()->addMonth();
		determineAlienMissions();
		_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - (_game->getSavedGame()->getBaseMaintenance() - _game->getSavedGame()->getBases()->front()->getPersonnelMaintenance()));
	}
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

	if (_popups.empty() && _dogfights.empty() && (!_zoomInEffectTimer->isRunning() || _zoomInEffectDone) && (!_zoomOutEffectTimer->isRunning() || _zoomOutEffectDone))
	{
		// Handle timers
		_gameTimer->think(this, 0);
	}
	else
	{
		if (!_dogfights.empty() || _minimizedDogfights != 0)
		{
			// If all dogfights are minimized rotate the globe, etc.
			if (_dogfights.size() == _minimizedDogfights)
			{
				_pause = false;
				_gameTimer->think(this, 0);
			}
			_dogfightTimer->think(this, 0);
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
		_game->getSavedGame()->setEnding(END_LOSE);
	}
	if (_game->getSavedGame()->getEnding() == END_LOSE)
	{
		_game->pushState(new CutsceneState(CutsceneState::LOSE_GAME));
		if (_game->getSavedGame()->isIronman())
		{
			_game->pushState(new SaveGameState(OPT_GEOSCAPE, SAVE_IRONMAN, _palette));
		}
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
					size_t count = _game->getSavedGame()->getMissionSites()->size();
					AlienMission *mission = (*i)->getMission();
					bool detected = (*i)->getDetected();
					mission->ufoReachedWaypoint(**i, *_game, *_globe);
					if (detected != (*i)->getDetected() && !(*i)->getFollowers()->empty())
					{
						if (!((*i)->getTrajectory().getID() == UfoTrajectory::RETALIATION_ASSAULT_RUN && (*i)->getStatus() == Ufo::LANDED))
							popup(new UfoLostState((*i)->getName(_game->getLanguage())));
					}
					if (count < _game->getSavedGame()->getMissionSites()->size())
					{
						MissionSite *site = _game->getSavedGame()->getMissionSites()->back();
						site->setDetected(true);
						popup(new MissionDetectedState(site, this));
					}
					// If UFO was destroyed, don't spawn missions
					if ((*i)->getStatus() == Ufo::DESTROYED)
						return;
					if (Base *base = dynamic_cast<Base*>((*i)->getDestination()))
					{
						mission->setWaveCountdown(30 * (RNG::generate(0, 400) + 48));
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
			}
			break;
		case Ufo::LANDED:
			(*i)->think();
			if ((*i)->getSecondsRemaining() == 0)
			{
				AlienMission *mission = (*i)->getMission();
				bool detected = (*i)->getDetected();
				mission->ufoLifting(**i, *_game->getSavedGame());
				if (detected != (*i)->getDetected() && !(*i)->getFollowers()->empty())
				{
					popup(new UfoLostState((*i)->getName(_game->getLanguage())));
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
				for (std::vector<Country*>::iterator country = _game->getSavedGame()->getCountries()->begin(); country != _game->getSavedGame()->getCountries()->end(); ++country)
				{
					if ((*country)->getRules()->insideCountry((*j)->getLongitude(), (*j)->getLatitude()))
					{
						(*country)->addActivityXcom(-(*j)->getRules()->getScore());
						break;
					}
				}
				for (std::vector<Region*>::iterator region = _game->getSavedGame()->getRegions()->begin(); region != _game->getSavedGame()->getRegions()->end(); ++region)
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
							k = _game->getSavedGame()->killSoldier(*k);
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
				if (u != 0)
				{
					if (!u->getDetected())
					{
						if (u->getTrajectory().getID() == UfoTrajectory::RETALIATION_ASSAULT_RUN && (u->getStatus() == Ufo::LANDED || u->getStatus() == Ufo::DESTROYED))
						{
							(*j)->returnToBase();
						}
						else
						{
							Waypoint *w = new Waypoint();
							w->setLongitude((*j)->getMeetLongitude());
							w->setLatitude((*j)->getMeetLatitude());
							(*j)->setDestination(0);
							popup(new GeoscapeCraftState((*j), _globe, w));
						}
					}
					if (u->getStatus() == Ufo::LANDED && (*j)->isInDogfight())
					{
						(*j)->setInDogfight(false);
					}
					else if (u->getStatus() == Ufo::DESTROYED)
					{
						(*j)->returnToBase();
					}
				}
				else
				{
					if ((*j)->isInDogfight())
					{
						(*j)->setInDogfight(false);
					}
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
				MissionSite* m = dynamic_cast<MissionSite*>((*j)->getDestination());
				AlienBase* b = dynamic_cast<AlienBase*>((*j)->getDestination());
				bool underwater = false;
				if (u != 0)
				{
					switch (u->getStatus())
					{
					case Ufo::FLYING:
						// Not more than 4 interceptions at a time.
						if (_dogfights.size() + _dogfightsToBeStarted.size() >= 4)
						{
							++j;
							continue;
						}
						// Can we actually fight it
						if ((*j)->getDestination()->getSiteDepth() > (*j)->getRules()->getMaxDepth())
						{
							popup(new DogfightErrorState((*j), tr("STR_UNABLE_TO_ENGAGE_DEPTH")));
							++j;
							continue;
						}
						else
						{
							underwater = (*j)->getRules()->getMaxDepth() > 0;
						}
						if (!(*j)->isInDogfight() && !(*j)->getDistance(u))
						{
							_dogfightsToBeStarted.push_back(new DogfightState(this, (*j), u));
							if (underwater && !_globe->insideLand((*j)->getLongitude(), (*j)->getLatitude()))
							{
								popup(new DogfightErrorState((*j), tr("STR_UNABLE_TO_ENGAGE_AIRBORNE")));
								_dogfightsToBeStarted.back()->setMinimized(true);
								_dogfightsToBeStarted.back()->setWaitForPoly(true);
							}
							if (!_dogfightStartTimer->isRunning())
							{
								_pause = true;
								timerReset();
								_globe->center((*j)->getLongitude(), (*j)->getLatitude());
								startDogfight();
								_dogfightStartTimer->start();
							}
							_game->getMod()->playMusic("GMINTER");
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
								popup(new ConfirmLandingState(*j, _game->getMod()->getGlobe()->getTexture(texture), shade));
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
				else if (m != 0)
				{
					if ((*j)->getNumSoldiers() > 0)
					{
						// look up polygons texture
						int texture, shade;
						_globe->getPolygonTextureAndShade(m->getLongitude(), m->getLatitude(), &texture, &shade);
						texture = m->getTexture();
						timerReset();
						popup(new ConfirmLandingState(*j, _game->getMod()->getGlobe()->getTexture(texture), shade));
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
						if ((*j)->getNumSoldiers() > 0)
						{
							int texture, shade;
							_globe->getPolygonTextureAndShade(b->getLongitude(), b->getLatitude(), &texture, &shade);
							timerReset();
							popup(new ConfirmLandingState(*j, _game->getMod()->getGlobe()->getTexture(texture), shade));
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
				for (std::list<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end();)
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
	if (ufo->getTrajectoryPoint() <= 1) return false;
	if (ufo->getTrajectory().getZone(ufo->getTrajectoryPoint()) == 5) return false;
	if ((ufo->getMission()->getRules().getObjective() != OBJECTIVE_RETALIATION && !Options::aggressiveRetaliation) || // only UFOs on retaliation missions actively scan for bases
		ufo->getTrajectory().getID() == UfoTrajectory::RETALIATION_ASSAULT_RUN || 									// UFOs attacking a base don't detect!
		ufo->isCrashed() ||																				// Crashed UFOs don't detect!
		_base.getDistance(ufo) >= ufo->getRules()->getSightRange() * (1 / 60.0) * (M_PI / 180.0))		// UFOs have a detection range of 80 XCOM units. - we use a great circle fomrula and nautical miles.
	{
		return false;
	}
	return RNG::percent(_base.getDetectionChance());
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
					for (std::vector<AlienBase*>::iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); b++)
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
	if (Options::aggressiveRetaliation)
	{
		// Detect as many bases as possible.
		for (std::vector<Base*>::iterator iBase = _game->getSavedGame()->getBases()->begin(); iBase != _game->getSavedGame()->getBases()->end(); ++iBase)
		{
			// Find a UFO that detected this base, if any.
			std::vector<Ufo*>::const_iterator uu = std::find_if (_game->getSavedGame()->getUfos()->begin(), _game->getSavedGame()->getUfos()->end(), DetectXCOMBase(**iBase));
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
			std::vector<Ufo*>::const_iterator uu = std::find_if (_game->getSavedGame()->getUfos()->begin(), _game->getSavedGame()->getUfos()->end(), DetectXCOMBase(**iBase));
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

/** @brief Process a MissionSite.
 * This function object will count down towards expiring a MissionSite, and handle expired MissionSites.
 * @param ts Pointer to mission site.
 * @return Has mission site expired?
 */
bool GeoscapeState::processMissionSite(MissionSite *site) const
{
	bool removeSite = site->getSecondsRemaining() < 30 * 60;
	if (!removeSite)
	{
		site->setSecondsRemaining(site->getSecondsRemaining() - 30 * 60);
	}
	else
	{
		removeSite = site->getFollowers()->empty(); // CHEEKY EXPLOIT
	}

	int score = removeSite ? site->getDeployment()->getDespawnPenalty() : site->getDeployment()->getPoints();

	Region *region = _game->getSavedGame()->locateRegion(*site);
	if (region)
	{
		region->addActivityAlien(score);
	}
	for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
	{
		if ((*k)->getRules()->insideCountry(site->getLongitude(), site->getLatitude()))
		{
			(*k)->addActivityAlien(score);
			break;
		}
	}
	if (!removeSite)
	{
		return false;
	}
	delete site;
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
				if (item.empty())
				{
					(*j)->refuel();
				}
				else
				{
					if ((*i)->getStorageItems()->getItem(item) > 0)
					{
						(*i)->getStorageItems()->removeItem(item);
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
		int points = (*u)->getRules()->getMissionScore(); //one point per UFO in-flight per half hour
		switch ((*u)->getStatus())
		{
		case Ufo::LANDED:
			points *= 2;
		case Ufo::FLYING:
			// Get area
			for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
			{
				if ((*k)->getRules()->insideRegion((*u)->getLongitude(), (*u)->getLatitude()))
				{
					(*k)->addActivityAlien(points);
					break;
				}
			}
			// Get country
			for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
			{
				if ((*k)->getRules()->insideCountry((*u)->getLongitude(), (*u)->getLatitude()))
				{
					(*k)->addActivityAlien(points);
					break;
				}
			}
			if (!(*u)->getDetected())
			{
				bool detected = false, hyperdetected = false;
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
			else
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
						if ((*c)->getStatus() == "STR_OUT" && (*c)->insideRadarRange(*u))
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

	// Processes MissionSites
	for (std::vector<MissionSite*>::iterator site = _game->getSavedGame()->getMissionSites()->begin(); site != _game->getSavedGame()->getMissionSites()->end();)
	{
		if (processMissionSite(*site))
		{
			site = _game->getSavedGame()->getMissionSites()->erase(site);
		}
		else
		{
			++site;
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
				std::string s = (*j)->rearm(_game->getMod());
				if (!s.empty())
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
			if (!window && (*j)->getHours() <= 0)
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
			toRemove[(*j)] = (*j)->step((*i), _game->getSavedGame(), _game->getMod());
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
			popup(new ErrorMessageState(tr("STR_STORAGE_EXCEEDED").arg((*i)->getName()), _palette, _game->getMod()->getInterface("geoscape")->getElement("errorMessage")->color, "BACK13.SCR", _game->getMod()->getInterface("geoscape")->getElement("errorPalette")->color));
			popup(new SellState((*i)));
		}
	}
	for (std::vector<MissionSite*>::iterator i = _game->getSavedGame()->getMissionSites()->begin(); i != _game->getSavedGame()->getMissionSites()->end(); ++i)
	{
		if (!(*i)->getDetected())
		{
			(*i)->setDetected(true);
			popup(new MissionDetectedState(*i, this));
			break;
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
	GenerateSupplyMission(const Mod &mod, SavedGame &save) : _mod(mod), _save(save) { /* Empty by design */ }
	/// Check and spawn mission.
	void operator()(const AlienBase *base) const;
private:
	const Mod &_mod;
	SavedGame &_save;
};

/**
 * Check and create supply mission for the given base.
 * There is a 6/101 chance of the mission spawning.
 * @param base A pointer to the alien base.
 */
void GenerateSupplyMission::operator()(const AlienBase *base) const
{
	if (_mod.getAlienMission(base->getDeployment()->getGenMissionType()))
	{
		if (RNG::percent(base->getDeployment()->getGenMissionFrequency()))
		{
			//Spawn supply mission for this base.
			const RuleAlienMission &rule = *_mod.getAlienMission(base->getDeployment()->getGenMissionType());
			AlienMission *mission = new AlienMission(rule);
			mission->setRegion(_save.locateRegion(*base)->getRules()->getType(), _mod);
			mission->setId(_save.getId("ALIEN_MISSIONS"));
			mission->setRace(base->getAlienRace());
			mission->setAlienBase(base);
			mission->start();
			_save.getAlienMissions().push_back(mission);
		}
	}
	else if (base->getDeployment()->getGenMissionType() != "")
	{
		throw Exception("Alien Base tried to generate undefined mission: " + base->getDeployment()->getGenMissionType());
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
		for (std::vector<ResearchProject*>::const_iterator iter = (*i)->getResearch().begin(); iter != (*i)->getResearch().end(); ++iter)
		{
			if ((*iter)->step())
			{
				finished.push_back(*iter);
			}
		}
		for (std::vector<ResearchProject*>::const_iterator iter = finished.begin(); iter != finished.end(); ++iter)
		{
			(*i)->removeResearch(*iter);
			RuleResearch * bonus = 0;
			const RuleResearch * research = (*iter)->getRules();
			if (Options::retainCorpses && research->destroyItem() && _game->getMod()->getUnit(research->getName()))
			{
				(*i)->getStorageItems()->addItem(_game->getMod()->getArmor(_game->getMod()->getUnit(research->getName())->getArmor())->getCorpseGeoscape());
			}
			if (!(*iter)->getRules()->getGetOneFree().empty())
			{
				std::vector<std::string> possibilities;
				for (std::vector<std::string>::const_iterator f = research->getGetOneFree().begin(); f != research->getGetOneFree().end(); ++f)
				{
					bool newFound = true;
					for (std::vector<const RuleResearch*>::const_iterator discovered = _game->getSavedGame()->getDiscoveredResearch().begin(); discovered != _game->getSavedGame()->getDiscoveredResearch().end() && newFound; ++discovered)
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
				if (!possibilities.empty())
				{
					size_t pick = RNG::generate(0, possibilities.size()-1);
					std::string sel = possibilities.at(pick);
					bonus = _game->getMod()->getResearch(sel);
					_game->getSavedGame()->addFinishedResearch(bonus, _game->getMod());
					if (!bonus->getLookup().empty())
					{
						_game->getSavedGame()->addFinishedResearch(_game->getMod()->getResearch(bonus->getLookup()), _game->getMod());
					}
				}
			}
			const RuleResearch * newResearch = research;
			std::string name = research->getLookup().empty() ? research->getName() : research->getLookup();
			if (_game->getSavedGame()->isResearched(name))
			{
				newResearch = 0;
			}
			_game->getSavedGame()->addFinishedResearch(research, _game->getMod());
			if (!research->getLookup().empty())
			{
				_game->getSavedGame()->addFinishedResearch(_game->getMod()->getResearch(research->getLookup()), _game->getMod());
			}
			if (!research->getCutscene().empty())
			{
				popup(new CutsceneState(research->getCutscene()));
			}
			if (bonus && !bonus->getCutscene().empty())
			{
				popup(new CutsceneState(bonus->getCutscene()));
			}
			popup(new ResearchCompleteState(newResearch, bonus, research));
			std::vector<RuleResearch *> newPossibleResearch;
			_game->getSavedGame()->getDependableResearch (newPossibleResearch, research, _game->getMod(), *i);
			std::vector<RuleManufacture *> newPossibleManufacture;
			_game->getSavedGame()->getDependableManufacture (newPossibleManufacture, research, _game->getMod(), *i);
			timerReset();
			// check for possible researching weapon before clip
			if (newResearch)
			{
				RuleItem *item = _game->getMod()->getItem(newResearch->getName());
				if (item && item->getBattleType() == BT_FIREARM && !item->getCompatibleAmmo()->empty())
				{
					RuleManufacture *man = _game->getMod()->getManufacture(item->getType());
					if (man && !man->getRequirements().empty())
					{
						const std::vector<std::string> &req = man->getRequirements();
						RuleItem *ammo = _game->getMod()->getItem(item->getCompatibleAmmo()->front());
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
					if (research->getName() == (*iter2)->getRules()->getName() &&
						_game->getMod()->getUnit((*iter2)->getRules()->getName()) == 0)
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
				(*s)->calcStatString(_game->getMod()->getStatStrings(), (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getMod()->getPsiRequirements())));
			}
		}
	}
	// handle regional and country points for alien bases
	for (std::vector<AlienBase*>::const_iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); ++b)
	{
		for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
		{
			if ((*k)->getRules()->insideRegion((*b)->getLongitude(), (*b)->getLatitude()))
			{
				(*k)->addActivityAlien((*b)->getDeployment()->getPoints());
				break;
			}
		}
		for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
		{
			if ((*k)->getRules()->insideCountry((*b)->getLongitude(), (*b)->getLatitude()))
			{
				(*k)->addActivityAlien((*b)->getDeployment()->getPoints());
				break;
			}
		}
	}

	// Handle resupply of alien bases.
	std::for_each(_game->getSavedGame()->getAlienBases()->begin(), _game->getSavedGame()->getAlienBases()->end(),
			  GenerateSupplyMission(*_game->getMod(), *_game->getSavedGame()));

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

	// Determine alien mission for this month.
	determineAlienMissions();

	// Handle Psi-Training and initiate a new retaliation mission, if applicable
	bool psi = false;
	if (!Options::anytimePsiTraining)
	{
		for (std::vector<Base*>::const_iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end(); ++b)
		{
			if ((*b)->getAvailablePsiLabs() > 0)
			{
				psi = true;
				for (std::vector<Soldier*>::const_iterator s = (*b)->getSoldiers()->begin(); s != (*b)->getSoldiers()->end(); ++s)
				{
					if ((*s)->isInPsiTraining())
					{
						(*s)->trainPsi();
						(*s)->calcStatString(_game->getMod()->getStatStrings(), (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getMod()->getPsiRequirements())));
					}
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
		for (std::vector<AlienBase*>::const_iterator b = _game->getSavedGame()->getAlienBases()->begin(); b != _game->getSavedGame()->getAlienBases()->end(); ++b)
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
		int texture, shade;
		_globe->cartToPolar(mouseX, mouseY, &lon, &lat);
		double lonDeg = lon / M_PI * 180, latDeg = lat / M_PI * 180;
		_globe->getPolygonTextureAndShade(lon, lat, &texture, &shade);
		std::wostringstream ss;
		ss << "rad: " << lon << ", " << lat << std::endl;
		ss << "deg: " << lonDeg << ", " << latDeg << std::endl;
		ss << "texture: " << texture << ", shade: " << shade << std::endl;

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
	// Handle dogfights logic.
	_minimizedDogfights = 0;

	std::list<DogfightState*>::iterator d = _dogfights.begin();
	for (; d != _dogfights.end(); ++d)
	{
		(*d)->getUfo()->setInterceptionProcessed(false);
	}
	d = _dogfights.begin();
	while (d != _dogfights.end())
	{
		if ((*d)->isMinimized())
		{
			if ((*d)->getWaitForPoly() && _globe->insideLand((*d)->getUfo()->getLongitude(), (*d)->getUfo()->getLatitude()))
			{
				(*d)->setMinimized(false);
				(*d)->setWaitForPoly(false);
			}
			else
			{
				_minimizedDogfights++;
			}
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
		_dogfightTimer->stop();
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
	for (std::list<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
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
		_dogfightTimer->start();
		timerReset();
		while (!_dogfightsToBeStarted.empty())
		{
			_dogfights.push_back(_dogfightsToBeStarted.back());
			_dogfightsToBeStarted.pop_back();
			_dogfights.back()->setInterceptionNumber(getFirstFreeDogfightSlot());
			_dogfights.back()->setInterceptionsCount(_dogfights.size() + _dogfightsToBeStarted.size());
		}
		// Set correct number of interceptions for every dogfight.
		for (std::list<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
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
	for (std::list<DogfightState*>::iterator d = _dogfights.begin(); d != _dogfights.end(); ++d)
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
 */
void GeoscapeState::determineAlienMissions()
{
	SavedGame *save = _game->getSavedGame();
	AlienStrategy &strategy = save->getAlienStrategy();
	Mod *mod = _game->getMod();
	int month = _game->getSavedGame()->getMonthsPassed();
	std::vector<RuleMissionScript*> availableMissions;
	std::map<int, bool> conditions;

	// well, here it is, ladies and gents, the nuts and bolts behind the geoscape mission scheduling.

	// first we need to build a list of "valid" commands
	for (std::vector<std::string>::const_iterator i = mod->getMissionScriptList()->begin(); i != mod->getMissionScriptList()->end(); ++i)
	{
		RuleMissionScript *command = mod->getMissionScript(*i);

			// level one condition check: make sure we're within our time constraints
		if (command->getFirstMonth() <= month &&
			(command->getLastMonth() >= month || command->getLastMonth() == -1) &&
			// make sure we haven't hit our run limit, if we have one
			(command->getMaxRuns() == -1 ||	command->getMaxRuns() > strategy.getMissionsRun(command->getVarName())) &&
			// and make sure we satisfy the difficulty restrictions
			command->getMinDifficulty() <= save->getDifficulty())
		{
			// level two condition check: make sure we meet any research requirements, if any.
			bool triggerHappy = true;
			for (std::map<std::string, bool>::const_iterator j = command->getResearchTriggers().begin(); triggerHappy && j != command->getResearchTriggers().end(); ++j)
			{
				triggerHappy = (save->isResearched(j->first) == j->second);
			}
			// levels one and two passed: insert this command into the array.
			if (triggerHappy)
			{
				availableMissions.push_back(command);
			}
		}
	}

	// start processing command array.
	for (std::vector<RuleMissionScript*>::const_iterator i = availableMissions.begin(); i != availableMissions.end(); ++i)
	{
		RuleMissionScript *command = *i;
		bool process = true;
		bool success = false;
		// level three condition check: make sure our conditionals are met, if any. this list is dynamic, and must be checked here.
		for (std::vector<int>::const_iterator j = command->getConditionals().begin(); process && j != command->getConditionals().end(); ++j)
		{
			std::map<int, bool>::const_iterator found = conditions.find(std::abs(*j));
			// just an FYI: if you add a 0 to your conditionals, this flag will never resolve to true, and your command will never run.
			process = (found == conditions.end() || (found->second == true && *j > 0) || (found->second == false && *j < 0));
		}
		if (command->getLabel() > 0 && conditions.find(command->getLabel()) != conditions.end())
		{
			std::ostringstream ss;
			ss << "Mission generator encountered an error: multiple commands: " << command->getType() << " and ";
			for (std::vector<RuleMissionScript*>::const_iterator j = availableMissions.begin(); j != availableMissions.end(); ++j)
			{
				if (command->getLabel() == (*j)->getLabel() && (*j) != (*i))
				{
					ss << (*j)->getType() << ", ";
				}
			}
			ss  << "are sharing the same label: " << command->getLabel();
			throw Exception(ss.str());
		}
		// level four condition check: does random chance favour this command's execution?
		if (process && RNG::percent(command->getExecutionOdds()))
		{
			// good news, little command pointer! you're FDA approved! off to the main processing facility with you!
			success = processCommand(command);
		}
		if (command->getLabel() > 0)
		{
			// tsk, tsk. you really should be careful with these unique labels, they're supposed to be unique.
			if (conditions.find(command->getLabel()) != conditions.end())
			{
				throw Exception("Error in mission scripts: " + command->getType() + ". Two or more commands sharing the same label. That's bad, Mmmkay?");
			}
			// keep track of what happened to this command, so others may reference it.
			conditions[command->getLabel()] = success;
		}
	}
}


/**
 * Proccesses a directive to start up a mission, if possible.
 * @param command the directive from which to read information.
 * @return whether the command successfully produced a new mission.
 */
bool GeoscapeState::processCommand(RuleMissionScript *command)
{
	SavedGame *save = _game->getSavedGame();
	AlienStrategy &strategy = save->getAlienStrategy();
	Mod *mod = _game->getMod();
	int month = _game->getSavedGame()->getMonthsPassed();
	std::string targetRegion;
	const RuleAlienMission *missionRules;
	std::string missionType;
	std::string missionRace;
	int targetZone = -1;

	// terror mission type deal? this will require special handling.
	if (command->getSiteType())
	{
		// we know for a fact that this command has mission weights defined, otherwise this flag could not be set.
		missionType = command->generate(month, GEN_MISSION);
		std::vector<std::string> missions = command->getMissionTypes(month);
		int max = missions.size();
		int currPos = 0;
		for (; currPos != max; ++currPos)
		{
			if (missions[currPos] == missionType)
			{
				break;
			}
		}

		// let's build a list of regions with spawn zones to pick from
		std::vector<std::pair<std::string, int> > validAreas;

		// this is actually a bit of a cheat, we ARE using the mission weights as defined, but we'll try them all if the one we pick first isn't valid.
		for (int h = 0; h != max; ++h)
		{
			// we'll use the regions listed in the command, if any, otherwise check all the regions in the ruleset looking for matches
			std::vector<std::string> regions = (command->hasRegionWeights()) ? command->getRegions(month) : mod->getRegionsList();
			missionRules = mod->getAlienMission(missionType);
			targetZone = missionRules->getSpawnZone();

			for (std::vector<std::string>::iterator i = regions.begin(); i != regions.end();)
			{
				// we don't want the same mission running in any given region twice simultaneously, so prune the list as needed.
				bool processThisRegion = true;
				for (std::vector<AlienMission*>::const_iterator j = save->getAlienMissions().begin(); j != save->getAlienMissions().end(); ++j)
				{
					if ((*j)->getRules().getType() == missionRules->getType() && (*j)->getRegion() == *i)
					{
						processThisRegion = false;
						break;
					}
				}
				if (!processThisRegion)
				{
					i = regions.erase(i);
					continue;
				}
				// ok, we found a region that doesn't have our mission in it, let's see if it has an appropriate landing zone.
				// if it does, let's add it to our list of valid areas, taking note of which mission area(s) matched.
				RuleRegion *region = mod->getRegion(*i);
				if ((int)(region->getMissionZones().size()) > targetZone)
				{
					std::vector<MissionArea> areas = region->getMissionZones()[targetZone].areas;
					int counter = 0;
					for (std::vector<MissionArea>::const_iterator j = areas.begin(); j != areas.end(); ++j)
					{
						// validMissionLocation checks to make sure this city/whatever hasn't been used by the last n missions using this varName
						// this prevents the same location getting hit more than once every n missions.
						if ((*j).isPoint() && strategy.validMissionLocation(command->getVarName(), region->getType(), counter))
						{
							validAreas.push_back(std::make_pair(region->getType(), counter));
						}
						counter++;
					}
				}
				++i;
			}

			// oh bother, we couldn't find anything valid, this mission won't run this month.
			if (validAreas.empty())
			{
				if (max > 1 && ++currPos == max)
				{
					currPos = 0;
				}
				missionType = missions[currPos];
			}
			else
			{
				break;
			}
		}

		if (validAreas.empty())
		{
			// now we're in real trouble, we've managed to make it out of the loop and we still don't have any valid choices
			// this command cannot run this month, we have failed, forgive us senpai.
			return false;
		}
		// reset this, we may have used it earlier, it longer represents the target zone type, but the target zone number within that type
		targetZone = -1;
		// everything went according to plan: we can now pick a city/whatever to attack.
		while (targetZone == -1)
		{
			if (command->hasRegionWeights())
			{
				// if we have a weighted region list, we know we have at least one valid choice for this mission
				targetRegion = command->generate(month, GEN_REGION);
			}
			else
			{
				// if we don't have a weighted list, we'll select a region at random from the ruleset,
				// validate that it's in our list, and pick one of its cities at random
				// this will give us an even distribution between regions regardless of the number of cities.
				targetRegion = mod->getRegionsList().at(RNG::generate(0, mod->getRegionsList().size() - 1));
			}

			// we need to know the range of the region within our vector, in order to randomly select a city from it
			int min = -1;
			int max = -1;
			int curr = 0;
			for (std::vector<std::pair<std::string, int> >::const_iterator i = validAreas.begin(); i != validAreas.end(); ++i)
			{
				if ((*i).first == targetRegion)
				{
					if (min == -1)
					{
						min = curr;
					}
					max = curr;
				}
				else if (min > -1)
				{
					// if we've stopped detecting matches, we're done looking.
					break;
				}
				++curr;
			}
			if (min != -1)
			{
				// we have our random range, we can make a selection, and we're done.
				targetZone = validAreas[RNG::generate(min, max)].second;
			}
		}
		// now add that city to the list of sites we've hit, store the array, etc.
		strategy.addMissionLocation(command->getVarName(), targetRegion, targetZone, command->getRepeatAvoidance());
	}
	else if (RNG::percent(command->getTargetBaseOdds()))
	{
		// build a list of the mission types we're dealing with, if any
		std::vector<std::string> types = command->getMissionTypes(month);
		// now build a list of regions with bases in.
		std::vector<std::string> regionsMaster;
		for (std::vector<Base*>::const_iterator i = save->getBases()->begin(); i != save->getBases()->end(); ++i)
		{
			regionsMaster.push_back(save->locateRegion(*(*i))->getRules()->getType());
		}
		// no defined mission types? then we'll prune the region list to ensure we only have a region that can generate a mission.
		if (types.empty())
		{
			for (std::vector<std::string>::iterator i = regionsMaster.begin(); i != regionsMaster.end();)
			{
				if (!strategy.validMissionRegion(*i))
				{
					i = regionsMaster.erase(i);
					continue;
				}
				++i;
			}
			// no valid missions in any base regions? oh dear, i guess we failed.
			if (regionsMaster.empty())
			{
				return false;
			}
			// pick a random region from our list
			targetRegion = regionsMaster[RNG::generate(0, regionsMaster.size()-1)];
		}
		else
		{
			// we don't care about regional mission distributions, we're targetting a base with whatever mission we pick, so let's pick now
			// we'll iterate the mission list, starting at a random point, and wrapping around to the beginning
			int max = types.size();
			int entry = RNG::generate(0,  max - 1);
			std::vector<std::string> regions;

			for (int i = 0; i != max; ++i)
			{
				regions = regionsMaster;
				for (std::vector<AlienMission*>::const_iterator j = save->getAlienMissions().begin(); j != save->getAlienMissions().end(); ++j)
				{
					// if the mission types match
					if (types[entry] == (*j)->getRules().getType())
					{
						for (std::vector<std::string>::iterator k = regions.begin(); k != regions.end();)
						{
							// and the regions match
							if ((*k) == (*j)->getRegion())
							{
								// prune the entry from the list
								k = regions.erase(k);
								continue;
							}
							++k;
						}
					}
				}

				// we have a valid list of regions containing bases, pick one.
				if (!regions.empty())
				{
					missionType = types[entry];
					targetRegion = regions[RNG::generate(0, regions.size()-1)];
					break;
				}
				// otherwise, try the next mission in the list.
				if (max > 1 && ++entry == max)
				{
					entry = 0;
				}
			}
		}
	}
	// now the easy stuff
	else if (!command->hasRegionWeights())
	{
		// no regionWeights means we pick from the table
		targetRegion = strategy.chooseRandomRegion(mod);
	}
	else
	{
		// otherwise, let the command dictate the region.
		targetRegion = command->generate(month, GEN_REGION);
	}

	if (targetRegion == "")
	{
		// something went horribly wrong, we should have had at LEAST a region by now.
		return false;
	}

	// we're bound to end up with typos, so let's throw an exception instead of simply returning false
	// that way, the modder can fix their mistake
	if (mod->getRegion(targetRegion) == 0)
	{
		throw Exception("Error proccessing mission script named: " + command->getType() + ", region named: " + targetRegion + " is not defined");
	}

	if (missionType == "") // ie: not a terror mission, not targetting a base, or otherwise not already chosen
	{
		if (!command->hasMissionWeights())
		{
			// no weights means let the strategy pick
			missionType = strategy.chooseRandomMission(targetRegion);
		}
		else
		{
			// otherwise the command gives us the weights.
			missionType = command->generate(month, GEN_MISSION);
		}
	}

	if (missionType == "")
	{
		// something went horribly wrong, we didn't manage to choose a mission type
		return false;
	}

	missionRules = mod->getAlienMission(missionType);

	// we're bound to end up with typos, so let's throw an exception instead of simply returning false
	// that way, the modder can fix their mistake
	if (missionRules == 0)
	{
		throw Exception("Error proccessing mission script named: " + command->getType() + ", mission type: " + missionType + " is not defined");
	}

	// do i really need to comment this? shouldn't it be obvious what's happening here?
	if (!command->hasRaceWeights())
	{
		missionRace = missionRules->generateRace(month);
	}
	else
	{
		missionRace = command->generate(month, GEN_RACE);
	}

	// we're bound to end up with typos, so let's throw an exception instead of simply returning false
	// that way, the modder can fix their mistake
	if (mod->getAlienRace(missionRace) == 0)
	{
		throw Exception("Error proccessing mission script named: " + command->getType() + ", race: " + missionRace + " is not defined");
	}

	// ok, we've derived all the variables we need to start up our mission, let's do magic to turn those values into a mission
	AlienMission *mission = new AlienMission(*missionRules);
	mission->setRace(missionRace);
	mission->setId(_game->getSavedGame()->getId("ALIEN_MISSIONS"));
	mission->setRegion(targetRegion, *_game->getMod());
	mission->setMissionSiteZone(targetZone);
	strategy.addMissionRun(command->getVarName());
	mission->start(command->getDelay());
	_game->getSavedGame()->getAlienMissions().push_back(mission);
	// if this flag is set, we want to delete it from the table so it won't show up again until the schedule resets.
	if (command->getUseTable())
	{
		strategy.removeMission(targetRegion, missionType);
	}

	// we did it, we can go home now.
	return true;

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
