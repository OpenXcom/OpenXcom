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
#include "../Interface/TextButton.h"
#include "../Savegame/GameTime.h"
#include "../Savegame/SavedGame.h"
#include "../Interface/TextList.h"
#include "../Engine/Action.h"
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
	_txtTitle = new Text(220, 16, 100, 28);
	_txtMonths = new TextList(205, 8, 115, 183);
	_txtYears = new TextList(200, 8, 121, 191);
	for(int scaleText = 0; scaleText != 10; ++scaleText)
	{
		_txtScale.push_back(new Text(42, 16, 84, 171 - (scaleText*14)));
		add(_txtScale.at(scaleText));
	}
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_2")->getColors());
	
	//add all our elements
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
	//create buttons (sooooo many buttons)
	int offset = 0;
	for(std::vector<Region *>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
	{
		_btnAlienRegions.push_back(new TextButton(80, 11, 0, offset*11));
		_btnAlienRegionToggles.push_back(false);
		_btnAlienRegions.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnAlienRegions.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnAlienRegions.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnAlienRegionListClick);
		add(_btnAlienRegions.at(offset));
		_alienRegionLines.push_back(new Surface(320,200,0,0));
		add(_alienRegionLines.at(offset));

		_btnXcomRegions.push_back(new TextButton(80, 11, 0, offset*11));
		_btnXcomRegionToggles.push_back(false);
		_btnXcomRegions.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnXcomRegions.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnXcomRegions.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnXcomRegionListClick);
		add(_btnXcomRegions.at(offset));
		_xcomRegionLines.push_back(new Surface(320,200,0,0));
		add(_xcomRegionLines.at(offset));

		++offset;
	}

	_btnAlienRegionTotal = new TextButton(80, 11, 0, offset*11);
	_btnAlienRegionToggles.push_back(false);
	_btnAlienRegionTotal->onMouseClick((ActionHandler)&GraphsState::btnAlienRegionListClick);
	_btnAlienRegionTotal->setColor(Palette::blockOffset(9)+7);
	_btnAlienRegionTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_alienRegionLines.push_back(new Surface(320,200,0,0));
	add(_alienRegionLines.at(offset));
	add(_btnAlienRegionTotal);

	_btnXcomRegionTotal = new TextButton(80, 11, 0, offset*11);
	_btnXcomRegionToggles.push_back(false);
	_btnXcomRegionTotal->onMouseClick((ActionHandler)&GraphsState::btnXcomRegionListClick);
	_btnXcomRegionTotal->setColor(Palette::blockOffset(9)+7);
	_btnXcomRegionTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_xcomRegionLines.push_back(new Surface(320,200,0,0));
	add(_xcomRegionLines.at(offset));
	add(_btnXcomRegionTotal);
	
	offset = 0;
	for(std::vector<Country *>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		_btnAlienCountries.push_back(new TextButton(80, 11, 0, offset*11));
		_btnAlienCountryToggles.push_back(false);
		_btnAlienCountries.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnAlienCountries.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnAlienCountries.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnAlienCountryListClick);
		add(_btnAlienCountries.at(offset));
		_alienCountryLines.push_back(new Surface(320,200,0,0));
		add(_alienCountryLines.at(offset));

		_btnXcomCountries.push_back(new TextButton(80, 11, 0, offset*11));
		_btnXcomCountryToggles.push_back(false);
		_btnXcomCountries.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnXcomCountries.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnXcomCountries.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnXcomCountryListClick);
		add(_btnXcomCountries.at(offset));
		_xcomCountryLines.push_back(new Surface(320,200,0,0));
		add(_xcomCountryLines.at(offset));
		
		_btnIncomeCountries.push_back(new TextButton(80, 11, 0, offset*11));
		_btnIncomeToggles.push_back(false);
		_btnIncomeCountries.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnIncomeCountries.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnIncomeCountries.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnIncomeListClick);
		add(_btnIncomeCountries.at(offset));
		_incomeLines.push_back(new Surface(320,200,0,0));
		add(_incomeLines.at(offset));

		++offset;
	}

	_btnAlienCountryTotal = new TextButton(80, 11, 0, offset*11);
	_btnAlienCountryToggles.push_back(false);
	_btnAlienCountryTotal->onMouseClick((ActionHandler)&GraphsState::btnAlienCountryListClick);
	_btnAlienCountryTotal->setColor(Palette::blockOffset(9)+7);
	_btnAlienCountryTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_alienCountryLines.push_back(new Surface(320,200,0,0));
	add(_alienCountryLines.at(offset));
	add(_btnAlienCountryTotal);

	_btnXcomCountryTotal = new TextButton(80, 11, 0, offset*11);
	_btnXcomCountryToggles.push_back(false);
	_btnXcomCountryTotal->onMouseClick((ActionHandler)&GraphsState::btnXcomCountryListClick);
	_btnXcomCountryTotal->setColor(Palette::blockOffset(9)+7);
	_btnXcomCountryTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_xcomCountryLines.push_back(new Surface(320,200,0,0));
	add(_xcomCountryLines.at(offset));
	add(_btnXcomCountryTotal);
	
	_btnIncomeTotal = new TextButton(80, 11, 0, offset*11);
	_btnIncomeToggles.push_back(false);
	_btnIncomeTotal->onMouseClick((ActionHandler)&GraphsState::btnIncomeListClick);
	_btnIncomeTotal->setColor(Palette::blockOffset(9)+7);
	_btnIncomeTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_incomeLines.push_back(new Surface(320,200,0,0));
	add(_incomeLines.at(offset));
	add(_btnIncomeTotal);

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

	//set up the horizontal measurement units
	std::string months[] = {"STR_JAN", "STR_FEB", "STR_MAR", "STR_APR", "STR_MAY", "STR_JUN", "STR_JUL", "STR_AUG", "STR_SEP", "STR_OCT", "STR_NOV", "STR_DEC"};
	int month = _game->getSavedGame()->getTime()->getMonth();
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
			ss << _game->getSavedGame()->getTime()->getYear();
			_txtYears->setCellText(0, iter/2, ss.str());
			if(iter > 2)
			{
				std::wstringstream ss2;
				ss2 << (_game->getSavedGame()->getTime()->getYear()-1);
				_txtYears->setCellText(0, 0, ss2.str());
			}
		}
		_txtMonths->setCellText(0, iter, _game->getLanguage()->getString(months[month]));
		++month;
	}

	// set up the vertical measurement units
	for(std::vector<Text *>::iterator iter = _txtScale.begin(); iter != _txtScale.end(); ++iter)
	{
		(*iter)->setAlign(ALIGN_RIGHT);
		(*iter)->setColor(Palette::blockOffset(6)+8);
	}
	btnUfoRegionClick(0);

	// Set up objects
	_game->getResourcePack()->getSurface("GRAPHS.SPK")->blit(_bg);
	_txtTitle->setBig();
	_txtTitle->setColor(Palette::blockOffset(8)+8);

	// Set up buttons
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
	resetScreen();
	for(std::vector<TextButton *>::iterator iter = _btnAlienRegions.begin(); iter != _btnAlienRegions.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnAlienRegionTotal->setVisible(true);
	_scale = 0;

	for(int numberOfMonths = 0; numberOfMonths != _game->getSavedGame()->getFundsList().size(); ++numberOfMonths)
	{
		int total = 0;
		for(std::vector<Region *>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
		{
			total += (*iter)->getActivityAlien().at(numberOfMonths);
		}
		if(total > _scale)
		{
			_scale = total;
		}
	}
	for(int check = 100; check <= 50000; check *= 2)
	{
		if(_scale < check - (check/10))
		{
			_scale = check;
			break;
		}
	}
	updateScale();
	_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_ACTIVITY_IN_AREAS"));
	
	// set up lines
	int counter = 0;
	int total[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(std::vector<Region*>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
	{
		std::vector<Sint16> newLineVector;
		for(int iter2 = 0; iter2 != 12; ++iter2)
		{
			int x = 312 - (iter2*17);
			int y = 175;
			if(iter2 < (*iter)->getActivityAlien().size() && (*iter)->getActivityAlien().at(iter2))
			{
				double dscale = _scale;
				double units = dscale / 140;
				int reduction = (*iter)->getActivityAlien().at(iter2) / units;
				y -= reduction;
				total[iter2] += (*iter)->getActivityAlien().at(iter2);
			}
			newLineVector.push_back(y);
			int offset = 0;
			if(counter % 2)
				offset = 8;
			if(newLineVector.size() > 1)
				_alienRegionLines.at(counter)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((counter/2)+1)+offset);
		}
		_alienRegionLines.at(counter)->setVisible(_btnAlienRegionToggles.at(counter));
		++counter;
	}
	
	std::vector<Sint16> newLineVector;
	for(int iter = 0; iter != 12; ++iter)
	{
		int x = 312 - (iter*17);
		int y = 175;
		if(total[iter] > 0)
		{
			double dscale = _scale;
			double units = dscale / 140;
			int reduction = total[iter] / units;
			y -= reduction;
		}
		newLineVector.push_back(y);
		int offset = 0;
		if(counter % 2)
			offset = 8;
		if(newLineVector.size() > 1)
			_alienRegionLines.at(_alienRegionLines.size()-1)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
	}
	_alienRegionLines.at(_alienRegionLines.size()-1)->setVisible(_btnAlienRegionToggles.at(_alienRegionLines.size()-1));
}




void GraphsState::btnUfoCountryClick(Action *action)
{
	resetScreen();
	for(std::vector<TextButton *>::iterator iter = _btnAlienCountries.begin(); iter != _btnAlienCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnAlienCountryTotal->setVisible(true);

	_scale = 0;

	for(int numberOfMonths = 0; numberOfMonths != _game->getSavedGame()->getFundsList().size(); ++numberOfMonths)
	{
		int total = 0;
		for(std::vector<Country *>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
		{
			total += (*iter)->getActivityAlien().at(numberOfMonths);
		}
		if(total > _scale)
		{
			_scale = total;
		}
	}
	for(int check = 100; check <= 50000; check *= 2)
	{
		if(_scale < check - (check/10))
		{
			_scale = check;
			break;
		}
	}
	updateScale();
	_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_ACTIVITY_IN_COUNTRIES"));
	
	// set up lines
	int counter = 0;
	int total[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(std::vector<Country*>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		std::vector<Sint16> newLineVector;
		for(int iter2 = 0; iter2 != 12; ++iter2)
		{
			int x = 312 - (iter2*17);
			int y = 175;
			if(iter2 < (*iter)->getActivityAlien().size() && (*iter)->getActivityAlien().at(iter2))
			{
				double dscale = _scale;
				double units = dscale / 140;
				int reduction = (*iter)->getActivityAlien().at(iter2) / units;
				y -= reduction;
				total[iter2] += (*iter)->getActivityAlien().at(iter2);
			}
			newLineVector.push_back(y);
			int offset = 0;
			if(counter % 2)
				offset = 8;
			if(newLineVector.size() > 1)
				_alienCountryLines.at(counter)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((counter/2)+1)+offset);
		}
		_alienCountryLines.at(counter)->setVisible(_btnAlienCountryToggles.at(counter));
		++counter;
	}
	
	std::vector<Sint16> newLineVector;
	for(int iter = 0; iter != 12; ++iter)
	{
		int x = 312 - (iter*17);
		int y = 175;
		if(total[iter] > 0)
		{
			double dscale = _scale;
			double units = dscale / 140;
			int reduction = total[iter] / units;
			y -= reduction;
		}
		newLineVector.push_back(y);
		int offset = 0;
		if(counter % 2)
			offset = 8;
		if(newLineVector.size() > 1)
			_alienCountryLines.at(_alienCountryLines.size()-1)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
	}
	_alienCountryLines.at(_alienCountryLines.size()-1)->setVisible(_btnAlienCountryToggles.at(_alienCountryLines.size()-1));
}




void GraphsState::btnXcomRegionClick(Action *action)
{
	resetScreen();
	for(std::vector<TextButton *>::iterator iter = _btnXcomRegions.begin(); iter != _btnXcomRegions.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnXcomRegionTotal->setVisible(true);
	_scale = 0;

	for(int numberOfMonths = 0; numberOfMonths != _game->getSavedGame()->getFundsList().size(); ++numberOfMonths)
	{
		int total = 0;
		for(std::vector<Region *>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
		{
			total += (*iter)->getActivityXcom().at(numberOfMonths);
		}
		if(total > _scale)
		{
			_scale = total;
		}
	}
	for(int check = 100; check <= 50000; check *= 2)
	{
		if(_scale < check - (check/10))
		{
			_scale = check;
			break;
		}
	}
	updateScale();
	_txtTitle->setText(_game->getLanguage()->getString("STR_XCOM_ACTIVITY_IN_AREAS"));

	// set up lines
	int counter = 0;
	int total[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(std::vector<Region*>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
	{
		std::vector<Sint16> newLineVector;
		for(int iter2 = 0; iter2 != 12; ++iter2)
		{
			int x = 312 - (iter2*17);
			int y = 175;
			if(iter2 < (*iter)->getActivityXcom().size() && (*iter)->getActivityXcom().at(iter2))
			{
				double dscale = _scale;
				double units = dscale / 140;
				int reduction = (*iter)->getActivityXcom().at(iter2) / units;
				y -= reduction;
				total[iter2] += (*iter)->getActivityXcom().at(iter2);
			}
			newLineVector.push_back(y);
			int offset = 0;
			if(counter % 2)
				offset = 8;
			if(newLineVector.size() > 1)
				_xcomRegionLines.at(counter)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((counter/2)+1)+offset);
		}
		_xcomRegionLines.at(counter)->setVisible(_btnXcomRegionToggles.at(counter));
		++counter;
	}
	
	std::vector<Sint16> newLineVector;
	for(int iter = 0; iter != 12; ++iter)
	{
		int x = 312 - (iter*17);
		int y = 175;
		if(total[iter] > 0)
		{
			double dscale = _scale;
			double units = dscale / 140;
			int reduction = total[iter] / units;
			y -= reduction;
		}
		newLineVector.push_back(y);
		int offset = 0;
		if(counter % 2)
			offset = 8;
		if(newLineVector.size() > 1)
			_xcomRegionLines.at(_xcomRegionLines.size()-1)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
	}
	_xcomRegionLines.at(_xcomRegionLines.size()-1)->setVisible(_btnXcomRegionToggles.at(_xcomRegionLines.size()-1));
}




void GraphsState::btnXcomCountryClick(Action *action)
{
	resetScreen();
	for(std::vector<TextButton *>::iterator iter = _btnXcomCountries.begin(); iter != _btnXcomCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnXcomCountryTotal->setVisible(true);
	_scale = 0;

	for(int numberOfMonths = 0; numberOfMonths != _game->getSavedGame()->getFundsList().size(); ++numberOfMonths)
	{
		int total = 0;
		for(std::vector<Region *>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
		{
			total += (*iter)->getActivityXcom().at(numberOfMonths);
		}
		if(total > _scale)
		{
			_scale = total;
		}
	}
	for(int check = 100; check <= 50000; check *= 2)
	{
		if(_scale < check - (check/10))
		{
			_scale = check;
			break;
		}
	}
	updateScale();
	_txtTitle->setText(_game->getLanguage()->getString("STR_XCOM_ACTIVITY_IN_COUNTRIES"));
	
	// set up lines
	int counter = 0;
	int total[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(std::vector<Country*>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		std::vector<Sint16> newLineVector;
		for(int iter2 = 0; iter2 != 12; ++iter2)
		{
			int x = 312 - (iter2*17);
			int y = 175;
			if(iter2 < (*iter)->getActivityXcom().size() && (*iter)->getActivityXcom().at(iter2))
			{
				double dscale = _scale;
				double units = dscale / 140;
				int reduction = (*iter)->getActivityXcom().at(iter2) / units;
				y -= reduction;
				total[iter2] += (*iter)->getActivityXcom().at(iter2);
			}
			newLineVector.push_back(y);
			int offset = 0;
			if(counter % 2)
				offset = 8;
			if(newLineVector.size() > 1)
				_xcomCountryLines.at(counter)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((counter/2)+1)+offset);
		}
		_xcomCountryLines.at(counter)->setVisible(_btnXcomCountryToggles.at(counter));
		++counter;
	}
	
	std::vector<Sint16> newLineVector;
	for(int iter = 0; iter != 12; ++iter)
	{
		int x = 312 - (iter*17);
		int y = 175;
		if(total[iter] > 0)
		{
			double dscale = _scale;
			double units = dscale / 140;
			int reduction = total[iter] / units;
			y -= reduction;
		}
		newLineVector.push_back(y);
		int offset = 0;
		if(counter % 2)
			offset = 8;
		if(newLineVector.size() > 1)
			_xcomCountryLines.at(_xcomCountryLines.size()-1)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
	}
	_xcomCountryLines.at(_xcomCountryLines.size()-1)->setVisible(_btnXcomCountryToggles.at(_xcomCountryLines.size()-1));
}




void GraphsState::btnIncomeClick(Action *action)
{
	resetScreen();
	for(std::vector<TextButton *>::iterator iter = _btnIncomeCountries.begin(); iter != _btnIncomeCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnIncomeTotal->setVisible(true);

	double highest = 0;
	for(int numberOfMonths = 0; numberOfMonths != _game->getSavedGame()->getFundsList().size(); ++numberOfMonths)
	{
		int total = 0;
		for(std::vector<Country *>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
		{
			total += (*iter)->getFunding().at(numberOfMonths);
		}
		if(total > highest)
		{
			highest = total;
		}
	}
	
	for(int check = 100; check <= 1000000000; check *= 10)
	{
		if(highest < check - (check/10))
		{
			highest /=check;
			highest = (highest/9)*10;
			_scale = ++highest;
			_scale *= check;
			break;
		}
	}
	updateScale();
	_txtTitle->setText(_game->getLanguage()->getString("STR_INCOME"));

	// set up lines
	int counter = 0;
	int total[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(std::vector<Country*>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		std::vector<Sint16> newLineVector;
		for(int iter2 = 0; iter2 != 12; ++iter2)
		{
			int x = 312 - (iter2*17);
			int y = 175;
			if(iter2 < (*iter)->getFunding().size() && (*iter)->getFunding().at(iter2))
			{
				double dscale = _scale;
				double units = dscale / 140;
				int reduction = (*iter)->getFunding().at(iter2) / units;
				y -= reduction;
				total[iter2] += (*iter)->getFunding().at(iter2);
			}
			newLineVector.push_back(y);
			int offset = 0;
			if(counter % 2)
				offset = 8;
			if(newLineVector.size() > 1)
				_incomeLines.at(counter)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((counter/2)+1)+offset);
		}
		_incomeLines.at(counter)->setVisible(_btnIncomeToggles.at(counter));
		++counter;
	}
	
	std::vector<Sint16> newLineVector;
	for(int iter = 0; iter != 12; ++iter)
	{
		int x = 312 - (iter*17);
		int y = 175;
		if(total[iter] > 0)
		{
			double dscale = _scale;
			double units = dscale / 140;
			int reduction = total[iter] / units;
			y -= reduction;
		}
		newLineVector.push_back(y);
		int offset = 0;
		if(counter % 2)
			offset = 8;
		if(newLineVector.size() > 1)
			_incomeLines.at(_xcomCountryLines.size()-1)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
	}
	_incomeLines.at(_incomeLines.size()-1)->setVisible(_btnIncomeToggles.at(_incomeLines.size()-1));
}




void GraphsState::btnFinanceClick(Action *action)
{
	resetScreen();

	for(std::vector<TextButton *>::iterator iter = _btnFinanceButtons.begin(); iter != _btnFinanceButtons.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnFinanceTotal->setVisible(true);

	double highest = 0;
	for(int numberOfMonths = 0; numberOfMonths != _game->getSavedGame()->getFundsList().size(); ++numberOfMonths)
	{
		if(_game->getSavedGame()->getFundsList().at(numberOfMonths) > highest)
		{
			highest = _game->getSavedGame()->getFundsList().at(numberOfMonths);
		}
	}

	if(highest > 999999999)
	{
		highest /= 100000000;
		highest = (highest/9)*10;
		_scale = ++highest;
		_scale *= 100000000;
	}
	else if(highest > 99999999)
	{
		highest /= 10000000;
		highest = (highest/9)*10;
		_scale = ++highest;
		_scale *= 10000000;
	}
	else if(highest > 9999999)
	{
		highest /= 1000000;
		highest = (highest/9)*10;
		_scale = ++highest;
		_scale *= 1000000;
	}
	else if(highest > 999999)
	{
		highest /= 100000;
		highest = (highest/9)*10;
		_scale = ++highest;
		_scale *= 100000;
	}
	else if(highest > 99999)
	{
		highest /= 10000;
		highest = (highest/9)*10;
		_scale = ++highest;
		_scale *= 10000;
	}
	else if(highest > 9999)
	{
		highest /= 1000;
		highest = (highest/9)*10;
		_scale = ++highest;
		_scale *= 1000;
	}
	else if(highest > 999)
	{
		highest /= 100;
		highest = (highest/9)*10;
		_scale = ++highest;
		_scale *= 100;
	}
	else if(highest > 99)
	{
		highest /= 10;
		highest = (highest/9)*10;
		_scale = ++highest;
		_scale *= 10;
	}
	if(_scale < 90)
		_scale = 90;
	updateScale();
	_txtTitle->setText(_game->getLanguage()->getString("STR_FINANCE"));

}




void GraphsState::updateScale()
{
	int offset = 0;
	for(std::vector<Text *>::iterator iter = _txtScale.begin(); iter != _txtScale.end(); ++iter)
	{
		std::wstringstream ss;
		ss << (_scale/10)*offset << " ";
		(*iter)->setText(ss.str());
		offset ++;
	}
}




void GraphsState::btnAlienRegionListClick(Action *action)
{
	int number = action->getSender()->getY()/11;
	TextButton *button = 0;
	int adjustment = -42 + (4*number);

	if(number == _btnAlienRegions.size())
	{
		button = _btnAlienRegionTotal;
		adjustment = 22;
	}
	else
	{
		button = _btnAlienRegions.at(number);
	}

	if(_btnAlienRegionToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
		_alienRegionLines.at(number)->setVisible(false);
		_btnAlienRegionToggles.at(number) = false;
	}
	else
	{
		button->invert(adjustment);
		_alienRegionLines.at(number)->setVisible(true);
		_btnAlienRegionToggles.at(number) = true;
	}
}




void GraphsState::btnAlienCountryListClick(Action *action)
{
	int number = action->getSender()->getY()/11;
	TextButton *button = 0;
	int adjustment = -42 + (4*number);
	if(number == _btnAlienCountries.size())
	{
		button = _btnAlienCountryTotal;
		adjustment = 22;
	}
	else
	{
		button = _btnAlienCountries.at(number);
	}
	if(_btnAlienCountryToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
		_btnAlienCountryToggles.at(number) = false;
		_alienCountryLines.at(number)->setVisible(false);
	}
	else
	{
		button->invert(adjustment);
		_btnAlienCountryToggles.at(number) = true;
		_alienCountryLines.at(number)->setVisible(true);
	}
}

void GraphsState::btnXcomRegionListClick(Action *action)
{
	int number = action->getSender()->getY()/11;
	TextButton *button = 0;
	int adjustment = -42 + (4*number);

	if(number == _btnXcomRegions.size())
	{
		button = _btnXcomRegionTotal;
		adjustment = 22;
	}
	else
	{
		button = _btnXcomRegions.at(number);
	}

	if(_btnXcomRegionToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
		_xcomRegionLines.at(number)->setVisible(false);
		_btnXcomRegionToggles.at(number) = false;
	}
	else
	{
		button->invert(adjustment);
		_xcomRegionLines.at(number)->setVisible(true);
		_btnXcomRegionToggles.at(number) = true;
	}
}




void GraphsState::btnXcomCountryListClick(Action *action)
{
	int number = action->getSender()->getY()/11;
	TextButton *button = 0;
	int adjustment = -42 + (4*number);
	if(number == _btnXcomCountries.size())
	{
		button = _btnXcomCountryTotal;
		adjustment = 22;
	}
	else
	{
		button = _btnXcomCountries.at(number);
	}
	if(_btnXcomCountryToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
		_btnXcomCountryToggles.at(number) = false;
		_xcomCountryLines.at(number)->setVisible(false);
	}
	else
	{
		button->invert(adjustment);
		_btnXcomCountryToggles.at(number) = true;
		_xcomCountryLines.at(number)->setVisible(true);
	}
}


void GraphsState::btnIncomeListClick(Action *action)
{
	int number = action->getSender()->getY()/11;
	TextButton *button = 0;
	int adjustment = -42 + (4*number);
	if(number == _btnIncomeCountries.size())
	{
		button = _btnIncomeTotal;
		adjustment = 22;
	}
	else
	{
		button = _btnIncomeCountries.at(number);
	}
	if(_btnIncomeToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
		_btnIncomeToggles.at(number) = false;
		_incomeLines.at(number)->setVisible(false);
	}
	else
	{
		button->invert(adjustment);
		_btnIncomeToggles.at(number) = true;
		_incomeLines.at(number)->setVisible(true);
	}
}
void GraphsState::resetScreen()
{
	for(std::vector<Surface *>::iterator iter = _alienRegionLines.begin(); iter != _alienRegionLines.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<Surface *>::iterator iter = _alienCountryLines.begin(); iter != _alienCountryLines.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<Surface *>::iterator iter = _xcomRegionLines.begin(); iter != _xcomRegionLines.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<Surface *>::iterator iter = _xcomCountryLines.begin(); iter != _xcomCountryLines.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<Surface *>::iterator iter = _incomeLines.begin(); iter != _incomeLines.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<Surface *>::iterator iter = _financeLines.begin(); iter != _financeLines.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}


	for(std::vector<TextButton *>::iterator iter = _btnAlienRegions.begin(); iter != _btnAlienRegions.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnAlienCountries.begin(); iter != _btnAlienCountries.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnXcomRegions.begin(); iter != _btnXcomRegions.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnXcomCountries.begin(); iter != _btnXcomCountries.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnIncomeCountries.begin(); iter != _btnIncomeCountries.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnFinanceButtons.begin(); iter != _btnFinanceButtons.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}

	_btnXcomRegionTotal->setVisible(false);
	_btnXcomCountryTotal->setVisible(false);
	_btnAlienRegionTotal->setVisible(false);
	_btnAlienCountryTotal->setVisible(false);
	// _btnFinanceTotal->setVisible(false);
	_btnIncomeTotal->setVisible(false);
}
}
