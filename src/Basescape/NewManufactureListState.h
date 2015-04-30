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
#ifndef OPENXCOM_NEWMANUFACTURELISTSTATE_H
#define OPENXCOM_NEWMANUFACTURELISTSTATE_H

#include "../Engine/State.h"
#include <vector>

namespace OpenXcom
{
class Base;
class TextButton;
class Window;
class Text;
class TextList;
class RuleManufacture;
class ComboBox;

/**
 * Screen which list possible productions.
 */
class NewManufactureListState : public State
{
private:
	Base *_base;
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtCategory;
	TextList *_lstManufacture;
	ComboBox *_cbxCategory;
	std::vector<RuleManufacture *> _possibleProductions;
	std::vector<std::string> _catStrings;
	std::vector<std::string> _displayedStrings;

public:
	/// Creates the state.
	NewManufactureListState(Base *base);
	/// Initializes state.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action * action);
	/// Handler for clicking on the list.
	void lstProdClick (Action * action);
	/// Handler for changing the category filter
	void cbxCategoryChange (Action * action);
	/// Fills the list of possible productions.
	void fillProductionList();
};
}
#endif
