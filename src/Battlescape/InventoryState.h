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
#ifndef OPENXCOM_INVENTORYSTATE_H
#define OPENXCOM_INVENTORYSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Surface;
class Text;
class InteractiveSurface;
class Inventory;
class SavedBattleGame;

/**
 * Screen which displays soldier's inventory.
 */
class InventoryState : public State
{
private:
	Surface *_bg, *_soldier;
	Text *_txtName, *_txtItem, *_txtAmmo, *_txtWeight, *_txtTus, *_txtFAcc, *_txtReact, *_txtPSkill, *_txtPStr;
	InteractiveSurface *_btnOk, *_btnPrev, *_btnNext, *_btnUnload, *_btnGround, *_btnRank;
	Surface *_selAmmo;
	Inventory *_inv;
	SavedBattleGame *_battleGame;
	bool _tu, _showMoreStatsInInventoryView;
public:
	/// Creates the Inventory state.
	InventoryState(Game *game, bool tu);
	/// Cleans up the Inventory state.
	~InventoryState();
	/// Updates all soldier info.
	void init();
	/// Updates the soldier info (Weight, TU).
	void updateStats();
	/// Saves the soldiers' equipment-layout.
	void saveEquipmentLayout();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Previous button.
	void btnPrevClick(Action *action);
	/// Handler for clicking the Next button.
	void btnNextClick(Action *action);
	/// Handler for clicking the Rank button.
	void btnUnloadClick(Action *action);
	/// Handler for clicking on the Ground -> button.
	void btnGroundClick(Action *action);
	/// Handler for clicking on the inventory.
	void btnRankClick(Action *action);
	/// Handler for clicking the Unload button.
	void invClick(Action *action);
	/// Handle keypresses.
	void handle(Action *action);
};

}

#endif
