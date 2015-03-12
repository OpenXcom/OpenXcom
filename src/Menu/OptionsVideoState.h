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
#ifndef OPENXCOM_OPTIONSVIDEOSTATE_H
#define OPENXCOM_OPTIONSVIDEOSTATE_H

#include "../Engine/State.h"
#include "OptionsBaseState.h"
#include <SDL.h>

namespace OpenXcom
{

class TextButton;
class ToggleTextButton;
class Text;
class TextEdit;
class ArrowButton;
class ComboBox;
class InteractiveSurface;

/**
 * Screen that lets the user configure various
 * Video options.
 */
class OptionsVideoState : public OptionsBaseState
{
private:
	static const std::string GL_EXT, GL_FOLDER, GL_STRING;

	InteractiveSurface *_displaySurface;
	Text *_txtDisplayResolution, *_txtDisplayX;
	TextEdit *_txtDisplayWidth, *_txtDisplayHeight;
	ArrowButton *_btnDisplayResolutionUp, *_btnDisplayResolutionDown;

	Text *_txtLanguage, *_txtFilter, *_txtGeoScale, *_txtBattleScale;
	ComboBox *_cbxLanguage, *_cbxFilter, *_cbxDisplayMode, *_cbxGeoScale, *_cbxBattleScale;
	Text *_txtMode;
	Text *_txtOptions;
	ToggleTextButton *_btnLetterbox, *_btnLockMouse;

	SDL_Rect** _res;
	int _resAmount, _resCurrent;
	std::vector<std::string> _langs, _filters;

	void updateDisplayResolution();
public:
	/// Creates the Options state.
	OptionsVideoState(OptionsOrigin origin);
	/// Cleans up the Options state.
	~OptionsVideoState();
	/// Handler for clicking the Next Resolution button.
	void btnDisplayResolutionUpClick(Action *action);
	/// Handler for clicking the Previous Resolution button.
	void btnDisplayResolutionDownClick(Action *action);
	/// Handler for changing the Display Width text.
	void txtDisplayWidthChange(Action *action);
	/// Handler for changing the Display Height text.
	void txtDisplayHeightChange(Action *action);
	/// Handler for changing the Language combobox.
	void cbxLanguageChange(Action *action);
	/// Handler for changing the Filter combobox.
	void cbxFilterChange(Action *action);
	/// Handler for clicking the Display Mode combobox.
	void updateDisplayMode(Action *action);
	/// Handler for clicking the Letterboxed button.
	void btnLetterboxClick(Action *action);
	/// Handler for clicking the Lock Mouse button.
	void btnLockMouseClick(Action *action);
	/// Handler for updating the selected battlescape scale.
	void updateBattlescapeScale(Action *action);
	/// Handler for updating the selected geoscape scale.
	void updateGeoscapeScale(Action *action);
	/// Update the resolution settings, we just resized the window.
	void resize(int &, int &);
	/// Handles keypresses.
	void handle(Action *action);
};

}

#endif
