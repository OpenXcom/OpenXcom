/*
 * Copyright 2010 OpenXcom Developers.
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
#include <cmath>
#include <sstream>
#include "Map.h"
#include "Camera.h"
#include "BattlescapeState.h"
#include "NextTurnState.h"
#include "AbortMissionState.h"
#include "BattleState.h"
#include "UnitTurnBState.h"
#include "UnitWalkBState.h"
#include "ProjectileFlyBState.h"
#include "ExplosionBState.h"
#include "TileEngine.h"
#include "ActionMenuState.h"
#include "UnitInfoState.h"
#include "UnitDieBState.h"
#include "InventoryState.h"
#include "AggroBAIState.h"
#include "PatrolBAIState.h"
#include "Pathfinding.h"
#include "../Engine/Game.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Screen.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Cursor.h"
#include "../Interface/FpsCounter.h"
#include "../Interface/Text.h"
#include "../Interface/Bar.h"
#include "../Interface/ImageButton.h"
#include "../Interface/NumberText.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/BattleItem.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleArmor.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include "WarningMessage.h"
#include "BattlescapeOptionsState.h"
#include "DebriefingState.h"
#include "../Engine/RNG.h"
#include "InfoboxState.h"
#include "MiniMapState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Battlescape screen.
 * @param game Pointer to the core game.
 */
BattlescapeState::BattlescapeState(Game *game) : State(game), _popups()
{
	//game->getScreen()->setScale(1.0);
	int mapWidth = int(game->getScreen()->getWidth() / game->getScreen()->getXScale());
	int mapHeight = int(game->getScreen()->getHeight() / game->getScreen()->getYScale());
	int iconsWidth = 320;
	int iconsHeight = 56;

	// Create buttonbar - this should be on the centerbottom of the screen
	_icons = new Surface(iconsWidth, iconsHeight, mapWidth/2 - iconsWidth/2, mapHeight - iconsHeight);

	// Create the battlemap view
	// the actual map height is the total height minus the height of the buttonbar
	int visibleMapHeight = mapHeight - iconsHeight;
	_map = new Map(_game, mapWidth, mapHeight, 0, 0, visibleMapHeight);

	_numLayers = new NumberText(3, 5, _icons->getX() + 232, _icons->getY() + 6);
	_rank = new Surface(26, 23, _icons->getX() + 107, _icons->getY() + 33);

	// Create buttons
	_btnUnitUp = new InteractiveSurface(32, 16, _icons->getX() + 48, _icons->getY());
	_btnUnitDown = new InteractiveSurface(32, 16, _icons->getX() + 48, _icons->getY() + 16);
	_btnMapUp = new InteractiveSurface(32, 16, _icons->getX() + 80, _icons->getY());
	_btnMapDown = new InteractiveSurface(32, 16, _icons->getX() + 80, _icons->getY() + 16);
	_btnShowMap = new InteractiveSurface(32, 16, _icons->getX() + 112, _icons->getY());
	_btnKneel = new InteractiveSurface(32, 16, _icons->getX() + 112, _icons->getY() + 16);
	_btnInventory = new InteractiveSurface(32, 16, _icons->getX() + 144, _icons->getY());
	_btnCenter = new InteractiveSurface(32, 16, _icons->getX() + 144, _icons->getY() + 16);
	_btnNextSoldier = new InteractiveSurface(32, 16, _icons->getX() + 176, _icons->getY());
	_btnNextStop = new InteractiveSurface(32, 16, _icons->getX() + 176, _icons->getY() + 16);
	_btnShowLayers = new InteractiveSurface(32, 16, _icons->getX() + 208, _icons->getY());
	_btnHelp = new InteractiveSurface(32, 16, _icons->getX() + 208, _icons->getY() + 16);
	_btnEndTurn = new InteractiveSurface(32, 16, _icons->getX() + 240, _icons->getY());
	_btnAbort = new InteractiveSurface(32, 16, _icons->getX() + 240, _icons->getY() + 16);
	_btnStats = new InteractiveSurface(166, 24, _icons->getX() + 106, _icons->getY() + 32);
	_btnReserveNone = new ImageButton(28, 11, _icons->getX() + 49, _icons->getY() + 33);
	_btnReserveSnap = new ImageButton(28, 11, _icons->getX() + 78, _icons->getY() + 33);
	_btnReserveAimed = new ImageButton(28, 11, _icons->getX() + 49, _icons->getY() + 45);
	_btnReserveAuto = new ImageButton(28, 11, _icons->getX() + 78, _icons->getY() + 45);
	_btnLeftHandItem = new InteractiveSurface(32, 48, _icons->getX() + 8, _icons->getY() + 5);
	_numAmmoLeft = new NumberText(30, 5, _icons->getX() + 8, _icons->getY() + 5);
	_btnRightHandItem = new InteractiveSurface(32, 48, _icons->getX() + 280, _icons->getY() + 5);
	_numAmmoRight = new NumberText(30, 5, _icons->getX() + 280, _icons->getY() + 5);
	for (int i = 0; i < 10; ++i)
	{
		_btnVisibleUnit[i] = new InteractiveSurface(15, 12, 300, 128 - (i * 13));
		_numVisibleUnit[i] = new NumberText(15, 12, 306, 132 - (i * 13));
	}
	_numVisibleUnit[9]->setX(304); // center number 10
	_warning = new WarningMessage(224, 24, _icons->getX() + 48, _icons->getY() + 32);

	// Create soldier stats summary
	_txtName = new Text(120, 10, _icons->getX() + 135, _icons->getY() + 32);

	_numTimeUnits = new NumberText(15, 5, _icons->getX() + 136, _icons->getY() + 42);
	_barTimeUnits = new Bar(102, 3, _icons->getX() + 170, _icons->getY() + 41);

	_numEnergy = new NumberText(15, 5, _icons->getX() + 154, _icons->getY() + 42);
	_barEnergy = new Bar(102, 3, _icons->getX() + 170, _icons->getY() + 45);

	_numHealth = new NumberText(15, 5, _icons->getX() + 136, _icons->getY() + 50);
	_barHealth= new Bar(102, 3, _icons->getX() + 170, _icons->getY() + 49);

	_numMorale = new NumberText(15, 5, _icons->getX() + 154, _icons->getY() + 50);
	_barMorale = new Bar(102, 3, _icons->getX() + 170, _icons->getY() + 53);

	_txtDebug = new Text(300, 10, 20, 0);

	_reserve = _btnReserveNone;

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());

	// Last 16 colors are a greyish gradient
	SDL_Color color[] = {{140, 152, 148},
						 {132, 136, 140},
						 {116, 124, 132},
						 {108, 116, 124},
						 {92, 104, 108},
						 {84, 92, 100},
						 {76, 80, 92},
						 {56, 68, 84},
						 {48, 56, 68},
						 {40, 48, 56},
						 {32, 36, 48},
						 {24, 28, 32},
						 {16, 20, 24},
						 {8, 12, 16},
						 {3, 4, 8},
						 {3, 3, 6}};
	_game->setPalette(color, Palette::backPos+16, 16);

	// Fix system colors
	_game->getCursor()->setColor(Palette::blockOffset(9));
	_game->getFpsCounter()->setColor(Palette::blockOffset(9));

	add(_map);
	add(_icons);
	add(_numLayers);
	add(_rank);
	add(_btnUnitUp);
	add(_btnUnitDown);
	add(_btnMapUp);
	add(_btnMapDown);
	add(_btnShowMap);
	add(_btnKneel);
	add(_btnInventory);
	add(_btnCenter);
	add(_btnNextSoldier);
	add(_btnNextStop);
	add(_btnShowLayers);
	add(_btnHelp);
	add(_btnEndTurn);
	add(_btnAbort);
	add(_btnStats);
	add(_txtName);
	add(_numTimeUnits);
	add(_numEnergy);
	add(_numHealth);
	add(_numMorale);
	add(_barTimeUnits);
	add(_barEnergy);
	add(_barHealth);
	add(_barMorale);
	add(_btnReserveNone);
	add(_btnReserveSnap);
	add(_btnReserveAimed);
	add(_btnReserveAuto);
	add(_btnLeftHandItem);
	add(_numAmmoLeft);
	add(_btnRightHandItem);
	add(_numAmmoRight);
	for (int i = 0; i < 10; ++i)
	{
		add(_btnVisibleUnit[i]);
		add(_numVisibleUnit[i]);
	}
	add(_warning);

	add(_txtDebug);

	// Set up objects

	_battleGame = _game->getSavedGame()->getBattleGame();
	_map->init();
	_map->onMouseClick((ActionHandler)&BattlescapeState::mapClick);

	// there is some cropping going on here, because the icons image is 320x200 while we only need the bottom of it.
	Surface *s = _game->getResourcePack()->getSurface("ICONS.PCK");
	SDL_Rect *r = s->getCrop();
	r->x = 0;
	r->y = mapHeight - iconsHeight;
	r->w = iconsWidth;
	r->h = iconsHeight;
	s->blit(_icons);

	_numLayers->setColor(Palette::blockOffset(1)-2);
	_numLayers->setValue(1);

	_numAmmoLeft->setColor(2);
	_numAmmoLeft->setValue(999);

	_numAmmoRight->setColor(2);
	_numAmmoRight->setValue(999);

	_btnUnitUp->onMouseClick((ActionHandler)&BattlescapeState::btnUnitUpClick);
	_btnUnitDown->onMouseClick((ActionHandler)&BattlescapeState::btnUnitDownClick);
	_btnMapUp->onMouseClick((ActionHandler)&BattlescapeState::btnMapUpClick);
	_btnMapDown->onMouseClick((ActionHandler)&BattlescapeState::btnMapDownClick);
	_btnShowMap->onMouseClick((ActionHandler)&BattlescapeState::btnShowMapClick);
	_btnKneel->onMouseClick((ActionHandler)&BattlescapeState::btnKneelClick);
	_btnInventory->onMouseClick((ActionHandler)&BattlescapeState::btnInventoryClick);
	_btnCenter->onMouseClick((ActionHandler)&BattlescapeState::btnCenterClick);
	_btnNextSoldier->onMouseClick((ActionHandler)&BattlescapeState::btnNextSoldierClick);
	_btnNextStop->onMouseClick((ActionHandler)&BattlescapeState::btnNextStopClick);
	_btnShowLayers->onMouseClick((ActionHandler)&BattlescapeState::btnShowLayersClick);
	_btnHelp->onMouseClick((ActionHandler)&BattlescapeState::btnHelpClick);
	_btnEndTurn->onMouseClick((ActionHandler)&BattlescapeState::btnEndTurnClick);
	_btnAbort->onMouseClick((ActionHandler)&BattlescapeState::btnAbortClick);
	_btnStats->onMouseClick((ActionHandler)&BattlescapeState::btnStatsClick);
	_btnLeftHandItem->onMouseClick((ActionHandler)&BattlescapeState::btnLeftHandItemClick);
	_btnRightHandItem->onMouseClick((ActionHandler)&BattlescapeState::btnRightHandItemClick);
	_btnReserveNone->onMouseClick((ActionHandler)&BattlescapeState::btnReserveNoneClick);
	_btnReserveSnap->onMouseClick((ActionHandler)&BattlescapeState::btnReserveSnapClick);
	_btnReserveAimed->onMouseClick((ActionHandler)&BattlescapeState::btnReserveAimedClick);
	_btnReserveAuto->onMouseClick((ActionHandler)&BattlescapeState::btnReserveAutoClick);

	for (int i = 0; i < 10; ++i)
	{
		_btnVisibleUnit[i]->onMouseClick((ActionHandler)&BattlescapeState::btnVisibleUnitClick);
		_numVisibleUnit[i]->setColor(16);
		_numVisibleUnit[i]->setValue(i+1);
	}
	_warning->setColor(Palette::blockOffset(2));
	_warning->setTextColor(Palette::blockOffset(1));

	_txtName->setColor(Palette::blockOffset(8));
	_txtName->setHighContrast(true);
	_numTimeUnits->setColor(Palette::blockOffset(4));
	_numEnergy->setColor(Palette::blockOffset(1));
	_numHealth->setColor(Palette::blockOffset(2));
	_numMorale->setColor(Palette::blockOffset(12));
	_barTimeUnits->setColor(Palette::blockOffset(4));
	_barTimeUnits->setScale(1.0);
	_barEnergy->setColor(Palette::blockOffset(1));
	_barEnergy->setScale(1.0);
	_barHealth->setColor(Palette::blockOffset(2));
	_barHealth->setColor2(Palette::blockOffset(5)+2);
	_barHealth->setScale(1.0);
	_barMorale->setColor(Palette::blockOffset(12));
	_barMorale->setScale(1.0);

	_txtDebug->setColor(Palette::blockOffset(8));
	_txtDebug->setHighContrast(true);

	_btnReserveNone->copy(_icons);
	_btnReserveNone->setColor(Palette::blockOffset(4)+3);
	_btnReserveNone->setGroup(&_reserve);

	_btnReserveSnap->copy(_icons);
	_btnReserveSnap->setColor(Palette::blockOffset(2)+3);
	_btnReserveSnap->setGroup(&_reserve);

	_btnReserveAimed->copy(_icons);
	_btnReserveAimed->setColor(Palette::blockOffset(2)+3);
	_btnReserveAimed->setGroup(&_reserve);

	_btnReserveAuto->copy(_icons);
	_btnReserveAuto->setColor(Palette::blockOffset(2)+3);
	_btnReserveAuto->setGroup(&_reserve);

	// Set music
	_game->getResourcePack()->getMusic("GMTACTIC")->play();

	_stateTimer = new Timer(DEFAULT_ANIM_SPEED);
	_stateTimer->onTimer((StateHandler)&BattlescapeState::handleState);
	_stateTimer->start();

	_animTimer = new Timer(DEFAULT_ANIM_SPEED);
	_animTimer->onTimer((StateHandler)&BattlescapeState::animate);
	_animTimer->start();

	_action.type = BA_NONE;
	_action.TU = 0;
	_action.targeting = false;
	_tuReserved = BA_NONE;
	_debugPlay = false;
	_playerPanicHandled = true;

	updateSoldierInfo();
	_map->getCamera()->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
	setupCursor();
	checkForCasualties(0, 0, true);
}


/**
 * Delete battlescapestate.
 */
BattlescapeState::~BattlescapeState()
{
	delete _stateTimer;
	delete _animTimer;
}

/**
 * Init battlescapestate.
 */
void BattlescapeState::init()
{
	_map->focus();
	_map->cacheUnits();
	_map->draw();
	updateSoldierInfo();
	if (_battleGame->getSide() == FACTION_PLAYER)
		_playerPanicHandled = false;
}

/**
  * think
  */
void BattlescapeState::think()
{
	static bool popped = false;

	if (_popups.empty())
	{
		State::think();
		_stateTimer->think(this, 0);
		_animTimer->think(this, 0);
		if (popped)
		{
			handleNonTargetAction();
			setupCursor();
			popped = false;
		}
	}
	else
	{
		// Handle popups
		_game->pushState(*_popups.begin());
		_popups.erase(_popups.begin());
		popped = true;
		return;
	}

	// nothing is happening - see if we need some alien AI or units panicking or what have you
	if (_states.empty())
	{
		// it's a non player side (ALIENS or CIVILIANS)
		if (_battleGame->getSide() != FACTION_PLAYER)
		{
			if (!_debugPlay)
			{
				if (_battleGame->getSelectedUnit())
				{
					if (!handlePanickingUnit(_battleGame->getSelectedUnit()))
						handleAI(_battleGame->getSelectedUnit());
				}
			}
		}
		else
		{
			// it's a player side && we have not handled all panicking units
			if (!_playerPanicHandled)
			{
				_playerPanicHandled = handlePanickingPlayer();
			}

		}

	}

}


/**
 * Handles the processing of the AI states of a unit.
 * @param unit Pointer to an unit.
 */
void BattlescapeState::handleAI(BattleUnit *unit)
{
	BattleAIState *ai = unit->getCurrentAIState();
	if (!ai) return;

	AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(ai);
	PatrolBAIState *patrol = dynamic_cast<PatrolBAIState*>(ai);


	unit->think(&_action);
	if (_action.type == BA_WALK)
	{
		if (patrol)
			debug(L"Patrolling");
		else
			debug(L"Walking to target");
		_battleGame->getPathfinding()->calculate(_action.actor, _action.target);
		statePushBack(new UnitWalkBState(this, _action));
	}

	if (_action.type == BA_SNAPSHOT || _action.type == BA_AUTOSHOT)
	{
		debug(L"Firing 1");
		statePushBack(new ProjectileFlyBState(this, _action));
		// give the option to take cover or keep shooting
		unit->think(&_action);
		if (_action.type == BA_WALK)
		{
			debug(L"Take cover");
			_battleGame->getPathfinding()->calculate(_action.actor, _action.target);
			statePushBack(new UnitWalkBState(this, _action));
		}
		if (_action.type == BA_SNAPSHOT || _action.type == BA_AUTOSHOT)
		{
			debug(L"Firing 2");
			statePushBack(new ProjectileFlyBState(this, _action));
		}
	}

	if (_action.type == BA_NONE)
	{
		if (aggro != 0)
		{
			debug(L"Aggro lost");
			// we lost aggro
			unit->setAIState(new PatrolBAIState(_battleGame, unit, 0));
		}
		if (_battleGame->selectNextPlayerUnit(true) == 0)
		{
			if (!_battleGame->getDebugMode())
			{
				statePushBack(0); // end AI turn
			}
			else
			{
				_battleGame->selectNextPlayerUnit(false);
				_debugPlay = true;
			}
		}
		if (_battleGame->getSelectedUnit())
		{
			_map->getCamera()->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
		}
	}
}

/**
 * Processes any clicks on the map to
 * command units.
 * @param action Pointer to an action.
 */
void BattlescapeState::mapClick(Action *action)
{
	bool bPreviewed = Options::getBool("battlePreviewPath");

	// right-click aborts walking state
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if (_battleGame->getPathfinding()->removePreview() && bPreviewed)
			return;

		if (_states.empty())
		{
			if (_action.targeting)
			{
				_action.targeting = false;
				_action.type = BA_NONE;
				setupCursor();
				_game->getCursor()->setVisible(true);
				return;
			}
		}
		else
		{
			_states.front()->cancel();
			return;
		}
	}

	// don't handle mouseclicks below 140, because they are in the buttons area (it overlaps with map surface)
	int my = int(action->getAbsoluteYMouse());
	int mx = int(action->getAbsoluteXMouse());
	if ( my > _icons->getY() && my < _icons->getY()+_icons->getHeight() && mx > _icons->getX() && mx < _icons->getX()+_icons->getWidth()) return;


	// don't accept leftclicks if there is no cursor or there is an action busy
	if (_map->getCursorType() == CT_NONE || !_states.empty()) return;

	Position pos;
	_map->getSelectorPosition(&pos);

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (_action.targeting && _battleGame->getSelectedUnit())
		{
			//  -= fire weapon or throw =-
			if (_battleGame->getSelectedUnit()->getTimeUnits() < _action.TU && _battleGame->getSide() == FACTION_PLAYER)
			{
				_warning->showMessage(_game->getLanguage()->getString("STR_NOT_ENOUGH_TIME_UNITS"));
				return;
			}
			_action.target = pos;
			_map->setCursorType(CT_NONE);
			_game->getCursor()->setVisible(false);
			_states.push_back(new ProjectileFlyBState(this, _action));
			statePushFront(new UnitTurnBState(this, _action)); // first of all turn towards the target
		}
		else
		{
			BattleUnit *unit = _battleGame->selectUnit(pos);
			if (unit && !unit->isOut())
			{
			//  -= select unit =-
				if (unit->getFaction() == _battleGame->getSide())
				{
					_battleGame->setSelectedUnit(unit);
					updateSoldierInfo();
					_action.targeting = false;
					_action.type = BA_NONE;
					setupCursor();
				}
			}
			else if (playableUnitSelected())
			{
				if (_action.target != pos && bPreviewed)
					_battleGame->getPathfinding()->removePreview();
				_action.target = pos;
				_battleGame->getPathfinding()->calculate(_action.actor, _action.target);
				if (bPreviewed && !_battleGame->getPathfinding()->previewPath())
				{
					_battleGame->getPathfinding()->removePreview();
					bPreviewed = false;
				}

				if (!bPreviewed)
				{
					//  -= start walking =-
					_map->setCursorType(CT_NONE);
					_game->getCursor()->setVisible(false);
					if (_battleGame->getSelectedUnit()->isKneeled())
					{
						kneel(_battleGame->getSelectedUnit());
					}
					statePushBack(new UnitWalkBState(this, _action));
				}
			}
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT && playableUnitSelected())
	{
		//  -= turn to or open door =-
		_action.target = pos;
		statePushBack(new UnitTurnBState(this, _action));
	}

}

/**
 * Move unit up.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *action)
{
	if (playableUnitSelected() && _battleGame->getPathfinding()->validateUpDown(_battleGame->getSelectedUnit(), _battleGame->getSelectedUnit()->getPosition(), Pathfinding::DIR_UP))
	{
		_action.target = _battleGame->getSelectedUnit()->getPosition();
		_action.target.z++;
		_map->setCursorType(CT_NONE);
		_game->getCursor()->setVisible(false);
		if (_battleGame->getSelectedUnit()->isKneeled())
		{
			kneel(_battleGame->getSelectedUnit());
		}
		_battleGame->getPathfinding()->calculate(_action.actor, _action.target);
		statePushBack(new UnitWalkBState(this, _action));
	}
}

/**
 * Move unit down.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitDownClick(Action *action)
{
	if (playableUnitSelected() && _battleGame->getPathfinding()->validateUpDown(_battleGame->getSelectedUnit(), _battleGame->getSelectedUnit()->getPosition(), Pathfinding::DIR_DOWN))
	{
	//  -= start walking =-
		_action.target = _battleGame->getSelectedUnit()->getPosition();
		_action.target.z--;
		_map->setCursorType(CT_NONE);
		_game->getCursor()->setVisible(false);
		if (_battleGame->getSelectedUnit()->isKneeled())
		{
			kneel(_battleGame->getSelectedUnit());
		}
		_battleGame->getPathfinding()->calculate(_action.actor, _action.target);
		statePushBack(new UnitWalkBState(this, _action));
	}
}

/**
 * Show next map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapUpClick(Action *action)
{
	_map->getCamera()->up();
}

/**
 * Show previous map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapDownClick(Action *action)
{
	_map->getCamera()->down();
}

/**
 * Show minimap.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowMapClick(Action *action)
{
	//MiniMapState
	_game->pushState (new MiniMapState (_game, _map, _battleGame));
}

/**
 * Kneel/Standup.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnKneelClick(Action *action)
{
	BattleUnit *bu = _battleGame->getSelectedUnit();
	if (bu)
	{
		kneel(bu);
	}
}

/**
 * Kneel/Standup.
 * @param bu Pointer to a unit.
 */
void BattlescapeState::kneel(BattleUnit *bu)
{
	int tu = bu->isKneeled()?8:4;
	if (checkReservedTU(bu, tu))
	{
		if (bu->spendTimeUnits(tu, _battleGame->getDebugMode()))
		{
			bu->kneel(!bu->isKneeled());
			// kneeling or standing up can reveil new terrain or units. I guess.
			_battleGame->getTileEngine()->calculateFOV(bu);
			_map->cacheUnits();
			updateSoldierInfo();
			BattleAction action;
			if (_battleGame->getTileEngine()->checkReactionFire(bu, &action, 0, false))
			{
				statePushBack(new ProjectileFlyBState(this, action));
			}
		}
		else
		{
			_warning->showMessage(_game->getLanguage()->getString("STR_NOT_ENOUGH_TIME_UNITS"));
		}
	}
}
/**
 * Go to soldier info screen.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnInventoryClick(Action *action)
{
	if (playableUnitSelected() && _battleGame->getSelectedUnit()->getUnit()->getArmor()->getSize() == 1)
	{
		_game->pushState(new InventoryState(_game, !_battleGame->getDebugMode()));
	}
}

/**
 * Center on currently selected soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnCenterClick(Action *action)
{
	if (playableUnitSelected())
	{
		_map->getCamera()->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
	}
}

/**
 * Select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextSoldierClick(Action *action)
{
	selectNextPlayerUnit(false);
}

/**
 * Don't reselect current soldier and select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextStopClick(Action *action)
{
	selectNextPlayerUnit(true);
}

/**
 * Select next soldier.
 * @param checkReselect When true, don't reselect current unit.
 */
void BattlescapeState::selectNextPlayerUnit(bool checkReselect)
{
	BattleUnit *unit = _battleGame->selectNextPlayerUnit(checkReselect);
	updateSoldierInfo();
	if (unit) _map->getCamera()->centerOnPosition(unit->getPosition());
	_action.targeting = false;
	_action.type = BA_NONE;
	setupCursor();
}

/**
 * Show/hide all map layers.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowLayersClick(Action *action)
{

}

/**
 * Show options.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnHelpClick(Action *action)
{
	_game->pushState(new BattlescapeOptionsState(_game));
}

/**
 * End turn request. This will add a 0 to the end of the state queue,
 * so all ongoing actions, like explosions are finished first before really switching turn.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnEndTurnClick(Action *action)
{
	_action.targeting = false;
	_action.type = BA_NONE;
	statePushBack(0);
}

/**
 * End turn.
 */
void BattlescapeState::endTurn()
{
	Position p;

	_debugPlay = false;
	_action.type = BA_NONE;

	// check for hot grenades on the ground
	for (int i = 0; i < _battleGame->getWidth() * _battleGame->getLength() * _battleGame->getHeight(); ++i)
	{
		for (std::vector<BattleItem*>::iterator it = _battleGame->getTiles()[i]->getInventory()->begin(); it != _battleGame->getTiles()[i]->getInventory()->end(); )
		{
			if ((*it)->getRules()->getBattleType() == BT_GRENADE && (*it)->getExplodeTurn() > 0 && (*it)->getExplodeTurn() <= _battleGame->getTurn())  // it's a grenade to explode now
			{
				p.x = _battleGame->getTiles()[i]->getPosition().x*16 + 8;
				p.y = _battleGame->getTiles()[i]->getPosition().y*16 + 8;
				p.z = _battleGame->getTiles()[i]->getPosition().z*24 + _battleGame->getTiles()[i]->getTerrainLevel();
				statePushNext(new ExplosionBState(this, p, (*it), (*it)->getPreviousOwner()));
				it = _battleGame->getTiles()[i]->getInventory()->erase(it);
				statePushBack(0);
				return;
			}
			++it;
		}
	}

	// check for hot grenades in the hands (by default grenades don't explode in soldiers hands)
	if (Options::getBool("battleAltGrenade"))
	{
		for (std::vector<BattleUnit*>::iterator i = _battleGame->getUnits()->begin(); i != _battleGame->getUnits()->end(); ++i)
		{
			for (std::vector<BattleItem*>::iterator it = _battleGame->getItems()->begin(); it != _battleGame->getItems()->end(); )
			{
				if ((*it)->getOwner() == (*i)) // TODO refactor - removal of getOwner
				{
					if ((*it)->getRules()->getBattleType() == BT_GRENADE && (*it)->getExplodeTurn() > 0 && (*it)->getExplodeTurn() <= _battleGame->getTurn())  // it's a grenade to explode now
					{
							p.x = (*i)->getPosition().x*16 + 8;
							p.y = (*i)->getPosition().y*16 + 8;
							p.z = (*i)->getPosition().z*24 + 18;
							statePushNext(new ExplosionBState(this, p, (*it), (*it)->getPreviousOwner()));
							it = _battleGame->getItems()->erase(it);
							statePushBack(0);
							return;
					}
				}
				++it;
			}
		}
	}


	if (_battleGame->getTileEngine()->closeUfoDoors())
	{
		_game->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(21)->play(); // ufo door closed
	}

	_battleGame->endTurn();

	if (_battleGame->getSide() == FACTION_PLAYER)
	{
		setupCursor();
	}
	else
	{
		_map->setCursorType(CT_NONE);
	}

	// check for terrain explosions
	Tile *t = _battleGame->getTileEngine()->checkForTerrainExplosions();
	if (t)
	{
		Position p = Position(t->getPosition().x * 16, t->getPosition().y * 16, t->getPosition().z * 24);
		statePushNext(new ExplosionBState(this, p, 0, 0, t));
	}

	bool bBattleIsOver = checkForCasualties(0, 0);
	if (bBattleIsOver)
	{
		return;
	}

	updateSoldierInfo();

	if (playableUnitSelected())
	{
		_map->getCamera()->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
		setupCursor();
	}

	_game->pushState(new NextTurnState(_game, _battleGame, this));

}


/**
 * Checks for casualties and adjusts morale accordingly.
 * @param murderweapon
 * @param murderer
 * @return Whether the battle is finished.
 */
bool BattlescapeState::checkForCasualties(BattleItem *murderweapon, BattleUnit *murderer, bool noSound)
{
	for (std::vector<BattleUnit*>::iterator j = _battleGame->getUnits()->begin(); j != _battleGame->getUnits()->end(); ++j)
	{
		if ((*j)->getHealth() == 0 && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_FALLING)
		{
			BattleUnit *victim = (*j);

			if (murderer)
			{
				// if there is a known murderer, he will get a morale bonus if he is of a different faction (what with neutral?)
				if ((victim->getFaction() == FACTION_PLAYER && murderer->getFaction() == FACTION_HOSTILE) ||
					(victim->getFaction() == FACTION_HOSTILE && murderer->getFaction() == FACTION_PLAYER))
				{
					murderer->moraleChange(+20);
					Soldier *s = dynamic_cast<Soldier*>(murderer->getUnit());
					if (s != 0)
					{
						s->addKillCount();
					}
				}
				// murderer will get a penalty with friendly fire
				if (victim->getFaction() == murderer->getFaction())
				{
					murderer->moraleChange(-20);
				}
			}

			for (std::vector<BattleUnit*>::iterator i = _battleGame->getUnits()->begin(); i != _battleGame->getUnits()->end(); ++i)
			{
				// the losing squad all get a morale loss
				if ((*i)->getFaction() == victim->getFaction())
				{
					(*i)->moraleChange(-(22 - ((*i)->getUnit()->getBravery() / 10)*2));

					// revenge procedure:
					// if the victim is hostile, the nearest other hostile will aggro if he wasn't already
					if (victim->getFaction() == FACTION_HOSTILE && murderer)
					{
						int closest = 1000000;
						BattleUnit *revenger = 0;
						bool revenge = RNG::generate(0,100) < 50;
						for (std::vector<BattleUnit*>::iterator h = _battleGame->getUnits()->begin(); h != _battleGame->getUnits()->end(); ++h)
						{
							if ((*h)->getFaction() == FACTION_HOSTILE && !(*h)->isOut() && (*h) != victim)
							{
								int x = abs(victim->getPosition().x - (*h)->getPosition().x);
								int y = abs(victim->getPosition().y - (*h)->getPosition().y);
								int distance = int(floor(sqrt(float(x*x + y*y)) + 0.5));
								if (distance < closest)
								{
									revenger = (*h);
									closest = distance;
								}
							}
						}
						// aliens with aggression level 2 always revenge
						// aliens with aggression level 1 have 50% chance to revenge
						// aliens with aggression level 0 never revenge
						if (revenger && (revenger->getUnit()->getAggression() == 2 || (revenger->getUnit()->getAggression() == 1 && revenge)))
						{
							AggroBAIState *aggro = dynamic_cast<AggroBAIState*>(revenger->getCurrentAIState());
							if (aggro == 0)
							{
								aggro = new AggroBAIState(_battleGame, revenger);
								revenger->setAIState(aggro);
							}
							debug(L"Aggro Target : revenge");
							aggro->setAggroTarget(murderer);
						}
					}
				}
				// the winning squad all get a morale increase
				if ((*i)->getFaction() != victim->getFaction())
				{
					(*i)->moraleChange(+10);
				}
			}

			if (murderweapon)
			{
				statePushNext(new UnitDieBState(this, (*j), murderweapon->getRules()->getDamageType(), noSound));
			}
			else
			{
				if (noSound)
					statePushNext(new UnitDieBState(this, (*j), DT_HE, noSound)); // simulate instant death
				else
					statePushNext(new UnitDieBState(this, (*j), DT_AP, noSound)); // do a die animation
			}
		}
		else if ((*j)->getStunlevel() >= (*j)->getHealth() && (*j)->getStatus() != STATUS_DEAD && (*j)->getStatus() != STATUS_UNCONSCIOUS && (*j)->getStatus() != STATUS_FALLING)
		{
				statePushNext(new UnitDieBState(this, (*j), DT_STUN, noSound));
		}
	}

	return false;
}

/**
 * Abort game.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnAbortClick(Action *action)
{
	_game->pushState(new AbortMissionState(_game, _battleGame, this));
}

/**
 * Show selected soldier info.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnStatsClick(Action *action)
{
	if (playableUnitSelected())
	{
		popup(new UnitInfoState(_game, _battleGame->getSelectedUnit()));
	}
}

/**
 * Shows action popup menu. When clicked, create the action.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnLeftHandItemClick(Action *action)
{
	if (_action.type != BA_NONE) return;
	if (playableUnitSelected())
	{
		BattleItem *leftHandItem = _battleGame->getSelectedUnit()->getItem("STR_LEFT_HAND");
		handleItemClick(leftHandItem);
	}
}

/**
 * Shows action popup menu. When clicked, create the action.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnRightHandItemClick(Action *action)
{
	if (_action.type != BA_NONE) return;
	if (playableUnitSelected())
	{
		BattleItem *rightHandItem = _battleGame->getSelectedUnit()->getItem("STR_RIGHT_HAND");
		handleItemClick(rightHandItem);
	}
}

/**
 * Center on the unit corresponding with this button.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnVisibleUnitClick(Action *action)
{
	int btnID = -1;

	_states.clear();
	setupCursor();
	_game->getCursor()->setVisible(true);

	// got to find out which button was pressed
	for (int i = 0; i < 10 && btnID == -1; ++i)
	{
		if (action->getSender() == _btnVisibleUnit[i])
		{
			btnID = i;
		}
	}

	if (btnID != -1)
	{
		_map->getCamera()->centerOnPosition(_visibleUnit[btnID]->getPosition());
	}

	action->getDetails()->type = SDL_NOEVENT; // consume the event
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveNoneClick(Action *action)
{
	_tuReserved = BA_NONE;
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveSnapClick(Action *action)
{
	_tuReserved = BA_SNAPSHOT;
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveAimedClick(Action *action)
{
	_tuReserved = BA_AIMEDSHOT;
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveAutoClick(Action *action)
{
	_tuReserved = BA_AUTOSHOT;
}

/**
 * Handles non target actions, like priming a grenade.
 */
void BattlescapeState::handleNonTargetAction()
{
	if (!_action.targeting)
	{
		if (_action.type == BA_PRIME && _action.value > -1)
		{
			if (_action.actor->spendTimeUnits(_action.TU, dontSpendTUs()))
			{
				_action.weapon->setExplodeTurn(_game->getSavedGame()->getBattleGame()->getTurn() + _action.value);
			}
			else
			{
				_warning->showMessage(_game->getLanguage()->getString("STR_NOT_ENOUGH_TIME_UNITS"));
			}
		}
		if (_action.type == BA_USE)
		{
			if (_action.result.length() > 0)
			{
				_warning->showMessage(_game->getLanguage()->getString(_action.result));
				_action.result = "";
			}
			_battleGame->getTileEngine()->reviveUnconsciousUnits();
		}
		_action.type = BA_NONE;
		updateSoldierInfo();
	}
}

/**
 * Set the cursor according to the selected action.
 */
void BattlescapeState::setupCursor()
{
	if (_action.targeting)
	{
		if (_action.type == BA_THROW)
		{
			_map->setCursorType(CT_THROW);
		}
		else
		{
			_map->setCursorType(CT_AIM);
		}
	}
	else
	{
		_map->setCursorType(CT_NORMAL, _action.actor->getUnit()->getArmor()->getSize());
	}
}

/**
 * Whether a playable unit is selected
 * @return whether a playable unit is selected.
 */
bool BattlescapeState::playableUnitSelected()
{
	return _battleGame->getSelectedUnit() != 0 && (_battleGame->getSide() == FACTION_PLAYER || _battleGame->getDebugMode());
}

/**
 * Updates soldier name/rank/tu/energy/health/morale.
 */
void BattlescapeState::updateSoldierInfo()
{
	BattleUnit *battleUnit = _battleGame->getSelectedUnit();

	for (int i = 0; i < 10; ++i)
	{
		_btnVisibleUnit[i]->setVisible(false);
		_numVisibleUnit[i]->setVisible(false);
		_visibleUnit[i] = 0;
	}

	_rank->setVisible(playableUnitSelected());
	_numTimeUnits->setVisible(playableUnitSelected());
	_barTimeUnits->setVisible(playableUnitSelected());
	_barTimeUnits->setVisible(playableUnitSelected());
	_numEnergy->setVisible(playableUnitSelected());
	_barEnergy->setVisible(playableUnitSelected());
	_barEnergy->setVisible(playableUnitSelected());
	_numHealth->setVisible(playableUnitSelected());
	_barHealth->setVisible(playableUnitSelected());
	_barHealth->setVisible(playableUnitSelected());
	_numMorale->setVisible(playableUnitSelected());
	_barMorale->setVisible(playableUnitSelected());
	_barMorale->setVisible(playableUnitSelected());
	_btnLeftHandItem->setVisible(playableUnitSelected());
	_btnRightHandItem->setVisible(playableUnitSelected());
	_numAmmoLeft->setVisible(playableUnitSelected());
	_numAmmoRight->setVisible(playableUnitSelected());
	if (!playableUnitSelected())
	{
		_txtName->setText(L"");
		return;
	}

	_action.actor = battleUnit;

	_txtName->setText(battleUnit->getUnit()->getName(_game->getLanguage()));
	Soldier *soldier = dynamic_cast<Soldier*>(battleUnit->getUnit());
	if (soldier != 0)
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
		texture->getFrame(soldier->getRankSprite())->blit(_rank);
	}
	_numTimeUnits->setValue(battleUnit->getTimeUnits());
	_barTimeUnits->setMax(battleUnit->getUnit()->getTimeUnits());
	_barTimeUnits->setValue(battleUnit->getTimeUnits());
	_numEnergy->setValue(battleUnit->getEnergy());
	_barEnergy->setMax(battleUnit->getUnit()->getStamina());
	_barEnergy->setValue(battleUnit->getEnergy());
	_numHealth->setValue(battleUnit->getHealth());
	_barHealth->setMax(battleUnit->getUnit()->getHealth());
	_barHealth->setValue(battleUnit->getHealth());
	_barHealth->setValue2(battleUnit->getStunlevel());
	_numMorale->setValue(battleUnit->getMorale());
	_barMorale->setMax(100);
	_barMorale->setValue(battleUnit->getMorale());

	BattleItem *leftHandItem = battleUnit->getItem("STR_LEFT_HAND");
	_btnLeftHandItem->clear();
	_numAmmoLeft->setVisible(false);
	if (leftHandItem)
	{
		leftHandItem->getRules()->drawHandSprite(_game->getResourcePack()->getSurfaceSet("BIGOBS.PCK"), _btnLeftHandItem);
		if (leftHandItem->getRules()->getBattleType() == BT_FIREARM && leftHandItem->needsAmmo())
		{
			_numAmmoLeft->setVisible(true);
			if (leftHandItem->getAmmoItem())
				_numAmmoLeft->setValue(leftHandItem->getAmmoItem()->getAmmoQuantity());
			else
				_numAmmoLeft->setValue(0);
		}
	}
	BattleItem *rightHandItem = battleUnit->getItem("STR_RIGHT_HAND");
	_btnRightHandItem->clear();
	_numAmmoRight->setVisible(false);
	if (rightHandItem)
	{
		rightHandItem->getRules()->drawHandSprite(_game->getResourcePack()->getSurfaceSet("BIGOBS.PCK"), _btnRightHandItem);
		if (rightHandItem->getRules()->getBattleType() == BT_FIREARM && rightHandItem->needsAmmo())
		{
			_numAmmoRight->setVisible(true);
			if (rightHandItem->getAmmoItem())
				_numAmmoRight->setValue(rightHandItem->getAmmoItem()->getAmmoQuantity());
			else
				_numAmmoRight->setValue(0);
		}
	}

	_battleGame->getTileEngine()->calculateFOV(_battleGame->getSelectedUnit());
	int j = 0;
	for (std::vector<BattleUnit*>::iterator i = battleUnit->getVisibleUnits()->begin(); i != battleUnit->getVisibleUnits()->end(); ++i)
	{
		_btnVisibleUnit[j]->setVisible(true);
		_numVisibleUnit[j]->setVisible(true);
		_visibleUnit[j] = (*i);
		++j;
	}
}

/**
  * Shift the red colors of the visible unit buttons backgrounds.
  */
void BattlescapeState::blinkVisibleUnitButtons()
{
	static int delta = 1, color = 32;

	SDL_Rect square1;
	square1.x = 0;
	square1.y = 0;
	square1.w = 15;
	square1.h = 12;
	SDL_Rect square2;
	square2.x = 1;
	square2.y = 1;
	square2.w = 13;
	square2.h = 10;

	for (int i = 0; i < 10;  ++i)
	{
		if (_btnVisibleUnit[i]->getVisible() == true)
		{
			_btnVisibleUnit[i]->drawRect(&square1, 15);
			_btnVisibleUnit[i]->drawRect(&square2, color);
		}
	}

	if (color == 44) delta = -2;
	if (color == 32) delta = 1;

	color += delta;
}

/*
 * This function popups a context sensitive list of actions the user can choose from.
 * Some actions result in a change of gamestate.
 * @param item Item the user clicked on (righthand/lefthand)
 */
void BattlescapeState::handleItemClick(BattleItem *item)
{
	// make sure there is an item, and the battlescape is in an idle state
	if (item && _states.empty())
	{
		BattleUnit *bu = _battleGame->getSelectedUnit();
		_action.actor = bu;
		_action.weapon = item;
		popup(new ActionMenuState(_game, &_action, _icons->getX(), _icons->getY()+20));
	}
}

/**
 * Give time slice to the front state.
 */
void BattlescapeState::handleState()
{
	if (!_states.empty())
	{
		_states.front()->think();
		_map->draw(); // redraw map
	}
}

/**
 * Animate map objects on the map, also smoke,fire,....
 */
void BattlescapeState::animate()
{
	_map->animate(_states.empty());

	blinkVisibleUnitButtons();
}

/**
 * Get pointer to the game. Some states need this info.
 * @return Pointer to game.
 */
Game *BattlescapeState::getGame() const
{
	return _game;
}

/**
 * Get pointer to the map. Some states need this info.
 * @return Pointer to map.
 */
Map *BattlescapeState::getMap() const
{
	return _map;
}

/**
 * Push a state at the front of the queue and start it.
 * @param bs Battlestate.
 */
void BattlescapeState::statePushFront(BattleState *bs)
{
	_states.push_front(bs);
	bs->init();
}

/**
 * Push a state as the next state after the current one.
 * @param bs Battlestate.
 */
void BattlescapeState::statePushNext(BattleState *bs)
{
	if (_states.empty())
	{
		_states.push_front(bs);
		bs->init();
	}
	else
	{
		_states.insert(++_states.begin(), bs);
	}

}

/**
 * Push a state at the back.
 * @param bs Battlestate.
 */
void BattlescapeState::statePushBack(BattleState *bs)
{
	if (_states.empty())
	{
		_states.push_front(bs);
		// end turn request?
		if (_states.front() == 0)
		{
			_states.pop_front();
			endTurn();
			return;
		}
		else
		{
			bs->init();
		}
	}
	else
	{
		_states.push_back(bs);
	}
}

/**
 * Pop the current state. Handle errors and mouse cursor appearing again.
 * States pop themselves when they are finished.
 */
void BattlescapeState::popState()
{
	bool actionFailed = false;

	if (_states.empty()) return;

	if (_states.front()->getResult().length() > 0 && _battleGame->getSide() == FACTION_PLAYER && !dontSpendTUs())
	{
		_warning->showMessage(_game->getLanguage()->getString(_states.front()->getResult()));
		actionFailed = true;
	}
	_states.pop_front();

	if (_states.empty())
	{
		if (_action.targeting && _battleGame->getSelectedUnit() && !actionFailed)
		{
			_action.actor->spendTimeUnits(_action.TU, dontSpendTUs());
		}
		// after throwing the cursor returns, otherwise it stays in targeting mode
		if (_action.type == BA_THROW && !actionFailed)
		{
			_action.targeting = false;
			_action.type = BA_NONE;
		}
		_game->getCursor()->setVisible(true);
		setupCursor();
		// it's the alien side, select next unit
		if (_battleGame->getSide() != FACTION_PLAYER && !_debugPlay)
		{
			if (_battleGame->selectNextPlayerUnit(true) == 0)
			{
				if (!_battleGame->getDebugMode())
				{
					statePushBack(0); // end AI turn
				}
				else
				{
					_battleGame->selectNextPlayerUnit(false);
					_debugPlay = true;
				}
			}
			if (_battleGame->getSelectedUnit())
			{
				_map->getCamera()->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
			}
		}
	}
	else
	{
		// end turn request?
		if (_states.front() == 0)
		{
			_states.pop_front();
			endTurn();
			return;
		}
		// init the next state in queue
		_states.front()->init();
	}
	if (_battleGame->getSelectedUnit() == 0 || _battleGame->getSelectedUnit()->isOut())
	{
		_action.targeting = false;
		_action.type = BA_NONE;
		_map->setCursorType(CT_NORMAL, _action.actor->getUnit()->getArmor()->getSize());
		_game->getCursor()->setVisible(true);
		_battleGame->setSelectedUnit(0);
	}
	updateSoldierInfo();
}

/**
 * Sets the timer interval for think() calls of the state.
 * @param interval An interval in ms.
 */
void BattlescapeState::setStateInterval(Uint32 interval)
{
	_stateTimer->setInterval(interval);
}

/**
 * Show a debug message in the topleft corner.
 * @param message Debug message.
 */
void BattlescapeState::debug(const std::wstring message)
{
	if (_battleGame->getDebugMode())
	{
		_txtDebug->setText(message);
	}
}

/**
 * Takes care of any events from the core game engine.
 * @param action Pointer to an action.
 */
void BattlescapeState::handle(Action *action)
{
	if (_game->getCursor()->getVisible() || action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		State::handle(action);

		if (action->getDetails()->type == SDL_KEYDOWN)
		{
#ifdef _DEBUG
			// "d" - enable debug mode
			if (action->getDetails()->key.keysym.sym == SDLK_d)
			{
				_battleGame->setDebugMode();
				debug(L"Debug Mode");
			}
#endif
		}
	}

}

/**
 * Adds a new popup window to the queue
 * (this prevents popups from overlapping)
 * @param state Pointer to popup state.
 */
void BattlescapeState::popup(State *state)
{
	_popups.push_back(state);
}

/**
 * Check against reserved time units.
 * @param bu Pointer to the unit.
 * @param tu Number of time units to check.
 * @return bool Whether or not we got enough time units.
 */
bool BattlescapeState::checkReservedTU(BattleUnit *bu, int tu)
{
	if (dontSpendTUs() || _battleGame->getSide() != FACTION_PLAYER) return true; // aliens don't reserve TUs

	if (_tuReserved != BA_NONE &&
		tu + bu->getActionTUs(_tuReserved, bu->getMainHandWeapon()) > bu->getTimeUnits())
	{
		if (_battleGame->getSide() == FACTION_PLAYER)
		{
			switch (_tuReserved)
			{
			case BA_SNAPSHOT: _warning->showMessage(_game->getLanguage()->getString("STR_TIME_UNITS_RESERVED_FOR_SNAP_SHOT")); break;
			case BA_AUTOSHOT: _warning->showMessage(_game->getLanguage()->getString("STR_TIME_UNITS_RESERVED_FOR_AUTO_SHOT")); break;
			case BA_AIMEDSHOT: _warning->showMessage(_game->getLanguage()->getString("STR_TIME_UNITS_RESERVED_FOR_AIMED_SHOT")); break;
			default: ;
			}
		}
		return false;
	}

	return true;
}

/**
 * Finishes up the current battle, shuts down the battlescape
 * and presents the debriefing the screen for the mission.
 * @param abort Was the mission aborted?
 */
void BattlescapeState::finishBattle(bool abort)
{
	_game->popState();
	_battleGame->setAborted(abort);
	_game->pushState(new DebriefingState(_game));
	_game->getCursor()->setColor(Palette::blockOffset(15)+12);
	_game->getFpsCounter()->setColor(Palette::blockOffset(15)+12);
}

/**
 * Drop item to the floor & affect with gravity.
 * @param position Position to spawn the item.
 * @param item Pointer to the item.
 * @param newItem Bool whether this is a new item.
 */
void BattlescapeState::dropItem(const Position &position, BattleItem *item, bool newItem)
{
	Position p = position;

	// don't spawn anything outside of bounds
	if (_battleGame->getTile(p) == 0)
		return;

	while (_battleGame->getTile(p)->getMapData(MapData::O_FLOOR) == 0 && p.z > 0)
	{
		p.z--;
	}

	_battleGame->getTile(p)->addItem(item);

	if(newItem)
	{
		_battleGame->getItems()->push_back(item);
	}

	item->setSlot(_game->getRuleset()->getInventory("STR_GROUND"));
	item->setOwner(0);

	if (item->getRules()->getBattleType() == BT_FLARE)
	{
		_battleGame->getTileEngine()->calculateTerrainLighting();
	}

}

/**
 * Pick the first soldier from the list in status panic.
 * @return True when all panicking is over.
 */
bool BattlescapeState::handlePanickingPlayer()
{
	for (std::vector<BattleUnit*>::iterator j = _battleGame->getUnits()->begin(); j != _battleGame->getUnits()->end(); ++j)
	{
		if (handlePanickingUnit(*j))
			return false;
	}
	return true;
}

/**
 * Cmmon function for panicking units.
 * @return False when unit not in panicking mode.
 */
bool BattlescapeState::handlePanickingUnit(BattleUnit *unit)
{
	UnitStatus status = unit->getStatus();
	if (status != STATUS_PANICKING && status != STATUS_BERSERK) return false;
	unit->setVisible(true);
	_map->getCamera()->centerOnPosition(unit->getPosition());

	std::wstringstream ss;
	ss << unit->getUnit()->getName(_game->getLanguage()) << L'\n' << _game->getLanguage()->getString(status==STATUS_PANICKING?"STR_HAS_PANICKED":"STR_HAS_GONE_BERSERK");
	_game->pushState(new InfoboxState(_game, ss.str()));

	unit->abortTurn(); //makes the unit go to status STANDING :p

	int flee = RNG::generate(0,100);
	switch (status)
	{
	case STATUS_PANICKING: // 1/2 chance to freeze and 1/2 chance try to flee
		if (flee <= 50)
		{
			BattleItem *item = unit->getItem("STR_RIGHT_HAND");
			if (item)
			{
				dropItem(unit->getPosition(), item);
				item->moveToOwner(0);
			}
			item = unit->getItem("STR_LEFT_HAND");
			if (item)
			{
				dropItem(unit->getPosition(), item);
				item->moveToOwner(0);
			}
			unit->setCache(0);
			_action.actor = unit;
			_action.target = Position(unit->getPosition().x + RNG::generate(-5,5), unit->getPosition().y + RNG::generate(-5,5), unit->getPosition().z);
			_battleGame->getPathfinding()->calculate(_action.actor, _action.target);
			statePushBack(new UnitWalkBState(this, _action));
		}
		break;
	case STATUS_BERSERK: // berserk - do some weird turning around and then aggro towards an enemy unit or shoot towards random place
		for (int i= 0; i < 4; i++)
		{
			_action.actor = unit;
			_action.target = Position(unit->getPosition().x + RNG::generate(-5,5), unit->getPosition().y + RNG::generate(-5,5), unit->getPosition().z);
			statePushBack(new UnitTurnBState(this, _action));
		}
		for (std::vector<BattleUnit*>::iterator j = unit->getVisibleUnits()->begin(); j != unit->getVisibleUnits()->end(); ++j)
		{
			_action.target = (*j)->getPosition();
			statePushBack(new UnitTurnBState(this, _action));
		}
		_action.type = BA_SNAPSHOT;
		_action.weapon = unit->getMainHandWeapon();
		for (int i= 0; i < 10; i++)
		{
			statePushBack(new ProjectileFlyBState(this, _action));
		}
		_action.type = BA_NONE;
		break;
	}
	unit->setTimeUnits(0);
	unit->moraleChange(+15);

	return true;
}

/**
 * TUs are not spent when handling panicking mode or in debug mode.
 * @return Whether TUs are spent or not.
 */
bool BattlescapeState::dontSpendTUs()
{
	if (_battleGame->getDebugMode())
		return true;
	if (!_playerPanicHandled)
		return true;

	return false;
}

}
