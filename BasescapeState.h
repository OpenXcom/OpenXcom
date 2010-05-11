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

#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "Button.h"
#include "Text.h"
#include "GeoscapeState.h"
#include "BaseInfoState.h"
#include "SoldiersState.h"
#include "CraftsState.h"
#include "ResearchState.h"
#include "ManufactureState.h"
#include "PurchaseState.h"
#include "SellState.h"

class BasescapeState : public State
{
private:
	Text *_txtFacility, *_txtBase, *_txtLocation, *_txtFunds;
	Button *_btnNewBase, *_btnBaseInfo, *_btnSoldiers, *_btnCrafts, *_btnFacilities, *_btnResearch, *_btnManufacture, *_btnTransfer, *_btnPurchase, *_btnSell, *_btnGeoscape;
	Base *_base;
public:
	BasescapeState(Game *game);
	~BasescapeState();
	void init();
	void btnBaseInfoClick(SDL_Event *ev, int scale);
	void btnSoldiersClick(SDL_Event *ev, int scale);
	void btnCraftsClick(SDL_Event *ev, int scale);
	void btnResearchClick(SDL_Event *ev, int scale);
	void btnManufactureClick(SDL_Event *ev, int scale);
	void btnPurchaseClick(SDL_Event *ev, int scale);
	void btnSellClick(SDL_Event *ev, int scale);
	void btnGeoscapeClick(SDL_Event *ev, int scale);
};

#endif
