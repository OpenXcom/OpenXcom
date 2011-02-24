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
#include "PurchaseState.h"
#include <string>
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Timer.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Purchase/Hire screen.
 * @param game Pointer to the core game.
 */
PurchaseState::PurchaseState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtFunds = new Text(150, 9, 10, 24);
	_txtPurchases = new Text(150, 9, 160, 24);
	_txtItem = new Text(140, 9, 10, 32);
	_txtCost = new Text(102, 9, 152, 32);
	_txtQuantity = new Text(60, 9, 256, 32);
	_lstItems = new TextList(288, 128, 8, 40);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtFunds);
	add(_txtPurchases);
	add(_txtItem);
	add(_txtCost);
	add(_txtQuantity);
	add(_lstItems);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+13);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PurchaseState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(13)+13);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&PurchaseState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_PURCHASE_HIRE_PERSONNEL"));

	std::wstring s1 = _game->getLanguage()->getString("STR_CURRENT_FUNDS");
	s1 += Text::formatFunding(_game->getSavedGame()->getFunds());
	_txtFunds->setColor(Palette::blockOffset(13)+10);
	_txtFunds->setText(s1);

	std::wstring s2 = _game->getLanguage()->getString("STR_COST_OF_PURCHASES");
	s2 += Text::formatFunding(0);
	_txtPurchases->setColor(Palette::blockOffset(13)+10);
	_txtPurchases->setText(s2);

	_txtItem->setColor(Palette::blockOffset(13)+10);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtCost->setColor(Palette::blockOffset(13)+10);
	_txtCost->setText(_game->getLanguage()->getString("STR_COST_PER_UNIT_UC"));

	_txtQuantity->setColor(Palette::blockOffset(13)+10);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_lstItems->setColor(Palette::blockOffset(13)+10);
	_lstItems->setArrowColor(Palette::blockOffset(13)+13);
	_lstItems->setArrowColumn(227);
	_lstItems->setColumns(3, 162, 92, 32);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);

	_total = 0;
	_sel = -1;
	_crafts.push_back("STR_SKYRANGER");
	_crafts.push_back("STR_INTERCEPTOR");
	_items.push_back("STR_STINGRAY_LAUNCHER");
	_items.push_back("STR_AVALANCHE_LAUNCHER");
	_items.push_back("STR_CANNON");
	_items.push_back("STR_STINGRAY_MISSILES");
	_items.push_back("STR_AVALANCHE_MISSILES");
	_items.push_back("STR_CANNON_ROUNDS_X50");
	_items.push_back("STR_PISTOL");
	_items.push_back("STR_PISTOL_CLIP");
	_items.push_back("STR_RIFLE");
	_items.push_back("STR_RIFLE_CLIP");
	_items.push_back("STR_HEAVY_CANNON");
	_items.push_back("STR_HC_AP_AMMO");
	_items.push_back("STR_HC_HE_AMMO");
	_items.push_back("STR_AUTO_CANNON");
	_items.push_back("STR_AC_AP_AMMO");
	_items.push_back("STR_ROCKET_LAUNCHER");
	_items.push_back("STR_SMALL_ROCKET");
	_items.push_back("STR_GRENADE");
	_items.push_back("STR_SMOKE_GRENADE");

	_qtys.push_back(0);
	_lstItems->addRow(3, _game->getLanguage()->getString("STR_SOLDIER").c_str(), Text::formatFunding(_game->getRuleset()->getSoldierCost() * 2).c_str(), L"0");
	_qtys.push_back(0);
	_lstItems->addRow(3, _game->getLanguage()->getString("STR_SCIENTIST").c_str(), Text::formatFunding(_game->getRuleset()->getScientistCost() * 2).c_str(), L"0");
	_qtys.push_back(0);
	_lstItems->addRow(3, _game->getLanguage()->getString("STR_ENGINEER").c_str(), Text::formatFunding(_game->getRuleset()->getEngineerCost() * 2).c_str(), L"0");
	for (std::vector<std::string>::iterator i = _crafts.begin(); i != _crafts.end(); i++)
	{
		_qtys.push_back(0);
		_lstItems->addRow(3, _game->getLanguage()->getString(*i).c_str(), Text::formatFunding(_game->getRuleset()->getCraft(*i)->getCost()).c_str(), L"0");
	}
	for (std::vector<std::string>::iterator i = _items.begin(); i != _items.end(); i++)
	{
		_qtys.push_back(0);
		_lstItems->addRow(3, _game->getLanguage()->getString(*i).c_str(), Text::formatFunding(_game->getRuleset()->getItem(*i)->getCost()).c_str(), L"0");
	}
	_lstItems->onLeftArrowPress((ActionHandler)&PurchaseState::lstItemsLeftArrowPress);
	_lstItems->onLeftArrowRelease((ActionHandler)&PurchaseState::lstItemsLeftArrowRelease);
	_lstItems->onRightArrowPress((ActionHandler)&PurchaseState::lstItemsRightArrowPress);
	_lstItems->onRightArrowRelease((ActionHandler)&PurchaseState::lstItemsRightArrowRelease);

	_timerInc = new Timer(50);
	_timerInc->onTimer((StateHandler)&PurchaseState::increase);
	_timerDec = new Timer(50);
	_timerDec->onTimer((StateHandler)&PurchaseState::decrease);
}

/**
 *
 */
PurchaseState::~PurchaseState()
{
	
}

/**
 * Runs the game timer and handles popups.
 */
void PurchaseState::think()
{
	State::think();

	_timerInc->think(this, 0);
	_timerDec->think(this, 0);
}

void PurchaseState::btnOkClick(Action *action)
{
	
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void PurchaseState::btnCancelClick(Action *action)
{
	_game->popState();
}

/**
 * Starts increasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	_timerInc->start();
	
}

/**
 * Stops increasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsLeftArrowRelease(Action *action)
{
	_timerInc->stop();
}

/**
 * Starts decreasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowPress(Action *action)
{
	_sel = _lstItems->getSelectedRow();
	_timerDec->start();
	
}

/**
 * Stops decreasing the item.
 * @param action Pointer to an action.
 */
void PurchaseState::lstItemsRightArrowRelease(Action *action)
{
	_timerDec->stop();	
}

/**
 * Increases the quantity of the selected item on the list.
 */
void PurchaseState::increase()
{
	_qtys[_sel]++;
	std::wstringstream ss;
	ss << _qtys[_sel];
	_lstItems->getCell(_sel, 2)->setText(ss.str());
	_lstItems->draw();
}

/**
 * Decreases the quantity of the selected item on the list.
 */
void PurchaseState::decrease()
{
	if (_qtys[_sel] > 0)
		_qtys[_sel]--;
	std::wstringstream ss;
	ss << _qtys[_sel];
	_lstItems->getCell(_sel, 2)->setText(ss.str());
	_lstItems->draw();
}

}
