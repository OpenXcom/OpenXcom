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
#ifndef OPENXCOM_OPTIONSSTATE_H
#define OPENXCOM_OPTIONSSTATE_H

#include "../Engine/State.h"
#include <SDL.h>

namespace OpenXcom
{

class TextButton;
class ToggleTextButton;
class Window;
class Text;
class TextEdit;
class ArrowButton;

/**
 * Options window that displays all
 * the settings the player can configure.
 */
class OptionsState : public State
{
private:
	Window *_window;
	Text *_txtTitle;
	Text *_txtDisplayResolution, *_txtDisplayX;
	TextEdit *_txtDisplayWidth, *_txtDisplayHeight;
	ArrowButton *_btnDisplayUp, *_btnDisplayDown;
	Text *_txtDisplayMode;
	TextButton *_displayMode, *_btnDisplayWindowed, *_btnDisplayFullscreen;
    ToggleTextButton *_btnDisplayOpenGL;
	Text *_txtMusicVolume;
	TextButton *_musicVolume, *_btnMusicVolume1, *_btnMusicVolume2, *_btnMusicVolume3, *_btnMusicVolume4, *_btnMusicVolume5;
	Text *_txtSoundVolume;
	TextButton *_soundVolume, *_btnSoundVolume1, *_btnSoundVolume2, *_btnSoundVolume3, *_btnSoundVolume4, *_btnSoundVolume5;
	TextButton *_btnOk, *_btnCancel, *_btnDefault, *_btnLanguage, *_btnControls;
	SDL_Rect** _res;
	int _resAmount, _resCurrent;
	bool _wClicked, _hClicked, _OpenGL;
public:
	/// Creates the Options state.
	OptionsState(Game *game);
	/// Cleans up the Options state.
	~OptionsState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Restore Defaults button.
	void btnDefaultClick(Action *action);
	/// Handler for clicking the Language button.
	void btnLanguageClick(Action *action);
	/// Handler for clicking the Language button.
	void btnControlsClick(Action *action);
	/// Handler for clicking the Next Resolution button.
	void btnDisplayUpClick(Action *action);
	/// Handler for clicking the Previous Resolution button.
	void btnDisplayDownClick(Action *action);
	/// unclick height if necessary.
	void DisplayWidthClick(Action *action);
	/// unclick width if necessary.
	void DisplayHeightClick(Action *action);
    /// Handler for clicking the OpenGL button
    void btnOpenGLClick(Action *action);
};

}

#endif
