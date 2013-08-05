/*
 * Copyright 2010-2013 OpenXcom Developers.
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
 * Initializes all elements in the Production settings screen (new Production).
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param item The RuleManufacture to produce.
 */
ManufactureInfoState::ManufactureInfoState (Game * game, Base * base, RuleManufacture * item) : State (game), _base(base), _item(item), _production(0)
{
	buildUi();
}

/**
 * Initializes all elements in the Production settings screen (modifying Production).
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param production The Production to modify.
 */
ManufactureInfoState::ManufactureInfoState (Game * game, Base * base, Production * production) : State (game), _base(base), _item(0), _production(production)
{
	buildUi();
}

/**
 * Builds screen User Interface.
 */
void ManufactureInfoState::buildUi()
{
	_changeValueByMouseWheel = Options::getInt("changeValueByMouseWheel");

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

	_surface1 = new InteractiveSurface((_btnEngineerUp->getX()+_btnEngineerUp->getWidth()+_txtUnitToProduce->getX()) / 2, height, start_x, start_y);
	_surface1->onMouseClick((ActionHandler)&ManufactureInfoState::handleWheelEngineer, 0);

	_surface2 = new InteractiveSurface(_surface1->getWidth(), height, start_x + _surface1->getWidth(), start_y);
	_surface2->onMouseClick((ActionHandler)&ManufactureInfoState::handleWheelUnit, 0);

	add(_surface1);
	add(_surface2);
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

	centerAllSurfaces();

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
	_btnOk->onKeyboardPress((ActionHandler)&ManufactureInfoState::btnOkClick, (SDLKey)Options::getInt("keyOk"));
	_btnOk->onKeyboardPress((ActionHandler)&ManufactureInfoState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_btnStop->setColor(Palette::blockOffset(13)+10);
	_btnStop->setText(_game->getLanguage()->getString("STR_STOP_PRODUCTION"));
	_btnStop->onMouseClick((ActionHandler)&ManufactureInfoState::btnStopClick);
	if(!_production)
	{
		_production = new Production (_item, 0);
		_base->addProduction(_production);
	}
	setAssignedEngineer();

	_timerMoreEngineer = new Timer(250);
	_timerLessEngineer = new Timer(250);
	_timerMoreUnit = new Timer(250);
	_timerLessUnit = new Timer(250);
	_timerMoreEngineer->onTimer((StateHandler)&ManufactureInfoState::onMoreEngineer);
	_timerLessEngineer->onTimer((StateHandler)&ManufactureInfoState::onLessEngineer);
	_timerMoreUnit->onTimer((StateHandler)&ManufactureInfoState::onMoreUnit);
	_timerLessUnit->onTimer((StateHandler)&ManufactureInfoState::onLessUnit);
}

/**
 * Stops this Production. Returns to the previous screen.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::btnStopClick (Action *)
{
	_base->removeProduction(_production);
	exitState();
}

/**
 * Starts this Production (if new). Returns to the previous screen.
 * @param action A pointer to an Action.
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
 * Returns to the previous screen.
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
 * Updates display of assigned/available engineer/workshop space.
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
 * Adds given number of engineers to the project if possible.
 * @param change How much we want to add.
 */
void ManufactureInfoState::moreEngineer(int change)
{
	if (0 >= change) return;
	int availableEngineer = _base->getAvailableEngineers();
	int availableWorkSpace = _base->getFreeWorkshops();
	if (availableEngineer > 0 && availableWorkSpace > 0)
	{
		change = std::min(std::min(availableEngineer, availableWorkSpace), change);
		_production->setAssignedEngineers(_production->getAssignedEngineers()+change);
		_base->setEngineers(_base->getEngineers()-change);
		setAssignedEngineer();
	}
}

/**
 * Starts the timerMoreEngineer.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreEngineerPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerMoreEngineer->start();
}

/**
 * Stops the timerMoreEngineer.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreEngineerRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerMoreEngineer->setInterval(250);
		_timerMoreEngineer->stop();
	}
}

/**
 * Allocates all engineers.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreEngineerClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) moreEngineer(std::numeric_limits<int>::max());
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) moreEngineer(1);
}

/**
 * Removes the given number of engineers from the project if possible.
 * @param change How much we want to subtract.
 */
void ManufactureInfoState::lessEngineer(int change)
{
	if (0 >= change) return;
	int assigned = _production->getAssignedEngineers();
	if(assigned > 0)
	{
		change = std::min(assigned, change);
		_production->setAssignedEngineers(assigned-change);
		_base->setEngineers(_base->getEngineers()+change);
		setAssignedEngineer();
	}
}

/**
 * Starts the timerLessEngineer.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessEngineerPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLessEngineer->start();
}

/**
 * Stops the timerLessEngineer.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessEngineerRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerLessEngineer->setInterval(250);
		_timerLessEngineer->stop();
	}
}

/**
 * Removes engineers from the production.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessEngineerClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) lessEngineer(std::numeric_limits<int>::max());
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) lessEngineer(1);
}

/**
 * Adds given number of units to produce to the project if possible.
 * @param change How much we want to add.
 */
void ManufactureInfoState::moreUnit(int change)
{
	if (0 >= change) return;
	if (_production->getRules()->getCategory() == "STR_CRAFT" && _base->getAvailableHangars() - _base->getUsedHangars() == 0)
	{
		_timerMoreUnit->stop();
		_game->pushState(new ErrorMessageState(_game, "STR_NO_FREE_HANGARS_FOR_CRAFT_PRODUCTION", Palette::blockOffset(15)+1, "BACK17.SCR", 6));
	}
	else
	{
		int units = _production->getAmountTotal();
		change = std::min(std::numeric_limits<int>::max()-units, change);
		if (_production->getRules()->getCategory() == "STR_CRAFT")
			change = std::min(_base->getAvailableHangars() - _base->getUsedHangars(), change);
		_production->setAmountTotal(units+change);
		setAssignedEngineer();
	}
}

/**
 * Starts the timerMoreUnit.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreUnitPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && _production->getAmountTotal() < std::numeric_limits<int>::max())
		_timerMoreUnit->start();
}

/**
 * Stops the timerMoreUnit.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreUnitRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerMoreUnit->setInterval(250);
		_timerMoreUnit->stop();
	}
}

/**
 * Increases the units to produce to 999 or to $$$ when allowAutoSellProduction is true.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreUnitClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		moreUnit(Options::getBool("allowAutoSellProduction") ? std::numeric_limits<int>::max() : (999 - _production->getAmountTotal()));
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) moreUnit(1);
}

/**
 * Removes the given number of units to produce from the project if possible.
 * @param change How much we want to subtract.
 */
void ManufactureInfoState::lessUnit(int change)
{
	if (0 >= change) return;
	if (Options::getBool("allowAutoSellProduction") && _production->getAmountTotal() == std::numeric_limits<int>::max())
		_production->setAmountTotal(std::max(_production->getAmountProduced()+1,999));
	int units = _production->getAmountTotal();
	change = std::min(units-(_production->getAmountProduced()+1), change);
	_production->setAmountTotal(units-change);
	setAssignedEngineer();
}

/**
 * Starts the timerLessUnit.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessUnitPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLessUnit->start();
}

/**
 * Stops the timerLessUnit.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessUnitRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerLessUnit->setInterval(250);
		_timerLessUnit->stop();
	}
}

/**
 * Decreases the units to produce.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessUnitClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) lessUnit(std::numeric_limits<int>::max());
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) lessUnit(1);
}

/**
 * Assigns one more engineer (if possible).
 */
void ManufactureInfoState::onMoreEngineer()
{
	_timerMoreEngineer->setInterval(50);
	moreEngineer(1);
}

/**
 * Removes one engineer (if possible).
 */
void ManufactureInfoState::onLessEngineer()
{
	_timerLessEngineer->setInterval(50);
	lessEngineer(1);
}

/**
 * Increases or decreases the Engineers according the mouse-wheel used.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::handleWheelEngineer(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP) moreEngineer(_changeValueByMouseWheel);
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN) lessEngineer(_changeValueByMouseWheel);
}

/**
 * Builds one more unit.
 */
void ManufactureInfoState::onMoreUnit()
{
	_timerMoreUnit->setInterval(50);
	moreUnit(1);
}

/**
 * Builds one less unit( if possible).
 */
void ManufactureInfoState::onLessUnit()
{
	_timerLessUnit->setInterval(50);
	lessUnit(1);
}

/**
 * Increases or decreases the Units to produce according the mouse-wheel used.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::handleWheelUnit(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP) moreUnit(_changeValueByMouseWheel);
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN) lessUnit(_changeValueByMouseWheel);
}

/**
 * Runs state functionality every cycle (used to update the timer).
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
