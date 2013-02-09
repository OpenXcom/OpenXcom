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
	_btnUfoRegion = new InteractiveSurface(32, 24, 96, 0);
	_btnUfoCountry = new InteractiveSurface(32, 24, 128, 0);
	_btnXcomRegion = new InteractiveSurface(32, 24, 160, 0);
	_btnXcomCountry = new InteractiveSurface(32, 24, 192, 0);
	_btnIncome = new InteractiveSurface(32, 24, 224, 0);
	_btnFinance = new InteractiveSurface(32, 24, 256, 0);
	_btnGeoscape = new InteractiveSurface(32, 24, 288, 0);
	_txtTitle = new Text(220, 16, 100, 28);
	_txtFactor = new Text(38, 11, 96, 28);
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
	add(_txtFactor);

	//create buttons (sooooo many buttons)
	int offset = 0;
	for(std::vector<Region *>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
	{
		_btnRegions.push_back(new TextButton(80, 11, 0, offset*11));
		_regionToggles.push_back(false);
		_btnRegions.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnRegions.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnRegions.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnRegionListClick);
		add(_btnRegions.at(offset));
		_alienRegionLines.push_back(new Surface(320,200,0,0));
		add(_alienRegionLines.at(offset));
		_xcomRegionLines.push_back(new Surface(320,200,0,0));
		add(_xcomRegionLines.at(offset));

		++offset;
	}

	_btnRegionTotal = new TextButton(80, 11, 0, offset*11);
	_regionToggles.push_back(false);
	_btnRegionTotal->onMouseClick((ActionHandler)&GraphsState::btnRegionListClick);
	_btnRegionTotal->setColor(Palette::blockOffset(9)+7);
	_btnRegionTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_alienRegionLines.push_back(new Surface(320,200,0,0));
	add(_alienRegionLines.at(offset));
	_xcomRegionLines.push_back(new Surface(320,200,0,0));
	add(_xcomRegionLines.at(offset));
	add(_btnRegionTotal);
	
	offset = 0;
	for(std::vector<Country *>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
	{
		_btnCountries.push_back(new TextButton(80, 11, 0, offset*11));
		_countryToggles.push_back(false);
		_btnCountries.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnCountries.at(offset)->setText(_game->getLanguage()->getString((*iter)->getRules()->getType()));
		_btnCountries.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnCountryListClick);
		add(_btnCountries.at(offset));
		_alienCountryLines.push_back(new Surface(320,200,0,0));
		add(_alienCountryLines.at(offset));
		_xcomCountryLines.push_back(new Surface(320,200,0,0));
		add(_xcomCountryLines.at(offset));
		_incomeLines.push_back(new Surface(320,200,0,0));
		add(_incomeLines.at(offset));

		++offset;
	}

	_btnCountryTotal = new TextButton(80, 11, 0, offset*11);
	_countryToggles.push_back(false);
	_btnCountryTotal->onMouseClick((ActionHandler)&GraphsState::btnCountryListClick);
	_btnCountryTotal->setColor(Palette::blockOffset(9)+7);
	_btnCountryTotal->setText(_game->getLanguage()->getString("STR_TOTAL_UC"));
	_alienCountryLines.push_back(new Surface(320,200,0,0));
	add(_alienCountryLines.at(offset));
	_xcomCountryLines.push_back(new Surface(320,200,0,0));
	add(_xcomCountryLines.at(offset));
	_incomeLines.push_back(new Surface(320,200,0,0));
	add(_incomeLines.at(offset));
	add(_btnCountryTotal);
	

	for(int iter = 0; iter != 5; ++iter)
	{
		offset = iter;
		_btnFinances.push_back(new TextButton(80, 11, 0, offset*11));
		_financeToggles.push_back(false);
		_btnFinances.at(offset)->setColor(Palette::blockOffset(9)+7);
		_btnFinances.at(offset)->onMouseClick((ActionHandler)&GraphsState::btnFinanceListClick);
		add(_btnFinances.at(offset));
		_financeLines.push_back(new Surface(320,200,0,0));
		add(_financeLines.at(offset));
	}

	_btnFinances.at(0)->setText(_game->getLanguage()->getString("STR_INCOME"));
	_btnFinances.at(1)->setText(_game->getLanguage()->getString("STR_EXPENDITURE"));
	_btnFinances.at(2)->setText(_game->getLanguage()->getString("STR_MAINTENANCE"));
	_btnFinances.at(3)->setText(_game->getLanguage()->getString("STR_BALANCE"));
	_btnFinances.at(4)->setText(_game->getLanguage()->getString("STR_SCORE"));

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
	_txtMonths->addRow(12, L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ");
	_txtMonths->setRowColor(0, Palette::blockOffset(6)+8);
	_txtYears->setColumns(6, 34, 34, 34, 34, 34, 34);
	_txtYears->addRow(6, L" ", L" ", L" ", L" ", L" ", L" ");
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
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setColor(Palette::blockOffset(8)+8);
	
	_txtFactor->setText(L"$1000's");
	_txtFactor->setColor(Palette::blockOffset(8)+8);

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
void GraphsState::btnGeoscapeClick(Action *)
{
	_game->popState();
}

/**
 * Swtiches to the ufo country activity screen
 */
void GraphsState::btnUfoRegionClick(Action *)
{
	_alien = true;
	_income = false;
	_country = false;
	_finance = false;
	resetScreen();
	drawLines();
	updateScale(1);
	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnRegionTotal->setVisible(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_ACTIVITY_IN_AREAS"));
}

/**
 * Swtiches to the ufo country activity screen
 */
void GraphsState::btnUfoCountryClick(Action *)
{
	_alien = true;
	_income = false;
	_country = true;
	_finance = false;
	resetScreen();
	drawLines();
	updateScale(1);
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnCountryTotal->setVisible(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_ACTIVITY_IN_COUNTRIES"));
}

/**
 * Swtiches to the xcom region activity screen
 */
void GraphsState::btnXcomRegionClick(Action *)
{
	_alien = false;
	_income = false;
	_country = false;
	_finance = false;
	resetScreen();
	drawLines();
	updateScale(1);
	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnRegionTotal->setVisible(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_XCOM_ACTIVITY_IN_AREAS"));
}

/**
 * Swtiches to the xcom country activity screen
 */
void GraphsState::btnXcomCountryClick(Action *)
{
	_alien = false;
	_income = false;
	_country = true;
	_finance = false;
	resetScreen();
	drawLines();
	updateScale(1);
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnCountryTotal->setVisible(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_XCOM_ACTIVITY_IN_COUNTRIES"));
}

/**
 * Swtiches to the income screen
 */
void GraphsState::btnIncomeClick(Action *)
{
	_alien = false;
	_income = true;
	_country = true;
	_finance = false;
	resetScreen();
	drawLines();
	updateScale(1000);
	_txtFactor->setVisible(true);
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_btnCountryTotal->setVisible(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_INCOME"));
}

/**
 * Swtiches to the finances screen
 */
void GraphsState::btnFinanceClick(Action *)
{
	_alien = false;
	_income = false;
	_country = false;
	_finance = true;
	resetScreen();
	drawLines();
	updateScale(1000);
	_txtFactor->setVisible(true);

	for(std::vector<TextButton *>::iterator iter = _btnFinances.begin(); iter != _btnFinances.end(); ++iter)
	{
		(*iter)->setVisible(true);
	}
	_txtTitle->setText(_game->getLanguage()->getString("STR_FINANCE"));

}

/**
 * handles a click on a region button
 */
void GraphsState::btnRegionListClick(Action * action)
{
	size_t number = action->getSender()->getY()/11;
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

	_regionToggles.at(number) = !_regionToggles.at(number);
	
	if(!_regionToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
	}
	else
	{
		button->invert(adjustment);
	}
	drawLines();
	updateScale(1);
}

/**
 * handles a click on a country button
 */
void GraphsState::btnCountryListClick(Action * action)
{
	size_t number = action->getSender()->getY()/11;
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

	_countryToggles.at(number) = !_countryToggles.at(number);
	
	if(!_countryToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
	}
	else
	{
		button->invert(adjustment);
	}
	drawLines();
	if(!_income)
		updateScale(1);
	else
		updateScale(1000);
}

/**
 * handles a click on a finances button
 */
void GraphsState::btnFinanceListClick(Action *action)
{
	int number = action->getSender()->getY()/11;
	TextButton *button = _btnFinances.at(number);
	int adjustment = -42 + (4*number);
	
	_financeLines.at(number)->setVisible(!_financeToggles.at(number));
	_financeToggles.at(number) = !_financeToggles.at(number);

	if(!_financeToggles.at(number))
	{
		button->setColor(Palette::blockOffset(9)+7);
	}
	else
	{
		button->invert(adjustment);
	}
	drawLines();
	updateScale(1000);
}

/**
 * remove all elements from view
 */
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


	for(std::vector<TextButton *>::iterator iter = _btnRegions.begin(); iter != _btnRegions.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnCountries.begin(); iter != _btnCountries.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}
	for(std::vector<TextButton *>::iterator iter = _btnFinances.begin(); iter != _btnFinances.end(); ++iter)
	{
		(*iter)->setVisible(false);
	}

	_btnRegionTotal->setVisible(false);
	_btnCountryTotal->setVisible(false);
	_txtFactor->setVisible(false);
	_scale = 100;
	updateScale(1);
}

/**
 * updates the text on the vertical scale
 */
void GraphsState::updateScale(int factor)
{
	int offset = 0;
	for(std::vector<Text *>::iterator iter = _txtScale.begin(); iter != _txtScale.end(); ++iter)
	{
		std::wstringstream ss;
		ss << ((_scale/10)/factor)*offset << " ";
		(*iter)->setText(ss.str());
		offset ++;
	}
}

/**
 * instead of having all our line drawing in one giant ridiculous routine, just use the one we need.
 */
void GraphsState::drawLines()
{
	if(!_country && !_finance)
	{
		drawRegionLines();
	}
	else if(!_finance)
	{
		drawCountryLines();
	}
	else
	{
		drawFinanceLines();
	}
}

/**
 * Sets up the screens and draws the lines for country buttons
 * to toggle on and off
 */
void GraphsState::drawCountryLines()
{
	//calculate the totals, and set up our upward maximum
	int roof = 0;
	int totals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(size_t entry = 0; entry != _game->getSavedGame()->getFundsList().size(); ++entry)
	{
		int total = 0;
		if(_alien)
		{
			for(size_t iter = 0; iter != _game->getSavedGame()->getCountries()->size(); ++iter)
			{
				total += _game->getSavedGame()->getCountries()->at(iter)->getActivityAlien().at(entry);
				if(_game->getSavedGame()->getCountries()->at(iter)->getActivityAlien().at(entry) > roof && _countryToggles.at(iter))
				{
					roof = _game->getSavedGame()->getCountries()->at(iter)->getActivityAlien().at(entry);
				}
			}
		}
		else if(_income)
		{
			for(size_t iter = 0; iter != _game->getSavedGame()->getCountries()->size(); ++iter)
			{
				total += _game->getSavedGame()->getCountries()->at(iter)->getFunding().at(entry);
				if(_game->getSavedGame()->getCountries()->at(iter)->getFunding().at(entry) > roof && _countryToggles.at(iter))
				{
					roof = _game->getSavedGame()->getCountries()->at(iter)->getFunding().at(entry);
				}
			}
		}
		else
		{
			for(size_t iter = 0; iter != _game->getSavedGame()->getCountries()->size(); ++iter)
			{
				total += _game->getSavedGame()->getCountries()->at(iter)->getActivityXcom().at(entry);
				if(_game->getSavedGame()->getCountries()->at(iter)->getActivityXcom().at(entry) > roof && _countryToggles.at(iter))
				{
					roof = _game->getSavedGame()->getCountries()->at(iter)->getActivityXcom().at(entry);
				}
			}
		}
		if(_countryToggles.back() && total > roof)
			roof = total;
	}

	// set the scale to fit
	if(!_income)
	{
		for(int check = 100; check <= 100000; check *= 2)
		{
			if(roof < check - (check/10))
			{
				_scale = check;
				break;
			}
		}
	}
	else
	{
		for(int check = 100000; check <= 1000000000; check *= 10)
		{
			if(roof < check - (check/10))
			{
				_scale = check;
				break;
			}
		}
	}
	double dscale = _scale;
	double units = dscale / 140;

	// draw country lines
	for(size_t entry = 0; entry != _game->getSavedGame()->getCountries()->size(); ++entry)
	{
		Country *country = _game->getSavedGame()->getCountries()->at(entry);
		_alienCountryLines.at(entry)->clear();
		_xcomCountryLines.at(entry)->clear();
		_incomeLines.at(entry)->clear();
		std::vector<Sint16> newLineVector;
		int reduction = 0;
		for(size_t iter = 0; iter != 12; ++iter)
		{
			int x = 312 - (iter*17);
			int y = 175;
			if(_alien)
			{
				if(iter < country->getActivityAlien().size())
				{
					reduction = country->getActivityAlien().at(country->getActivityAlien().size()-(1+iter)) / units;
					y -= reduction;
					totals[iter] += country->getActivityAlien().at(country->getActivityAlien().size()-(1+iter));
				}
			}
			else if(_income)
			{
				if(iter < country->getFunding().size())
				{
					reduction = country->getFunding().at(country->getFunding().size()-(1+iter)) / units;
					y -= reduction;
					totals[iter] += country->getFunding().at(country->getFunding().size()-(1+iter));
				}
			}
			else
			{
				if(iter < country->getActivityXcom().size())
				{
					reduction = country->getActivityXcom().at(country->getActivityXcom().size()-(1+iter)) / units;
					y -= reduction;
					totals[iter] += country->getActivityXcom().at(country->getActivityXcom().size()-(1+iter));
				}
			}
			if (y >=175)
				y = 175;
			newLineVector.push_back(y);
			int offset = 0;
			if(entry % 2)
			offset = 8;
			if(newLineVector.size() > 1 && _alien)
			_alienCountryLines.at(entry)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((entry/2)+1)+offset);
			else if(newLineVector.size() > 1 && _income)
				_incomeLines.at(entry)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((entry/2)+1)+offset);
			else if(newLineVector.size() > 1)
				_xcomCountryLines.at(entry)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((entry/2)+1)+offset);
			}
		if(_alien)
			_alienCountryLines.at(entry)->setVisible(_countryToggles.at(entry));
		else if(_income)
			_incomeLines.at(entry)->setVisible(_countryToggles.at(entry));
		else
			_xcomCountryLines.at(entry)->setVisible(_countryToggles.at(entry));
	}
	if(_alien)
		_alienCountryLines.back()->clear();
	else if(_income)
		_incomeLines.back()->clear();
	else
		_xcomCountryLines.back()->clear();

	// set up the "total" line
	std::vector<Sint16> newLineVector;
	for(int iter = 0; iter != 12; ++iter)
	{
		int x = 312 - (iter*17);
		int y = 175;
		if(totals[iter] > 0)
		{
			double dscale = _scale;
			double units = dscale / 140;
			int reduction = totals[iter] / units;
			y -= reduction;
		}
		newLineVector.push_back(y);
		if(newLineVector.size() > 1)
		{
			if(_alien)
				_alienCountryLines.back()->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
			else if(_income)
				_incomeLines.back()->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
			else
				_xcomCountryLines.back()->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
		}
	}
	if(_alien)
		_alienCountryLines.back()->setVisible(_countryToggles.back());
	else if(_income)
		_incomeLines.back()->setVisible(_countryToggles.back());
	else
		_xcomCountryLines.back()->setVisible(_countryToggles.back());
}

/**
 * Sets up the screens and draws the lines for region buttons
 * to toggle on and off
 */
void GraphsState::drawRegionLines()
{
	//calculate the totals, and set up our upward maximum
	int roof = 0;
	int totals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(size_t entry = 0; entry != _game->getSavedGame()->getFundsList().size(); ++entry)
	{
		int total = 0;
		if(_alien)
		{
			for(size_t iter = 0; iter != _game->getSavedGame()->getRegions()->size(); ++iter)
			{
				total += _game->getSavedGame()->getRegions()->at(iter)->getActivityAlien().at(entry);
				if(_game->getSavedGame()->getRegions()->at(iter)->getActivityAlien().at(entry) > roof && _regionToggles.at(iter))
				{
					roof = _game->getSavedGame()->getRegions()->at(iter)->getActivityAlien().at(entry);
				}
			}
		}
		else
		{
			for(size_t iter = 0; iter != _game->getSavedGame()->getRegions()->size(); ++iter)
			{
				total += _game->getSavedGame()->getRegions()->at(iter)->getActivityXcom().at(entry);
				if(_game->getSavedGame()->getRegions()->at(iter)->getActivityXcom().at(entry) > roof && _regionToggles.at(iter))
				{
					roof = _game->getSavedGame()->getRegions()->at(iter)->getActivityXcom().at(entry);
				}
			}
		}
		if(_regionToggles.back() && total > roof)
				roof = total;
	}

	// set the scale to fit
	for(int check = 100; check <= 100000; check *= 2)
	{
		if(roof < check - (check/10))
		{
			_scale = check;
			break;
		}
	}
	double dscale = _scale;
	double units = dscale / 140;

	// draw region lines
	for(size_t entry = 0; entry != _game->getSavedGame()->getRegions()->size(); ++entry)
	{
		Region *region = _game->getSavedGame()->getRegions()->at(entry);
		_alienRegionLines.at(entry)->clear();
		_xcomRegionLines.at(entry)->clear();
		std::vector<Sint16> newLineVector;
		int reduction = 0;
		for(size_t iter = 0; iter != 12; ++iter)
		{
			int x = 312 - (iter*17);
			int y = 175;
			if(_alien)
			{
				if(iter < region->getActivityAlien().size())
				{
					reduction = region->getActivityAlien().at(region->getActivityAlien().size()-(1+iter)) / units;
					y -= reduction;
					totals[iter] += region->getActivityAlien().at(region->getActivityAlien().size()-(1+iter));
				}
			}
			else
			{
				if(iter < region->getActivityXcom().size())
				{
					reduction = region->getActivityXcom().at(region->getActivityXcom().size()-(1+iter)) / units;
					y -= reduction;
					totals[iter] += region->getActivityXcom().at(region->getActivityXcom().size()-(1+iter));
				}
			}
			if (y >=175)
				y = 175;
			newLineVector.push_back(y);
			int offset = 0;
			if(entry % 2)
				offset = 8;
			if(newLineVector.size() > 1 && _alien)
				_alienRegionLines.at(entry)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((entry/2)+1)+offset);
			else if(newLineVector.size() > 1)
				_xcomRegionLines.at(entry)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((entry/2)+1)+offset);
		}

		if(_alien)
			_alienRegionLines.at(entry)->setVisible(_regionToggles.at(entry));
		else
			_xcomRegionLines.at(entry)->setVisible(_regionToggles.at(entry));
	}

	// set up the "total" line
	if(_alien)
		_alienRegionLines.back()->clear();
	else
		_xcomRegionLines.back()->clear();

	std::vector<Sint16> newLineVector;
	for(int iter = 0; iter != 12; ++iter)
	{
		int x = 312 - (iter*17);
		int y = 175;
		if(totals[iter] > 0)
		{
			int reduction = totals[iter] / units;
			y -= reduction;
		}
		newLineVector.push_back(y);
		if(newLineVector.size() > 1)
		{
			if(_alien)
				_alienRegionLines.back()->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
			else
				_xcomRegionLines.back()->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset(9));
		}
	}
	if(_alien)
		_alienRegionLines.back()->setVisible(_regionToggles.back());
	else
		_xcomRegionLines.back()->setVisible(_regionToggles.back());
}

/**
 * Sets up the screens and draws the lines for the finance buttons
 * to toggle on and off
 */
void GraphsState::drawFinanceLines()
{
	//set up arrays
	int roof = 0;
	int incomeTotals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int balanceTotals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int expendTotals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int maintTotals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int scoreTotals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	// start filling those arrays with score values
	// determine which is the highest one being displayed, so we can adjust the scale
	for(size_t entry = 0; entry != _game->getSavedGame()->getFundsList().size(); ++entry)
	{
		maintTotals[entry] = _game->getSavedGame()->getMaintenances().at(_game->getSavedGame()->getMaintenances().size() -(1+entry));
		balanceTotals[entry] = _game->getSavedGame()->getFundsList().at(_game->getSavedGame()->getFundsList().size()-(1+entry));

		for(std::vector<Country*>::iterator iter = _game->getSavedGame()->getCountries()->begin(); iter != _game->getSavedGame()->getCountries()->end(); ++iter)
		{
			scoreTotals[entry] += (*iter)->getActivityXcom().at(entry) - (*iter)->getActivityAlien().at(entry);
			incomeTotals[entry] += (*iter)->getFunding().at((*iter)->getFunding().size()-(1+entry));
			if(_financeToggles.at(0) && incomeTotals[entry] > roof)
				roof = incomeTotals[entry];
		}
		for(std::vector<Region*>::iterator iter = _game->getSavedGame()->getRegions()->begin(); iter != _game->getSavedGame()->getRegions()->end(); ++iter)
		{
			scoreTotals[entry] += (*iter)->getActivityXcom().at(entry) - (*iter)->getActivityAlien().at(entry);
		}
		
		if(_financeToggles.at(2) && maintTotals[entry] > roof)
			roof = maintTotals[entry];
		if(_financeToggles.at(3) && balanceTotals[entry] > roof)
			roof = balanceTotals[entry];
		if(_financeToggles.at(4) && scoreTotals[entry] > roof)
			roof = scoreTotals[entry];
	}
	maintTotals[0] = _game->getSavedGame()->getBaseMaintenance();
	if(_financeToggles.at(2) && maintTotals[0] > roof)
		roof = maintTotals[0];
	expendTotals[0] = balanceTotals[1] - balanceTotals[0];
	if(expendTotals[0] < 0)
		expendTotals[0] = 0;
	if(_financeToggles.at(1) && expendTotals[0] > roof)
		roof = expendTotals[0];

	for(size_t entry = 1; entry != _game->getSavedGame()->getFundsList().size(); ++entry)
	{
		expendTotals[entry] = ((balanceTotals[entry+1] + incomeTotals[entry]) - maintTotals[entry])-balanceTotals[entry];
		if(expendTotals[entry] < 0)
			expendTotals[entry] = 0;
		if(_financeToggles.at(1) && expendTotals[entry] > roof)
			roof = expendTotals[entry];
	}

	//adjust the scale to fit the upward maximum
	for(int check = 100000; check <= 1000000000; check *= 10)
	{
		if(roof < check - (check/10))
		{
			_scale = check;
			break;
		}
	}
	
	//toggle screens
	for(int button = 0; button != 5; ++button)
	{
		_financeLines.at(button)->setVisible(_financeToggles.at(button));
		_financeLines.at(button)->clear();
	}

	//figure out how many units to the pixel, then plot the points for the graph and connect the dots.
	double dscale = _scale;
	double units = dscale / 140;
	for(int button = 0; button != 5; ++button)
	{
		std::vector<Sint16> newLineVector;
		for(int iter = 0; iter != 12; ++iter)
		{
			int x = 312 - (iter*17);
			int y = 175;
			int reduction = 0;
			switch(button)
			{
			case 0:
				reduction = incomeTotals[iter] / units;
				break;
			case 1:
				reduction = expendTotals[iter] / units;
				break;
			case 2:
				reduction = maintTotals[iter] / units;
				break;
			case 3:
				reduction = balanceTotals[iter] / units;
				break;
			case 4:
				reduction = scoreTotals[iter] / units;
				break;
			}
			y -= reduction;
			newLineVector.push_back(y);
			int offset = 0;
			if(button % 2)
				offset = 8;
			if(newLineVector.size() > 1)
				_financeLines.at(button)->drawLine(x, y, x+17, newLineVector.at(newLineVector.size()-2), Palette::blockOffset((button/2)+1)+offset);
		}
	}
}
}
