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
#include "OptionsBaseState.h"
#include <SDL.h>
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Engine/Screen.h"
#include "../Resource/ResourcePack.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Engine/Action.h"
#include "MainMenuState.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Battlescape/BattlescapeState.h"
#include "OptionsVideoState.h"
#include "OptionsAudioState.h"
#include "OptionsNoAudioState.h"
#include "OptionsControlsState.h"
#include "OptionsGeoscapeState.h"
#include "OptionsBattlescapeState.h"
#include "OptionsAdvancedState.h"
#include "OptionsModsState.h"
#include "OptionsDefaultsState.h"
#include "OptionsConfirmState.h"
#include "StartState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Options window.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsBaseState::OptionsBaseState(OptionsOrigin origin) : _origin(origin)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);

	_btnVideo = new TextButton(80, 16, 8, 8);
	_btnAudio = new TextButton(80, 16, 8, 28);
	_btnControls = new TextButton(80, 16, 8, 48);
	_btnGeoscape = new TextButton(80, 16, 8, 68);
	_btnBattlescape = new TextButton(80, 16, 8, 88);
	_btnAdvanced = new TextButton(80, 16, 8, 108);
	_btnMods = new TextButton(80, 16, 8, 128);

	_btnOk = new TextButton(100, 16, 8, 176);
	_btnCancel = new TextButton(100, 16, 110, 176);
	_btnDefault = new TextButton(100, 16, 212, 176);

	_txtTooltip = new Text(305, 25, 8, 148);

	// Set palette
	setInterface("optionsMenu", false, _origin == OPT_BATTLESCAPE);

	add(_window, "window", "optionsMenu");

	add(_btnVideo, "button", "optionsMenu");
	add(_btnAudio, "button", "optionsMenu");
	add(_btnControls, "button", "optionsMenu");
	add(_btnGeoscape, "button", "optionsMenu");
	add(_btnBattlescape, "button", "optionsMenu");
	add(_btnAdvanced, "button", "optionsMenu");
	add(_btnMods, "button", "optionsMenu");

	add(_btnOk, "button", "optionsMenu");
	add(_btnCancel, "button", "optionsMenu");
	add(_btnDefault, "button", "optionsMenu");

	add(_txtTooltip, "tooltip", "optionsMenu");

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnVideo->setText(tr("STR_VIDEO"));
	_btnVideo->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress, SDL_BUTTON_LEFT);

	_btnAudio->setText(tr("STR_AUDIO"));
	_btnAudio->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress, SDL_BUTTON_LEFT);

	_btnControls->setText(tr("STR_CONTROLS"));
	_btnControls->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress, SDL_BUTTON_LEFT);

	_btnGeoscape->setText(tr("STR_GEOSCAPE_UC"));
	_btnGeoscape->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress, SDL_BUTTON_LEFT);

	_btnBattlescape->setText(tr("STR_BATTLESCAPE_UC"));
	_btnBattlescape->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress, SDL_BUTTON_LEFT);

	_btnAdvanced->setText(tr("STR_ADVANCED"));
	_btnAdvanced->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress, SDL_BUTTON_LEFT);

	_btnMods->setText(tr("STR_MODS"));
	_btnMods->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress, SDL_BUTTON_LEFT);
	_btnMods->setVisible(_origin == OPT_MENU); // Mods require a restart, don't enable them in-game

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&OptionsBaseState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&OptionsBaseState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&OptionsBaseState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&OptionsBaseState::btnCancelClick, Options::keyCancel);

	_btnDefault->setText(tr("STR_RESTORE_DEFAULTS"));
	_btnDefault->onMouseClick((ActionHandler)&OptionsBaseState::btnDefaultClick);

	_txtTooltip->setWordWrap(true);
}

/**
 *
 */
OptionsBaseState::~OptionsBaseState()
{

}

void OptionsBaseState::restart(OptionsOrigin origin)
{
	if (origin == OPT_MENU)
	{
		_game->setState(new MainMenuState);
	}
	else if (origin == OPT_GEOSCAPE)
	{
		_game->setState(new GeoscapeState);
	}
	else if (origin == OPT_BATTLESCAPE)
	{
		_game->setState(new GeoscapeState);
		BattlescapeState *bs = new BattlescapeState;
		_game->pushState(bs);
		_game->getSavedGame()->getSavedBattle()->setBattleState(bs);
	}
}

/**
 * Initializes UI colors according to origin.
 */
void OptionsBaseState::init()
{
	State::init();
	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}
}

/**
 * Handles the pressed-button state for the category buttons.
 * @param button Button to press.
 */
void OptionsBaseState::setCategory(TextButton *button)
{
	_group = button;
	_btnVideo->setGroup(&_group);
	_btnAudio->setGroup(&_group);
	_btnControls->setGroup(&_group);
	_btnGeoscape->setGroup(&_group);
	_btnBattlescape->setGroup(&_group);
	_btnAdvanced->setGroup(&_group);
	_btnMods->setGroup(&_group);
}

/**
 * Saves the new options and returns to the proper origin screen.
 * @param action Pointer to an action.
 */
void OptionsBaseState::btnOkClick(Action *)
{
	int dX = Options::baseXResolution;
	int dY = Options::baseYResolution;
	Screen::updateScale(Options::battlescapeScale, Options::newBattlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, _origin == OPT_BATTLESCAPE);
	Screen::updateScale(Options::geoscapeScale, Options::newGeoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, _origin != OPT_BATTLESCAPE);
	dX = Options::baseXResolution - dX;
	dY = Options::baseYResolution - dY;
	recenter(dX, dY);
	Options::switchDisplay();
	Options::save();
	_game->loadLanguage(Options::language);
	SDL_WM_GrabInput(Options::captureMouse);
	_game->getScreen()->resetDisplay();
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
	if (Options::reload && _origin == OPT_MENU)
	{
		_game->setState(new StartState);
	}
	else
	{
		// Confirm any video options changes
		if (Options::displayWidth != Options::newDisplayWidth ||
			Options::displayHeight != Options::newDisplayHeight ||
			Options::useOpenGL != Options::newOpenGL ||
			Options::useScaleFilter != Options::newScaleFilter ||
			Options::useHQXFilter != Options::newHQXFilter ||
			Options::useOpenGLShader != Options::newOpenGLShader)
		{
			_game->pushState(new OptionsConfirmState(_origin));
		}
		else
		{
			restart(_origin);
		}
	}
}

/**
 * Loads previous options and returns to the previous screen.
 * @param action Pointer to an action.
 */
void OptionsBaseState::btnCancelClick(Action *)
{
	Options::reload = false;
	Options::load();
	SDL_WM_GrabInput(Options::captureMouse);
	Screen::updateScale(Options::newBattlescapeScale, Options::battlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, _origin == OPT_BATTLESCAPE);
	Screen::updateScale(Options::newGeoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, _origin != OPT_BATTLESCAPE);
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
	_game->popState();
}

/**
 * Restores the Options to default settings.
 * @param action Pointer to an action.
 */
void OptionsBaseState::btnDefaultClick(Action *action)
{
	_game->pushState(new OptionsDefaultsState(_origin, this));
}

void OptionsBaseState::btnGroupPress(Action *action)
{
	Surface *sender = action->getSender();
	//if (sender != _group)
	{
		_game->popState();
		if (sender == _btnVideo)
		{
			_game->pushState(new OptionsVideoState(_origin));
		}
		else if (sender == _btnAudio)
		{
			if (!Options::mute)
			{
				_game->pushState(new OptionsAudioState(_origin));
			}
			else
			{
				_game->pushState(new OptionsNoAudioState(_origin));
			}
		}
		else if (sender == _btnControls)
		{
			_game->pushState(new OptionsControlsState(_origin));
		}
		else if (sender == _btnGeoscape)
		{
			_game->pushState(new OptionsGeoscapeState(_origin));
		}
		else if (sender == _btnBattlescape)
		{
			_game->pushState(new OptionsBattlescapeState(_origin));
		}
		else if (sender == _btnAdvanced)
		{
			_game->pushState(new OptionsAdvancedState(_origin));
		}
		else if (sender == _btnMods)
		{
			_game->pushState(new OptionsModsState(_origin));
		}
	}
}

/**
* Shows a tooltip for the appropriate button.
* @param action Pointer to an action.
*/
void OptionsBaseState::txtTooltipIn(Action *action)
{
	_currentTooltip = action->getSender()->getTooltip();
	_txtTooltip->setText(tr(_currentTooltip));
}

/**
* Clears the tooltip text.
* @param action Pointer to an action.
*/
void OptionsBaseState::txtTooltipOut(Action *action)
{
	if (_currentTooltip == action->getSender()->getTooltip())
	{
		_txtTooltip->setText(L"");
	}
}

/**
 * Updates the scale.
 * @param dX delta of X;
 * @param dY delta of Y;
 */
void OptionsBaseState::resize(int &dX, int &dY)
{
	Options::newDisplayWidth = Options::displayWidth;
	Options::newDisplayHeight = Options::displayHeight;
	State::resize(dX, dY);

}
}
