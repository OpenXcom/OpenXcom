/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__BASESCAPESTATE_H
#define OPENXCOM__BASESCAPESTATE_H

#include "State.h"

class BaseView;
class MiniBaseView;
class Text;
class TextButton;
class Base;
class Globe;

/**
 * Basescape screen that shows a base's layout
 * and lets the player manage their bases.
 */
class BasescapeState : public State
{
private:
	BaseView *_view;
	MiniBaseView *_mini;
	Text *_txtFacility, *_txtBase, *_txtLocation, *_txtFunds;
	TextButton *_btnNewBase, *_btnBaseInfo, *_btnSoldiers, *_btnCrafts, *_btnFacilities, *_btnResearch, *_btnManufacture, *_btnTransfer, *_btnPurchase, *_btnSell, *_btnGeoscape;
	Base *_base;
	Globe *_globe;
public:
	/// Creates the Basescape state.
	BasescapeState(Game *game, Base *base, Globe *globe);
	/// Cleans up the Basescape state.
	~BasescapeState();
	/// Updates the base stats.
	void init();
	/// Sets a new base to display.
	void setBase(Base *base);
	/// Handler for clicking the Build New Base button.
	void btnNewBaseClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Base Information button.
	void btnBaseInfoClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Soldiers button.
	void btnSoldiersClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Equip Craft button.
	void btnCraftsClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Build Facilities button.
	void btnFacilitiesClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Research button.
	void btnResearchClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Manufacture button.
	void btnManufactureClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Purchase/Hire button.
	void btnPurchaseClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Sell/Sack button.
	void btnSellClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Geoscape button.
	void btnGeoscapeClick(SDL_Event *ev, int scale);
	/// Handler for clicking the base view.
	void viewClick(SDL_Event *ev, int scale);
	/// Handler for hovering the base view.
	void viewMouseOver(SDL_Event *ev, int scale);
	/// Handler for hovering out of the base view.
	void viewMouseOut(SDL_Event *ev, int scale);
	/// Handler for clicking the mini base view.
	void miniClick(SDL_Event *ev, int scale);
};

#endif
