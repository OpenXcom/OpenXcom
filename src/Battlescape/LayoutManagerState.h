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
#ifndef OPENXCOM_LAYOUTMANAGERSTATE_H
#define OPENXCOM_LAYOUTMANAGERSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class SavedGame;
class SavedBattleGame;
class Surface;
class InteractiveSurface;
class Window;
class Text;
class TextEdit;
class TextButton;
class ToggleTextButton;
class TextList;
class ArrowButton;
class BattleUnit;
class EquipmentLayout;
class WarningMessage;

/**
 * Screen which displays the Layout Manager.
 */
class LayoutManagerState : public State
{
private:
	SavedGame *_save;
	SavedBattleGame *_battleGame;
	Window *_window;
	TextButton *_btnOk;
	Text *_txtTitle, *_txtMode, *_txtSelectedLayout, *_txtSoldiers, *_txtName, *_txtLayout, *_txtLayoutName, *_txtOutsideBrightness;
	ToggleTextButton *_btnToSoldier, *_btnFromSoldier;
	std::vector<ToggleTextButton*> _btnLayouts;
	int _selectedLayout;
	ArrowButton *_btnLayoutsUp, *_btnLayoutsDown;
	int _btnLayoutsStartIndex;
	Timer *_timerLayoutsUp, *_timerLayoutsDown;
	std::vector<BattleUnit*> _soldiers;
	TextList *_lstSoldiers;
	TextEdit *_edtLayout;
	TextButton *_btnCreate, *_btnRename, *_btnDelete;
	Surface *_outsideBrightness;
	WarningMessage *_warning;
	void addLayoutButton(const std::wstring &text, int &buttons);
public:
	/// Creates the Layout Manager State.
	LayoutManagerState(Game *game, bool isBattlescapeGame);
	/// Cleans up the Layout Manager State.
	~LayoutManagerState();
	/// Handler for clicking the Ok button.
	void btnOkClick(Action *action);
	/// Handler for pressing one of the Mode buttons.
	void btnModeXPress(Action *action);
	/// Handler for pressing one of the Layout buttons.
	void btnLayoutXPress(Action *action);
	/// Handler for pressing the up arrowbutton.
	void btnLayoutsUpPress(Action * action);
	/// Handler for releasing the up arrowbutton.
	void btnLayoutsUpRelease(Action * action);
	/// Handler for clicking the up arrowbutton.
	void btnLayoutsUpClick(Action * action);
	/// Handler for pressing the down arrowbutton.
	void btnLayoutsDownPress(Action * action);
	/// Handler for releasing the down arrowbutton.
	void btnLayoutsDownRelease(Action * action);
	/// Handler for clicking the down arrowbutton.
	void btnLayoutsDownClick(Action * action);
	/// Event handler for _timerLayoutsUp.
	void onTimerLayoutsUp();
	/// Event handler for _timerLayoutsDown.
	void onTimerLayoutsDown();
	/// Scrolls up by the number of change.
	void scrollLayoutsUp(int change);
	/// Scrolls down by the number of change.
	void scrollLayoutsDown(int change);
	/// Updates the layout buttons, and arrowButtons.
	void updateLayoutsButtons();
	/// Updates the visibility of Create, Rename, and Delete layout buttons.
	void updateEditButtons();
	/// Runs state functionality every cycle.
	void think();
	/// Handler for pressing the Soldiers list.
	void lstSoldiersPress(Action *action);
	/// Handler for pressing a key on the Layout name edit.
	void edtLayoutKeyPress(Action *action);
	/// Handler for clicking the Create button.
	void btnCreateClick(Action *action);
	/// Handler for clicking the Rename button.
	void btnRenameClick(Action *action);
	/// Handler for clicking the Delete button.
	void btnDeleteClick(Action *action);
};

}

#endif
