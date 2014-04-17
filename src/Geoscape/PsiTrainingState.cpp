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
#include <sstream>
#include "PsiTrainingState.h"
#include "../Engine/Game.h"
#include "../Engine/Screen.h"
#include "../Engine/Action.h"
#include "../Engine/Timer.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/ArrowButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "GeoscapeState.h"
#include "AllocatePsiTrainingState.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Psi Training screen.
 * @param game Pointer to the core game.
 */
PsiTrainingState::PsiTrainingState(Game *game) : State(game), _timerUp(0), _timerDown(0), _startIndex(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(300, 17, 10, 16);
	_btnOk = new TextButton(160, 14, 80, 174);

	// Set palette
	setPalette("PAL_BASESCAPE", 7);

	add(_window);
	add(_btnOk);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+6);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PsiTrainingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&PsiTrainingState::btnOkClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_PSIONIC_TRAINING"));

	int buttons = 0;
	for(std::vector<Base*>::const_iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end(); ++b)
	{
		if((*b)->getAvailablePsiLabs())
		{
			TextButton *btnBase = new TextButton(160, 14, 80, 40 + 16 * buttons);
			btnBase->setColor(Palette::blockOffset(15) + 6);
			btnBase->onMouseClick((ActionHandler)&PsiTrainingState::btnBaseXClick);
			btnBase->setText((*b)->getName());
			add(btnBase);
			_bases.push_back(*b);
			_btnBases.push_back(btnBase);
			++buttons;
			if (buttons >= 8)
			{
				break;
			}
		}
	}

	if (_bases.size() > 8)
	{
		_up = new ArrowButton(ARROW_BIG_UP, 13, 14, 242, 40);
		_down = new ArrowButton(ARROW_BIG_DOWN, 13, 14, 242, 152);
		add(_up);
		add(_down);
		_up->setColor(Palette::blockOffset(13)+10);
		_up->onMousePress((ActionHandler)&PsiTrainingState::upPress);
		_up->onMouseRelease((ActionHandler)&PsiTrainingState::upRelease);
		_up->onMouseClick((ActionHandler)&PsiTrainingState::upClick, 0);
		_up->setVisible(false);
		_down->setColor(Palette::blockOffset(13)+10);
		_down->onMousePress((ActionHandler)&PsiTrainingState::downPress);
		_down->onMouseRelease((ActionHandler)&PsiTrainingState::downRelease);
		_down->onMouseClick((ActionHandler)&PsiTrainingState::downClick, 0);
		_down->setVisible(true);
		_timerUp = new Timer(250);
		_timerDown = new Timer(250);
		_timerUp->onTimer((StateHandler)&PsiTrainingState::onTimerUp);
		_timerDown->onTimer((StateHandler)&PsiTrainingState::onTimerDown);
	}

	centerAllSurfaces();
}
/**
 *
 */
PsiTrainingState::~PsiTrainingState()
{
	if (0 != _timerUp) delete _timerUp;
	if (0 != _timerDown) delete _timerDown;
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void PsiTrainingState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the allocation screen for the corresponding base.
 * @param action Pointer to an action.
 */
void PsiTrainingState::btnBaseXClick(Action *action)
{
	for (size_t i = 0; i < _btnBases.size(); ++i)
	{
		if (action->getSender() == _btnBases[i])
		{
			_game->pushState(new AllocatePsiTrainingState(_game, _bases.at(_startIndex+i)));
			break;
		}
	}
}

/**
 Handler for pressing the up arrowbutton.
 * @param action Pointer to an action.
 */
void PsiTrainingState::upPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerUp->start();
}

/**
 Handler for releasing the up arrowbutton.
 * @param action Pointer to an action.
 */
void PsiTrainingState::upRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerUp->setInterval(250);
		_timerUp->stop();
	}
}

/**
 Handler for clicking the up arrowbutton.
 * @param action Pointer to an action.
 */
void PsiTrainingState::upClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) scrollUp(std::numeric_limits<int>::max());
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) scrollUp(1);
}

/**
 Handler for pressing the down arrowbutton.
 * @param action Pointer to an action.
 */
void PsiTrainingState::downPress(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerDown->start();
}

/**
 Handler for releasing the down arrowbutton.
 * @param action Pointer to an action.
 */
void PsiTrainingState::downRelease(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerDown->setInterval(250);
		_timerDown->stop();
	}
}

/**
 Handler for clicking the down arrowbutton.
 * @param action Pointer to an action.
 */
void PsiTrainingState::downClick(Action * action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) scrollDown(std::numeric_limits<int>::max());
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) scrollDown(1);
}

/**
 Event handler for _timerUp.
 */
void PsiTrainingState::onTimerUp()
{
	_timerUp->setInterval(50);
	scrollUp(1);
}

/**
 Event handler for _timerDown.
 */
void PsiTrainingState::onTimerDown()
{
	_timerDown->setInterval(50);
	scrollDown(1);
}

/**
 Scrolls up by the number of change.
 * @param change The number of lines to scroll up.
 */
void PsiTrainingState::scrollUp(int change)
{
	if (0 >= change || 0 == _startIndex) return;
	if (change > _startIndex) _startIndex = 0; else _startIndex -= change;
	updateButtons();
}

/**
 Scrolls down by the number of change.
 * @param change The number of lines to scroll down.
 */
void PsiTrainingState::scrollDown(int change)
{
	int max = _bases.size() - 8;
	if (0 >= change || max == _startIndex) return;
	// we need to check 'change' alone first, because _startIndex+change can overflow
	if (change > max || _startIndex+change > max) _startIndex = max; else _startIndex += change;
	updateButtons();
}

/**
 Updates the names of the _btnBases, and arrowButtons.
 */
void PsiTrainingState::updateButtons()
{
	for (int i = 0; i < _btnBases.size(); ++i)
	{
		_btnBases.at(i)->setText(_bases.at(_startIndex+i)->getName());
	}
	bool b;
	b = _startIndex > 0;
	if (_up->getVisible() && !b) _up->unpress(this); // This is a workaround to avoid a crash!
	_up->setVisible(b);
	b = _startIndex < _bases.size() - 8;
	if (_down->getVisible() && !b) _down->unpress(this); // This is a workaround to avoid a crash!
	_down->setVisible(b);
}

/**
 Runs state functionality every cycle.
 */
void PsiTrainingState::think()
{
	State::think();
	_timerUp->think(this, 0);
	_timerDown->think(this, 0);
}

}
