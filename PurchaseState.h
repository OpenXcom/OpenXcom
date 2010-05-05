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
#ifndef OPENXCOM__PURCHASESTATE_H
#define OPENXCOM__PURCHASESTATE_H

#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "Button.h"
#include "Window.h"
#include "Text.h"
#include "TextList.h"

class PurchaseState : public State
{
private:
	Button *_btnOk, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtFunds, *_txtPurchases, *_txtItem, *_txtCost, *_txtQuantity;
	TextList *_lstItems;
public:
	PurchaseState(Game *game);
	~PurchaseState();
	void think();
	void btnOkClick(SDL_Event *ev, int scale);
	void btnCancelClick(SDL_Event *ev, int scale);
};

#endif
