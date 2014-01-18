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
#include "BuildFacilitiesState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/SavedGame.h"
#include "PlaceFacilityState.h"
#include "BaseView.h"
#include "BasescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Build Facilities window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param state Pointer to the base state to refresh.
 */
BuildFacilitiesState::BuildFacilitiesState(Game *game, Base *base, State *state, int viewCameraPosX, int viewCameraPosY) : State(game), _base(base), _state(state), _facilities()
{
	_screen = false;

	// Create objects
	_window = new Window(this, 128, 160, 192, 40, POPUP_VERTICAL);
	_txtFacility = new Text(192, 9, 0, 0);
	_view = new BaseView(game, (BaseViewClickHandler)&BuildFacilitiesState::viewLeftClick, 0, Options::infiniteBaseSizes, 192, 192, 0, 8);
	_btnOk = new TextButton(112, 16, 200, 176);
	_lstFacilities = new TextList(112, 104, 200, 64);
	_txtTitle = new Text(118, 17, 197, 48);

	// Set palette
	setPalette("PAL_BASESCAPE", 6);

	add(_window);
	add(_view);
	add(_txtFacility);
	add(_btnOk);
	add(_txtTitle);
	add(_lstFacilities);

	centerAllSurfaces();

	// Set up objects
	_view->setTexture(_game->getResourcePack()->getSurfaceSet("BASEBITS.PCK"));
	_view->setBase(_base);
	_view->onMouseOver((ActionHandler)&BuildFacilitiesState::viewMouseOver);
	_view->onMouseOut((ActionHandler)&BuildFacilitiesState::viewMouseOut);
	_view->setCameraPos(viewCameraPosX, viewCameraPosY);

	_txtFacility->setColor(Palette::blockOffset(13)+10);

	_window->setColor(Palette::blockOffset(13)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BuildFacilitiesState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BuildFacilitiesState::btnOkClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(13));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_INSTALLATION"));

	_lstFacilities->setColor(Palette::blockOffset(13)+5);
	_lstFacilities->setArrowColor(Palette::blockOffset(13)+5);
	_lstFacilities->setColumns(1, 112);
	_lstFacilities->setSelectable(true);
	_lstFacilities->setScrolling(true, -12);
	_lstFacilities->setBackground(_window);
	_lstFacilities->setMargin(2);
	_lstFacilities->setWordWrap(true);
	_lstFacilities->onMouseClick((ActionHandler)&BuildFacilitiesState::lstFacilitiesClick);

	PopulateBuildList();
}

/**
 *
 */
BuildFacilitiesState::~BuildFacilitiesState()
{

}

/**
 * Sets camera position of the view.
 * @param x x-position of the camera.
 * @param y y-position of the camera.
 */
void BuildFacilitiesState::setViewCameraPos(int x, int y)
{
	_view->setCameraPos(x,y);
}

/**
 * Populates the build list from the current "available" facilities.
 */
void BuildFacilitiesState::PopulateBuildList()
{
	const std::vector<std::string> &facilities = _game->getRuleset()->getBaseFacilitiesList();
	for (std::vector<std::string>::const_iterator i = facilities.begin(); i != facilities.end(); ++i)
	{
		RuleBaseFacility *rule = _game->getRuleset()->getBaseFacility(*i);
		if (_game->getSavedGame()->isResearched(rule->getRequirements()) && !rule->isLift())
			_facilities.push_back(rule);
	}

	for (std::vector<RuleBaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		_lstFacilities->addRow(1, tr((*i)->getType()).c_str());
	}
}

/**
 * The player can change the selected base
 * or change info on other screens.
 */
void BuildFacilitiesState::init()
{
	_view->setBase(_base); // to refresh the view
	State::init();
	_state->init();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BuildFacilitiesState::btnOkClick(Action *)
{
	BasescapeState* basescapeState = dynamic_cast<BasescapeState*>(_state);
	if (0 != basescapeState) basescapeState->setViewCameraPos(_view->getCameraPosX(), _view->getCameraPosY());
	_game->popState();
}

/**
 * Places the selected facility.
 * @param action Pointer to an action.
 */
void BuildFacilitiesState::lstFacilitiesClick(Action *)
{
	_game->pushState(new PlaceFacilityState(_game, _base, _facilities[_lstFacilities->getSelectedRow()], this, _view->getCameraPosX(), _view->getCameraPosY()));
}

/**
 * Processes clicking on facilities.
 * @param action Pointer to an action.
 */
void BuildFacilitiesState::viewLeftClick(Action *action)
{
	_view->tryDismantle();
}

/**
 * Displays the name of the facility the mouse is over.
 * @param action Pointer to an action.
 */
void BuildFacilitiesState::viewMouseOver(Action *action)
{
	_txtFacility->setText(_view->getSelectedFacilityName());
}

/**
 * Clears the facility name.
 * @param action Pointer to an action.
 */
void BuildFacilitiesState::viewMouseOut(Action *action)
{
	_txtFacility->setText(L"");
}

}
