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
#include "BattlescapeGame.h"
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
#include "../Ruleset/Armor.h"
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
	_btnLaunch = new InteractiveSurface(32, 24, game->getScreen()->getWidth() / game->getScreen()->getXScale() - 32, 0);
	_btnLaunch->setVisible(false);

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
	add(_btnLaunch);
	_game->getResourcePack()->getSurfaceSet("SPICONS.DAT")->getFrame(0)->blit(_btnLaunch);

	// Set up objects

	_save = _game->getSavedGame()->getBattleGame();
	_map->init();
	_map->onMouseOver((ActionHandler)&BattlescapeState::mapOver);
	_map->onMousePress((ActionHandler)&BattlescapeState::mapPress);
	_map->onMouseClick((ActionHandler)&BattlescapeState::mapClick, 0);

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
	_btnLaunch->onMouseClick((ActionHandler)&BattlescapeState::btnLaunchClick);

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

	_animTimer = new Timer(DEFAULT_ANIM_SPEED);
	_animTimer->onTimer((StateHandler)&BattlescapeState::animate);
	_animTimer->start();

	_gameTimer = new Timer(DEFAULT_ANIM_SPEED);
	_gameTimer->onTimer((StateHandler)&BattlescapeState::handleState);
	_gameTimer->start();

	_battleGame = new BattlescapeGame(_save, this);

	firstInit = true;

	isMouseScrolling = false;
	isMouseScrolled = false;
}


/**
 * Delete battlescapestate.
 */
BattlescapeState::~BattlescapeState()
{
	delete _animTimer;
	delete _gameTimer;
	delete _battleGame;
}

/**
 * Init battlescapestate.
 */
void BattlescapeState::init()
{
	_map->focus();
	_map->cacheUnits();
	_map->draw();
	_battleGame->init();
	updateSoldierInfo();
	if (firstInit && playableUnitSelected())
	{
		_battleGame->setupCursor();
		_map->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
		firstInit = false;
	}
}

/**
  * think
  */
void BattlescapeState::think()
{
	static bool popped = false;

	if (_gameTimer->isRunning())
	{
		if (_popups.empty())
		{
			State::think();
			_battleGame->think();
			_animTimer->think(this, 0);
			_gameTimer->think(this, 0);
			if (popped)
			{
				_battleGame->handleNonTargetAction();
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
	}
}

/**
 * Processes any mouse moving over the map
 * @param action Pointer to an action.
 */
void BattlescapeState::mapOver(Action *action)
{
	if (isMouseScrolling && action->getDetails()->type == SDL_MOUSEMOTION)
	{
		isMouseScrolled = true;

		// Set the mouse cursor back
		SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse(xBeforeMouseScrolling, yBeforeMouseScrolling);
		SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
		
		// Check the threshold
		if (!mouseMovedOverThreshold)
		{
			absMouseMoveX += action->getDetails()->motion.xrel;
			absMouseMoveY += action->getDetails()->motion.yrel;
			mouseMovedOverThreshold = ((std::abs(absMouseMoveX) > _save->getScrollButtonPixelTolerancy()) || (std::abs(absMouseMoveY) > _save->getScrollButtonPixelTolerancy()));
		}

		// Scrolling
		_map->getCamera()->scrollXY(-action->getDetails()->motion.xrel * _save->getScrollButtonInvertMode(), -action->getDetails()->motion.yrel * _save->getScrollButtonInvertMode(), false);
		
		// We don't want to look the mouse-cursor jumping :)
		action->getDetails()->motion.x=xBeforeMouseScrolling; action->getDetails()->motion.y=yBeforeMouseScrolling;
		_game->getCursor()->handle(action);
	}
}

/**
 * Processes any presses on the map
 * @param action Pointer to an action.
 */
void BattlescapeState::mapPress(Action *action)
{
	// don't handle mouseclicks below 140, because they are in the buttons area (it overlaps with map surface)
	int my = int(action->getAbsoluteYMouse());
	int mx = int(action->getAbsoluteXMouse());
	if ( my > _icons->getY() && my < _icons->getY()+_icons->getHeight() && mx > _icons->getX() && mx < _icons->getX()+_icons->getWidth()) return;

	if (-1 != _save->getScrollButton())
	{
		if (action->getDetails()->button.button == _save->getScrollButton())
		{
			isMouseScrolling = true;
			isMouseScrolled = false;
			SDL_GetMouseState(&xBeforeMouseScrolling, &yBeforeMouseScrolling);
			mapOffsetBeforeMouseScrolling = _map->getCamera()->getMapOffset();
			absMouseMoveX = 0; absMouseMoveY = 0;
			mouseMovedOverThreshold = false;
			mouseScrollingStartTime = SDL_GetTicks();
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
	// Right-button release: release mouse-scroll-mode
	if (isMouseScrolling)
	{
		// While scrolling, other buttons are ineffective
		if (action->getDetails()->button.button == _save->getScrollButton()) isMouseScrolling = false; else return;
		// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
		if ((!mouseMovedOverThreshold) && (SDL_GetTicks() - mouseScrollingStartTime <= _save->getScrollButtonTimeTolerancy()))
		{
			isMouseScrolled = false;
			_map->getCamera()->setMapOffset(mapOffsetBeforeMouseScrolling);
		}
		if (isMouseScrolled) return;
	}

	// right-click aborts walking state
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if (_battleGame->cancelCurrentAction())
		{
			return;
		}
	}

	// don't handle mouseclicks below 140, because they are in the buttons area (it overlaps with map surface)
	int my = int(action->getAbsoluteYMouse());
	int mx = int(action->getAbsoluteXMouse());
	if ( my > _icons->getY() && my < _icons->getY()+_icons->getHeight() && mx > _icons->getX() && mx < _icons->getX()+_icons->getWidth()) return;


	// don't accept leftclicks if there is no cursor or there is an action busy
	if (_map->getCursorType() == CT_NONE || _battleGame->isBusy()) return;

	Position pos;
	_map->getSelectorPosition(&pos);

	if (_save->getTile(pos) != 0) // don't allow to click into void
	{
		if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
		{
			_battleGame->primaryAction(pos);
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT && playableUnitSelected())
		{
			_battleGame->secondaryAction(pos);
		}
	}
}

/**
 * Move unit up.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *action)
{
	if (playableUnitSelected() && _save->getPathfinding()->validateUpDown(_save->getSelectedUnit(), _save->getSelectedUnit()->getPosition(), Pathfinding::DIR_UP))
	{
		_battleGame->moveUpDown(_save->getSelectedUnit(), Pathfinding::DIR_UP);
	}
}

/**
 * Move unit down.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitDownClick(Action *action)
{
	if (playableUnitSelected() && _save->getPathfinding()->validateUpDown(_save->getSelectedUnit(), _save->getSelectedUnit()->getPosition(), Pathfinding::DIR_DOWN))
	{
		_battleGame->moveUpDown(_save->getSelectedUnit(), Pathfinding::DIR_DOWN);
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
	_game->pushState (new MiniMapState (_game, _map->getCamera(), _save));
}

/**
 * Kneel/Standup.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnKneelClick(Action *action)
{
	BattleUnit *bu = _save->getSelectedUnit();
	if (bu)
	{
		_battleGame->kneel(bu);
	}
}

/**
 * Go to soldier info screen.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnInventoryClick(Action *action)
{
	if (playableUnitSelected() && _save->getSelectedUnit()->getArmor()->getSize() == 1)
	{
		_game->pushState(new InventoryState(_game, !_save->getDebugMode()));
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
		_map->getCamera()->centerOnPosition(_save->getSelectedUnit()->getPosition());
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
	if (_battleGame->getCurrentAction()->type != BA_NONE) return;
	BattleUnit *unit = _save->selectNextPlayerUnit(checkReselect);
	updateSoldierInfo();
	if (unit) _map->getCamera()->centerOnPosition(unit->getPosition());
	_battleGame->cancelCurrentAction();
	_battleGame->getCurrentAction()->actor = unit;
	_battleGame->setupCursor();
}

/**
 * Show/hide all map layers.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowLayersClick(Action *action)
{
	_numLayers->setValue(_map->getCamera()->toggleShowAllLayers());
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
	_battleGame->requestEndTurn();
}
/**
 * Abort game.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnAbortClick(Action *action)
{
	_game->pushState(new AbortMissionState(_game, _save, this));
}

/**
 * Show selected soldier info.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnStatsClick(Action *action)
{
	if (playableUnitSelected())
	{
		popup(new UnitInfoState(_game, _save->getSelectedUnit()));
	}
}

/**
 * Shows action popup menu. When clicked, create the action.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnLeftHandItemClick(Action *action)
{
	if (_battleGame->getCurrentAction()->type != BA_NONE) return;
	if (playableUnitSelected())
	{
		_save->getSelectedUnit()->setActiveHand("STR_LEFT_HAND");
		_map->cacheUnits();
		_map->draw();
		BattleItem *leftHandItem = _save->getSelectedUnit()->getItem("STR_LEFT_HAND");
		handleItemClick(leftHandItem);
	}
}

/**
 * Shows action popup menu. When clicked, create the action.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnRightHandItemClick(Action *action)
{
	if (_battleGame->getCurrentAction()->type != BA_NONE) return;
	if (playableUnitSelected())
	{
		_save->getSelectedUnit()->setActiveHand("STR_RIGHT_HAND");
		_map->cacheUnits();
		_map->draw();
		BattleItem *rightHandItem = _save->getSelectedUnit()->getItem("STR_RIGHT_HAND");
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
 * Launch the blaster bomb.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnLaunchClick(Action *action)
{
	_battleGame->launchAction();
	action->getDetails()->type = SDL_NOEVENT; // consume the event
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveNoneClick(Action *action)
{
	_battleGame->setTUReserved(BA_NONE);
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveSnapClick(Action *action)
{
	_battleGame->setTUReserved(BA_SNAPSHOT);
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveAimedClick(Action *action)
{
	_battleGame->setTUReserved(BA_AIMEDSHOT);
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveAutoClick(Action *action)
{
	_battleGame->setTUReserved(BA_AUTOSHOT);
}

/**
 * Whether a playable unit is selected. Normally only player side units can be selected, but in debug mode one can play with aliens too :)
 * Is used to see if stats can be displayed and action buttons will work.
 * @return whether a playable unit is selected.
 */
bool BattlescapeState::playableUnitSelected()
{
	return _save->getSelectedUnit() != 0 && (_save->getSide() == FACTION_PLAYER || _save->getDebugMode());
}

/**
 * Updates soldier name/rank/tu/energy/health/morale.
 */
void BattlescapeState::updateSoldierInfo()
{
	BattleUnit *battleUnit = _save->getSelectedUnit();

	for (int i = 0; i < 10; ++i)
	{
		_btnVisibleUnit[i]->setVisible(false);
		_numVisibleUnit[i]->setVisible(false);
		_visibleUnit[i] = 0;
	}

	bool playableUnit = playableUnitSelected();
	_rank->setVisible(playableUnit);
	_numTimeUnits->setVisible(playableUnit);
	_barTimeUnits->setVisible(playableUnit);
	_barTimeUnits->setVisible(playableUnit);
	_numEnergy->setVisible(playableUnit);
	_barEnergy->setVisible(playableUnit);
	_barEnergy->setVisible(playableUnit);
	_numHealth->setVisible(playableUnit);
	_barHealth->setVisible(playableUnit);
	_barHealth->setVisible(playableUnit);
	_numMorale->setVisible(playableUnit);
	_barMorale->setVisible(playableUnit);
	_barMorale->setVisible(playableUnit);
	_btnLeftHandItem->setVisible(playableUnit);
	_btnRightHandItem->setVisible(playableUnit);
	_numAmmoLeft->setVisible(playableUnit);
	_numAmmoRight->setVisible(playableUnit);
	if (!playableUnit)
	{
		_txtName->setText(L"");
		return;
	}

	_txtName->setText(battleUnit->getName(_game->getLanguage()));
	Soldier *soldier = _game->getSavedGame()->getSoldier(battleUnit->getId());
	if (soldier != 0)
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
		texture->getFrame(soldier->getRankSprite())->blit(_rank);
	}
	else
	{
		_rank->clear();
	}
	_numTimeUnits->setValue(battleUnit->getTimeUnits());
	_barTimeUnits->setMax(battleUnit->getStats()->tu);
	_barTimeUnits->setValue(battleUnit->getTimeUnits());
	_numEnergy->setValue(battleUnit->getEnergy());
	_barEnergy->setMax(battleUnit->getStats()->stamina);
	_barEnergy->setValue(battleUnit->getEnergy());
	_numHealth->setValue(battleUnit->getHealth());
	_barHealth->setMax(battleUnit->getStats()->health);
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

	_save->getTileEngine()->calculateFOV(_save->getSelectedUnit());
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
	if (item && !_battleGame->isBusy())
	{
		if (_game->getSavedGame()->isResearched(item->getRules()->getRequirements()))
		{
			_battleGame->getCurrentAction()->weapon = item;
			popup(new ActionMenuState(_game, _battleGame->getCurrentAction(), _icons->getX(), _icons->getY()+16));
		}
		else
		{
			warning("STR_UNABLE_TO_USE_ALIEN_ARTIFACT_UNTIL_RESEARCHED");
		}
	}
}

/**
 * Animate map objects on the map, also smoke,fire,....
 */
void BattlescapeState::animate()
{
	_map->animate(!_battleGame->isBusy());

	blinkVisibleUnitButtons();
}

/**
 * handles the battle game state
 */
void BattlescapeState::handleState()
{
	_battleGame->handleState();
}

/**
 * Sets the timer interval for think() calls of the state.
 * @param interval An interval in ms.
 */
void BattlescapeState::setStateInterval(Uint32 interval)
{
	_gameTimer->setInterval(interval);
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
 * Show a debug message in the topleft corner.
 * @param message Debug message.
 */
void BattlescapeState::debug(const std::wstring &message)
{
	if (_save->getDebugMode())
	{
		_txtDebug->setText(message);
	}
}

void BattlescapeState::warning(const std::string &message)
{
	_warning->showMessage(_game->getLanguage()->getString(message));
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
			// "d" - enable debug mode
			if (Options::getBool("debug") && action->getDetails()->key.keysym.sym == SDLK_d)
			{
				_save->setDebugMode();
				debug(L"Debug Mode");
			}
			// "l" - toggle personal lighting
			if (action->getDetails()->key.keysym.sym == SDLK_l)
			{
				_save->getTileEngine()->togglePersonalLighting();
			}
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
 * Finishes up the current battle, shuts down the battlescape
 * and presents the debriefing the screen for the mission.
 * @param abort Was the mission aborted?
 */
void BattlescapeState::finishBattle(bool abort)
{
	_popups.clear();
	_animTimer->stop();
	_gameTimer->stop();
	_save->setAborted(abort);
	_game->popState();
	_game->pushState(new DebriefingState(_game));
	_game->getCursor()->setColor(Palette::blockOffset(15)+12);
	_game->getFpsCounter()->setColor(Palette::blockOffset(15)+12);
}

/**
 * Show launch button.
 * @param show Show launch button?
 */
void BattlescapeState::showLaunchButton(bool show)
{
	_btnLaunch->setVisible(show);
}

/**
 * Clears mouse-scrolling state (isMouseScrolling)
 */
void BattlescapeState::clearMouseScrollingState()
{
	isMouseScrolling = false;
}


}
