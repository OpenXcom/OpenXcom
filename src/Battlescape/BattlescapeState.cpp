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

#include <sstream>
#include <cmath>
#include "Map.h"
#include "BattlescapeState.h"
#include "NextTurnState.h"
#include "BattleState.h"
#include "UnitTurnBState.h"
#include "UnitWalkBState.h"
#include "ProjectileFlyBState.h"
#include "ExplosionBState.h"
#include "TerrainModifier.h"
#include "ActionMenuState.h"
#include "UnitInfoState.h"
#include "UnitFallBState.h"
#include "InventoryState.h"
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
#include "../Engine/Timer.h"
#include "../Interface/FpsCounter.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Battlescape screen.
 * @param game Pointer to the core game.
 */
BattlescapeState::BattlescapeState(Game *game) : State(game), _popups()
{
	// Create the battlemap view
	_map = new Map(320, 200, 0, 0);

	// Create buttonbar
	_icons = new Surface(320, 200, 0, 0);
	_numLayers = new NumberText(3, 5, 232, 150);
	_rank = new Surface(26,23,107,177);

	// Create buttons
	_btnUnitUp = new InteractiveSurface(32, 16, 48, 144);
	_btnUnitDown = new InteractiveSurface(32, 16, 48, 160);
	_btnMapUp = new InteractiveSurface(32, 16, 80, 144);
	_btnMapDown = new InteractiveSurface(32, 16, 80, 160);
	_btnShowMap = new InteractiveSurface(32, 16, 112, 144);
	_btnKneel = new InteractiveSurface(32, 16, 112, 160);
	_btnInventory = new InteractiveSurface(32, 16, 144, 144);
	_btnCenter = new InteractiveSurface(32, 16, 144, 160);
	_btnNextSoldier = new InteractiveSurface(32, 16, 176, 144);
	_btnNextStop = new InteractiveSurface(32, 16, 176, 160);
	_btnShowLayers = new InteractiveSurface(32, 16, 208, 144);
	_btnHelp = new InteractiveSurface(32, 16, 208, 160);
	_btnEndTurn = new InteractiveSurface(32, 16, 240, 144);
	_btnAbort = new InteractiveSurface(32, 16, 240, 160);

	_btnStats = new InteractiveSurface(166, 24, 106, 176);
	_btnReserveNone = new ImageButton(28, 11, 49, 177);
	_btnReserveSnap = new ImageButton(28, 11, 78, 177);
	_btnReserveAimed = new ImageButton(28, 11, 49, 189);
	_btnReserveAuto = new ImageButton(28, 11, 78, 189);
	_btnLeftHandItem = new InteractiveSurface(32, 48, 8, 149);
	_numAmmoLeft = new NumberText(30, 5, 8, 149);
	_btnRightHandItem = new InteractiveSurface(32, 48, 280, 149);
	_numAmmoRight = new NumberText(30, 5, 280, 149);
	for (int i = 0; i < 10; ++i)
	{
		_btnVisibleUnit[i] = new InteractiveSurface(15, 12, 300, 128 - (i * 13));
		_numVisibleUnit[i] = new NumberText(15, 12, 306, 132 - (i * 13));
	}
	_numVisibleUnit[9]->setX(304); // center number 10
	_warningMessageBackground = new Surface(224, 24, 48, 176);
	_txtWarningMessage = new Text(224, 24, 48, 184);

	// Create soldier stats summary
	_txtName = new Text(120, 10, 135, 176);

	_numTimeUnits = new NumberText(15, 5, 136, 186);
	_barTimeUnits = new Bar(102, 3, 170, 185);

	_numEnergy = new NumberText(15, 5, 154, 186);
	_barEnergy = new Bar(102, 3, 170, 189);

	_numHealth = new NumberText(15, 5, 136, 194);
	_barHealth= new Bar(102, 3, 170, 193);

	_numMorale = new NumberText(15, 5, 154, 194);
	_barMorale = new Bar(102, 3, 170, 197);

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
						 {0, 4, 8},
						 {0, 0, 0}};
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
	add(_warningMessageBackground);
	add(_txtWarningMessage);

	add(_txtDebug);

	// Set up objects
	_game->getResourcePack()->getSurface("ICONS.PCK")->blit(_icons);

	_battleGame = _game->getSavedGame()->getBattleGame();
	_map->setResourcePack(_game->getResourcePack());
	_map->setSavedGame(_battleGame, _game);
	_map->init();
	_map->onMouseClick((ActionHandler)&BattlescapeState::mapClick);

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

	for (int i = 0; i < 10; ++i)
	{
		_btnVisibleUnit[i]->onMouseClick((ActionHandler)&BattlescapeState::btnVisibleUnitClick);
		_numVisibleUnit[i]->setColor(16);
		_numVisibleUnit[i]->setValue(i+1);
	}
	_txtWarningMessage->setColor(Palette::blockOffset(1)-1);
	_txtWarningMessage->setHighContrast(true);
	_txtWarningMessage->setAlign(ALIGN_CENTER);
	_warningMessageBackground->setVisible(false);

	_txtName->setColor(Palette::blockOffset(8)-1);
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
	_barHealth->setScale(1.0);
	_barMorale->setColor(Palette::blockOffset(12));
	_barMorale->setScale(1.0);

	_txtDebug->setColor(Palette::blockOffset(8));
	_txtDebug->setHighContrast(true);

	updateSoldierInfo(_battleGame->getSelectedUnit());
	_map->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());

	_btnReserveNone->copy(_icons);
	_btnReserveNone->setColor(Palette::blockOffset(4)+6);
	_btnReserveNone->setGroup(&_reserve);

	_btnReserveSnap->copy(_icons);
	_btnReserveSnap->setColor(Palette::blockOffset(2)+6);
	_btnReserveSnap->setGroup(&_reserve);

	_btnReserveAimed->copy(_icons);
	_btnReserveAimed->setColor(Palette::blockOffset(2)+6);
	_btnReserveAimed->setGroup(&_reserve);

	_btnReserveAuto->copy(_icons);
	_btnReserveAuto->setColor(Palette::blockOffset(2)+6);
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
}

/**
 *
 */
BattlescapeState::~BattlescapeState()
{
	delete _stateTimer;
	delete _animTimer;
}

void BattlescapeState::init()
{
	_map->focus();
	_map->cacheUnits();
	_map->draw(true);
	updateSoldierInfo(_battleGame->getSelectedUnit());
}

/**
  * think
  */
void BattlescapeState::think()
{
	static bool popped = false;

	if (_popups.empty())
	{
		_stateTimer->think(this, 0);
		_animTimer->think(this, 0);
		_map->think();
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
	}
}

/**
 * Processes any clicks on the map to
 * command units.
 * @param action Pointer to an action.
 */
void BattlescapeState::mapClick(Action *action)
{
	// right-click aborts walking state
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
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
	if (action->getYMouse() / action->getYScale() > BUTTONS_AREA) return;

	// don't accept leftclicks if there is no cursor or there is an action busy
	if (_map->getCursorType() == CT_NONE || !_states.empty()) return;

	Position pos;
	_map->getSelectorPosition(&pos);

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (_action.targeting && _battleGame->getSelectedUnit())
		{
			//  -= fire weapon or throw =-
			if (_battleGame->getSelectedUnit()->getTimeUnits() < _action.TU)
			{
				showWarningMessage("STR_NOT_ENOUGH_TIME_UNITS");
				return;
			}
			_action.target = pos;
			_map->setCursorType(CT_NONE);
			_game->getCursor()->setVisible(false);
			_states.push_back(new ProjectileFlyBState(this, _action));
			if (_action.type == BA_AUTOSHOT) // TODO - this is not very good, an autoshot gets 3 reaction shots this way
			{
				_states.push_back(new ProjectileFlyBState(this, _action));
				_states.push_back(new ProjectileFlyBState(this, _action));
			}
			statePushFront(new UnitTurnBState(this));
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
					updateSoldierInfo(unit);
				}
			}
			else if (_battleGame->getSelectedUnit())
			{
			//  -= start walking =-
				_action.target = pos;
				_map->setCursorType(CT_NONE);
				_game->getCursor()->setVisible(false);
				statePushBack(new UnitWalkBState(this));
			}
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT && _battleGame->getSelectedUnit())
	{
		//  -= turn to or open door =-
		_action.target = pos;
		statePushBack(new UnitTurnBState(this));
	}

}

/**
 * Move unit up.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitUpClick(Action *action)
{
	/*Pathfinding *pf = _battleGame->getPathfinding();
	Position start = _battleGame->getSelectedUnit()->getPosition();
	Position end = start + Position(0, 0, +1);
	pf->calculate(_battleGame->getSelectedUnit(), end);*/
}

/**
 * Move unit down.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnUnitDownClick(Action *action)
{
	/*Pathfinding *pf = _battleGame->getPathfinding();
	Position start = _battleGame->getSelectedUnit()->getPosition();
	Position end = start + Position(0, 0, -1);
	pf->calculate((BattleUnit*)_battleGame->getSelectedUnit(), end);*/
}

/**
 * Show next map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapUpClick(Action *action)
{
	_map->up();
}

/**
 * Show previous map layer.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnMapDownClick(Action *action)
{
	_map->down();
}

/**
 * Show minimap.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnShowMapClick(Action *action)
{

}

/**
 * Kneel/Standup.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnKneelClick(Action *action)
{
	// TODO: check for timeunits... check for FOV...
	BattleUnit *bu = _battleGame->getSelectedUnit();
	if (bu)
	{
		if (bu->spendTimeUnits(bu->isKneeled()?8:4, _battleGame->getDebugMode()))
		{
			bu->kneel(!bu->isKneeled());
			_map->cacheUnits();
			updateSoldierInfo(bu);
			BattleAction action;
			if (_battleGame->getTerrainModifier()->checkReactionFire(bu, &action))
			{
				statePushBack(new ProjectileFlyBState(this, action));
			}
		}
	}
}

/**
 * Go to soldier info screen.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnInventoryClick(Action *action)
{
	if (_battleGame->getSelectedUnit())
	{
		_game->pushState(new InventoryState(_game));
	}
}

/**
 * Center on currently selected soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnCenterClick(Action *action)
{
	if (_battleGame->getSelectedUnit())
	{
		_map->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
	}
}

/**
 * Select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextSoldierClick(Action *action)
{
	BattleUnit *unit = _battleGame->selectNextPlayerUnit();
	updateSoldierInfo(unit);
	if (unit) _map->centerOnPosition(unit->getPosition());
}

/**
 * Don't select current soldier and select next soldier.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnNextStopClick(Action *action)
{

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

}

/**
 * End turn request. This will add a 0 to the end of the state queue,
 * so all ongoing actions, like explosions are finished first before really switching turn.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnEndTurnClick(Action *action)
{
	statePushBack(0);
}

/**
 * End turn.
 */
void BattlescapeState::endTurn()
{
	Position p;

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
	if (ALT_GRENADE)
	{
		for (std::vector<BattleUnit*>::iterator i = _battleGame->getUnits()->begin(); i != _battleGame->getUnits()->end(); ++i)
		{
			for (std::vector<BattleItem*>::iterator it = _battleGame->getItems()->begin(); it != _battleGame->getItems()->end(); )
			{
				if ((*it)->getOwner() == (*i))
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


	if (_battleGame->getTerrainModifier()->closeUfoDoors())
	{
		_game->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(21)->play(); // ufo door closed
	}

	_battleGame->endTurn();
	checkForCasualties(0, 0);

	updateSoldierInfo(_battleGame->getSelectedUnit());
	if (_battleGame->getSelectedUnit())
	{
		_map->centerOnPosition(_battleGame->getSelectedUnit()->getPosition());
	}
	_game->pushState(new NextTurnState(_game, _battleGame));
}


/**
 * Checks for casualties and adjusts morale accordingly.
 * @param murderweapon
 * @param murderer
 */
void BattlescapeState::checkForCasualties(BattleItem *murderweapon, BattleUnit *murderer)
{
	// TODO : include rank bonusses and penalties !!
	for (std::vector<BattleUnit*>::iterator j = _battleGame->getUnits()->begin(); j != _battleGame->getUnits()->end(); ++j)
	{
		if ((*j)->getHealth() == 0 && (*j)->getStatus() != STATUS_DEAD)
		{
			BattleUnit *victim = (*j);

			if (murderer)
			{
				// if there is a known murderer, he will get a morale bonus if he is of a different faction (what with neutral?)
				if (victim->getFaction() == FACTION_PLAYER && murderer->getFaction() == FACTION_HOSTILE ||
					victim->getFaction() == FACTION_HOSTILE && murderer->getFaction() == FACTION_PLAYER)
				{
					murderer->moraleChange(+20);
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
				}
				// the winning squad all get a morale increase
				if ((*i)->getFaction() != victim->getFaction())
				{
					(*i)->moraleChange(+10);
				}
			}

			if (murderweapon)
				statePushNext(new UnitFallBState(this, (*j), murderweapon->getRules()->getDamageType() == DT_HE)); // explosions insta-kill a unit
			else
				statePushNext(new UnitFallBState(this, (*j), false));
		}
	}
}

/**
 * Abort game.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnAbortClick(Action *action)
{
	_game->getSavedGame()->endBattle();
	_game->getCursor()->setColor(Palette::blockOffset(15)+12);
	_game->getFpsCounter()->setColor(Palette::blockOffset(15)+12);
	_game->popState();
}

/**
 * Show selected soldier info.
 * @param action Pointer to an action.
 */
void BattlescapeState::btnStatsClick(Action *action)
{
	if (_battleGame->getSelectedUnit())
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
	if (_battleGame->getSelectedUnit())
	{
		BattleItem *leftHandItem = _battleGame->getItemFromUnit(_battleGame->getSelectedUnit(), LEFT_HAND);
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
	if (_battleGame->getSelectedUnit())
	{
		BattleItem *rightHandItem = _battleGame->getItemFromUnit(_battleGame->getSelectedUnit(), RIGHT_HAND);
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
		_map->centerOnPosition(_visibleUnit[btnID]->getPosition());
	}
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
			if (_action.actor->spendTimeUnits(_action.TU, _game->getSavedGame()->getBattleGame()->getDebugMode()))
			{
				_action.weapon->setExplodeTurn(_game->getSavedGame()->getBattleGame()->getTurn() + _action.value);
			}
			else
			{
				showWarningMessage("STR_NOT_ENOUGH_TIME_UNITS");
			}
		}
		_action.type = BA_NONE;
		updateSoldierInfo(_battleGame->getSelectedUnit());
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
		_map->setCursorType(CT_NORMAL);
	}
}

/**
 * Updates soldier name/rank/tu/energy/health/morale.
 * @param battleUnit Pointer to current unit.
 */
void BattlescapeState::updateSoldierInfo(BattleUnit *battleUnit)
{
	for (int i = 0; i < 10; ++i)
	{
		_btnVisibleUnit[i]->hide();
		_numVisibleUnit[i]->hide();
		_visibleUnit[i] = 0;
	}

	if (battleUnit == 0)
	{
		_txtName->setText(L"");
		_rank->clear();
		_numTimeUnits->clear();
		_barTimeUnits->clear();
		_barTimeUnits->clear();
		_numEnergy->clear();
		_barEnergy->clear();
		_barEnergy->clear();
		_numHealth->clear();
		_barHealth->clear();
		_barHealth->clear();
		_numMorale->clear();
		_barMorale->clear();
		_barMorale->clear();
		_btnLeftHandItem->clear();
		_btnRightHandItem->clear();
		_numAmmoLeft->clear();
		_numAmmoRight->clear();
		return;
	}

	_txtName->setText(battleUnit->getUnit()->getName());
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
	_numMorale->setValue(battleUnit->getMorale());
	_barMorale->setMax(100);
	_barMorale->setValue(battleUnit->getMorale());

	BattleItem *leftHandItem = _battleGame->getItemFromUnit(battleUnit, LEFT_HAND);
	_btnLeftHandItem->clear();
	_numAmmoLeft->clear();
	if (leftHandItem)
	{
		drawItemSprite(leftHandItem, _btnLeftHandItem);
		if (leftHandItem->getAmmoItem())
			_numAmmoLeft->setValue(leftHandItem->getAmmoItem()->getAmmoQuantity());
	}
	BattleItem *rightHandItem = _battleGame->getItemFromUnit(battleUnit, RIGHT_HAND);
	_btnRightHandItem->clear();
	_numAmmoRight->clear();
	if (rightHandItem)
	{
		drawItemSprite(rightHandItem, _btnRightHandItem);
		if (rightHandItem->getAmmoItem())
			_numAmmoRight->setValue(rightHandItem->getAmmoItem()->getAmmoQuantity());
	}

	_battleGame->getTerrainModifier()->calculateFOV(_battleGame->getSelectedUnit());
	int j = 0;
	for (std::vector<BattleUnit*>::iterator i = battleUnit->getVisibleUnits()->begin(); i != battleUnit->getVisibleUnits()->end(); ++i)
	{
		_btnVisibleUnit[j]->show();
		_numVisibleUnit[j]->show();
		_visibleUnit[j] = (*i);
		j++;
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


/**
  * Shift the red color of the warning message.
  */
void BattlescapeState::blinkWarningMessage()
{
	static int color = 32, delay = 12;

	if (_warningMessageBackground->getVisible() == false)
		return;

	SDL_Rect square1;
	square1.x = 0;
	square1.y = 0;
	square1.w = 224;
	square1.h = 48;
	_warningMessageBackground->drawRect(&square1, color);

	if (color >= 44)
	{
		delay--;
	}
	else
	{
		color++;
	}

	if (delay == 0)
	{
		_warningMessageBackground->setVisible(false);
		_txtWarningMessage->setVisible(false);
		color = 32;
		delay = 12;
	}

}

/**
  * Show warning message.
  * @param message untranslated
  */
void BattlescapeState::showWarningMessage(std::string message)
{
	std::wstring messageText = _game->getLanguage()->getString(message);
	_warningMessageBackground->setVisible(true);
	_txtWarningMessage->setVisible(true);
	_txtWarningMessage->setText(messageText);
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
		popup(new ActionMenuState(_game, &_action));
	}
}

/**
 * Draws the item's sprite on a surface.
 * @param item the given item
 * @surface surface the given surface
 */
void BattlescapeState::drawItemSprite(BattleItem *item, Surface *surface)
{
	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BIGOBS.PCK");
	Surface *frame = texture->getFrame(item->getRules()->getBigSprite());
	frame->setX((2 - item->getRules()->getInventoryWidth()) * 8);
	frame->setY((3 - item->getRules()->getInventoryHeight()) * 8);
	texture->getFrame(item->getRules()->getBigSprite())->blit(surface);
}

/**
 * Give time slice to the front state.
 */
void BattlescapeState::handleState()
{
	if (!_states.empty())
	{
		_states.front()->think();
		_map->draw(true); // redraw map
	}
}

/**
 * Animate map objects on the map, also smoke,fire,....
 */
void BattlescapeState::animate()
{
	_animFrame++;
	if (_animFrame == 8) _animFrame = 0;
	_map->animate();

	blinkVisibleUnitButtons();
	blinkWarningMessage();
}

/**
 * Get a pointer to the current action.
 */
BattleAction *BattlescapeState::getAction()
{
	return &_action;
}

/**
 * Get pointer to the game. Some states need this info.
 */
Game *BattlescapeState::getGame() const
{
	return _game;
}

/**
 * Get pointer to the map. Some states need this info.
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

	if (_states.front()->getResult().length() > 0)
	{
		showWarningMessage(_states.front()->getResult());
		actionFailed = true;
	}
	_states.pop_front();

	if (_states.empty())
	{
		if (_action.targeting && _battleGame->getSelectedUnit() && !actionFailed)
		{
			_action.actor->spendTimeUnits(_action.TU, _battleGame->getDebugMode());
		}
		if (_action.type == BA_THROW && !actionFailed)
		{
			_action.targeting = false;
			_action.type = BA_NONE;
		}
		_game->getCursor()->setVisible(true);
		setupCursor();
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
		_map->setCursorType(CT_NORMAL);
		_game->getCursor()->setVisible(true);
		_battleGame->setSelectedUnit(0);
	}
	updateSoldierInfo(_battleGame->getSelectedUnit());
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
				// "d" - enable debug mode
			if (action->getDetails()->key.keysym.sym == SDLK_d)
			{
				_battleGame->setDebugMode();
				debug(L"Debug Mode");
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

}
