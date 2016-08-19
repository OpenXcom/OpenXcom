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
#include "TargetInfoState.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Savegame/Target.h"
#include "../Engine/Options.h"
#include "InterceptState.h"
#include "../Engine/Action.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Target Info window.
 * @param game Pointer to the core game.
 * @param target Pointer to the target to show info from.
 * @param globe Pointer to the Geoscape globe.
 */
TargetInfoState::TargetInfoState(Target *target, Globe *globe) : _target(target), _globe(globe)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 192, 120, 32, 40, POPUP_BOTH);
	_btnIntercept = new TextButton(160, 12, 48, 124);
	_btnOk = new TextButton(160, 12, 48, 140);
	_edtTitle = new TextEdit(this, 182, 32, 37, 46);
	_txtTargetted = new Text(182, 9, 37, 78);
	_txtFollowers = new Text(182, 40, 37, 88);

	// Set palette
	setInterface("targetInfo");

	add(_window, "window", "targetInfo");
	add(_btnIntercept, "button", "targetInfo");
	add(_btnOk, "button", "targetInfo");
	add(_edtTitle, "text2", "targetInfo");
	add(_txtTargetted, "text1", "targetInfo");
	add(_txtFollowers, "text1", "targetInfo");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_btnIntercept->setText(tr("STR_INTERCEPT"));
	_btnIntercept->onMouseClick((ActionHandler)&TargetInfoState::btnInterceptClick);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&TargetInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&TargetInfoState::btnOkClick, Options::keyCancel);

	_edtTitle->setBig();
	_edtTitle->setAlign(ALIGN_CENTER);
	_edtTitle->setVerticalAlign(ALIGN_MIDDLE);
	_edtTitle->setWordWrap(true);
	_edtTitle->setText(_target->getName(_game->getLanguage()));
	_edtTitle->onChange((ActionHandler)&TargetInfoState::edtTitleChange);

	_txtTargetted->setAlign(ALIGN_CENTER);
	_txtTargetted->setText(tr("STR_TARGETTED_BY"));
	_txtFollowers->setAlign(ALIGN_CENTER);
	std::wostringstream ss;
	for (std::vector<Target*>::iterator i = _target->getFollowers()->begin(); i != _target->getFollowers()->end(); ++i)
	{
		ss << (*i)->getName(_game->getLanguage()) << L'\n';
	}
	_txtFollowers->setText(ss.str());
}

/**
 *
 */
TargetInfoState::~TargetInfoState()
{

}

/**
 * Picks a craft to intercept the UFO.
 * @param action Pointer to an action.
 */
void TargetInfoState::btnInterceptClick(Action *)
{
	_game->pushState(new InterceptState(_globe, 0, _target));
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void TargetInfoState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Changes the target name.
 * @param action Pointer to an action.
 */
void TargetInfoState::edtTitleChange(Action *action)
{
	if (_edtTitle->getText() == _target->getDefaultName(_game->getLanguage()))
	{
		_target->setName(L"");
	}
	else
	{
		_target->setName(_edtTitle->getText());
	}
	if (action->getDetails()->key.keysym.sym == SDLK_RETURN ||
		action->getDetails()->key.keysym.sym == SDLK_KP_ENTER)
	{
		_edtTitle->setText(_target->getName(_game->getLanguage()));
	}
}

}
