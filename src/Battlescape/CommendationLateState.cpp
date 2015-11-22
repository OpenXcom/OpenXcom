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
#include "CommendationLateState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDiary.h"
#include "../Engine/Options.h"
#include "../Ruleset/RuleCommendations.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Medals screen.
 * @param game Pointer to the core game.
 */
CommendationLateState::CommendationLateState(std::vector<Soldier*> soldiersMedalled)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(288, 16, 16, 176);
	_txtTitle = new Text(300, 16, 10, 8);
	_lstSoldiers = new TextList(288, 128, 8, 32);

	// Set palette
	setInterface("soldierMemorial");

	_game->getMod()->playMusic("GMLOSE");

	add(_window, "window", "soldierMemorial");
	add(_btnOk, "button", "soldierMemorial");
	add(_txtTitle, "text", "soldierMemorial");
	add(_lstSoldiers, "list", "soldierMemorial");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK02.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CommendationLateState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CommendationLateState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&CommendationLateState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_LOST_IN_SERVICE"));

	_lstSoldiers->setColumns(5, 51, 51, 51, 51, 84);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(8);
	_lstSoldiers->setFlooding(true);

    /***
    
                                            LOST IN SERVICE
                
    SOLDIER NAME, RANK: ___, SCORE: ___, KILLS: ___, NUMBER OF MISSIONS: ___, DAYS WOUNDED: ___, TIMES HIT: ___    
      COMMENDATION
      COMMENDATION
      COMMENDATION
      CAUSE OF DEATH: KILLED BY ALIEN_RACE ALIEN_RANK, USING WEAPON
    
    
    ***/

	std::map<std::string, RuleCommendations *> commendationsList = _game->getMod()->getCommendation();
	bool modularCommendation;
	std::string noun;



	// Loop over dead soldiers
	for (std::vector<Soldier*>::iterator s = soldiersMedalled.begin() ; s != soldiersMedalled.end(); ++s)
	{
		// Establish some base information
		std::wstringstream soldierName, soldierRank, soldierScore, SoldierStat;
		soldierName << (*s)->getName().c_str();
		soldierRank << tr((*s)->getRankString()).c_str();
		soldierScore << "Score: " << (*s)->getDiary()->getScoreTotal(); // TODO: Translatable
		SoldierStat << "Kills: " << (*s)->getDiary()->getKillTotal(); // TODO: Translatable

		// Soldier name and rank
		_lstSoldiers->addRow(5, soldierName.str().c_str(),
								L"",
								soldierRank.str().c_str(),
								soldierScore.str().c_str(),
								SoldierStat.str().c_str());

		// Loop over all commendations
		for (std::map<std::string, RuleCommendations *>::const_iterator commList = commendationsList.begin(); commList != commendationsList.end();)
		{
			std::wstringstream commendationName, commendationDecoration;
			modularCommendation = false;
			noun = "noNoun";

			// Loop over soldier's commendations
			for (std::vector<SoldierCommendations*>::const_iterator soldierComm = (*s)->getDiary()->getSoldierCommendations()->begin(); soldierComm != (*s)->getDiary()->getSoldierCommendations()->end(); ++soldierComm)
			{
				if ((*soldierComm)->getType() == (*commList).first && (*soldierComm)->isNew() && noun == "noNoun")
				{
					(*soldierComm)->makeOld();

					if ((*soldierComm)->getNoun() != "noNoun")
					{
						noun = (*soldierComm)->getNoun();
						modularCommendation = true;
					}
					// Decoration level name
					int skipCounter = 0;
					int lastInt = -2;
					int thisInt = -1;
					int vectorIterator = 0;
					for (std::vector<int>::const_iterator k = (*commList).second->getCriteria()->begin()->second.begin(); k != (*commList).second->getCriteria()->begin()->second.end(); ++k)
					{
						if (vectorIterator == (*soldierComm)->getDecorationLevelInt() + 1)
						{
							break;
						}
						thisInt = *k;
						if (k != (*commList).second->getCriteria()->begin()->second.begin())
						{
							--k;
							lastInt = *k;
							++k;
						}
						if (thisInt == lastInt)
						{
							skipCounter++;
						}
						vectorIterator++;					
					}
					commendationDecoration << tr((*soldierComm)->getDecorationLevelName(skipCounter)).c_str();
					// Establish comms name
					// Medal name
					commendationName << "   ";
					if (modularCommendation)
					{
					commendationName << tr((*commList).first).arg(tr(noun).c_str()).c_str();
					}
					else
					{
					commendationName << tr((*commList).first).c_str();
					}
					_lstSoldiers->addRow(5, commendationName.str().c_str(), L"", L"", L"", commendationDecoration.str().c_str());
					break;
				}
			} // END SOLDIER COMMS LOOP			
      
			if (noun == "noNoun")
			{
				++commList;
			}
		} // END COMMS LOOPS
		_lstSoldiers->addRow(5, L"", L"", L"", L"", L""); // Seperator
	} // END SOLDIER LOOP    
}

/**
 *
 */
CommendationLateState::~CommendationLateState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CommendationLateState::btnOkClick(Action *)
{
	_game->popState();
}

}
