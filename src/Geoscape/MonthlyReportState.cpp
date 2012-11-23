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
#include "MonthlyReportState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/GameTime.h"
#include "PsiTrainingState.h"
#include "../Savegame/Region.h"
#include "../Savegame/Country.h"
#include "../Ruleset/RuleCountry.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Monthly Report screen.
 * @param game Pointer to the core game.
 */
MonthlyReportState::MonthlyReportState(Game *game, bool psi) : State(game), 
_psi(psi), 
_ratingTotal(0), 
_fundingDiff(0), 
_generalSatisfaction(0), 
_happyList(0), 
_sadList(0), 
_pactList(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(50, 12, 135, 180);
	_txtTitle = new Text(300, 16, 16, 8);
	_txtMonth = new Text(110, 8, 16, 24);
	_txtRating = new Text(180, 8, 125, 24);
	_txtRatingTxt = new Text(100, 8, 225, 24);
	_txtChange = new Text(300, 8, 16, 32);
	_txtDesc = new Text(280, 140, 16, 40);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtMonth);
	add(_txtRating);
	add(_txtRatingTxt);
	add(_txtChange);
	add(_txtDesc);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+10);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&MonthlyReportState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString("STR_XCOM_PROJECT_MONTHLY_REPORT"));

	CalculateChanges();

	int month = _game->getSavedGame()->getTime()->getMonth() - 1, year = _game->getSavedGame()->getTime()->getYear();
	if (month == 0)
	{
		month = 12;
		year--;
	}
	std::string m;
	switch (month)
	{
	case 1: m = "STR_JAN"; break;
	case 2: m = "STR_FEB"; break;
	case 3: m = "STR_MAR"; break;
	case 4: m = "STR_APR"; break;
	case 5: m = "STR_MAY"; break;
	case 6: m = "STR_JUN"; break;
	case 7: m = "STR_JUL"; break;
	case 8: m = "STR_AUG"; break;
	case 9: m = "STR_SEP"; break;
	case 10: m = "STR_OCT"; break;
	case 11: m = "STR_NOV"; break;
	case 12: m = "STR_DEC"; break;
	}

	std::wstringstream ss;
	ss << _game->getLanguage()->getString("STR_MONTH") << L'\x01' << _game->getLanguage()->getString(m) << L" " << year;

	_txtMonth->setColor(Palette::blockOffset(15)-1);
	_txtMonth->setSecondaryColor(Palette::blockOffset(8)+10);
	_txtMonth->setText(ss.str());

	std::wstringstream ss2;
	ss2 << _game->getLanguage()->getString("STR_MONTHLY_RATING") << L'\x01' << _ratingTotal;

	_txtRating->setColor(Palette::blockOffset(15)-1);
	_txtRating->setSecondaryColor(Palette::blockOffset(8)+10);
	_txtRating->setText(ss2.str());
	
	// Calculate rating
	std::wstring rating;
	if (_ratingTotal <= -200)
	{
		rating = _game->getLanguage()->getString("STR_RATING_TERRIBLE");
	}
	else if (_ratingTotal <= 0)
	{
		rating = _game->getLanguage()->getString("STR_RATING_POOR");
	}
	else if (_ratingTotal <= 200)
	{
		rating = _game->getLanguage()->getString("STR_RATING_OK");
	}
	else if (_ratingTotal <= 500)
	{
		rating = _game->getLanguage()->getString("STR_RATING_GOOD");
	}
	else
	{
		rating += _game->getLanguage()->getString("STR_RATING_EXCELLENT");
	}
	_txtRatingTxt->setColor(Palette::blockOffset(15)-1);
	_txtRatingTxt->setText(rating);

	std::wstringstream ss3;
	ss3 << _game->getLanguage()->getString("STR_FUNDING_CHANGE") << L'\x01' << _fundingDiff;

	_txtChange->setColor(Palette::blockOffset(15)-1);
	_txtChange->setSecondaryColor(Palette::blockOffset(8)+10);
	_txtChange->setText(ss3.str());

	_txtDesc->setColor(Palette::blockOffset(8)+10);
	_txtDesc->setWordWrap(true);
	std::wstringstream ss4;
	switch (_generalSatisfaction)
	{
	case 0:
		ss4 << _game->getLanguage()->getString("STR_YOU_HAVE_NOT_SUCCEEDED");
		// game over man
		break;
	case 1:
		ss4 << _game->getLanguage()->getString("STR_COUNCIL_IS_DISSATISFIED");
		break;
	case 3:
		ss4 << _game->getLanguage()->getString("STR_COUNCIL_IS_VERY_PLEASED");
		break;
	default:
		ss4 << _game->getLanguage()->getString("STR_COUNCIL_IS_GENERALLY_SATISFIED");
		break;
	}
	if(_happyList.size())
	{
		ss4 << "\n\n";
		for(std::vector<std::string>::iterator happy = _happyList.begin(); happy != _happyList.end(); ++happy)
		{
			ss4 << _game->getLanguage()->getString(*happy);
			if(_happyList.size() > 1)
			{
				if(happy == _happyList.end()-2)
				{
					ss4 << _game->getLanguage()->getString("STR_AND");
				}
				if(_happyList.size() > 3)
				{
					if(happy != _happyList.end() - 1 && happy != _happyList.end() - 2)
					{
						ss4 << ", ";
					}
				}
			}
			if(happy == _happyList.end()-1)
			{
				if(_happyList.size() > 1)
				{
					ss4 << _game->getLanguage()->getString("STR_COUNTRIES_ARE_PARTICULARLY_HAPPY");	
				}
				else
				{
					ss4 << _game->getLanguage()->getString("STR_COUNTRY_IS_PARTICULARLY_PLEASED");
				}
			}
		}
	}
	if(_sadList.size())
	{
		ss4 << "\n\n";
		for(std::vector<std::string>::iterator sad = _sadList.begin(); sad != _sadList.end(); ++sad)
		{
			ss4 << _game->getLanguage()->getString(*sad);
			if(_sadList.size() > 1)
			{
				if(sad == _sadList.end()-2)
				{
					ss4 << _game->getLanguage()->getString("STR_AND");
				}
				if(_sadList.size() > 3)
				{
					if(sad != _sadList.end() - 1 && sad != _sadList.end() - 2)
					{
						ss4 << ", ";
					}
				}
			}
			if(sad == _sadList.end()-1)
			{
				if(_sadList.size() > 1)
				{
					ss4 << _game->getLanguage()->getString("STR_COUNTRIES_ARE_UNHAPPY_WITH_YOUR_ABILITY");	
				}
				else
				{
					ss4 << _game->getLanguage()->getString("STR_COUNTRY_IS_UNHAPPY_WITH_YOUR_ABILITY");
				}
			}
		}
	}
	if(_pactList.size())
	{
		ss4 << "\n\n";
		for(std::vector<std::string>::iterator pact = _pactList.begin(); pact != _pactList.end(); ++pact)
		{
			ss4 << _game->getLanguage()->getString(*pact);
			if(_pactList.size() > 1)
			{
				if(pact == _pactList.end()-2)
				{
					ss4 << _game->getLanguage()->getString("STR_AND");
				}
				if(_pactList.size() > 3)
				{
					if(pact != _pactList.end() - 1 && pact != _pactList.end() - 2)
					{
						ss4 << ", ";
					}
				}
			}
			if(pact == _pactList.end()-1)
			{
				if(_pactList.size() > 1)
				{
					ss4 << _game->getLanguage()->getString("STR_COUNTRIES_HAVE_SIGNED_A_SECRET_PACT");	
				}
				else
				{
					ss4 << _game->getLanguage()->getString("STR_COUNTRY_HAS_SIGNED_A_SECRET_PACT");
				}
			}
		}
	}
	_txtDesc->setText(ss4.str());
}

/**
 *
 */
MonthlyReportState::~MonthlyReportState()
{

}

/**
 * Resets the palette.
 */
void MonthlyReportState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void MonthlyReportState::btnOkClick(Action *action)
{
	_game->popState();
	if(_psi)
	{
		_game->pushState (new PsiTrainingState(_game));
	}
}

void MonthlyReportState::CalculateChanges()
{
	// update activity counters
	for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
	{
		(*k)->newMonth();
		_ratingTotal += (*k)->getActivityXcom().at((*k)->getActivityXcom().size()-2)-(*k)->getActivityAlien().at((*k)->getActivityAlien().size()-2);
	}
	// update activity counters
	for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
	{
		_generalSatisfaction += (*k)->getSatisfaction(_game->getSavedGame()->getDifficulty());
		_ratingTotal += (*k)->getActivityXcom().at((*k)->getActivityXcom().size()-1)-(*k)->getActivityAlien().at((*k)->getActivityAlien().size()-1);
		switch((*k)->getSatisfaction(_game->getSavedGame()->getDifficulty()))
		{
		case 0:
			if((*k)->isNewPact())
			{
				_pactList.push_back((*k)->getRules()->getType());
			}
			break;
		case 1:
			_sadList.push_back((*k)->getRules()->getType());
			break;
		case 3:
			_happyList.push_back((*k)->getRules()->getType());
			break;
		default:
			break;
		}
		(*k)->newMonth(_game->getSavedGame()->getDifficulty());
		_fundingDiff += (*k)->getFunding().at((*k)->getFunding().size()-1)-(*k)->getFunding().at((*k)->getFunding().size()-2);
	}
	int areas = (_game->getSavedGame()->getRegions()->size()) + (_game->getSavedGame()->getCountries()->size());
	_ratingTotal = _ratingTotal / areas;
	_generalSatisfaction = _generalSatisfaction / _game->getSavedGame()->getCountries()->size();
}
}
