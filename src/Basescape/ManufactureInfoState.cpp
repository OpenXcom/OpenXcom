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
#include "ManufactureInfoState.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/ArrowButton.h"
#include "../Engine/Action.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleManufacture.h"
#include "../Savegame/Base.h"
#include "../Savegame/Production.h"
#include "../Engine/Timer.h"
#include "../Menu/ErrorMessageState.h"
#include <limits>

namespace OpenXcom
{
/**
 * Initialize all elements in the Production settings screen(new Production)
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param item the RuleManufacture to produce
*/
ManufactureInfoState::ManufactureInfoState (Game * game, Base * base, RuleManufacture * item) : State (game), _base(base), _item(item), _production(0)
{
	buildUi();
}

/**
 * Initialize all elements in the Production settings screen(modifying Production)
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param production the Production to modify
*/
ManufactureInfoState::ManufactureInfoState (Game * game, Base * base, Production * production) : State (game), _base(base), _item(0), _production(production)
{
	buildUi();
}

/**
 * build screen User Interface
*/
void ManufactureInfoState::buildUi()
{
	_screen = false;
	int width = 320;
	int height = 170;
	int max_width = 320;
	int max_height = 200;
	int start_x = (max_width - width) / 2;
	int start_y = (max_height - height) / 2;
	int button_x_border = 10;
	int button_y_border = 10;
	int button_height = 16;

	int button_width = (width - 5 * button_x_border) / 2;
	_window = new Window(this, width, height, start_x, start_y);
	_txtTitle = new Text (width - 4 * button_x_border, button_height * 2, start_x + button_x_border, start_y + button_y_border);
	_btnOk = new TextButton (button_width, button_height, width - button_width - button_x_border, start_y + height - button_height - button_y_border);
	_btnStop = new TextButton (button_width, button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);
	_txtAvailableEngineer = new Text(width - 4 * button_x_border, button_height, start_x + button_x_border, start_y + 2 * button_height);
	_txtAvailableSpace = new Text(width - 4 * button_x_border, button_height, start_x + button_x_border, start_y + 2.7f * button_height);
	_txtAllocatedEngineer = new Text(button_width, 2*button_height, start_x + button_x_border, start_y + 3.5f * button_height);
	_txtUnitToProduce = new Text(button_width, 2*button_height, width - button_width - button_x_border, start_y + 3.5f * button_height);
	_txtEngineerUp = new Text(button_width, 2*button_height, start_x + 3*button_x_border, start_y + 6 * button_height);
	_txtEngineerDown = new Text(button_width, 2*button_height, start_x + 3*button_x_border, start_y + 7.5f * button_height);
	_txtUnitUp = new Text(button_width, 2*button_height, width - button_width - button_x_border + 3*button_x_border, start_y + 6 * button_height);
	_txtUnitDown = new Text(button_width, 2*button_height, width - button_width - button_x_border + 3*button_x_border, start_y + 7.5f * button_height);
	_btnEngineerUp = new ArrowButton (ARROW_BIG_UP, 1.4f*button_x_border, button_height-2, width - button_width - 4*button_x_border, start_y + 6 * button_height);
	_btnEngineerDown = new ArrowButton (ARROW_BIG_DOWN, 1.4f*button_x_border, button_height-2, width - button_width - 4*button_x_border, start_y + 7.5f * button_height);
	_btnUnitUp = new ArrowButton (ARROW_BIG_UP, 1.4f*button_x_border, button_height-2, width - 4*button_x_border, start_y + 6 * button_height);
	_btnUnitDown = new ArrowButton (ARROW_BIG_DOWN, 1.4f*button_x_border, button_height-2, width - 4*button_x_border, start_y + 7.5f * button_height);
	_txtAllocated = new Text(button_width, 2*button_height, width - button_width - 5*button_x_border, start_y + 4 * button_height);
	_txtTodo = new Text(button_width, 2*button_height, width - 5*button_x_border, start_y + 4 * button_height);
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_txtTitle);
	add(_txtAvailableEngineer);
	add(_txtAvailableSpace);
	add(_txtAllocatedEngineer);
	add(_txtAllocated);
	add(_txtUnitToProduce);
	add(_txtTodo);
	add(_txtEngineerUp);
	add(_txtEngineerDown);
	add(_btnEngineerUp);
	add(_btnEngineerDown);
	add(_txtUnitUp);
	add(_txtUnitDown);
	add(_btnUnitUp);
	add(_btnUnitDown);
	add(_btnOk);
	add(_btnStop);
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));
	_txtTitle->setColor(Palette::blockOffset(15)+1);
	_txtTitle->setText(_game->getLanguage()->getString(_item ? _item->getName() : _production->getRules()->getName()));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtAvailableEngineer->setColor(Palette::blockOffset(15)+1);
	_txtAvailableEngineer->setSecondaryColor(Palette::blockOffset(13));
	_txtAvailableSpace->setColor(Palette::blockOffset(15)+1);
	_txtAvailableSpace->setSecondaryColor(Palette::blockOffset(13));

	_txtAllocatedEngineer->setColor(Palette::blockOffset(15)+1);
	_txtAllocatedEngineer->setText(_game->getLanguage()->getString("STR_ENGINEERS__ALLOCATED"));
	_txtAllocatedEngineer->setBig();

	_txtAllocated->setColor(Palette::blockOffset(15)+1);
	_txtAllocated->setSecondaryColor(Palette::blockOffset(13));
	_txtAllocated->setBig();
	_txtTodo->setColor(Palette::blockOffset(15)+1);
	_txtTodo->setSecondaryColor(Palette::blockOffset(13));
	_txtTodo->setBig();

	_txtUnitToProduce->setColor(Palette::blockOffset(15)+1);
	_txtUnitToProduce->setText(_game->getLanguage()->getString("STR_TOTAL_TO_PRODUCE"));
	_txtUnitToProduce->setBig();

	_txtEngineerUp->setColor(Palette::blockOffset(15)+1);
	_txtEngineerUp->setText(_game->getLanguage()->getString("STR_INCREASE_UC"));
	_txtEngineerDown->setColor(Palette::blockOffset(15)+1);
	_txtEngineerDown->setText(_game->getLanguage()->getString("STR_DECREASE_UC"));
	_btnEngineerUp->setColor(Palette::blockOffset(15)+1);
	_btnEngineerUp->onMousePress((ActionHandler)&ManufactureInfoState::moreEngineerPress);
	_btnEngineerUp->onMouseRelease((ActionHandler)&ManufactureInfoState::moreEngineerRelease);
	_btnEngineerUp->onMouseClick((ActionHandler)&ManufactureInfoState::moreEngineerClick, 0);

	_btnEngineerDown->setColor(Palette::blockOffset(15)+1);
	_btnEngineerDown->onMousePress((ActionHandler)&ManufactureInfoState::lessEngineerPress);
	_btnEngineerDown->onMouseRelease((ActionHandler)&ManufactureInfoState::lessEngineerRelease);
	_btnEngineerDown->onMouseClick((ActionHandler)&ManufactureInfoState::lessEngineerClick, 0);

	_btnUnitUp->setColor(Palette::blockOffset(15)+1);
	_btnUnitUp->onMousePress((ActionHandler)&ManufactureInfoState::moreUnitPress);
	_btnUnitUp->onMouseRelease((ActionHandler)&ManufactureInfoState::moreUnitRelease);
	_btnUnitUp->onMouseClick((ActionHandler)&ManufactureInfoState::moreUnitClick, 0);

	_btnUnitDown->setColor(Palette::blockOffset(15)+1);
	_btnUnitDown->onMousePress((ActionHandler)&ManufactureInfoState::lessUnitPress);
	_btnUnitDown->onMouseRelease((ActionHandler)&ManufactureInfoState::lessUnitRelease);
	_btnUnitDown->onMouseClick((ActionHandler)&ManufactureInfoState::lessUnitClick, 0);

	_txtUnitUp->setColor(Palette::blockOffset(15)+1);
	_txtUnitUp->setText(_game->getLanguage()->getString("STR_INCREASE_UC"));
	_txtUnitDown->setColor(Palette::blockOffset(15)+1);
	_txtUnitDown->setText(_game->getLanguage()->getString("STR_DECREASE_UC"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ManufactureInfoState::btnOkClick);

	_btnStop->setColor(Palette::blockOffset(13)+10);
	_btnStop->setText(_game->getLanguage()->getString("STR_STOP_PRODUCTION"));
	_btnStop->onMouseClick((ActionHandler)&ManufactureInfoState::btnStopClick);
	if(!_production)
	{
		_production = new Production (_item, 0);
		_base->addProduction(_production);
	}
	setAssignedEngineer();

	_timerMoreEngineer = new Timer(30);
	_timerLessEngineer = new Timer(30);
	_timerMoreUnit = new Timer(30);
	_timerLessUnit = new Timer(30);
	_timerMoreEngineer->onTimer((StateHandler)&ManufactureInfoState::onMoreEngineer);
	_timerLessEngineer->onTimer((StateHandler)&ManufactureInfoState::onLessEngineer);
	_timerMoreUnit->onTimer((StateHandler)&ManufactureInfoState::onMoreUnit);
	_timerLessUnit->onTimer((StateHandler)&ManufactureInfoState::onLessUnit);
}

/**
 * Stop this Production. Return to previous screen
 * @param action a pointer to an Action
*/
void ManufactureInfoState::btnStopClick (Action *)
{
	_base->removeProduction(_production);
	exitState();
}

/**
 * Start this Production(if new). Return to previous screen
 * @param action a pointer to an Action
*/
void ManufactureInfoState::btnOkClick (Action *)
{
	if(_item)
	{
		_production->startItem(_base, _game->getSavedGame());
	}
	exitState();
}

/**
 * Return to previous screen
*/
void ManufactureInfoState::exitState()
{
	_game->popState();
	if(_item)
	{
		_game->popState();
	}
}

/**
 * Update display of assigned/available engineer/workshop space
 */
void ManufactureInfoState::setAssignedEngineer()
{
	int availableEngineer = _base->getAvailableEngineers();
	int availableWorkSpace = _base->getFreeWorkshops();
	std::wstringstream s1;
	s1 << _game->getLanguage()->getString("STR_ENGINEERS_AVAILABLE_UC") << L"\x01" << availableEngineer;
	_txtAvailableEngineer->setText(s1.str());
	std::wstringstream s2;
	s2 << _game->getLanguage()->getString("STR_WORKSHOP_SPACE_AVAILABLE_UC") << L"\x01" << availableWorkSpace;
	_txtAvailableSpace->setText(s2.str());
	std::wstringstream s3;
	s3 << L">\x01" << _production->getAssignedEngineers();
	_txtAllocated->setText(s3.str());
	std::wstringstream s4;
	s4 << L">\x01";
	if (Options::getBool("allowAutoSellProduction") && _production->getAmountTotal() == std::numeric_limits<int>::max())
		s4 << "$$$";
	else s4 << _production->getAmountTotal();
	_txtTodo->setText(s4.str());
}

/**
 * Start the timerMoreEngineer
 * @param action a pointer to an Action
*/
void ManufactureInfoState::moreEngineerPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerMoreEngineer->start();
}

/**
 * Stop the timerMoreEngineer
 * @param action a pointer to an Action
*/
void ManufactureInfoState::moreEngineerRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerMoreEngineer->stop();
}

/**
 * Allocate all engineers
 * @param action a pointer to an Action
*/
void ManufactureInfoState::moreEngineerClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		int assigned = _production->getAssignedEngineers();
		int availableEngineer = _base->getAvailableEngineers();
		int availableWorkSpace = _base->getFreeWorkshops();
		if (availableEngineer > 0 && availableWorkSpace > 0)
		{
			int change=std::min(availableEngineer, availableWorkSpace);
			_production->setAssignedEngineers(assigned+=change);
			_base->setEngineers(_base->getEngineers()-change);
			setAssignedEngineer();
		}
	}
}

/**
 * Start the timerLessEngineer
 * @param action a pointer to an Action
*/
void ManufactureInfoState::lessEngineerPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLessEngineer->start();
}

/**
 * Stop the timerLessEngineer
 * @param action a pointer to an Action
*/
void ManufactureInfoState::lessEngineerRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLessEngineer->stop();
}

/**
 * Allocate 0 engineers
 * @param action a pointer to an Action
*/
void ManufactureInfoState::lessEngineerClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		int assigned = _production->getAssignedEngineers();
		if (assigned > 0)
		{
			_production->setAssignedEngineers(0);
			_base->setEngineers(_base->getEngineers()+assigned);
			setAssignedEngineer();
		}
	}
}

/**
 * Start the timerMoreUnit
 * @param action a pointer to an Action
*/
void ManufactureInfoState::moreUnitPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && _production->getAmountTotal() < std::numeric_limits<int>::max())
		_timerMoreUnit->start();
}

/**
 * Stop the timerMoreUnit
 * @param action a pointer to an Action
*/
void ManufactureInfoState::moreUnitRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerMoreUnit->stop();
}

/**
 * This could set a virtual infinite value for units
 * which could mean produce & sell automatically just like in UFOextender. :)
 * Perhaps someone will implement that feature later.
 * @param action a pointer to an Action
*/
void ManufactureInfoState::moreUnitClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		int more = _production->getAmountTotal ();
		if (_production->getRules()->getCategory() == "STR_CRAFT")
		{
			if (_base->getAvailableHangars() - _base->getUsedHangars() > 0)
			{
				_production->setAmountTotal(std::max(more,_base->getAvailableHangars() - _base->getUsedHangars()));
				setAssignedEngineer();
			}
		}
		else
		{
			if (Options::getBool("allowAutoSellProduction")) _production->setAmountTotal(std::numeric_limits<int>::max());
			else _production->setAmountTotal(std::max(more,999));
			setAssignedEngineer();
		}
	}
}

/**
 * Start the timerLessUnit
 * @param action a pointer to an Action
*/
void ManufactureInfoState::lessUnitPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (Options::getBool("allowAutoSellProduction") && _production->getAmountTotal() == std::numeric_limits<int>::max())
			_production->setAmountTotal(std::max(_production->getAmountProduced()+1,999));
		_timerLessUnit->start();
	}
}

/**
 * Stop the timerLessUnit
 * @param action a pointer to an Action
*/
void ManufactureInfoState::lessUnitRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLessUnit->stop();
}

/**
 * Build no more unit
 * @param action a pointer to an Action
*/
void ManufactureInfoState::lessUnitClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		int less = _production->getAmountTotal ();
		if (less > (_production->getAmountProduced () + 1))
		{
			_production->setAmountTotal(_production->getAmountProduced () + 1);
			setAssignedEngineer();
		}
	}
}

/**
 * Assign one more engineer(if possible)
 */
void ManufactureInfoState::onMoreEngineer()
{
	int assigned = _production->getAssignedEngineers();
	int availableEngineer = _base->getAvailableEngineers();
	int availableWorkSpace = _base->getFreeWorkshops();
	if (availableEngineer > 0 && availableWorkSpace > 0)
	{
		_production->setAssignedEngineers(++assigned);
		_base->setEngineers(_base->getEngineers()-1);
		setAssignedEngineer();
	}
}

/**
 * Remove one engineer(if possible)
 */
void ManufactureInfoState::onLessEngineer()
{
	int assigned = _production->getAssignedEngineers();
	if(assigned > 0)
	{
		_production->setAssignedEngineers(--assigned);
		_base->setEngineers(_base->getEngineers()+1);
		setAssignedEngineer();
	}
}

/**
 * Build one more unit
 */
void ManufactureInfoState::onMoreUnit()
{
	int more = _production->getAmountTotal ();
	if (_production->getRules()->getCategory() == "STR_CRAFT" && _base->getAvailableHangars() - _base->getUsedHangars() == 0)
	{
		_timerMoreUnit->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NO_FREE_HANGARS_FOR_CRAFT_PRODUCTION", Palette::blockOffset(15)+1, "BACK17.SCR", 6));
	}
	else
	{
		_production->setAmountTotal (++more);
		setAssignedEngineer();
	}
}

/**
 * Build one less unit(if possible)
 */
void ManufactureInfoState::onLessUnit()
{
	int less = _production->getAmountTotal ();
	if(less > (_production->getAmountProduced () + 1))
	{
		_production->setAmountTotal (--less);
		setAssignedEngineer();
	}
}

/**
 * Runs state functionality every cycle(used to update the timer).
 */
void ManufactureInfoState::think()
{
	State::think();
	_timerMoreEngineer->think(this, 0);
	_timerLessEngineer->think(this, 0);
	_timerMoreUnit->think(this, 0);
	_timerLessUnit->think(this, 0);
}
}
