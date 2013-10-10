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
#ifndef OPENXCOM_OPTIONSSTATE_H
#define OPENXCOM_OPTIONSSTATE_H

#include "../Engine/State.h"
#include "OptionsBaseState.h"
#include <SDL.h>

namespace OpenXcom
{

class TextButton;
class ToggleTextButton;
class Window;
class Text;
class TextEdit;
class ArrowButton;
class Slider;

/**
 * Options window that displays all
 * the settings the player can configure.
 */
class OptionsState : public OptionsBaseState
{
private:
	static const std::string GL_EXT, GL_FOLDER, GL_STRING;

	Window *_window;
	Text *_txtTitle;
	Text *_txtDisplayResolution, *_txtDisplayX;
	TextEdit *_txtDisplayWidth, *_txtDisplayHeight;
	ArrowButton *_btnDisplayUp, *_btnDisplayDown;
	Text *_txtDisplayMode;
	TextButton *_displayMode, *_btnDisplayWindowed, *_btnDisplayFullscreen;
	Text *_txtDisplayFilter;
	TextButton *_btnDisplayFilter;
	Text *_txtMusicVolume;
	Slider *_slrMusicVolume;
	Text *_txtSoundVolume;
	Slider *_slrSoundVolume;
	TextButton *_btnOk, *_btnCancel, *_btnDefault, *_btnLanguage, *_btnControls, *_btnAdvanced;
	SDL_Rect** _res;
	int _resAmount, _resCurrent;
	bool _wClicked, _hClicked;
	std::vector<std::string> _filters, _filterPaths;
	size_t _selFilter;
	int _musicVolume, _soundVolume;
public:
	/// Creates the Options state.
	OptionsState(Game *game, OptionsOrigin origin);
	/// Cleans up the Options state.
	~OptionsState();
	/// Initilizes the Options state.
	void init();
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
	void txtDisplayWidthClick(Action *action);
	/// unclick width if necessary.
	void txtDisplayHeightClick(Action *action);
    /// Handler for clicking the Display Filter button
    void btnDisplayFilterClick(Action *action);
    /// Handler for clicking the advanced options button
    void btnAdvancedClick(Action *action);
	/// Handler for music slider release.
    void slrMusicVolumeRelease(Action *);
    /// Handler for sound slider release.
    void slrSoundVolumeRelease(Action *);
};

}

#endif
