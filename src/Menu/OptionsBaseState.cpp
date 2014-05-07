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
OptionsBaseState::OptionsBaseState(Game *game, OptionsOrigin origin) : State(game), _origin(origin)
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
	if (_origin == OPT_BATTLESCAPE)
	{
		setPalette("PAL_BATTLESCAPE");
	}
	else
	{
		setPalette("PAL_GEOSCAPE", 0);
	}

	add(_window);

	add(_btnVideo);
	add(_btnAudio);
	add(_btnControls);
	add(_btnGeoscape);
	add(_btnBattlescape);
	add(_btnAdvanced);
	add(_btnMods);

	add(_btnOk);
	add(_btnCancel);
	add(_btnDefault);

	add(_txtTooltip);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnVideo->setColor(Palette::blockOffset(8)+5);
	_btnVideo->setText(tr("STR_VIDEO"));
	_btnVideo->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress);

	_btnAudio->setColor(Palette::blockOffset(8)+5);
	_btnAudio->setText(tr("STR_AUDIO"));
	_btnAudio->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress);

	_btnControls->setColor(Palette::blockOffset(8)+5);
	_btnControls->setText(tr("STR_CONTROLS"));
	_btnControls->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress);

	_btnGeoscape->setColor(Palette::blockOffset(8)+5);
	_btnGeoscape->setText(tr("STR_GEOSCAPE_UC"));
	_btnGeoscape->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress);

	_btnBattlescape->setColor(Palette::blockOffset(8)+5);
	_btnBattlescape->setText(tr("STR_BATTLESCAPE_UC"));
	_btnBattlescape->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress);

	_btnAdvanced->setColor(Palette::blockOffset(8)+5);
	_btnAdvanced->setText(tr("STR_ADVANCED"));
	_btnAdvanced->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress);

	_btnMods->setColor(Palette::blockOffset(8)+5);
	_btnMods->setText(tr("STR_MODS"));
	_btnMods->onMousePress((ActionHandler)&OptionsBaseState::btnGroupPress);
	_btnMods->setVisible(_origin == OPT_MENU); // Mods require a restart, don't enable them in-game

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&OptionsBaseState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&OptionsBaseState::btnOkClick, Options::keyOk);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&OptionsBaseState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&OptionsBaseState::btnCancelClick, Options::keyCancel);

	_btnDefault->setColor(Palette::blockOffset(8)+5);
	_btnDefault->setText(tr("STR_RESTORE_DEFAULTS"));
	_btnDefault->onMouseClick((ActionHandler)&OptionsBaseState::btnDefaultClick);

	_txtTooltip->setColor(Palette::blockOffset(8)+5);
	_txtTooltip->setWordWrap(true);
}

/**
 *
 */
OptionsBaseState::~OptionsBaseState()
{

}

void OptionsBaseState::restart(Game *game, OptionsOrigin origin)
{
	if (origin == OPT_MENU)
	{
		game->setState(new MainMenuState(game));
	}
	else if (origin == OPT_GEOSCAPE)
	{
		game->setState(new GeoscapeState(game));
	}
	else if (origin == OPT_BATTLESCAPE)
	{
		game->setState(new GeoscapeState(game));
		BattlescapeState *bs = new BattlescapeState(game);
		game->pushState(bs);
		game->getSavedGame()->getSavedBattle()->setBattleState(bs);
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
 * Saves the new options and returns to the proper origin screen..
 * @param action Pointer to an action.
 */
void OptionsBaseState::btnOkClick(Action *)
{

	updateScale(Options::battlescapeScale, Options::newBattlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, _origin == OPT_BATTLESCAPE);
	updateScale(Options::geoscapeScale, Options::newGeoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, _origin != OPT_BATTLESCAPE);
	Options::switchDisplay();
	Options::save();
	_game->loadLanguage(Options::language);
	SDL_WM_GrabInput(Options::captureMouse);
	_game->getScreen()->resetDisplay();
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
	if (Options::reload && _origin == OPT_MENU)
	{
		_game->setState(new StartState(_game));
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
			_game->pushState(new OptionsConfirmState(_game, _origin));
		}
		else
		{
			restart(_game, _origin);
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
	updateScale(Options::battlescapeScale, Options::newBattlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, _origin == OPT_BATTLESCAPE);
	updateScale(Options::geoscapeScale, Options::newGeoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, _origin != OPT_BATTLESCAPE);
	_game->setVolume(Options::soundVolume, Options::musicVolume, Options::uiVolume);
	_game->popState();
}

/**
 * Restores the Options to default settings.
 * @param action Pointer to an action.
 */
void OptionsBaseState::btnDefaultClick(Action *action)
{
	_game->pushState(new OptionsDefaultsState(_game, _origin, this));
}

void OptionsBaseState::btnGroupPress(Action *action)
{
	Surface *sender = action->getSender();
	//if (sender != _group)
	{
		_game->popState();
		if (sender == _btnVideo)
		{
			_game->pushState(new OptionsVideoState(_game, _origin));
		}
		else if (sender == _btnAudio)
		{
			_game->pushState(new OptionsAudioState(_game, _origin));
		}
		else if (sender == _btnControls)
		{
			_game->pushState(new OptionsControlsState(_game, _origin));
		}
		else if (sender == _btnGeoscape)
		{
			_game->pushState(new OptionsGeoscapeState(_game, _origin));
		}
		else if (sender == _btnBattlescape)
		{
			_game->pushState(new OptionsBattlescapeState(_game, _origin));
		}
		else if (sender == _btnAdvanced)
		{
			_game->pushState(new OptionsAdvancedState(_game, _origin));
		}
		else if (sender == _btnMods)
		{
			_game->pushState(new OptionsModsState(_game, _origin));
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
* Changes a given scale, and if necessary, switch the current base resolution.
* @param type reference to which scale option we are using, battlescape or geoscape.
* @param selection the new scale level.
* @param width reference to which x scale to adjust.
* @param height reference to which y scale to adjust.
* @param change should we change the current scale.
*/
void OptionsBaseState::updateScale(int &type, int selection, int &width, int &height, bool change)
{
	type = selection;
	switch (type)
	{
	case SCALE_15X:
		width = Screen::ORIGINAL_WIDTH * 1.5;
		height = Screen::ORIGINAL_HEIGHT * 1.5;
		break;
	case SCALE_2X:
		width = Screen::ORIGINAL_WIDTH * 2;
		height = Screen::ORIGINAL_HEIGHT * 2;
		break;
	case SCALE_SCREEN_DIV_3:
		width = Options::newDisplayWidth / 3;
		height = Options::newDisplayHeight / 3;
		break;
	case SCALE_SCREEN_DIV_2:
		width = Options::newDisplayWidth / 2;
		height = Options::newDisplayHeight / 2;
		break;
	case SCALE_SCREEN:
		width = Options::newDisplayWidth;
		height = Options::newDisplayHeight;
		break;
	case SCALE_ORIGINAL:
	default:
		width = Screen::ORIGINAL_WIDTH;
		height = Screen::ORIGINAL_HEIGHT;
		break;
	}

	// don't go under minimum resolution... it's bad, mmkay?
	width = std::max(width, Screen::ORIGINAL_WIDTH);
	height = std::max(height, Screen::ORIGINAL_HEIGHT);

	if (change && (Options::baseXResolution != width || Options::baseYResolution != height))
	{
		Options::baseXResolution = width;
		Options::baseYResolution = height;
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
