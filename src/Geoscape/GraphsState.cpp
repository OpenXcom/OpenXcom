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
#include "../Engine/Surface.h"
#include "../Engine/InteractiveSurface.h"
#include "../Savegame/Country.h"
#include "../Savegame/Region.h"
#include "../Ruleset/RuleCountry.h"
#include "../Ruleset/RuleRegion.h"
#include "../Interface/Text.h"
#include "../Engine/Language.h"
#include "../Savegame/GameTime.h"
#include "../Savegame/SavedGame.h"
#include "../Interface/TextList.h"
#include <sstream>

namespace OpenXcom
{

/**
 * Initializes all the elements in the Graphs screen.
 * @param game Pointer to the core game.
 */
GraphsState::GraphsState(Game *game) : State(game)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_btnUfoRegion = new InteractiveSurface(32, 24, 90, 0);
	_btnUfoCountry = new InteractiveSurface(32, 24, 123, 0);
	_btnXcomRegion = new InteractiveSurface(32, 24, 156, 0);
	_btnXcomCountry = new InteractiveSurface(32, 24, 189, 0);
	_btnIncome = new InteractiveSurface(32, 24, 222, 0);
	_btnFinance = new InteractiveSurface(32, 24, 255, 0);
	_btnGeoscape = new InteractiveSurface(32, 24, 288, 0);
	_txtTitle = new Text(200, 16, 118, 28);
	_txtMonths = new TextList(205, 8, 115, 183);
	_txtYears = new TextList(200, 8, 121, 191);
	_txtScale9 = new Text(22, 16, 104, 45);
	_txtScale8 = new Text(22, 16, 104, 59);
	_txtScale7 = new Text(22, 16, 104, 73);
	_txtScale6 = new Text(22, 16, 104, 87);
	_txtScale5 = new Text(22, 16, 104, 101);
	_txtScale4 = new Text(22, 16, 104, 115);
	_txtScale3 = new Text(22, 16, 104, 129);
	_txtScale2 = new Text(22, 16, 104, 143);
	_txtScale1 = new Text(22, 16, 104, 157);
	_txtScale0 = new Text(22, 16, 104, 171);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_2")->getColors());

	add(_bg);
	add(_btnUfoRegion);
	add(_btnUfoCountry);
	add(_btnXcomRegion);
	add(_btnXcomCountry);
	add(_btnIncome);
	add(_btnFinance);
	add(_btnGeoscape);
	add(_txtMonths);
	add(_txtYears);
	add(_txtTitle);
	add(_txtScale9);
	add(_txtScale8);
	add(_txtScale7);
	add(_txtScale6);
	add(_txtScale5);
	add(_txtScale4);
	add(_txtScale3);
	add(_txtScale2);
	add(_txtScale1);
	add(_txtScale0);

	// set up the grid
	SDL_Rect current;
	current.w = 188;
	current.h = 127;
	current.x = 125;
	current.y = 49;
	_bg->drawRect(&current, Palette::blockOffset(10));

	for(int grid = 0; grid !=5; ++grid)
	{
	current.w = 16 - (grid*2);
	current.h = 13 - (grid*2);
		for(int y = 50 + grid; y <= 163 + grid; y += 14)
		{
			current.y = y;
			for(int x = 126 + grid; x <= 297 + grid; x += 17)
			{
				current.x = x;
				Uint8 color = Palette::blockOffset(10)+grid+1;
				if(grid == 4)
				{
					color = 0;
				} 
				_bg->drawRect(&current, color);
			}
		}
	}

	std::string months[] = {"STR_JAN", "STR_FEB", "STR_MAR", "STR_APR", "STR_MAY", "STR_JUN", "STR_JUL", "STR_AUG", "STR_SEP", "STR_OCT", "STR_NOV", "STR_DEC"};
	int month = game->getSavedGame()->getTime()->getMonth();
	// i know using textlist for this is ugly and brutal, but YOU try getting this damn text to line up.
	// also, there's nothing wrong with being ugly or brutal, you should learn tolerance.
	_txtMonths->setColumns(12, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17);
	_txtMonths->addRow(12, "", "", "", "", "", "", "", "", "", "", "", "");
	_txtMonths->setRowColor(0, Palette::blockOffset(6)+8);
	_txtYears->setColumns(6, 34, 34, 34, 34, 34, 34);
	_txtYears->addRow(6, " ", " ", " ", " ", " ", " ");
	_txtYears->setRowColor(0, Palette::blockOffset(6)+8);

	for(int iter = 0; iter != 12; ++iter)
	{
		if(month > 11)
		{
			month = 0;
			std::wstringstream ss;
			ss << game->getSavedGame()->getTime()->getYear();
			_txtYears->setCellText(0, iter/2, ss.str());
			if(iter > 2)
			{
				std::wstringstream ss2;
				ss2 << (game->getSavedGame()->getTime()->getYear()-1);
				_txtYears->setCellText(0, 0, ss2.str());
			}
		}
		_txtMonths->setCellText(0, iter, game->getLanguage()->getString(months[month]));
		++month;
	}
	int highest = 0;
	for(std::vector<Country *>::iterator iter = game->getSavedGame()->getCountries()->begin(); iter != game->getSavedGame()->getCountries()->end(); ++iter)
	{
		for(int numberOfMonths = 0; numberOfMonths != (*iter)->getActivityAlien().size(); ++numberOfMonths)
		{
			if((*iter)->getActivityAlien().at(numberOfMonths) > highest)
			{
				highest = (*iter)->getActivityAlien().at(numberOfMonths);
			}
			if((*iter)->getActivityXcom().at(numberOfMonths) > highest)
			{
				highest = (*iter)->getActivityXcom().at(numberOfMonths);
			}
		}
	}
	for(std::vector<Region *>::iterator iter = game->getSavedGame()->getRegions()->begin(); iter != game->getSavedGame()->getRegions()->end(); ++iter)
	{
		for(int numberOfMonths = 0; numberOfMonths != (*iter)->getActivityAlien().size(); ++numberOfMonths)
		{
			if((*iter)->getActivityAlien().at(numberOfMonths) > highest)
			{
				highest = (*iter)->getActivityAlien().at(numberOfMonths);
			}
			if((*iter)->getActivityXcom().at(numberOfMonths) > highest)
			{
				highest = (*iter)->getActivityXcom().at(numberOfMonths);
			}
		}
	}
	int scale = (highest/9)+1;
	if(scale < 10)
		scale = 10;
	//oh god this is horrible, what am i even doing here?
	//there has to be an easier way to convert an integer to a wstring
	std::wstringstream ss1, ss2, ss3, ss4, ss5, ss6, ss7, ss8, ss9;
	ss9 << scale*9 << " ";
	_txtScale9->setText(ss9.str());
	_txtScale9->setAlign(ALIGN_RIGHT);
	_txtScale9->setColor(Palette::blockOffset(6)+8);
	ss8 << scale*8 << " ";
	_txtScale8->setText(ss8.str());
	_txtScale8->setAlign(ALIGN_RIGHT);
	_txtScale8->setColor(Palette::blockOffset(6)+8);
	ss7 << scale*7 << " ";
	_txtScale7->setText(ss7.str());
	_txtScale7->setAlign(ALIGN_RIGHT);
	_txtScale7->setColor(Palette::blockOffset(6)+8);
	ss6 << scale*6 << " ";
	_txtScale6->setText(ss6.str());
	_txtScale6->setAlign(ALIGN_RIGHT);
	_txtScale6->setColor(Palette::blockOffset(6)+8);
	ss5 << scale*5 << " ";
	_txtScale5->setText(ss5.str());
	_txtScale5->setAlign(ALIGN_RIGHT);
	_txtScale5->setColor(Palette::blockOffset(6)+8);
	ss4 << scale*4 << " ";
	_txtScale4->setText(ss4.str());
	_txtScale4->setAlign(ALIGN_RIGHT);
	_txtScale4->setColor(Palette::blockOffset(6)+8);
	ss3 << scale*3 << " ";
	_txtScale3->setText(ss3.str());
	_txtScale3->setAlign(ALIGN_RIGHT);
	_txtScale3->setColor(Palette::blockOffset(6)+8);
	ss2 << scale*2 << " ";
	_txtScale2->setText(ss2.str());
	_txtScale2->setAlign(ALIGN_RIGHT);
	_txtScale2->setColor(Palette::blockOffset(6)+8);
	ss1 << scale << " ";
	_txtScale1->setText(ss1.str());
	_txtScale1->setAlign(ALIGN_RIGHT);
	_txtScale1->setColor(Palette::blockOffset(6)+8);
	_txtScale0->setText(L"0 ");
	_txtScale0->setAlign(ALIGN_RIGHT);
	_txtScale0->setColor(Palette::blockOffset(6)+8);

	// Set up objects
	_game->getResourcePack()->getSurface("GRAPHS.SPK")->blit(_bg);

	_btnUfoRegion->onMouseClick((ActionHandler)&GraphsState::btnUfoRegionClick);
	_btnUfoCountry->onMouseClick((ActionHandler)&GraphsState::btnUfoCountryClick);
	_btnXcomRegion->onMouseClick((ActionHandler)&GraphsState::btnXcomRegionClick);
	_btnXcomCountry->onMouseClick((ActionHandler)&GraphsState::btnXcomCountryClick);
	_btnIncome->onMouseClick((ActionHandler)&GraphsState::btnIncomeClick);
	_btnFinance->onMouseClick((ActionHandler)&GraphsState::btnFinanceClick);
	_btnGeoscape->onMouseClick((ActionHandler)&GraphsState::btnGeoscapeClick);
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
void GraphsState::btnUfoRegionClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnUfoCountryClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnXcomRegionClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnXcomCountryClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnIncomeClick(Action *action)
{
	_game->popState();
}
void GraphsState::btnFinanceClick(Action *action)
{
	_game->popState();
}

}
