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
#include "InventoryState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/CrossPlatform.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Text.h"
#include "../Engine/Action.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/SurfaceSet.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Soldier.h"
#include "Inventory.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/Armor.h"
#include "UnitInfoState.h"
#include "TileEngine.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Inventory screen.
 * @param game Pointer to the core game.
 * @param tu Inventory Time Unit mode.
 */
InventoryState::InventoryState(Game *game, bool tu) : State(game), _tu(tu)
{
	_battleGame = _game->getSavedGame()->getBattleGame();

	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_soldier = new Surface(320, 200, 0, 0);
	_txtName = new Text(200, 16, 36, 6);
	_txtTus = new Text(40, 9, 250, 24);
	_txtItem = new Text(100, 9, 0, 23);
	_txtAmmo = new Text(64, 24, 256, 64);
	_btnOk = new InteractiveSurface(35, 22, 237, 1);
	_btnPrev = new InteractiveSurface(23, 22, 273, 1);
	_btnNext = new InteractiveSurface(23, 22, 297, 1);
	_btnUnload = new InteractiveSurface(32, 25, 288, 32);
	_btnGround = new InteractiveSurface(32, 15, 289, 137);
	_btnRank = new InteractiveSurface(26, 23, 0, 0);
	_selAmmo = new Surface(RuleInventory::HAND_W * RuleInventory::SLOT_W, RuleInventory::HAND_H * RuleInventory::SLOT_H, 272, 88);
	_inv = new Inventory(_game, 320, 200, 0, 0);

	add(_bg);
	add(_soldier);
	add(_txtName);
	add(_txtTus);
	add(_txtItem);
	add(_txtAmmo);
	add(_btnOk);
	add(_btnPrev);
	add(_btnNext);
	add(_btnUnload);
	add(_btnGround);
	add(_btnRank);
	add(_selAmmo);
	add(_inv);

	// Set up objects
	_game->getResourcePack()->getSurface("TAC01.SCR")->blit(_bg);

	_txtName->setColor(Palette::blockOffset(4));
	_txtName->setBig();
	_txtName->setHighContrast(true);

	_txtTus->setColor(Palette::blockOffset(4));
	_txtTus->setSecondaryColor(Palette::blockOffset(1));
	_txtTus->setHighContrast(true);

	_txtItem->setColor(Palette::blockOffset(3));
	_txtItem->setHighContrast(true);

	_txtAmmo->setColor(Palette::blockOffset(4));
	_txtAmmo->setSecondaryColor(Palette::blockOffset(1));
	_txtAmmo->setAlign(ALIGN_CENTER);
	_txtAmmo->setHighContrast(true);

	_btnOk->onMouseClick((ActionHandler)&InventoryState::btnOkClick);
	_btnPrev->onMouseClick((ActionHandler)&InventoryState::btnPrevClick);
	_btnNext->onMouseClick((ActionHandler)&InventoryState::btnNextClick);
	_btnUnload->onMouseClick((ActionHandler)&InventoryState::btnUnloadClick);
	_btnGround->onMouseClick((ActionHandler)&InventoryState::btnGroundClick);
	_btnRank->onMouseClick((ActionHandler)&InventoryState::btnRankClick);

	_inv->draw();
	_inv->setTuMode(_tu);
	_inv->setSelectedUnit(_game->getSavedGame()->getBattleGame()->getSelectedUnit());
	_inv->onMouseClick((ActionHandler)&InventoryState::invClick, 0);
}

/**
 *
 */
InventoryState::~InventoryState()
{

}

/**
 * Update soldier stats when the soldier changes.
 */
void InventoryState::init()
{
	BattleUnit *unit = _battleGame->getSelectedUnit();

	unit->setCache(0);
	_soldier->clear();
	_btnRank->clear();

	_txtName->setText(unit->getName(_game->getLanguage()));
	_inv->setSelectedUnit(unit);
	Soldier *s = _game->getSavedGame()->getSoldier(unit->getId());
	if (s)
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
		texture->getFrame(s->getRankSprite())->setX(0);
		texture->getFrame(s->getRankSprite())->setY(0);
		texture->getFrame(s->getRankSprite())->blit(_btnRank);

		std::string look = s->getArmor()->getSpriteInventory();
		if (s->getGender() == GENDER_MALE)
			look += "M";
		else
			look += "F";
		if (s->getLook() == LOOK_BLONDE)
			look += "0";
		if (s->getLook() == LOOK_BROWNHAIR)
			look += "1";
		if (s->getLook() == LOOK_ORIENTAL)
			look += "2";
		if (s->getLook() == LOOK_AFRICAN)
			look += "3";
		look += ".SPK";
		if (!CrossPlatform::fileExists(CrossPlatform::getDataFile("UFOGRAPH/" + look)))
		{
			look = s->getArmor()->getSpriteInventory() + ".SPK";
		}
		_game->getResourcePack()->getSurface(look)->blit(_soldier);
	}
	if (_tu)
	{
		std::wstringstream ss;
		ss << _game->getLanguage()->getString("STR_TUS") << L'\x01' << unit->getTimeUnits();
		_txtTus->setText(ss.str());
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void InventoryState::btnOkClick(Action *action)
{
	if (_inv->getSelectedItem() != 0)
		return;
	_game->popState();
	if (!_tu)
	{
		_battleGame->resetUnitTiles();
	}
	_battleGame->getTileEngine()->applyItemGravity(_battleGame->getSelectedUnit()->getTile());
	_battleGame->getTileEngine()->calculateTerrainLighting(); // dropping/picking up flares
}

/**
 * Selects the previous soldier.
 * @param action Pointer to an action.
 */
void InventoryState::btnPrevClick(Action *action)
{
	if (_inv->getSelectedItem() != 0)
		return;
	_battleGame->selectPreviousPlayerUnit();
	// skip large units
	while (_battleGame->getSelectedUnit()->getArmor()->getSize() > 1)
	{
		_battleGame->selectPreviousPlayerUnit();
	}
	init();
}

/**
 * Selects the next soldier.
 * @param action Pointer to an action.
 */
void InventoryState::btnNextClick(Action *action)
{
	if (_inv->getSelectedItem() != 0)
		return;
	_battleGame->selectNextPlayerUnit();
	// skip large units
	while (_battleGame->getSelectedUnit()->getArmor()->getSize() > 1)
	{
		_battleGame->selectNextPlayerUnit();
	}
	init();
}

/**
 * Unloads the selected weapon.
 * @param action Pointer to an action.
 */
void InventoryState::btnUnloadClick(Action *action)
{
	if (_inv->getSelectedItem() != 0 && _inv->getSelectedItem()->getAmmoItem() != 0 && _inv->getSelectedItem()->needsAmmo())
	{
		_inv->unload();
		_txtAmmo->setText(L"");
		_selAmmo->clear();
		if (_tu)
		{
			std::wstringstream ss;
			ss << _game->getLanguage()->getString("STR_TUS") << L'\x01' << _battleGame->getSelectedUnit()->getTimeUnits();
			_txtTus->setText(ss.str());
		}
	}
}

/**
 * Shows more ground items / rearranges them.
 * @param action Pointer to an action.
 */
void InventoryState::btnGroundClick(Action *action)
{
	_inv->arrangeGround();
}

/**
 * Shows the unit info screen.
 * @param action Pointer to an action.
 */
void InventoryState::btnRankClick(Action *action)
{
	_game->pushState(new UnitInfoState(_game, _battleGame->getSelectedUnit()));
}

/**
 * Updates item info.
 * @param action Pointer to an action.
 */
void InventoryState::invClick(Action *action)
{
	BattleItem *item = _inv->getSelectedItem();
	_txtItem->setText(L"");
	_txtAmmo->setText(L"");
	_selAmmo->clear();
	if (item != 0)
	{
		if (item->getUnit() && item->getUnit()->getStatus() == STATUS_UNCONSCIOUS)
		{
			_txtItem->setText(item->getUnit()->getName(_game->getLanguage()));
		}
		else
		{
			if (_game->getSavedGame()->isResearched(item->getRules()->getRequirements()))
			{
				_txtItem->setText(_game->getLanguage()->getString(item->getRules()->getName()));
			}
			else
			{
				_txtItem->setText(_game->getLanguage()->getString("STR_ALIEN_ARTIFACT"));
			}
		}
		std::wstringstream ss;
		if (item->getAmmoItem() != 0 && item->needsAmmo())
		{
			ss << _game->getLanguage()->getString("STR_AMMO_ROUNDS_LEFT") << L'\x01' << item->getAmmoItem()->getAmmoQuantity();
			SDL_Rect r;
			r.x = 0;
			r.y = 0;
			r.w = RuleInventory::HAND_W * RuleInventory::SLOT_W;
			r.h = RuleInventory::HAND_H * RuleInventory::SLOT_H;
			_selAmmo->drawRect(&r, Palette::blockOffset(0)+8);
			r.x++;
			r.y++;
			r.w -= 2;
			r.h -= 2;
			_selAmmo->drawRect(&r, 0);
			item->getAmmoItem()->getRules()->drawHandSprite(_game->getResourcePack()->getSurfaceSet("BIGOBS.PCK"), _selAmmo);
		}
		else if (item->getAmmoQuantity() != 0 && item->needsAmmo())
		{
			ss << _game->getLanguage()->getString("STR_AMMO_ROUNDS_LEFT") << L'\x01' << item->getAmmoQuantity();
		}
		_txtAmmo->setText(ss.str());
	}
	if (_tu)
	{
		std::wstringstream ss;
		ss << _game->getLanguage()->getString("STR_TUS") << L'\x01' << _battleGame->getSelectedUnit()->getTimeUnits();
		_txtTus->setText(ss.str());
	}
}

}
