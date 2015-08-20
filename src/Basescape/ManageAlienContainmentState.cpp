/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "ManageAlienContainmentState.h"
#include <climits>
#include <sstream>
#include <algorithm>
#include "../Engine/Action.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/ResearchProject.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleResearch.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include "../Menu/ErrorMessageState.h"
#include "SellState.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleInterface.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Manage Alien Containment screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param origin Game section that originated this state.
 */
ManageAlienContainmentState::ManageAlienContainmentState(Base *base, OptionsOrigin origin) : _base(base), _origin(origin), _sel(0), _aliensSold(0)
{
	bool overCrowded = Options::storageLimitsEnforced && _base->getFreeContainment() < 0;
	std::vector<std::string> researchList;
	for (std::vector<ResearchProject*>::const_iterator iter = _base->getResearch().begin(); iter != _base->getResearch().end(); ++iter)
	{
		const RuleResearch *research = (*iter)->getRules();
		RuleItem *item = _game->getRuleset()->getItem(research->getName());
		if (item && item->isAlien())
		{
			researchList.push_back(research->getName());
		}
	}

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(overCrowded ? 288:148, 16, overCrowded ? 16:8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtAvailable =  new Text(190, 9, 10, 24);
	_txtUsed = new Text(110, 9, 136, 24);
	_txtItem = new Text(120, 9, 10, 41);
	_txtLiveAliens = new Text(54, 18, 153, 32);
	_txtDeadAliens = new Text(54, 18, 207, 32);
	_txtInterrogatedAliens = new Text(54, 18, 261, 32);
	_lstAliens = new TextList(286, 112, 8, 53);

	// Set palette
	setInterface("manageContainment");

	add(_window, "window", "manageContainment");
	add(_btnOk, "button", "manageContainment");
	add(_btnCancel, "button", "manageContainment");
	add(_txtTitle, "text", "manageContainment");
	add(_txtAvailable, "text", "manageContainment");
	add(_txtUsed, "text", "manageContainment");
	add(_txtItem, "text", "manageContainment");
	add(_txtLiveAliens, "text", "manageContainment");
	add(_txtDeadAliens, "text", "manageContainment");
	add(_txtInterrogatedAliens, "text", "manageContainment");
	add(_lstAliens, "list", "manageContainment");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface((origin == OPT_BATTLESCAPE)? "BACK01.SCR" : "BACK05.SCR"));

	_btnOk->setText(tr("STR_REMOVE_SELECTED"));
	_btnOk->onMouseClick((ActionHandler)&ManageAlienContainmentState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ManageAlienContainmentState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&ManageAlienContainmentState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ManageAlienContainmentState::btnCancelClick, Options::keyCancel);

	if (overCrowded)
	{
		_btnCancel->setVisible(false);
		_btnOk->setVisible(false);
	}

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_MANAGE_CONTAINMENT"));

	_txtItem->setText(tr("STR_ALIEN"));

	_txtLiveAliens->setText(tr("STR_LIVE_ALIENS"));
	_txtLiveAliens->setWordWrap(true);
	_txtLiveAliens->setVerticalAlign(ALIGN_BOTTOM);

	_txtDeadAliens->setText(tr("STR_DEAD_ALIENS"));
	_txtDeadAliens->setWordWrap(true);
	_txtDeadAliens->setVerticalAlign(ALIGN_BOTTOM);

	_txtInterrogatedAliens->setText(tr("STR_UNDER_INTERROGATION"));
	_txtInterrogatedAliens->setWordWrap(true);
	_txtInterrogatedAliens->setVerticalAlign(ALIGN_BOTTOM);

	_txtAvailable->setText(tr("STR_SPACE_AVAILABLE").arg(_base->getFreeContainment()));

	_txtUsed->setText(tr("STR_SPACE_USED").arg( _base->getUsedContainment()));

	_lstAliens->setArrowColumn(184, ARROW_HORIZONTAL);
	_lstAliens->setColumns(4, 160, 64, 46, 46);
	_lstAliens->setSelectable(true);
	_lstAliens->setBackground(_window);
	_lstAliens->setMargin(2);
	_lstAliens->onLeftArrowPress((ActionHandler)&ManageAlienContainmentState::lstItemsLeftArrowPress);
	_lstAliens->onLeftArrowRelease((ActionHandler)&ManageAlienContainmentState::lstItemsLeftArrowRelease);
	_lstAliens->onLeftArrowClick((ActionHandler)&ManageAlienContainmentState::lstItemsLeftArrowClick);
	_lstAliens->onRightArrowPress((ActionHandler)&ManageAlienContainmentState::lstItemsRightArrowPress);
	_lstAliens->onRightArrowRelease((ActionHandler)&ManageAlienContainmentState::lstItemsRightArrowRelease);
	_lstAliens->onRightArrowClick((ActionHandler)&ManageAlienContainmentState::lstItemsRightArrowClick);
	_lstAliens->onMousePress((ActionHandler)&ManageAlienContainmentState::lstItemsMousePress);

	const std::vector<std::string> &items = _game->getRuleset()->getItemsList();
	for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i)
	{
		int qty = _base->getItems()->getItem(*i);
		if (qty > 0 && _game->getRuleset()->getItem(*i)->isAlien())
		{
			_qtys.push_back(0);
			_aliens.push_back(*i);
			std::wostringstream ss;
			ss << qty;
			std::wstring rqty;
			std::vector<std::string>::iterator research = std::find(researchList.begin(), researchList.end(), *i);
			if (research != researchList.end())
			{
				rqty = L"1";
				researchList.erase(research);
			}
			else
			{
				rqty = L"0";
			}
			_lstAliens->addRow(4, tr(*i).c_str(), ss.str().c_str(), L"0", rqty.c_str());
		}
	}

	for (std::vector<std::string>::const_iterator i = researchList.begin(); i != researchList.end(); ++i)
	{
		_aliens.push_back(*i);
		_qtys.push_back(0);
		_lstAliens->addRow(4, tr(*i).c_str(), L"0", L"0", L"1");
		_lstAliens->setRowColor(_qtys.size() -1, _lstAliens->getSecondaryColor());
	}
	_timerInc = new Timer(250);
	_timerInc->onTimer((StateHandler)&ManageAlienContainmentState::increase);
	_timerDec = new Timer(250);
	_timerDec->onTimer((StateHandler)&ManageAlienContainmentState::decrease);
}

/**
 *
 */
ManageAlienContainmentState::~ManageAlienContainmentState()
{
	delete _timerInc;
	delete _timerDec;
}

/**
 * Runs the arrow timers.
 */
void ManageAlienContainmentState::think()
{
	State::think();

	_timerInc->think(this, 0);
	_timerDec->think(this, 0);
}

/**
 * Deals with the selected aliens.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::btnOkClick(Action *)
{
	for (size_t i = 0; i < _qtys.size(); ++i)
	{
		if (_qtys[i] > 0)
		{
			// remove the aliens
			_base->getItems()->removeItem(_aliens[i], _qtys[i]);

			if (Options::canSellLiveAliens)
			{
				_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() + _game->getRuleset()->getItem(_aliens[i])->getSellCost() * _qtys[i]);
			}
			else
			{
				// add the corpses
				_base->getItems()->addItem(
					_game->getRuleset()->getArmor(
						_game->getRuleset()->getUnit(
							_aliens[i]
						)->getArmor()
					)->getCorpseGeoscape(), _qtys[i]
				); // ;)
			}
		}
	}
	_game->popState();

	if (Options::storageLimitsEnforced && _base->storesOverfull())
	{
		_game->pushState(new SellState(_base, _origin));
		if (_origin == OPT_BATTLESCAPE)
			_game->pushState(new ErrorMessageState(tr("STR_STORAGE_EXCEEDED").arg(_base->getName()).c_str(), _palette, _game->getRuleset()->getInterface("manageContainment")->getElement("errorMessage")->color, "BACK01.SCR", _game->getRuleset()->getInterface("manageContainment")->getElement("errorPalette")->color));
		else
			_game->pushState(new ErrorMessageState(tr("STR_STORAGE_EXCEEDED").arg(_base->getName()).c_str(), _palette, _game->getRuleset()->getInterface("manageContainment")->getElement("errorMessage")->color, "BACK13.SCR", _game->getRuleset()->getInterface("manageContainment")->getElement("errorPalette")->color));
 	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Starts increasing the alien count.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::lstItemsRightArrowPress(Action *action)
{
	_sel = _lstAliens->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && !_timerInc->isRunning()) _timerInc->start();
}

/**
 * Stops increasing the alien count.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::lstItemsRightArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerInc->stop();
	}
}

/**
 * Increases the selected alien count;
 * by one on left-click, to max on right-click.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::lstItemsRightArrowClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) increaseByValue(INT_MAX);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		increaseByValue(1);
		_timerInc->setInterval(250);
		_timerDec->setInterval(250);
	}
}

/**
 * Starts decreasing the alien count.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::lstItemsLeftArrowPress(Action *action)
{
	_sel = _lstAliens->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && !_timerDec->isRunning()) _timerDec->start();
}

/**
 * Stops decreasing the alien count.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::lstItemsLeftArrowRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerDec->stop();
	}
}

/**
 * Decreases the selected alien count;
 * by one on left-click, to 0 on right-click.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::lstItemsLeftArrowClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) decreaseByValue(INT_MAX);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		decreaseByValue(1);
		_timerInc->setInterval(250);
		_timerDec->setInterval(250);
	}
}

/**
 * Handles the mouse-wheels on the arrow-buttons.
 * @param action Pointer to an action.
 */
void ManageAlienContainmentState::lstItemsMousePress(Action *action)
{
	_sel = _lstAliens->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (action->getAbsoluteXMouse() >= _lstAliens->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstAliens->getArrowsRightEdge())
		{
			increaseByValue(Options::changeValueByMouseWheel);
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN)
	{
		_timerInc->stop();
		_timerDec->stop();
		if (action->getAbsoluteXMouse() >= _lstAliens->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstAliens->getArrowsRightEdge())
		{
			decreaseByValue(Options::changeValueByMouseWheel);
		}
	}
}

/**
 * Gets the quantity of the currently selected alien on the base.
 * @return Quantity of selected alien on the base.
 */
int ManageAlienContainmentState::getQuantity()
{
	return _base->getItems()->getItem(_aliens[_sel]);
}

/**
 * Increases the quantity of the selected alien to exterminate by one.
 */
void ManageAlienContainmentState::increase()
{
	_timerDec->setInterval(50);
	_timerInc->setInterval(50);
	increaseByValue(1);
}

/**
 * Increases the quantity of the selected alien to exterminate by "change".
 * @param change How much we want to add.
 */
void ManageAlienContainmentState::increaseByValue(int change)
{
	int qty = getQuantity() - _qtys[_sel];
	if (change <= 0 || qty <= 0) return;

	change = std::min(qty, change);
	_qtys[_sel] += change;
	_aliensSold += change;
	updateStrings();
}

/**
 * Decreases the quantity of the selected alien to exterminate by one.
 */
void ManageAlienContainmentState::decrease()
{
	_timerInc->setInterval(50);
	_timerDec->setInterval(50);
	decreaseByValue(1);
}

/**
 * Decreases the quantity of the selected alien to exterminate by "change".
 * @param change How much we want to remove.
 */
void ManageAlienContainmentState::decreaseByValue(int change)
{
	if (change <= 0 || _qtys[_sel] <= 0) return;
	change = std::min(_qtys[_sel], change);
	_qtys[_sel] -= change;
	_aliensSold -= change;
	updateStrings();
}

/**
 * Updates the quantity-strings of the selected alien.
 */
void ManageAlienContainmentState::updateStrings()
{
	std::wostringstream ss, ss2, ss3;
	int qty = getQuantity() - _qtys[_sel];
	ss << qty;
	ss2 << _qtys[_sel];

	_lstAliens->setRowColor(_sel, (qty == 0)? _lstAliens->getSecondaryColor() : _lstAliens->getColor());
	_lstAliens->setCellText(_sel, 1, ss.str());
	_lstAliens->setCellText(_sel, 2, ss2.str());

	int aliens = _base->getUsedContainment() - _aliensSold;
	int spaces = _base->getAvailableContainment() - aliens;
	if (Options::storageLimitsEnforced)
	{
		_btnOk->setVisible(spaces >= 0);
	}
	_txtAvailable->setText(tr("STR_SPACE_AVAILABLE").arg(spaces));
	_txtUsed->setText(tr("STR_SPACE_USED").arg(aliens));
}

}
