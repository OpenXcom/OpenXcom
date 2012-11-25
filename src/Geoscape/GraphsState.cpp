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

	//create buttons (sooooo many buttons)
	int offset = 0;
	for(std::vector<Region *>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
	{
		_btnRegions.push_back(new TextButton(80, 11, 0, offset*11));
		_btnRegionToggles.push_back(false);
		_btnRegions.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnRegions.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnRegions.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnRegionClick);
		add(_btnRegions.at(offset));
		_btnRegions.at(offset)->setVisible(false);
		++offset;
	}
	_btnRegionTotal = new TextButton(80, 11, 0, offset*11);
	_btnRegionToggles.push_back(false);
	_btnRegionTotal->onMouseClick((ActionHandler)&GraphsState::btnRegionClick);
	_btnRegionTotal->setColor(Palette::blockOffset(9)+7);
	_btnRegionTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_btnRegionTotal->setVisible(false);
	
	offset = 0;
	for(std::vector<Country *>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		_btnCountries.push_back(new TextButton(70, 11, 0, offset*11));
		_btnCountryToggles.push_back(false);
		_btnCountries.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnCountries.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnCountries.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnCountryClick);
		add(_btnCountries.at(offset));
		_btnCountries.at(offset)->setVisible(false);
		++offset;
	}
	_btnCountryTotal = new TextButton(70, 11, 0, offset*11);
	_btnCountryToggles.push_back(false);
	_btnCountryTotal->onMouseClick((ActionHandler)&GraphsState::btnCountryClick);
	_btnCountryTotal->setColor(Palette::blockOffset(9)+7);
	_btnCountryTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_btnCountryTotal->setVisible(false);


	//add all our elements (god damn that's a lot of elements)
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
	add(_btnRegionTotal);
	add(_btnCountryTotal);

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
	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	_btnRegionTotal->setVisible(true);
	_btnCountryTotal->setVisible(false);
	_scale = 0;
	for(std::vector<Region *>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
	{
		for(int numberOfMonths = 0; numberOfMonths != (*iter)->getActivityAlien().size(); ++numberOfMonths)
		{
			if((*iter)->getActivityAlien().at(numberOfMonths) > _scale)
			{
				_scale = (*iter)->getActivityAlien().at(numberOfMonths);
			}
		}
	}
	
	if(_scale < 90)
		_scale = 100;
	else if(_scale < 180)
		_scale = 200;
	else if(_scale < 360)
		_scale = 400;
	else if(_scale < 720)
		_scale = 800;
	else if(_scale < 1440)
		_scale = 1600;
	else if(_scale < 2880)
		_scale = 3400;
	else if(_scale < 5760) //this should never occur but what the heck.
		_scale = 6800;
	updateScale();
	_xcom = false;
	_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_ACTIVITY_IN_AREAS"));
}
void GraphsState::btnUfoCountryClick(Action *action)
{
	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnRegionTotal->setVisible(false);
	_btnCountryTotal->setVisible(true);
	_scale = 0;
	for(std::vector<Country *>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		for(int numberOfMonths = 0; numberOfMonths != (*iter)->getActivityAlien().size(); ++numberOfMonths)
		{
			if((*iter)->getActivityAlien().at(numberOfMonths) > _scale)
			{
				_scale = (*iter)->getActivityAlien().at(numberOfMonths);
			}
		}
	}
	if(_scale < 90)
		_scale = 100;
	else if(_scale < 180)
		_scale = 200;
	else if(_scale < 360)
		_scale = 400;
	else if(_scale < 720)
		_scale = 800;
	else if(_scale < 1440)
		_scale = 1600;
	else if(_scale < 2880)
		_scale = 3400;
	else if(_scale < 5760) //this should never occur but what the heck.
		_scale = 6800;
	updateScale();
	_xcom = false;
	_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_ACTIVITY_IN_COUNTRIES"));
	
	// set up lines
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
			}
			_bg->setPixel(x, y, Palette::blockOffset(iter2+1));
			newLineVector.push_back(y);
		}
	}
}
void GraphsState::btnXcomRegionClick(Action *action)
{
	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	_btnRegionTotal->setVisible(true);
	_btnCountryTotal->setVisible(false);
	_scale = 0;
	for(std::vector<Region *>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
	{
		for(int numberOfMonths = 0; numberOfMonths != (*iter)->getActivityXcom().size(); ++numberOfMonths)
		{
			if((*iter)->getActivityXcom().at(numberOfMonths) > _scale)
			{
				_scale = (*iter)->getActivityXcom().at(numberOfMonths);
			}
		}
	}
	if(_scale < 90)
		_scale = 100;
	else if(_scale < 180)
		_scale = 200;
	else if(_scale < 360)
		_scale = 400;
	else if(_scale < 720)
		_scale = 800;
	else if(_scale < 1440)
		_scale = 1600;
	else if(_scale < 2880)
		_scale = 3400;
	else if(_scale < 5760) //this should never occur but what the heck.
		_scale = 6800;
	updateScale();
	_xcom = true;
	_txtTitle->setText(_game->getLanguage()->getString("STR_XCOM_ACTIVITY_IN_AREAS"));
}
void GraphsState::btnXcomCountryClick(Action *action)
{
	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnRegionTotal->setVisible(false);
	_btnCountryTotal->setVisible(true);
	_scale = 0;
	for(std::vector<Country *>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		for(int numberOfMonths = 0; numberOfMonths != (*iter)->getActivityXcom().size(); ++numberOfMonths)
		{
			if((*iter)->getActivityXcom().at(numberOfMonths) > _scale)
			{
				_scale = (*iter)->getActivityXcom().at(numberOfMonths);
			}
		}
	}
	if(_scale < 90)
		_scale = 100;
	else if(_scale < 180)
		_scale = 200;
	else if(_scale < 360)
		_scale = 400;
	else if(_scale < 720)
		_scale = 800;
	else if(_scale < 1440)
		_scale = 1600;
	else if(_scale < 2880)
		_scale = 3400;
	else if(_scale < 5760) //this should never occur but what the heck.
		_scale = 6800;
	updateScale();
	_xcom = true;
	_txtTitle->setText(_game->getLanguage()->getString("STR_XCOM_ACTIVITY_IN_COUNTRIES"));
}
void GraphsState::btnIncomeClick(Action *action)
{
	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnRegionTotal->setVisible(false);
	_btnCountryTotal->setVisible(true);
	double highest = 0;
	for(std::vector<Country *>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		for(int numberOfMonths = 0; numberOfMonths != (*iter)->getFunding().size(); ++numberOfMonths)
		{
			if((*iter)->getFunding().at(numberOfMonths) > highest)
			{
				highest = (*iter)->getFunding().at(numberOfMonths);
			}
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
	_txtTitle->setText(_game->getLanguage()->getString("STR_INCOME"));
}
void GraphsState::btnFinanceClick(Action *action)
{
	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnRegionTotal->setVisible(false);
	_btnCountryTotal->setVisible(true);
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

void GraphsState::btnRegionClick(Action *action)
{
	int number = action->getSender()->getY()/11;
	TextButton *button = 0;
	int adjustment = -42 + (4*number);
	if(number == _btnRegions.size())
	{
		button = _btnRegionTotal;
		adjustment = 22;
	}
	else
	{
		button = _btnRegions.at(number);
	}
	if(_btnRegionToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
		_btnRegionToggles.at(number) = false;
	}
	else
	{
		button->invert(adjustment);
		_btnRegionToggles.at(number) = true;
	}
}
void GraphsState::btnCountryClick(Action *action)
{
	int number = action->getSender()->getY()/11;
	TextButton *button = 0;
	int adjustment = -42 + (4*number);
	if(number == _btnCountries.size())
	{
		button = _btnCountryTotal;
		adjustment = 22;
	}
	else
	{
		button = _btnCountries.at(number);
	}
	if(_btnCountryToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
		_btnCountryToggles.at(number) = false;
	}
	else
	{
		button->invert(adjustment);
		_btnCountryToggles.at(number) = true;
	}
}
}
