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
#include "GraphsState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Engine/InteractiveSurface.h"
#include "../Savegame/Country.h"
#include "../Savegame/Region.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Language.h"
#include "../Ruleset/RuleCountry.h"
#include "../Ruleset/RuleRegion.h"
#include "../Interface/Window.h"
#include "../Interface/TextList.h"
#include "../Interface/Text.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Graphs screen.
 * @param game Pointer to the core game.
 */
GraphsState::GraphsState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnGeoscape = new InteractiveSurface(32, 24, 288, 0);
	_btnUFO_Activity_Areas = new InteractiveSurface(32, 24, 96, 0);
	_btnUFO_Activity_Countries = new InteractiveSurface(32, 24, 128, 0);
	_btnXCOM_Activity_Areas = new InteractiveSurface(32, 24, 160, 0);
	_btnXCOM_Activity_Countries = new InteractiveSurface(32, 24, 192, 0);
	_txtLabel = new Text(280, 16, 60, 30);
	_lstResults = new TextList(280, 136, 32, 50);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_2")->getColors());

	add(_window);
	add(_btnGeoscape);
	add(_btnUFO_Activity_Areas);
	add(_btnUFO_Activity_Countries);
	add(_btnXCOM_Activity_Areas);
	add(_btnXCOM_Activity_Countries);
	add(_txtLabel);
	add(_lstResults);

	// Set up objects
	_game->getResourcePack()->getSurface("GRAPHS.SPK")->blit(_window);
	_lstResults->setColor(Palette::blockOffset(15)-1);
	_lstResults->setSecondaryColor(Palette::blockOffset(8)+10);
	_lstResults->setColumns(3, 108, 100, 72);
	_lstResults->setDot(true);

	_txtLabel->setColor(Palette::blockOffset(8)+5);
	_txtLabel->setAlign(ALIGN_CENTER);
	_txtLabel->setBig();

	_btnGeoscape->onMouseClick((ActionHandler)&GraphsState::btnGeoscapeClick);
	_btnUFO_Activity_Areas->onMouseClick((ActionHandler)&GraphsState::btnUFO_Activity_AreasClick);
	_btnUFO_Activity_Countries->onMouseClick((ActionHandler)&GraphsState::btnUFO_Activity_CountriesClick);
	_btnXCOM_Activity_Areas->onMouseClick((ActionHandler)&GraphsState::btnXCOM_Activity_AreasClick);
	_btnXCOM_Activity_Countries->onMouseClick((ActionHandler)&GraphsState::btnXCOM_Activity_CountriesClick);

	// Default to UFO_Activity_Areas action.
	this->btnUFO_Activity_AreasClick(NULL);
}

/**
 *
 */
GraphsState::~GraphsState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void GraphsState::btnGeoscapeClick(Action *action)
{
	_game->popState();
}

/**
 * Starts the UFO Activity in Areas graph.
 * @param action Pointer to an action.
 */
void GraphsState::btnUFO_Activity_CountriesClick(Action *action)
{
	_txtLabel->setText(_game->getLanguage()->getString("STR_UFO_ACTIVITY_IN_COUNTRIES"));

	_lstResults->clearList();
	for (std::vector<Country*>::iterator i = _game->getSavedGame()->getCountries()->begin(); i != _game->getSavedGame()->getCountries()->end(); ++i)
	{
		std::wstringstream ss;
		ss << (*i)->getActivityAlien().at((*i)->getActivityAlien().size()-1);
		_lstResults->addRow(2, _game->getLanguage()->getString((*i)->getRules()->getType()).c_str(), ss.str().c_str());
	}
}

/**
 * Starts the UFO Activity in Areas graph.
 * @param action Pointer to an action.
 */
void GraphsState::btnUFO_Activity_AreasClick(Action *action)
{
	_txtLabel->setText(_game->getLanguage()->getString("STR_UFO_ACTIVITY_IN_AREAS"));

	_lstResults->clearList();
	for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
	{
		std::wstringstream ss;
		ss << (*i)->getActivityAlien().at((*i)->getActivityAlien().size()-1);
		_lstResults->addRow(2, _game->getLanguage()->getString((*i)->getRules()->getType()).c_str(), ss.str().c_str());
	}
}

/**
 * Starts the XCOM Activity in Countries graph.
 * @param action Pointer to an action.
 */
void GraphsState::btnXCOM_Activity_CountriesClick(Action *action)
{
	_txtLabel->setText(_game->getLanguage()->getString("STR_XCOM_ACTIVITY_IN_COUNTRIES"));

	_lstResults->clearList();
	for (std::vector<Country*>::iterator i = _game->getSavedGame()->getCountries()->begin(); i != _game->getSavedGame()->getCountries()->end(); ++i)
	{
		std::wstringstream ss;
		ss << (*i)->getActivityXcom().at((*i)->getActivityXcom().size()-1);
		_lstResults->addRow(2, _game->getLanguage()->getString((*i)->getRules()->getType()).c_str(), ss.str().c_str());
	}
}

/**
 * Starts the XCOM Activity in Areas graph.
 * @param action Pointer to an action.
 */
void GraphsState::btnXCOM_Activity_AreasClick(Action *action)
{
	_txtLabel->setText(_game->getLanguage()->getString("STR_XCOM_ACTIVITY_IN_AREAS"));

	_lstResults->clearList();
	for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
	{
		std::wstringstream ss;
		ss << (*i)->getActivityXcom().at((*i)->getActivityXcom().size()-1);
		_lstResults->addRow(2, _game->getLanguage()->getString((*i)->getRules()->getType()).c_str(), ss.str().c_str());
	}
}

}
