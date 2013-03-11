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
#include <iomanip>
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
#include "../Engine/Options.h"
#include "../Engine/Music.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Screen.h"
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
#include "BattlescapeGenerator.h"
#include "BriefingState.h"
#include "../Geoscape/DefeatState.h"
#include "../Geoscape/VictoryState.h"
#include "../lodepng.h"
#include "../Engine/Logger.h"
#include "../Engine/CrossPlatform.h"

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
		_btnVisibleUnit[i] = new InteractiveSurface(15, 12, _icons->getX() + iconsWidth - 20, _icons->getY() - 16 - (i * 13));
		_numVisibleUnit[i] = new NumberText(15, 12, _icons->getX() + iconsWidth - 14 , _icons->getY() - 12 - (i * 13));
	}
	_numVisibleUnit[9]->setX(304); // center number 10
	_warning = new WarningMessage(224, 24, _icons->getX() + 48, _icons->getY() + 32);
	_btnLaunch = new InteractiveSurface(32, 24, game->getScreen()->getWidth() / game->getScreen()->getXScale() - 32, 0);
	_btnLaunch->setVisible(false);
	_btnPsi = new InteractiveSurface(32, 24, game->getScreen()->getWidth() / game->getScreen()->getXScale() - 32, 25);
	_btnPsi->setVisible(false);

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
	add(_btnPsi);
	_game->getResourcePack()->getSurfaceSet("SPICONS.DAT")->getFrame(1)->blit(_btnPsi);

	// Set up objects

	_save = _game->getSavedGame()->getBattleGame();
	_map->init();
	_map->onMouseOver((ActionHandler)&BattlescapeState::mapOver);
	_map->onMousePress((ActionHandler)&BattlescapeState::mapPress);
	_map->onMouseClick((ActionHandler)&BattlescapeState::mapClick, 0);
	_map->onMouseIn((ActionHandler)&BattlescapeState::mapIn);

	// there is some cropping going on here, because the icons image is 320x200 while we only need the bottom of it.
	Surface *s = _game->getResourcePack()->getSurface("ICONS.PCK");
	SDL_Rect *r = s->getCrop();
	r->x = 0;
	r->y = 200 - iconsHeight;
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
	_btnPsi->onMouseClick((ActionHandler)&BattlescapeState::btnPsiClick);

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
	_game->getResourcePack()->getRandomMusic("GMTACTIC")->play();

	_animTimer = new Timer(DEFAULT_ANIM_SPEED, true);
	_animTimer->onTimer((StateHandler)&BattlescapeState::animate);
	_animTimer->start();

	_gameTimer = new Timer(DEFAULT_ANIM_SPEED, true);
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

		if (_save->isDragInverted())
		{
			// Set the mouse cursor back
			SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouse(xBeforeMouseScrolling, yBeforeMouseScrolling);
			SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
		}
		// Check the threshold
		totalMouseMoveX += action->getDetails()->motion.xrel;
		totalMouseMoveY += action->getDetails()->motion.yrel;
		if (!mouseMovedOverThreshold)
			mouseMovedOverThreshold = ((std::abs(totalMouseMoveX) > _save->getDragPixelTolerance()) || (std::abs(totalMouseMoveY) > _save->getDragPixelTolerance()));

		// Scrolling
		if (_save->isDragInverted())
		{
			_map->getCamera()->scrollXY(
				-action->getDetails()->motion.xrel,
				-action->getDetails()->motion.yrel, false);

			// We don't want to look the mouse-cursor jumping :)
			action->getDetails()->motion.x=xBeforeMouseScrolling; action->getDetails()->motion.y=yBeforeMouseScrolling;
			_game->getCursor()->handle(action);
		}
		else
		{
			_map->getCamera()->setMapOffset(mapOffsetBeforeMouseScrolling);
			if (_map->getCamera()->scrollXY(
				(int)((double)totalMouseMoveX / action->getXScale()),
				(int)((double)totalMouseMoveY / action->getYScale()), false))
			{
				lastSucTotalMouseMoveX = totalMouseMoveX;
				lastSucTotalMouseMoveY = totalMouseMoveY;
			}
			else
				_map->getCamera()->scrollXY(
					(int)((double)lastSucTotalMouseMoveX / action->getXScale()),
					(int)((double)lastSucTotalMouseMoveY / action->getYScale()), false);
		}
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

	if (Options::getInt("battleScrollType") == SCROLL_DRAG)
	{
		if (action->getDetails()->button.button == _save->getDragButton())
		{
			isMouseScrolling = true;
			isMouseScrolled = false;
			SDL_GetMouseState(&xBeforeMouseScrolling, &yBeforeMouseScrolling);
			mapOffsetBeforeMouseScrolling = _map->getCamera()->getMapOffset();
			totalMouseMoveX = 0; totalMouseMoveY = 0;
			lastSucTotalMouseMoveX = 0; lastSucTotalMouseMoveY = 0;
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
		if (action->getDetails()->button.button == _save->getDragButton()) isMouseScrolling = false; else return;
		// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
		if ((!mouseMovedOverThreshold) && (SDL_GetTicks() - mouseScrollingStartTime <= ((Uint32)_save->getDragTimeTolerance())))
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

	if (_save->getDebugMode())
	{
		std::wstringstream ss;
		ss << L"Clicked " << pos.x << " "<< pos.y << " "<< pos.z;
		debug(ss.str());
	}

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
 * Processes when mouse enters to the map surface
 * @param action Pointer to an action.
 */
void BattlescapeState::mapIn(Action *)
{
	isMouseScrolling = false;
	_map->setButtonsPressed(SDL_BUTTON_RIGHT, false);
}

/**
 * Move unit up.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *)
{
	if (playableUnitSelected() && _save->getPathfinding()->validateUpDown(_save->getSelectedUnit(), _save->getSelectedUnit()->getPosition(), Pathfinding::DIR_UP))
	{
		_battleGame->cancelCurrentAction();
		_battleGame->moveUpDown(_save->getSelectedUnit(), Pathfinding::DIR_UP);
	}
}

/**
 * Move unit down.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitDownClick(Action *)
{
	if (playableUnitSelected() && _save->getPathfinding()->validateUpDown(_save->getSelectedUnit(), _save->getSelectedUnit()->getPosition(), Pathfinding::DIR_DOWN))
	{
		_battleGame->cancelCurrentAction();
		_battleGame->moveUpDown(_save->getSelectedUnit(), Pathfinding::DIR_DOWN);
	}
}

/**
 * Show next map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapUpClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_map->getCamera()->up();
}

/**
 * Show previous map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapDownClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_map->getCamera()->down();
}

/**
 * Show minimap.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowMapClick(Action *)
{
	//MiniMapState
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_game->pushState (new MiniMapState (_game, _map->getCamera(), _save));
}

/**
 * Kneel/Standup.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnKneelClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
	{
		BattleUnit *bu = _save->getSelectedUnit();
		if (bu)
		{
			_battleGame->kneel(bu);
		}
	}
}

/**
 * Go to soldier info screen.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnInventoryClick(Action *)
{
	if (playableUnitSelected() && (_save->getSelectedUnit()->getArmor()->getSize() == 1 || _save->getDebugMode()))
	{
		_game->pushState(new InventoryState(_game, !_save->getDebugMode(), this));
	}
}

/**
 * Center on currently selected soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnCenterClick(Action *)
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
void BattlescapeState::btnNextSoldierClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		selectNextPlayerUnit(true, false);
}

/**
 * Don't reselect current soldier and select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextStopClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		selectNextPlayerUnit(true, true);
}

/**
 * Select next soldier.
 * @param checkReselect When true, don't select a unit that has been previously flagged.
 * @param setReselect When true, flag the current unit first.
 */
void BattlescapeState::selectNextPlayerUnit(bool checkReselect, bool setReselect)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
	{
		if (_battleGame->getCurrentAction()->type != BA_NONE) return;
		BattleUnit *unit = _save->selectNextPlayerUnit(checkReselect, setReselect);
		updateSoldierInfo();
		if (unit) _map->getCamera()->centerOnPosition(unit->getPosition());
		_battleGame->cancelCurrentAction();
		_battleGame->getCurrentAction()->actor = unit;
		_battleGame->setupCursor();
	}
}

/**
 * Select previous soldier.
 * @param checkReselect When true, don't select a unit that has been previously flagged.
 */
void BattlescapeState::selectPreviousPlayerUnit(bool checkReselect)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
	{
		if (_battleGame->getCurrentAction()->type != BA_NONE) return;
		BattleUnit *unit = _save->selectPreviousPlayerUnit(checkReselect);
		updateSoldierInfo();
		if (unit) _map->getCamera()->centerOnPosition(unit->getPosition());
		_battleGame->cancelCurrentAction();
		_battleGame->getCurrentAction()->actor = unit;
		_battleGame->setupCursor();
	}
}
/**
 * Show/hide all map layers.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowLayersClick(Action *)
{	
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_numLayers->setValue(_map->getCamera()->toggleShowAllLayers());
}

/**
 * Show options.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnHelpClick(Action *)
{
		_game->pushState(new BattlescapeOptionsState(_game));
}

/**
 * End turn request. This will add a 0 to the end of the state queue,
 * so all ongoing actions, like explosions are finished first before really switching turn.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnEndTurnClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_battleGame->requestEndTurn();
}
/**
 * Abort game.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnAbortClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_game->pushState(new AbortMissionState(_game, _save, this));
}

/**
 * Show selected soldier info.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnStatsClick(Action *)
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
void BattlescapeState::btnLeftHandItemClick(Action *)
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
void BattlescapeState::btnRightHandItemClick(Action *)
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
 * Use psionics.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnPsiClick(Action *action)
{
	_battleGame->psiAction();
	action->getDetails()->type = SDL_NOEVENT; // consume the event
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveNoneClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_battleGame->setTUReserved(BA_NONE);
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveSnapClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_battleGame->setTUReserved(BA_SNAPSHOT);
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveAimedClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
		_battleGame->setTUReserved(BA_AIMEDSHOT);
}

/**
 * Reserve time units.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnReserveAutoClick(Action *)
{
	if(_save->getSide() == FACTION_PLAYER || _save->getDebugMode())
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

	_txtName->setText(battleUnit->getName(_game->getLanguage(), false));
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

	showPsiButton(battleUnit->getOriginalFaction() == FACTION_HOSTILE && battleUnit->getStats()->psiSkill > 0);
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
		if (_game->getSavedGame()->isResearched(item->getRules()->getRequirements()) || _save->getSelectedUnit()->getOriginalFaction() == FACTION_HOSTILE)
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

		if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
		{
			if (action->getDetails()->button.button == SDL_BUTTON_X1)
			{
				selectNextPlayerUnit(true, false);
			}
			else if (action->getDetails()->button.button == SDL_BUTTON_X2)
			{
				selectPreviousPlayerUnit(true);
			}
		}

		if (action->getDetails()->type == SDL_KEYDOWN)
		{
			// "d" - enable debug mode
			if (Options::getBool("debug") && action->getDetails()->key.keysym.sym == SDLK_d)
			{
				_save->setDebugMode();
				debug(L"Debug Mode");
			}
			// toggle personal lighting
			else if (action->getDetails()->key.keysym.sym == Options::getInt("keyBattlePersonalLighting"))
			{
				_save->getTileEngine()->togglePersonalLighting();
			}
			// next solider
			else if (action->getDetails()->key.keysym.sym == Options::getInt("keyBattleNextUnit"))
			{
				selectNextPlayerUnit(true, false);
			}
			// previous solider
			else if (action->getDetails()->key.keysym.sym == Options::getInt("keyBattlePrevUnit"))
			{
				selectPreviousPlayerUnit(true);
			}
			// options menu
			else if (action->getDetails()->key.keysym.sym == Options::getInt("keyBattleOptions"))
			{
				_game->pushState(new BattlescapeOptionsState(_game));
			}
			// reload
			else if (action->getDetails()->key.keysym.sym == Options::getInt("keyBattleReload") && playableUnitSelected())
			{
				if (_save->getSelectedUnit()->checkAmmo())
				{
					_game->getResourcePack()->getSound("BATTLE.CAT", 17)->play();
					updateSoldierInfo();
				}
			}
			// voxel map dump
			else if (action->getDetails()->key.keysym.sym == SDLK_F11)
			{
				if (_save->getDebugMode())
				{
					SaveVoxelMap();
				}
			}
			else if (action->getDetails()->key.keysym.sym == SDLK_F10)
			{
				SaveVoxelView();
			}
		}
	}

}

void BattlescapeState::SaveVoxelView()
{
	static const unsigned char pal[30]=
	//			ground		west wall	north wall		object		enem unit						xcom unit	neutr unit
	{0,0,0, 224,224,224,  192,224,255,  255,224,192, 128,255,128, 192,0,255,  0,0,0, 255,255,255,  224,192,0,  255,64,128 };

	BattleUnit * bu = _save->getSelectedUnit();
	if (bu==0) return; //no unit selected
	Position viewPos = _save->getSelectedUnit()->getPosition();
	std::vector<Position> _trajectory;

	double ang_x,ang_y;
	bool black;
	Tile *tile;
	std::stringstream ss;
	std::vector<unsigned char> image;
	int test;
	Position originVoxel = Position(viewPos.x*16+8,
		viewPos.y*16+8,
		viewPos.z*24 -_save->getTile(viewPos)->getTerrainLevel() + (bu->getFloatHeight() + bu->getHeight()-1) );
	if (bu->getArmor()->getSize() > 1)
	{
		originVoxel.x += 8;
		originVoxel.y += 8;
		originVoxel.z += 1; //topmost voxel
	}

	Position targetVoxel,hitPos;
	double dist;
	bool _debug = _save->getDebugMode();
	double dir = ((float)bu->getDirection()+4)/4*M_PI;
	image.clear();
	for (int y = -256+32; y < 256+32; ++y)
	{
		ang_y = (((double)y)/640*M_PI+M_PI/2);
		for (int x = -256; x < 256; ++x)
		{
			ang_x = ((double)x/1024)*M_PI+dir;

			targetVoxel.x=originVoxel.x + (int)(-sin(ang_x)*1024*sin(ang_y));
			targetVoxel.y=originVoxel.y + (int)(cos(ang_x)*1024*sin(ang_y));
			targetVoxel.z=originVoxel.z + (int)(cos(ang_y)*1024);

			_trajectory.clear();
			test = _save->getTileEngine()->calculateLine(originVoxel, targetVoxel, false, &_trajectory, bu, true, !_debug) +1;
			black = true;
			if (test!=0 && test!=6)
			{
				tile = _save->getTile(Position(_trajectory.at(0).x/16, _trajectory.at(0).y/16, _trajectory.at(0).z/24));
				if (_debug
					|| (tile->isDiscovered(0) && test == 2)
					|| (tile->isDiscovered(1) && test == 3)
					|| (tile->isDiscovered(2) && (test == 1 || test == 4))
					|| test==5
					)
				{
					if (test==5)
					{
						if (tile->getUnit())
						{
							if (tile->getUnit()->getFaction()==FACTION_NEUTRAL) test=9;
							else
							if (tile->getUnit()->getFaction()==FACTION_PLAYER) test=8;
						}
						else
						{
							tile = _save->getTile(Position(_trajectory.at(0).x/16, _trajectory.at(0).y/16, _trajectory.at(0).z/24-1));
							if (tile && tile->getUnit())
							{
								if (tile->getUnit()->getFaction()==FACTION_NEUTRAL) test=9;
								else
								if (tile->getUnit()->getFaction()==FACTION_PLAYER) test=8;
							}
						}
					}
					hitPos = Position(_trajectory.at(0).x, _trajectory.at(0).y, _trajectory.at(0).z);
					dist = sqrt((double)((hitPos.x-originVoxel.x)*(hitPos.x-originVoxel.x)
						+ (hitPos.y-originVoxel.y)*(hitPos.y-originVoxel.y)
						+ (hitPos.z-originVoxel.z)*(hitPos.z-originVoxel.z)) );
					black = false;
				}
			}

			if (black)
			{
				dist = 0;
			}
			else
			{
				if (dist>1000) dist=1000;
				if (dist<1) dist=1;
				dist=(1000-(log(dist))*140)/700;//140

				if (hitPos.x%16==15)
				{
					dist*=0.9;
				}
				if (hitPos.y%16==15)
				{
					dist*=0.9;
				}
				if (hitPos.z%24==23)
				{
					dist*=0.9;
				}
				if (dist > 1) dist = 1;
				if (tile) dist *= (16 - (float)tile->getShade())/16; 
			}
			
			image.push_back((int)((float)(pal[test*3+0])*dist));
			image.push_back((int)((float)(pal[test*3+1])*dist));
			image.push_back((int)((float)(pal[test*3+2])*dist));
		}
	}

	int i = 0;
	do
	{
		ss.str("");
		ss << Options::getUserFolder() << "fpslook" << std::setfill('0') << std::setw(3) << i << ".png";
		i++;
	}
	while (CrossPlatform::fileExists(ss.str()));


	unsigned error = lodepng::encode(ss.str(), image, 512, 512, LCT_RGB);
	if (error)
	{
		Log(LOG_ERROR) << "Saving to PNG failed: " << lodepng_error_text(error);
	}

	return;
}


void BattlescapeState::SaveVoxelMap()
{
	std::stringstream ss;
	std::vector<unsigned char> image;
	static const unsigned char pal[30]=
	{255,255,255, 224,224,224,  128,160,255,  255,160,128, 128,255,128, 192,0,255,  255,255,255, 255,255,255,  224,192,0,  255,64,128 };

	Tile *tile;

	for (int z = 0; z < _save->getMapSizeZ()*12; ++z)
	{
		image.clear();

		for (int y = 0; y < _save->getMapSizeY()*16; ++y)
		{
			for (int x = 0; x < _save->getMapSizeX()*16; ++x)
			{
				int test = _save->getTileEngine()->voxelCheck(Position(x,y,z*2),0,0) +1;
				float dist=1;
				if (x%16==15)
				{
					dist*=0.9;
				}
				if (y%16==15)
				{
					dist*=0.9;
				}

				if (test==5)
				{
					tile = _save->getTile(Position(x/16, y/16, z/12));
					if (tile->getUnit())
					{
						if (tile->getUnit()->getFaction()==FACTION_NEUTRAL) test=9;
						else
						if (tile->getUnit()->getFaction()==FACTION_PLAYER) test=8;
					}
					else
					{
						tile = _save->getTile(Position(x/16, y/16, z/12-1));
						if (tile && tile->getUnit())
						{
							if (tile->getUnit()->getFaction()==FACTION_NEUTRAL) test=9;
							else
							if (tile->getUnit()->getFaction()==FACTION_PLAYER) test=8;
						}
					}
				}

				image.push_back((int)((float)pal[test*3+0]*dist));
				image.push_back((int)((float)pal[test*3+1]*dist));
				image.push_back((int)((float)pal[test*3+2]*dist));
			}
		}

		ss.str("");
		ss << Options::getUserFolder() << "voxel" << std::setfill('0') << std::setw(2) << z << ".png";

		unsigned error = lodepng::encode(ss.str(), image, _save->getMapSizeX()*16, _save->getMapSizeY()*16, LCT_RGB);
		if (error)
		{
			Log(LOG_ERROR) << "Saving to PNG failed: " << lodepng_error_text(error);
		}
	}
	return;
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
 * @param number of soldiers in exit area OR number of survivors when battle finished due to either all aliens or objective was destroyed
 */
void BattlescapeState::finishBattle(bool abort, int inExitArea)
{
	if (_save->getNextStage() != "" && inExitArea)
	{
		// if there is a next mission stage + we have people in exit area OR we killed all aliens, load the next stage
		_popups.clear();
		_save->setMissionType(_save->getNextStage());
		BattlescapeGenerator bgen = BattlescapeGenerator(_game);
		bgen.setAlienRace("STR_MIXED");
		bgen.nextStage();
		_game->popState();
		_game->pushState(new BriefingState(_game, 0, 0));
	}
	else
	{
		_popups.clear();
		_animTimer->stop();
		_gameTimer->stop();
		_game->popState();
		if (abort || (!abort  && inExitArea == 0))
		{
			// abort was done or no player is still alive
			// this concludes to defeat when in mars or mars landing mission
			if ((_save->getMissionType() == "STR_MARS_THE_FINAL_ASSAULT" || _save->getMissionType() == "STR_MARS_CYDONIA_LANDING") && _game->getSavedGame()->getMonthsPassed() > -1)
			{
				_game->pushState (new DefeatState(_game));
			}
			else
			{
				_game->pushState(new DebriefingState(_game));
			}
		}
		else
		{
			// no abort was done and at least a player is still alive
			// this concludes to victory when in mars mission
			if (_save->getMissionType() == "STR_MARS_THE_FINAL_ASSAULT" && _game->getSavedGame()->getMonthsPassed() > -1)
			{
				_game->pushState (new VictoryState(_game));
			}
			else
			{
				_game->pushState(new DebriefingState(_game));
			}
		}
		_game->getCursor()->setColor(Palette::blockOffset(15)+12);
		_game->getFpsCounter()->setColor(Palette::blockOffset(15)+12);
	}
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
 * Show PSI button.
 * @param show Show PSI button?
 */
void BattlescapeState::showPsiButton(bool show)
{
	_btnPsi->setVisible(show);
}

/**
 * Clears mouse-scrolling state (isMouseScrolling)
 */
void BattlescapeState::clearMouseScrollingState()
{
	isMouseScrolling = false;
}


}
