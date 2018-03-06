/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "StatisticsState.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Options.h"
#include "../Savegame/SavedGame.h"
#include "MainMenuState.h"
#include "../Savegame/MissionStatistics.h"
#include "../Savegame/Base.h"
#include "../Savegame/SoldierDiary.h"
#include "../Savegame/SoldierDeath.h"
#include "../Savegame/BattleUnitStatistics.h"
#include "../Savegame/Country.h"
#include "../Savegame/AlienBase.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Statistics window.
 * @param game Pointer to the core game.
 */
StatisticsState::StatisticsState()
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnOk = new TextButton(50, 12, 135, 180);
	_txtTitle = new Text(310, 25, 5, 8);
	_lstStats = new TextList(280, 136, 12, 36);

	// Set palette
	setInterface("endGameStatistics");

	add(_window, "window", "endGameStatistics");
	add(_btnOk, "button", "endGameStatistics");
	add(_txtTitle, "text", "endGameStatistics");
	add(_lstStats, "list", "endGameStatistics");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&StatisticsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&StatisticsState::btnOkClick, Options::keyOk);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_lstStats->setColumns(2, 200, 80);
	_lstStats->setDot(true);

	listStats();
}

/**
 *
 */
StatisticsState::~StatisticsState()
{

}

template <typename T>
T StatisticsState::sumVector(const std::vector<T> &vec) const
{
	T total = 0;
	for (typename std::vector<T>::const_iterator i = vec.begin(); i != vec.end(); ++i)
	{
		total += *i;
	}
	return total;
}

void StatisticsState::listStats()
{
	SavedGame *save = _game->getSavedGame();

	std::wostringstream ss;
	GameTime *time = save->getTime();
	if (save->getEnding() == END_WIN)
	{
		ss << tr("STR_VICTORY");
	}
	else if (save->getEnding() == END_LOSE)
	{
		ss << tr("STR_DEFEAT");
	}
	else
	{
		ss << tr("STR_STATISTICS");
	}
	ss << L'\x02' << time->getDayString(_game->getLanguage()) << L" " << tr(time->getMonthString()) << L" " << time->getYear();
	_txtTitle->setText(ss.str());

	int monthlyScore = sumVector(save->getResearchScores()) / save->getResearchScores().size();
	int64_t totalIncome = sumVector(save->getIncomes());
	int64_t totalExpenses = sumVector(save->getExpenditures());

	int alienBasesDestroyed = 0, xcomBasesLost = 0;
	int missionsWin = 0, missionsLoss = 0, nightMissions = 0;
	int bestScore = -9999, worstScore = 9999;
	for (std::vector<MissionStatistics*>::const_iterator i = save->getMissionStatistics()->begin(); i != save->getMissionStatistics()->end(); ++i)
	{
		if ((*i)->success)
		{
			missionsWin++;
		}
		else
		{
			missionsLoss++;
		}
		bestScore = std::max(bestScore, (*i)->score);
		worstScore = std::min(worstScore, (*i)->score);
		if ((*i)->daylight > 5)
		{
			nightMissions++;
		}
		if ((*i)->isAlienBase() && (*i)->success)
		{
			alienBasesDestroyed++;
		}
		if ((*i)->isBaseDefense() && !(*i)->success)
		{
			xcomBasesLost++;
		}
	}
	// Make sure dummy values aren't left in
	bestScore = (bestScore == -9999) ? 0 : bestScore;
	worstScore = (worstScore == 9999) ? 0 : worstScore;

	std::vector<Soldier*> allSoldiers;
	for (std::vector<Base*>::const_iterator i = save->getBases()->begin(); i != save->getBases()->end(); ++i)
	{
		allSoldiers.insert(allSoldiers.end(), (*i)->getSoldiers()->begin(), (*i)->getSoldiers()->end());
	}
	allSoldiers.insert(allSoldiers.end(), save->getDeadSoldiers()->begin(), save->getDeadSoldiers()->end());
	int soldiersRecruited = allSoldiers.size();
	int soldiersLost = save->getDeadSoldiers()->size();

	int aliensKilled = 0, aliensCaptured = 0, friendlyKills = 0;
	int daysWounded = 0, longestMonths = 0;
	int shotsFired = 0, shotsLanded = 0;
	std::map<std::string, int> weaponKills, alienKills;
	for (std::vector<Soldier*>::iterator i = allSoldiers.begin(); i != allSoldiers.end(); ++i)
	{
		SoldierDiary *diary = (*i)->getDiary();
		aliensKilled += diary->getKillTotal();
		aliensCaptured += diary->getStunTotal();
		daysWounded += diary->getDaysWoundedTotal();
		longestMonths = std::max(longestMonths, diary->getMonthsService());
		std::map<std::string, int> weaponTotal = diary->getWeaponTotal();
		shotsFired += diary->getShotsFiredTotal();
		shotsLanded += diary->getShotsLandedTotal();
		for (std::map<std::string, int>::const_iterator j = weaponTotal.begin(); j != weaponTotal.end(); ++j)
		{
			if (weaponKills.find(j->first) == weaponKills.end())
			{
				weaponKills[j->first] = j->second;
			}
			else
			{
				weaponKills[j->first] += j->second;
			}
		}

		if ((*i)->getDeath() != 0 && (*i)->getDeath()->getCause() != 0)
		{
			const BattleUnitKills *kills = (*i)->getDeath()->getCause();
			if (kills->faction == FACTION_PLAYER)
			{
				friendlyKills++;
			}
			if (!kills->race.empty())
			{
				if (alienKills.find(kills->race) == alienKills.end())
				{
					alienKills[kills->race] = 1;
				}
				else
				{
					alienKills[kills->race] += 1;
				}
			}
		}
	}
	int accuracy = 0;
	if (shotsFired > 0)
	{
		accuracy = 100 * shotsLanded / shotsFired;
	}

	int maxWeapon = 0;
	std::string highestWeapon = "STR_NONE";
	for (std::map<std::string, int>::const_iterator i = weaponKills.begin(); i != weaponKills.end(); ++i)
	{
		if (i->second > maxWeapon)
		{
			maxWeapon = i->second;
			highestWeapon = i->first;
		}
	}
	int maxAlien = 0;
	std::string highestAlien = "STR_NONE";
	for (std::map<std::string, int>::const_iterator i = alienKills.begin(); i != alienKills.end(); ++i)
	{
		if (i->second > maxAlien)
		{
			maxAlien = i->second;
			highestAlien = i->first;
		}
	}

	std::map<std::string, int> ids = save->getAllIds();
	int alienBases = alienBasesDestroyed;
	for (std::vector<AlienBase*>::iterator i = save->getAlienBases()->begin(); i != save->getAlienBases()->end(); ++i)
	{
		if ((*i)->isDiscovered())
		{
			alienBases++;
		}
	}
	int ufosDetected = std::max(0, ids["STR_UFO"] - 1);
	int terrorSites = std::max(0, ids["STR_TERROR_SITE"] - 1);
	int totalCrafts = 0;
	for (std::vector<std::string>::const_iterator i = _game->getMod()->getCraftsList().begin(); i != _game->getMod()->getCraftsList().end(); ++i)
	{
		totalCrafts += std::max(0, ids[*i] - 1);
	}

	int xcomBases = save->getBases()->size() + xcomBasesLost;
	int currentScientists = 0, currentEngineers = 0;
	for (std::vector<Base*>::const_iterator i = save->getBases()->begin(); i != save->getBases()->end(); ++i)
	{
		currentScientists += (*i)->getTotalScientists();
		currentEngineers += (*i)->getTotalEngineers();
	}

	int countriesLost = 0;
	for (std::vector<Country*>::const_iterator i = save->getCountries()->begin(); i != save->getCountries()->end(); ++i)
	{
		if ((*i)->getPact())
		{
			countriesLost++;
		}
	}

	int researchDone = save->getDiscoveredResearch().size();
	
	std::string difficulty[] = { "STR_1_BEGINNER", "STR_2_EXPERIENCED", "STR_3_VETERAN", "STR_4_GENIUS", "STR_5_SUPERHUMAN" };

	_lstStats->addRow(2, tr("STR_DIFFICULTY").c_str(), tr(difficulty[save->getDifficulty()]).c_str());
	_lstStats->addRow(2, tr("STR_AVERAGE_MONTHLY_RATING").c_str(), Text::formatNumber(monthlyScore).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_INCOME").c_str(), Text::formatFunding(totalIncome).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_EXPENDITURE").c_str(), Text::formatFunding(totalExpenses).c_str());
	_lstStats->addRow(2, tr("STR_MISSIONS_WON").c_str(), Text::formatNumber(missionsWin).c_str());
	_lstStats->addRow(2, tr("STR_MISSIONS_LOST").c_str(), Text::formatNumber(missionsLoss).c_str());
	_lstStats->addRow(2, tr("STR_NIGHT_MISSIONS").c_str(), Text::formatNumber(nightMissions).c_str());
	_lstStats->addRow(2, tr("STR_BEST_RATING").c_str(), Text::formatNumber(bestScore).c_str());
	_lstStats->addRow(2, tr("STR_WORST_RATING").c_str(), Text::formatNumber(worstScore).c_str());
	_lstStats->addRow(2, tr("STR_SOLDIERS_RECRUITED").c_str(), Text::formatNumber(soldiersRecruited).c_str());
	_lstStats->addRow(2, tr("STR_SOLDIERS_LOST").c_str(), Text::formatNumber(soldiersLost).c_str());
	_lstStats->addRow(2, tr("STR_ALIEN_KILLS").c_str(), Text::formatNumber(aliensKilled).c_str());
	_lstStats->addRow(2, tr("STR_ALIEN_CAPTURES").c_str(), Text::formatNumber(aliensCaptured).c_str());
	_lstStats->addRow(2, tr("STR_FRIENDLY_KILLS").c_str(), Text::formatNumber(friendlyKills).c_str());
	_lstStats->addRow(2, tr("STR_AVERAGE_ACCURACY").c_str(), Text::formatPercentage(accuracy).c_str());
	_lstStats->addRow(2, tr("STR_WEAPON_MOST_KILLS").c_str(), tr(highestWeapon).c_str());
	_lstStats->addRow(2, tr("STR_ALIEN_MOST_KILLS").c_str(), tr(highestAlien).c_str());
	_lstStats->addRow(2, tr("STR_LONGEST_SERVICE").c_str(), Text::formatNumber(longestMonths).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_DAYS_WOUNDED").c_str(), Text::formatNumber(daysWounded).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_UFOS").c_str(), Text::formatNumber(ufosDetected).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_ALIEN_BASES").c_str(), Text::formatNumber(alienBases).c_str());
	_lstStats->addRow(2, tr("STR_COUNTRIES_LOST").c_str(), Text::formatNumber(countriesLost).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_TERROR_SITES").c_str(), Text::formatNumber(terrorSites).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_BASES").c_str(), Text::formatNumber(xcomBases).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_CRAFT").c_str(), Text::formatNumber(totalCrafts).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_SCIENTISTS").c_str(), Text::formatNumber(currentScientists).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_ENGINEERS").c_str(), Text::formatNumber(currentEngineers).c_str());
	_lstStats->addRow(2, tr("STR_TOTAL_RESEARCH").c_str(), Text::formatNumber(researchDone).c_str());
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void StatisticsState::btnOkClick(Action *)
{
	if (_game->getSavedGame()->getEnding() == END_NONE)
	{
		_game->popState();
	}
	else
	{
		_game->setSavedGame(0);
		_game->setState(new GoToMainMenuState);
	}
}

}
