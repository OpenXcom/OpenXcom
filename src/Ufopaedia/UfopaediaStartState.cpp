/*
 * Copyright 2010-2016 OpenXcom Developers.
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

#include "UfopaediaStartState.h"
#include "UfopaediaSelectState.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/ArrowButton.h"
#include "../Mod/Mod.h"
#include "../Engine/Timer.h"
#include "../fmath.h"

namespace OpenXcom
{
	const size_t UfopaediaStartState::CAT_MIN_BUTTONS = 9;
	const size_t UfopaediaStartState::CAT_MAX_BUTTONS = 10;

	UfopaediaStartState::UfopaediaStartState() : _offset(0), _scroll(0), _cats(_game->getMod()->getUfopaediaCategoryList())
	{
		_screen = false;

		// set background window
		_window = new Window(this, 256, 180, 32, 10, POPUP_BOTH);

		// set title
		_txtTitle = new Text(220, 17, 50, 33);

		// Set palette
		setInterface("ufopaedia");

		add(_window, "window", "ufopaedia");
		add(_txtTitle, "text", "ufopaedia");

		_btnOk = new TextButton(220, 12, 50, 167);
		add(_btnOk, "button1", "ufopaedia");

		// set buttons
		int y = 50;
		size_t numButtons = std::min(_cats.size(), CAT_MAX_BUTTONS);
		if (numButtons > CAT_MIN_BUTTONS)
			y -= 13 * (numButtons - CAT_MIN_BUTTONS);

		_btnScrollUp = new ArrowButton(ARROW_BIG_UP, 13, 14, 270, y);
		add(_btnScrollUp, "button1", "ufopaedia");
		_btnScrollDown = new ArrowButton(ARROW_BIG_DOWN, 13, 14, 270, 152);
		add(_btnScrollDown, "button1", "ufopaedia");

		for (size_t i = 0; i < numButtons; ++i)
		{
			TextButton *button = new TextButton(220, 12, 50, y);
			y += 13;

			add(button, "button1", "ufopaedia");

			button->onMouseClick((ActionHandler)&UfopaediaStartState::btnSectionClick);
			button->onMouseWheel((ActionHandler)&UfopaediaStartState::btnSectionWheel);

			_btnSections.push_back(button);
		}
		updateButtons();
		if (!_btnSections.empty())
			_txtTitle->setY(_btnSections.front()->getY() - _txtTitle->getHeight());

		centerAllSurfaces();

		_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(tr("STR_UFOPAEDIA"));

		_btnOk->setText(tr("STR_OK"));
		_btnOk->onMouseClick((ActionHandler)&UfopaediaStartState::btnOkClick);
		_btnOk->onKeyboardPress((ActionHandler)&UfopaediaStartState::btnOkClick, Options::keyCancel);
		_btnOk->onKeyboardPress((ActionHandler)&UfopaediaStartState::btnOkClick, Options::keyGeoUfopedia);

		_btnScrollUp->setVisible(_cats.size() > CAT_MAX_BUTTONS);
		_btnScrollUp->onMousePress((ActionHandler)&UfopaediaStartState::btnScrollUpPress);
		_btnScrollUp->onMouseRelease((ActionHandler)&UfopaediaStartState::btnScrollRelease);
		_btnScrollDown->setVisible(_cats.size() > CAT_MAX_BUTTONS);
		_btnScrollDown->onMousePress((ActionHandler)&UfopaediaStartState::btnScrollDownPress);
		_btnScrollDown->onMouseRelease((ActionHandler)&UfopaediaStartState::btnScrollRelease);

		_timerScroll = new Timer(50);
		_timerScroll->onTimer((StateHandler)&UfopaediaStartState::scroll);
	}

	/**
	 * Deletes timers.
	 */
	UfopaediaStartState::~UfopaediaStartState()
	{
		delete _timerScroll;
	}

	/**
	 * Run timers.
	 */
	void UfopaediaStartState::think()
	{
		State::think();
		_timerScroll->think(this, 0);
	}

	/**
	 * Returns to the previous screen.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnOkClick(Action *)
	{
		_game->popState();
	}

	/**
	 * Displays the list of articles for this section.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnSectionClick(Action *action)
	{
		for (size_t i = 0; i < _btnSections.size(); ++i)
		{
			if (action->getSender() == _btnSections[i])
			{
				_game->pushState(new UfopaediaSelectState(_cats[_offset + i]));
				break;
			}
		}
	}

	void UfopaediaStartState::btnSectionWheel(Action* action)
	{
		if (action->getDetails()->wheel.y > 0) btnScrollUpClick(action);
		else if (action->getDetails()->wheel.y < 0) btnScrollDownClick(action);
	}

	/**
	 * Starts scrolling the section buttons up.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnScrollUpPress(Action *)
	{
		_scroll = -1;
		_timerScroll->start();
	}

	/**
	 * Scrolls the section buttons up.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnScrollUpClick(Action *)
	{
		_scroll = -1;
		scroll();
	}

	/**
	 * Starts scrolling the section buttons down.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnScrollDownPress(Action *)
	{
		_scroll = 1;
		_timerScroll->start();
	}

	/**
	 * Scrolls the section buttons down.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnScrollDownClick(Action *)
	{
		_scroll = 1;
		scroll();
	}

	/**
	 * Stops scrolling the section buttons.
	 * @param action Pointer to an action.
	 */
	void UfopaediaStartState::btnScrollRelease(Action *)
	{
		_timerScroll->stop();
	}

	/**
	 * Offsets the list of section buttons.
	 */
	void UfopaediaStartState::scroll()
	{
		if (_cats.size() > CAT_MAX_BUTTONS)
		{
			_offset = Clamp(_offset + _scroll, 0, int(_cats.size() - CAT_MAX_BUTTONS));
			updateButtons();
		}
	}

	/**
	 * Updates the section button labels based on scroll.
	 */
	void UfopaediaStartState::updateButtons()
	{
		for (size_t i = 0; i < _btnSections.size(); ++i)
		{
			_btnSections[i]->setText(tr(_cats[_offset + i]));
		}
	}
}
