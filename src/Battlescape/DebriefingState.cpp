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
#include "DebriefingState.h"
#include <algorithm>
#include "CannotReequipState.h"
#include "../Engine/Action.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/Window.h"
#include "PromotionsState.h"
#include "CommendationState.h"
#include "CommendationLateState.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleCountry.h"
#include "../Mod/RuleCraft.h"
#include "../Mod/RuleItem.h"
#include "../Mod/RuleRegion.h"
#include "../Mod/RuleUfo.h"
#include "../Mod/Armor.h"
#include "../Savegame/AlienBase.h"
#include "../Savegame/AlienMission.h"
#include "../Savegame/Base.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Country.h"
#include "../Savegame/Craft.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/Region.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDiary.h"
#include "../Savegame/MissionSite.h"
#include "../Savegame/Tile.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Vehicle.h"
#include "../Savegame/BaseFacility.h"
#include <sstream>
#include "../Menu/ErrorMessageState.h"
#include "../Menu/MainMenuState.h"
#include "../Interface/Cursor.h"
#include "../Engine/Options.h"
#include "../Basescape/ManageAlienContainmentState.h"
#include "../Engine/Screen.h"
#include "../Basescape/SellState.h"
#include "../Menu/SaveGameState.h"
#include "../Mod/AlienDeployment.h"
#include "../Mod/RuleInterface.h"
#include "../Savegame/MissionStatistics.h"
#include "../Savegame/BattleUnitStatistics.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Debriefing screen.
 * @param game Pointer to the core game.
 */
DebriefingState::DebriefingState() : _region(0), _country(0), _positiveScore(true), _noContainment(false), _manageContainment(false), _destroyBase(false), _showSoldierStats(false)
{
	_missionStatistics = new MissionStatistics();

	Options::baseXResolution = Options::baseXGeoscape;
	Options::baseYResolution = Options::baseYGeoscape;
	_game->getScreen()->resetDisplay(false);

	// Restore the cursor in case something weird happened
	_game->getCursor()->setVisible(true);
	_limitsEnforced = Options::storageLimitsEnforced ? 1 : 0;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(40, 12, 16, 180);
	_btnStats = new TextButton(40, 12, 264, 180);
	_txtTitle = new Text(300, 17, 16, 8);
	_txtItem = new Text(180, 9, 16, 24);
	_txtQuantity = new Text(60, 9, 200, 24);
	_txtScore = new Text(55, 9, 270, 24);
	_txtRecovery = new Text(180, 9, 16, 60);
	_txtRating = new Text(200, 9, 64, 180);
	_lstStats = new TextList(290, 80, 16, 32);
	_lstRecovery = new TextList(290, 80, 16, 32);
	_lstTotal = new TextList(290, 9, 16, 12);

	// Second page (soldier stats)
	_txtSoldier     = new Text(90, 9,  16, 24); //16..106 = 90
	_txtTU          = new Text(18, 9, 106, 24); //106
	_txtStamina     = new Text(18, 9, 124, 24); //124
	_txtHealth      = new Text(18, 9, 142, 24); //142
	_txtBravery     = new Text(18, 9, 160, 24); //160
	_txtReactions   = new Text(18, 9, 178, 24); //178
	_txtFiring      = new Text(18, 9, 196, 24); //196
	_txtThrowing    = new Text(18, 9, 214, 24); //214
	_txtMelee       = new Text(18, 9, 232, 24); //232
	_txtStrength    = new Text(18, 9, 250, 24); //250
	_txtPsiStrength = new Text(18, 9, 268, 24); //268
	_txtPsiSkill    = new Text(18, 9, 286, 24); //286..304 = 18

	_lstSoldierStats = new TextList(288, 128, 16, 32);

	_txtTooltip = new Text(200, 9, 64, 180);

	applyVisibility();

	// Set palette
	setInterface("debriefing");

	add(_window, "window", "debriefing");
	add(_btnOk, "button", "debriefing");
	add(_btnStats, "button", "debriefing");
	add(_txtTitle, "heading", "debriefing");
	add(_txtItem, "text", "debriefing");
	add(_txtQuantity, "text", "debriefing");
	add(_txtScore, "text", "debriefing");
	add(_txtRecovery, "text", "debriefing");
	add(_txtRating, "text", "debriefing");
	add(_lstStats, "list", "debriefing");
	add(_lstRecovery, "list", "debriefing");
	add(_lstTotal, "totals", "debriefing");

	add(_txtSoldier, "text", "debriefing");
	add(_txtTU, "text", "debriefing");
	add(_txtStamina, "text", "debriefing");
	add(_txtHealth, "text", "debriefing");
	add(_txtBravery, "text", "debriefing");
	add(_txtReactions, "text", "debriefing");
	add(_txtFiring, "text", "debriefing");
	add(_txtThrowing, "text", "debriefing");
	add(_txtMelee, "text", "debriefing");
	add(_txtStrength, "text", "debriefing");
	add(_txtPsiStrength, "text", "debriefing");
	add(_txtPsiSkill, "text", "debriefing");
	add(_lstSoldierStats, "list", "debriefing");
	add(_txtTooltip, "text", "debriefing");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&DebriefingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&DebriefingState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&DebriefingState::btnOkClick, Options::keyCancel);

	_btnStats->onMouseClick((ActionHandler)&DebriefingState::btnStatsClick);

	_txtTitle->setBig();

	_txtItem->setText(tr("STR_LIST_ITEM"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));
	_txtQuantity->setAlign(ALIGN_RIGHT);

	_txtScore->setText(tr("STR_SCORE"));

	_lstStats->setColumns(3, 224, 30, 64);
	_lstStats->setDot(true);

	_lstRecovery->setColumns(3, 224, 30, 64);
	_lstRecovery->setDot(true);

	_lstTotal->setColumns(2, 254, 64);
	_lstTotal->setDot(true);

	// Second page
	_txtSoldier->setText(tr("STR_NAME_UC"));

	_txtTU->setAlign(ALIGN_CENTER);
	_txtTU->setText(tr("STR_TIME_UNITS_ABBREVIATION"));
	_txtTU->setTooltip("STR_TIME_UNITS");
	_txtTU->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtTU->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtStamina->setAlign(ALIGN_CENTER);
	_txtStamina->setText(tr("STR_STAMINA_ABBREVIATION"));
	_txtStamina->setTooltip("STR_STAMINA");
	_txtStamina->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtStamina->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtHealth->setAlign(ALIGN_CENTER);
	_txtHealth->setText(tr("STR_HEALTH_ABBREVIATION"));
	_txtHealth->setTooltip("STR_HEALTH");
	_txtHealth->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtHealth->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtBravery->setAlign(ALIGN_CENTER);
	_txtBravery->setText(tr("STR_BRAVERY_ABBREVIATION"));
	_txtBravery->setTooltip("STR_BRAVERY");
	_txtBravery->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtBravery->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtReactions->setAlign(ALIGN_CENTER);
	_txtReactions->setText(tr("STR_REACTIONS_ABBREVIATION"));
	_txtReactions->setTooltip("STR_REACTIONS");
	_txtReactions->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtReactions->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtFiring->setAlign(ALIGN_CENTER);
	_txtFiring->setText(tr("STR_FIRING_ACCURACY_ABBREVIATION"));
	_txtFiring->setTooltip("STR_FIRING_ACCURACY");
	_txtFiring->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtFiring->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtThrowing->setAlign(ALIGN_CENTER);
	_txtThrowing->setText(tr("STR_THROWING_ACCURACY_ABBREVIATION"));
	_txtThrowing->setTooltip("STR_THROWING_ACCURACY");
	_txtThrowing->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtThrowing->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtMelee->setAlign(ALIGN_CENTER);
	_txtMelee->setText(tr("STR_MELEE_ACCURACY_ABBREVIATION"));
	_txtMelee->setTooltip("STR_MELEE_ACCURACY");
	_txtMelee->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtMelee->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtStrength->setAlign(ALIGN_CENTER);
	_txtStrength->setText(tr("STR_STRENGTH_ABBREVIATION"));
	_txtStrength->setTooltip("STR_STRENGTH");
	_txtStrength->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtStrength->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtPsiStrength->setAlign(ALIGN_CENTER);
	_txtPsiStrength->setText(tr("STR_PSIONIC_STRENGTH_ABBREVIATION"));
	_txtPsiStrength->setTooltip("STR_PSIONIC_STRENGTH");
	_txtPsiStrength->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtPsiStrength->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_txtPsiSkill->setAlign(ALIGN_CENTER);
	_txtPsiSkill->setText(tr("STR_PSIONIC_SKILL_ABBREVIATION"));
	_txtPsiSkill->setTooltip("STR_PSIONIC_SKILL");
	_txtPsiSkill->onMouseIn((ActionHandler)&DebriefingState::txtTooltipIn);
	_txtPsiSkill->onMouseOut((ActionHandler)&DebriefingState::txtTooltipOut);

	_lstSoldierStats->setColumns(13, 90, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 0);
	_lstSoldierStats->setAlign(ALIGN_CENTER);
	_lstSoldierStats->setAlign(ALIGN_LEFT, 0);
	_lstSoldierStats->setDot(true);

	prepareDebriefing();

	for (std::vector<SoldierStatsEntry>::iterator i = _soldierStats.begin(); i != _soldierStats.end(); ++i)
	{
		_lstSoldierStats->addRow(13, (*i).first.c_str(),
				makeSoldierString((*i).second.tu).c_str(),
				makeSoldierString((*i).second.stamina).c_str(),
				makeSoldierString((*i).second.health).c_str(),
				makeSoldierString((*i).second.bravery).c_str(),
				makeSoldierString((*i).second.reactions).c_str(),
				makeSoldierString((*i).second.firing).c_str(),
				makeSoldierString((*i).second.throwing).c_str(),
				makeSoldierString((*i).second.melee).c_str(),
				makeSoldierString((*i).second.strength).c_str(),
				makeSoldierString((*i).second.psiStrength).c_str(),
				makeSoldierString((*i).second.psiSkill).c_str(),
				L"");
		// note: final dummy element to cause dot filling until the end of the line
	}

	int total = 0, statsY = 0, recoveryY = 0;
	int civiliansSaved = 0, civiliansDead = 0;
	int aliensKilled = 0, aliensStunned = 0;
	for (std::vector<DebriefingStat*>::iterator i = _stats.begin(); i != _stats.end(); ++i)
	{
		if ((*i)->qty == 0)
			continue;

		std::wostringstream ss, ss2;
		ss << L'\x01' << (*i)->qty << L'\x01';
		ss2 << L'\x01' << (*i)->score;
		total += (*i)->score;
		if ((*i)->recovery)
		{
			_lstRecovery->addRow(3, tr((*i)->item).c_str(), ss.str().c_str(), ss2.str().c_str());
			recoveryY += 8;
		}
		else
		{
			_lstStats->addRow(3, tr((*i)->item).c_str(), ss.str().c_str(), ss2.str().c_str());
			statsY += 8;
		}
		if ((*i)->item == "STR_CIVILIANS_SAVED")
		{
			civiliansSaved = (*i)->qty;
		}
		if ((*i)->item == "STR_CIVILIANS_KILLED_BY_XCOM_OPERATIVES" || (*i)->item == "STR_CIVILIANS_KILLED_BY_ALIENS")
		{
			civiliansDead += (*i)->qty;
		}
		if ((*i)->item == "STR_ALIENS_KILLED")
		{
			aliensKilled += (*i)->qty;
		}
		if ((*i)->item == "STR_LIVE_ALIENS_RECOVERED")
		{
			aliensStunned += (*i)->qty;
		}
	}
	if (civiliansSaved && !civiliansDead && _missionStatistics->success == true)
	{
		_missionStatistics->valiantCrux = true;
	}

	std::wostringstream ss3;
	ss3 << total;
	_lstTotal->addRow(2, tr("STR_TOTAL_UC").c_str(), ss3.str().c_str());

	// add the points to our activity score
	if (_region)
	{
		_region->addActivityXcom(total);
	}
	if (_country)
	{
		_country->addActivityXcom(total);
	}

	if (recoveryY > 0)
	{
		_txtRecovery->setY(_lstStats->getY() + statsY + 5);
		_lstRecovery->setY(_txtRecovery->getY() + 8);
		_lstTotal->setY(_lstRecovery->getY() + recoveryY + 5);
	}
	else
	{
		_txtRecovery->setText(L"");
		_lstTotal->setY(_lstStats->getY() + statsY + 5);
	}

	// Calculate rating
	std::string rating;
	if (total <= -200)
	{
		rating = "STR_RATING_TERRIBLE";
	}
	else if (total <= 0)
	{
		rating = "STR_RATING_POOR";
	}
	else if (total <= 200)
	{
		rating = "STR_RATING_OK";
	}
	else if (total <= 500)
	{
		rating = "STR_RATING_GOOD";
	}
	else
	{
		rating = "STR_RATING_EXCELLENT";
	}
	_missionStatistics->rating = rating;
	_missionStatistics->score = total;
	_txtRating->setText(tr("STR_RATING").arg(tr(rating)));

	SavedGame *save = _game->getSavedGame();
	SavedBattleGame *battle = save->getSavedBattle();
	
	_missionStatistics->daylight = save->getSavedBattle()->getGlobalShade();
	_missionStatistics->id = _game->getSavedGame()->getMissionStatistics()->size();
	_game->getSavedGame()->getMissionStatistics()->push_back(_missionStatistics);

	// Award Best-of commendations.
	int bestScoreID[7] = {0, 0, 0, 0, 0, 0, 0};
	int bestScore[7] = {0, 0, 0, 0, 0, 0, 0};
	int bestOverallScorersID = 0;
	int bestOverallScore = 0;

	// Check to see if any of the dead soldiers were exceptional.
	for (std::vector<BattleUnit*>::iterator deadUnit = battle->getUnits()->begin(); deadUnit != battle->getUnits()->end(); ++deadUnit)
	{
		if (!(*deadUnit)->getGeoscapeSoldier() || (*deadUnit)->getStatus() != STATUS_DEAD) 
		{
			continue;
		}

		/// Post-mortem kill award
		int killTurn = -1;
		for (std::vector<BattleUnit*>::iterator killerUnit = battle->getUnits()->begin(); killerUnit != battle->getUnits()->end(); ++killerUnit)
		{
			for(std::vector<BattleUnitKills*>::iterator kill = (*killerUnit)->getStatistics()->kills.begin(); kill != (*killerUnit)->getStatistics()->kills.end(); ++kill)
			{
				if ((*kill)->id == (*deadUnit)->getId())
				{
					killTurn = (*kill)->turn;
					break;
				}
			}
			if (killTurn != -1)
			{
				break;
			}
		}
		int postMortemKills = 0;
		if (killTurn != -1)
		{
			for(std::vector<BattleUnitKills*>::iterator deadUnitKill = (*deadUnit)->getStatistics()->kills.begin(); deadUnitKill != (*deadUnit)->getStatistics()->kills.end(); ++deadUnitKill)
			{
				if ((*deadUnitKill)->turn > killTurn && (*deadUnitKill)->faction == FACTION_HOSTILE)
				{
					postMortemKills++;
				}
			}
		}
		(*deadUnit)->getGeoscapeSoldier()->getDiary()->awardPostMortemKill(postMortemKills);

		SoldierRank rank = (*deadUnit)->getGeoscapeSoldier()->getRank();
		// Rookies don't get this next award. No one likes them.
		if (rank == RANK_ROOKIE) 
		{
			continue;
		}

		/// Best-of awards
		// Find the best soldier per rank by comparing score.
		for (std::vector<Soldier*>::iterator j = _game->getSavedGame()->getDeadSoldiers()->begin(); j != _game->getSavedGame()->getDeadSoldiers()->end(); ++j)
		{
			int score = (*j)->getDiary()->getScoreTotal(_game->getSavedGame()->getMissionStatistics());

			// Don't forget this mission's score!
			if ((*j)->getId() == (*deadUnit)->getId())
			{
				score += _missionStatistics->score;
			}

			if (score > bestScore[rank])
			{
				bestScoreID[rank] = (*deadUnit)->getId();
				bestScore[rank] = score;
				if (score > bestOverallScore)
				{
					bestOverallScorersID = (*deadUnit)->getId();
					bestOverallScore = score;
				}
			}
		}
	}
	// Now award those soldiers commendations!
	for (std::vector<BattleUnit*>::iterator deadUnit = battle->getUnits()->begin(); deadUnit != battle->getUnits()->end(); ++deadUnit)
	{
		if (!(*deadUnit)->getGeoscapeSoldier() || (*deadUnit)->getStatus() != STATUS_DEAD) 
		{
			continue;
		}
		if ((*deadUnit)->getId() == bestScoreID[(*deadUnit)->getGeoscapeSoldier()->getRank()])
		{
			(*deadUnit)->getGeoscapeSoldier()->getDiary()->awardBestOfRank(bestScore[(*deadUnit)->getGeoscapeSoldier()->getRank()]);
		}
		if ((*deadUnit)->getId() == bestOverallScorersID)
		{
			(*deadUnit)->getGeoscapeSoldier()->getDiary()->awardBestOverall(bestOverallScore);
		}
	}

	for (std::vector<BattleUnit*>::iterator j = battle->getUnits()->begin(); j != battle->getUnits()->end(); ++j)
	{
		if ((*j)->getGeoscapeSoldier())
		{
			int soldierAlienKills = 0;
			int soldierAlienStuns = 0;
			for (std::vector<BattleUnitKills*>::const_iterator k = (*j)->getStatistics()->kills.begin(); k != (*j)->getStatistics()->kills.end(); ++k)
			{
				if ((*k)->faction == FACTION_HOSTILE && (*k)->status == STATUS_DEAD)
				{
					soldierAlienKills++;
				}
				if ((*k)->faction == FACTION_HOSTILE && (*k)->status == STATUS_UNCONSCIOUS)
				{
					soldierAlienStuns++;
				}
			}

			if (aliensKilled != 0 && aliensKilled == soldierAlienKills && _missionStatistics->success == true && aliensStunned == soldierAlienStuns)
			{
				(*j)->getStatistics()->nikeCross = true;
			}
			if (aliensStunned != 0 && aliensStunned == soldierAlienStuns && _missionStatistics->success == true && aliensKilled == 0)
			{
				(*j)->getStatistics()->mercyCross = true;
			}
			(*j)->getStatistics()->daysWounded = (*j)->getGeoscapeSoldier()->getWoundRecovery();
			_missionStatistics->injuryList[(*j)->getGeoscapeSoldier()->getId()] = (*j)->getGeoscapeSoldier()->getWoundRecovery();

			// Award Martyr Medal
			if ((*j)->getMurdererId() == (*j)->getId() && (*j)->getStatistics()->kills.size() != 0)
			{
				int martyrKills = 0; // How many aliens were killed on the same turn?
				int martyrTurn = -1;
				for (std::vector<BattleUnitKills*>::iterator unitKill = (*j)->getStatistics()->kills.begin(); unitKill != (*j)->getStatistics()->kills.end(); ++unitKill)
				{
					if ( (*unitKill)->id == (*j)->getId() )
					{
						martyrTurn = (*unitKill)->turn;
						break;
					}
				}
				for (std::vector<BattleUnitKills*>::iterator unitKill = (*j)->getStatistics()->kills.begin(); unitKill != (*j)->getStatistics()->kills.end(); ++unitKill)
				{
					if ((*unitKill)->turn == martyrTurn && (*unitKill)->faction == FACTION_HOSTILE)
					{
						martyrKills++;
					}
				}
				if (martyrKills > 0) 
				{
					if (martyrKills > 10) 
					{
						martyrKills = 10;
					}
					(*j)->getStatistics()->martyr = martyrKills;
				}
			}

			// Set the UnitStats delta
			(*j)->getStatistics()->delta = *(*j)->getGeoscapeSoldier()->getCurrentStats() - *(*j)->getGeoscapeSoldier()->getInitStats();

			(*j)->getGeoscapeSoldier()->getDiary()->updateDiary((*j)->getStatistics(), _game->getSavedGame()->getMissionStatistics(), _game->getMod());
			if (!(*j)->getStatistics()->MIA && !(*j)->getStatistics()->KIA && (*j)->getGeoscapeSoldier()->getDiary()->manageCommendations(_game->getMod(), _game->getSavedGame()->getMissionStatistics()))
			{
				_soldiersCommended.push_back((*j)->getGeoscapeSoldier());
			}
			else if ((*j)->getStatistics()->MIA || (*j)->getStatistics()->KIA)
			{
				(*j)->getGeoscapeSoldier()->getDiary()->manageCommendations(_game->getMod(), _game->getSavedGame()->getMissionStatistics());
				_deadSoldiersCommended.push_back((*j)->getGeoscapeSoldier());
			}
		}
	}

	_positiveScore = (total > 0);
}

/**
 *
 */
DebriefingState::~DebriefingState()
{
	if (_game->isQuitting())
	{
		_game->getSavedGame()->setBattleGame(0);
	}
	for (std::vector<DebriefingStat*>::iterator i = _stats.begin(); i != _stats.end(); ++i)
	{
		delete *i;
	}
	for (std::map<int, RecoveryItem*>::iterator i = _recoveryStats.begin(); i != _recoveryStats.end(); ++i)
	{
		delete i->second;
	}
	_recoveryStats.clear();
	_rounds.clear();
}

std::wstring DebriefingState::makeSoldierString(int stat)
{
	if (stat == 0) return L"";

	std::wostringstream ss;
	ss << L'\x01' << L'+' << stat << L'\x01';
	return ss.str();
}

void DebriefingState::applyVisibility()
{
	// First page (scores)
	_txtItem->setVisible(!_showSoldierStats);
	_txtQuantity->setVisible(!_showSoldierStats);
	_txtScore->setVisible(!_showSoldierStats);
	_txtRecovery->setVisible(!_showSoldierStats);
	_txtRating->setVisible(!_showSoldierStats);
	_lstStats->setVisible(!_showSoldierStats);
	_lstRecovery->setVisible(!_showSoldierStats);
	_lstTotal->setVisible(!_showSoldierStats);

	// Second page (soldier stats)
	_txtSoldier->setVisible(_showSoldierStats);
	_txtTU->setVisible(_showSoldierStats);
	_txtStamina->setVisible(_showSoldierStats);
	_txtHealth->setVisible(_showSoldierStats);
	_txtBravery->setVisible(_showSoldierStats);
	_txtReactions->setVisible(_showSoldierStats);
	_txtFiring->setVisible(_showSoldierStats);
	_txtThrowing->setVisible(_showSoldierStats);
	_txtMelee->setVisible(_showSoldierStats);
	_txtStrength->setVisible(_showSoldierStats);
	_txtPsiStrength->setVisible(_showSoldierStats);
	_txtPsiSkill->setVisible(_showSoldierStats);
	_lstSoldierStats->setVisible(_showSoldierStats);
	_txtTooltip->setVisible(_showSoldierStats);

	// Set text on toggle button accordingly
	_btnStats->setText(_showSoldierStats ? tr("STR_SCORE") : tr("STR_STATS"));

}

void DebriefingState::init()
{
	State::init();
	if (_positiveScore)
	{
		_game->getMod()->playMusic(Mod::DEBRIEF_MUSIC_GOOD);
	}
	else
	{
		_game->getMod()->playMusic(Mod::DEBRIEF_MUSIC_BAD);
	}
	if (_noContainment)
	{
		_game->pushState(new ErrorMessageState(tr("STR_ALIEN_DIES_NO_ALIEN_CONTAINMENT_FACILITY"), _palette, _game->getMod()->getInterface("debriefing")->getElement("errorMessage")->color, "BACK01.SCR", _game->getMod()->getInterface("debriefing")->getElement("errorPalette")->color));
		_noContainment = false;
	}
}

/**
* Shows a tooltip for the appropriate text.
* @param action Pointer to an action.
*/
void DebriefingState::txtTooltipIn(Action *action)
{
	_currentTooltip = action->getSender()->getTooltip();
	_txtTooltip->setText(tr(_currentTooltip));}

/**
* Clears the tooltip text.
* @param action Pointer to an action.
*/
void DebriefingState::txtTooltipOut(Action *action)
{
	if (_currentTooltip == action->getSender()->getTooltip())
	{
		_txtTooltip->setText(L"");
	}
}

/**
 * Displays soldiers' stat increases.
 * @param action Pointer to an action.
 */
void DebriefingState::btnStatsClick(Action *)
{
	_showSoldierStats = !_showSoldierStats;
	applyVisibility();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void DebriefingState::btnOkClick(Action *)
{
	std::vector<Soldier*> participants;
	for (std::vector<BattleUnit*>::const_iterator i = _game->getSavedGame()->getSavedBattle()->getUnits()->begin();
		i != _game->getSavedGame()->getSavedBattle()->getUnits()->end(); ++i)
	{
		if ((*i)->getGeoscapeSoldier())
		{
			participants.push_back((*i)->getGeoscapeSoldier());
		}
	}
	_game->getSavedGame()->setBattleGame(0);
	_game->popState();
	if (_game->getSavedGame()->getMonthsPassed() == -1)
	{
		_game->setState(new MainMenuState);
	}
	else
	{
		if (!_deadSoldiersCommended.empty())
		{
			_game->pushState(new CommendationLateState(_deadSoldiersCommended));
		}
		if (!_soldiersCommended.empty())
		{
			_game->pushState(new CommendationState(_soldiersCommended));
		}
		if (!_destroyBase)
		{
			if (_game->getSavedGame()->handlePromotions(participants))
			{
				_game->pushState(new PromotionsState);
			}
			if (!_missingItems.empty())
			{
				_game->pushState(new CannotReequipState(_missingItems));
			}
			else if (_manageContainment)
			{
				_game->pushState(new ManageAlienContainmentState(_base, OPT_BATTLESCAPE));
				_game->pushState(new ErrorMessageState(tr("STR_CONTAINMENT_EXCEEDED").arg(_base->getName()), _palette, _game->getMod()->getInterface("debriefing")->getElement("errorMessage")->color, "BACK01.SCR", _game->getMod()->getInterface("debriefing")->getElement("errorPalette")->color));
			}
			if (!_manageContainment && Options::storageLimitsEnforced && _base->storesOverfull())
			{
				_game->pushState(new SellState(_base, OPT_BATTLESCAPE));
				_game->pushState(new ErrorMessageState(tr("STR_STORAGE_EXCEEDED").arg(_base->getName()), _palette, _game->getMod()->getInterface("debriefing")->getElement("errorMessage")->color, "BACK01.SCR", _game->getMod()->getInterface("debriefing")->getElement("errorPalette")->color));
			}
		}

		// Autosave after mission
		if (_game->getSavedGame()->isIronman())
		{
			_game->pushState(new SaveGameState(OPT_GEOSCAPE, SAVE_IRONMAN, _palette));
		}
		else if (Options::autosave)
		{
			_game->pushState(new SaveGameState(OPT_GEOSCAPE, SAVE_AUTO_GEOSCAPE, _palette));
		}
	}
}

/**
 * Adds to the debriefing stats.
 * @param name The untranslated name of the stat.
 * @param quantity The quantity to add.
 * @param score The score to add.
 */
void DebriefingState::addStat(const std::string &name, int quantity, int score)
{
	for (std::vector<DebriefingStat*>::iterator i = _stats.begin(); i != _stats.end(); ++i)
	{
		if ((*i)->item == name)
		{
			(*i)->qty = (*i)->qty + quantity;
			(*i)->score = (*i)->score + score;
			break;
		}
	}
}

/**
 * Clears the alien base from supply missions that use it.
 */
class ClearAlienBase: public std::unary_function<AlienMission *, void>
{
public:
	/// Remembers the base.
	ClearAlienBase(const AlienBase *base) : _base(base) { /* Empty by design. */ }
	/// Clears the base if required.
	void operator()(AlienMission *am) const;
private:
	const AlienBase *_base;
};

/**
 * Removes the association between the alien mission and the alien base,
 * if one existed.
 * @param am Pointer to the alien mission.
 */
void ClearAlienBase::operator()(AlienMission *am) const
{
	if (am->getAlienBase() == _base)
	{
		am->setAlienBase(0);
	}
}

/**
 * Prepares debriefing: gathers Aliens, Corpses, Artefacts, UFO Components.
 * Adds the items to the craft.
 * Also calculates the soldiers experience, and possible promotions.
 * If aborted, only the things on the exit area are recovered.
 */
void DebriefingState::prepareDebriefing()
{
	for (std::vector<std::string>::const_iterator i = _game->getMod()->getItemsList().begin(); i != _game->getMod()->getItemsList().end(); ++i)
	{
		RuleItem *rule = _game->getMod()->getItem(*i);
		if (rule->getSpecialType() > 1)
		{
			RecoveryItem *item = new RecoveryItem();
			item->name = *i;
			item->value = rule->getRecoveryPoints();
			_recoveryStats[rule->getSpecialType()] = item;
			_missionStatistics->lootValue = item->value;
		}
	}

	SavedGame *save = _game->getSavedGame();
	SavedBattleGame *battle = save->getSavedBattle();
	AlienDeployment *ruleDeploy = _game->getMod()->getDeployment(battle->getMissionType());

	bool aborted = battle->isAborted();
	bool success = !aborted || battle->allObjectivesDestroyed();
	Craft *craft = 0;
	std::vector<Craft*>::iterator craftIterator;
	Base *base = 0;
	std::string target;

	int playersInExitArea = 0; // if this stays 0 the craft is lost...
	int playersSurvived = 0; // if this stays 0 the craft is lost...
	int playersUnconscious = 0;
	int playersInEntryArea = 0;

	_stats.push_back(new DebriefingStat("STR_ALIENS_KILLED", false));
	_stats.push_back(new DebriefingStat("STR_ALIEN_CORPSES_RECOVERED", false));
	_stats.push_back(new DebriefingStat("STR_LIVE_ALIENS_RECOVERED", false));
	_stats.push_back(new DebriefingStat("STR_ALIEN_ARTIFACTS_RECOVERED", false));

	std::string objectiveCompleteText, objectiveFailedText;
	int objectiveCompleteScore = 0, objectiveFailedScore = 0;
	if (ruleDeploy)
	{
		if (ruleDeploy->getObjectiveCompleteInfo(objectiveCompleteText, objectiveCompleteScore))
		{
			_stats.push_back(new DebriefingStat(objectiveCompleteText, false));
		}
		if (ruleDeploy->getObjectiveFailedInfo(objectiveFailedText, objectiveFailedScore))
		{
			_stats.push_back(new DebriefingStat(objectiveFailedText, false));
		}
	}

	_stats.push_back(new DebriefingStat("STR_CIVILIANS_KILLED_BY_ALIENS", false));
	_stats.push_back(new DebriefingStat("STR_CIVILIANS_KILLED_BY_XCOM_OPERATIVES", false));
	_stats.push_back(new DebriefingStat("STR_CIVILIANS_SAVED", false));
	_stats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_KILLED", false));
	//_stats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_RETIRED_THROUGH_INJURY", false));
	_stats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_MISSING_IN_ACTION", false));
	_stats.push_back(new DebriefingStat("STR_TANKS_DESTROYED", false));
	_stats.push_back(new DebriefingStat("STR_XCOM_CRAFT_LOST", false));

	for (std::map<int, RecoveryItem*>::const_iterator i = _recoveryStats.begin(); i != _recoveryStats.end(); ++i)
	{
		_stats.push_back(new DebriefingStat((*i).second->name, true));
	}

	_missionStatistics->time = *save->getTime();
	_missionStatistics->type = battle->getMissionType();
	_stats.push_back(new DebriefingStat(_game->getMod()->getAlienFuelName(), true));

	for (std::vector<Base*>::iterator i = save->getBases()->begin(); i != save->getBases()->end(); ++i)
	{
		// in case we have a craft - check which craft it is about
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			if ((*j)->isInBattlescape())
			{
				for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
				{
					if ((*k)->getRules()->insideRegion((*j)->getLongitude(), (*j)->getLatitude()))
					{
						_region = (*k);
						_missionStatistics->region = _region->getRules()->getType();
						break;
					}
				}
				for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
				{
					if ((*k)->getRules()->insideCountry((*j)->getLongitude(), (*j)->getLatitude()))
					{
						_country = (*k);
						_missionStatistics->country = _country->getRules()->getType();
						break;
					}
				}
				craft = (*j);
				base = (*i);
				craftIterator = j;
				if (Ufo *u = dynamic_cast<Ufo*>(craft->getDestination()))
				{
					target = "STR_UFO";
					_missionStatistics->markerName = "STR_UFO_";
					_missionStatistics->markerId = u->getId();
				}
				else if (AlienBase *b = dynamic_cast<AlienBase*>(craft->getDestination()))
				{
					target = "STR_ALIEN_BASE";
					_missionStatistics->markerName = ruleDeploy->getMarkerName();
					_missionStatistics->markerId = b->getId();
				}
				else if (MissionSite *ms = dynamic_cast<MissionSite*>(craft->getDestination()))
				{
					target = "STR_MISSION_SITE";
					_missionStatistics->markerName = ruleDeploy->getMarkerName();
					_missionStatistics->markerId = ms->getId();
				}
				craft->returnToBase();
				craft->setMissionComplete(true);
				craft->setInBattlescape(false);
			}
			else if ((*j)->getDestination() != 0)
			{
				Ufo* u = dynamic_cast<Ufo*>((*j)->getDestination());
				if (u != 0 && u->isInBattlescape())
				{
					(*j)->returnToBase();
				}
				MissionSite* ms = dynamic_cast<MissionSite*>((*j)->getDestination());
				if (ms != 0 && ms->isInBattlescape())
				{
					(*j)->returnToBase();
				}
			}
		}
		// in case we DON'T have a craft (base defense)
		if ((*i)->isInBattlescape())
		{
			base = (*i);
			target = "STR_BASE";
			base->setInBattlescape(false);
			base->cleanupDefenses(false);
			for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
			{
				if ((*k)->getRules()->insideRegion(base->getLongitude(), base->getLatitude()))
				{
					_region = (*k);
					_missionStatistics->region = _region->getRules()->getType();
					break;
				}
			}
			for (std::vector<Country*>::iterator k = _game->getSavedGame()->getCountries()->begin(); k != _game->getSavedGame()->getCountries()->end(); ++k)
			{
				if ((*k)->getRules()->insideCountry(base->getLongitude(), base->getLatitude()))
				{
					_country = (*k);
					_missionStatistics->country= _country->getRules()->getType();
					break;
				}
			}
			// Loop through the UFOs and see which one is sitting on top of the base... that is probably the one attacking you.
			for (std::vector<Ufo*>::iterator k = save->getUfos()->begin(); k != save->getUfos()->end(); ++k)
			{
				if (AreSame((*k)->getLongitude(), base->getLongitude()) && AreSame((*k)->getLatitude(), base->getLatitude()))
				{
					_missionStatistics->ufo = (*k)->getRules()->getType();
					_missionStatistics->alienRace = (*k)->getAlienRace();
					break;
				}
			}
			if (aborted)
			{
				_destroyBase = true;
			}
			for (std::vector<BaseFacility*>::iterator k = base->getFacilities()->begin(); k != base->getFacilities()->end();)
			{
				// this facility was demolished
				if (battle->getModuleMap()[(*k)->getX()][(*k)->getY()].second == 0)
				{
					base->destroyFacility(k);
				}
				else
				{
					++k;
				}
			}
			// this may cause the base to become disjointed, destroy the disconnected parts.
			base->destroyDisconnectedFacilities();
		}
	}

	// UFO crash/landing site disappears
	for (std::vector<Ufo*>::iterator i = save->getUfos()->begin(); i != save->getUfos()->end(); ++i)
	{
		if ((*i)->isInBattlescape())
		{
			_missionStatistics->ufo = (*i)->getRules()->getType();
			if (save->getMonthsPassed() != -1)
			{
				_missionStatistics->alienRace = (*i)->getAlienRace();
			}
			_txtRecovery->setText(tr("STR_UFO_RECOVERY"));
			(*i)->setInBattlescape(false);
			if ((*i)->getStatus() == Ufo::LANDED && aborted)
			{
				 (*i)->setSecondsRemaining(5);
			}
			else if ((*i)->getStatus() == Ufo::CRASHED || !aborted)
			{
				delete *i;
				save->getUfos()->erase(i);
				break;
			}
		}
	}

	// mission site disappears (even when you abort)
	for (std::vector<MissionSite*>::iterator i = save->getMissionSites()->begin(); i != save->getMissionSites()->end(); ++i)
	{
		if ((*i)->isInBattlescape())
		{
			_missionStatistics->alienRace = (*i)->getAlienRace();
			delete *i;
			save->getMissionSites()->erase(i);
			break;
		}
	}

	// lets see what happens with units

	// first, we evaluate how many surviving XCom units there are, and how many are conscious
	// and how many have died (to use for commendations)
	int deadSoldiers = 0;
	for (std::vector<BattleUnit*>::iterator j = battle->getUnits()->begin(); j != battle->getUnits()->end(); ++j)
	{
		if ((*j)->getOriginalFaction() == FACTION_PLAYER && (*j)->getStatus() != STATUS_DEAD)
		{
			if ((*j)->getStatus() == STATUS_UNCONSCIOUS || (*j)->getFaction() == FACTION_HOSTILE)
			{
				playersUnconscious++;
			}
			else if ((*j)->isInExitArea(END_POINT))
			{
				playersInExitArea++;
			}
			else if ((*j)->isInExitArea(START_POINT))
			{
				playersInEntryArea++;
			}
			playersSurvived++;
		}
		else if ((*j)->getOriginalFaction() == FACTION_PLAYER && (*j)->getStatus() == STATUS_DEAD)
			deadSoldiers++;
	}
	// if all our men are unconscious, the aliens get to have their way with them.
	if (playersUnconscious == playersSurvived)
	{
		playersSurvived = 0;
		for (std::vector<BattleUnit*>::iterator j = battle->getUnits()->begin(); j != battle->getUnits()->end(); ++j)
		{
			if ((*j)->getOriginalFaction() == FACTION_PLAYER && (*j)->getStatus() != STATUS_DEAD)
			{
				(*j)->instaKill();
			}
		}
	}
	
	if (ruleDeploy && ruleDeploy->getEscapeType() != ESCAPE_NONE)
	{
		if (ruleDeploy->getEscapeType() != ESCAPE_EXIT)
		{
			success = playersInEntryArea > 0;
		}
		
		if (ruleDeploy->getEscapeType() != ESCAPE_ENTRY)
		{
			success = success || playersInExitArea > 0;
		}
	}

	playersInExitArea = 0;

	if (playersSurvived == 1)
	{
		for (std::vector<BattleUnit*>::iterator j = battle->getUnits()->begin(); j != battle->getUnits()->end(); ++j)
		{
			// if only one soldier survived, give him a medal! (unless he killed all the others...)
			if ((*j)->getStatus() != STATUS_DEAD && (*j)->getOriginalFaction() == FACTION_PLAYER && !(*j)->getStatistics()->hasFriendlyFired() && deadSoldiers != 0)
			{
				(*j)->getStatistics()->loneSurvivor = true;
				break;
			}
			// if only one soldier survived AND none have died, means only one soldier went on the mission...
			if ((*j)->getStatus() != STATUS_DEAD && (*j)->getOriginalFaction() == FACTION_PLAYER && deadSoldiers == 0)
			{
				(*j)->getStatistics()->ironMan = true;
			}
		}
	}
	// alien base disappears (if you didn't abort)
	for (std::vector<AlienBase*>::iterator i = save->getAlienBases()->begin(); i != save->getAlienBases()->end(); ++i)
	{
		if ((*i)->isInBattlescape())
		{
			_txtRecovery->setText(tr("STR_ALIEN_BASE_RECOVERY"));
			bool destroyAlienBase = true;

			if (aborted || playersSurvived == 0)
			{
				if (!battle->allObjectivesDestroyed())
					destroyAlienBase = false;
			}
			
			if (ruleDeploy && !ruleDeploy->getNextStage().empty())
			{
				_missionStatistics->alienRace = (*i)->getAlienRace();
				destroyAlienBase = false;
			}

			success = destroyAlienBase;
			if (destroyAlienBase)
			{
				if (objectiveCompleteText != "")
				{
					addStat(objectiveCompleteText, 1, objectiveCompleteScore);
				}
				// Take care to remove supply missions for this base.
				std::for_each(save->getAlienMissions().begin(), save->getAlienMissions().end(),
							ClearAlienBase(*i));

				delete *i;
				save->getAlienBases()->erase(i);
				break;
			}
			else
			{
				(*i)->setInBattlescape(false);
				break;
			}
		}
	}

	// time to care for units.
	for (std::vector<BattleUnit*>::iterator j = battle->getUnits()->begin(); j != battle->getUnits()->end(); ++j)
	{
		UnitStatus status = (*j)->getStatus();
		UnitFaction faction = (*j)->getFaction();
		UnitFaction oldFaction = (*j)->getOriginalFaction();
		int value = (*j)->getValue();
		Soldier *soldier = save->getSoldier((*j)->getId());

		if (!(*j)->getTile())
		{
			Position pos = (*j)->getPosition();
			if (pos == Position(-1, -1, -1))
			{
				for (std::vector<BattleItem*>::iterator k = battle->getItems()->begin(); k != battle->getItems()->end(); ++k)
				{
					if ((*k)->getUnit() && (*k)->getUnit() == *j)
					{
						if ((*k)->getOwner())
						{
							pos = (*k)->getOwner()->getPosition();
						}
						else if ((*k)->getTile())
						{
							pos = (*k)->getTile()->getPosition();
						}
					}
				}
			}
			(*j)->setTile(battle->getTile(pos));
		}

		if (status == STATUS_DEAD)
		{ // so this is a dead unit
			if (oldFaction == FACTION_HOSTILE && (*j)->killedBy() == FACTION_PLAYER)
			{
				addStat("STR_ALIENS_KILLED", 1, value);
			}
			else if (oldFaction == FACTION_PLAYER)
			{
				if (soldier != 0)
				{
					addStat("STR_XCOM_OPERATIVES_KILLED", 1, -value);
					(*j)->updateGeoscapeStats(soldier);
					(*j)->getStatistics()->KIA = true;
					save->killSoldier(soldier); // in case we missed the soldier death on battlescape
				}
				else
				{ // non soldier player = tank
					addStat("STR_TANKS_DESTROYED", 1, -value);
				}
			}
			else if (oldFaction == FACTION_NEUTRAL)
			{
				if ((*j)->killedBy() == FACTION_PLAYER)
					addStat("STR_CIVILIANS_KILLED_BY_XCOM_OPERATIVES", 1, -(*j)->getValue() - (2 * ((*j)->getValue() / 3)));
				else // if civilians happen to kill themselves XCOM shouldn't get penalty for it
					addStat("STR_CIVILIANS_KILLED_BY_ALIENS", 1, -(*j)->getValue());
			}
		}
		else
		{ // so this unit is not dead...
			if (oldFaction == FACTION_PLAYER)
			{
				if ((((*j)->isInExitArea(START_POINT) || (*j)->getStatus() == STATUS_IGNORE_ME) && (battle->getMissionType() != "STR_BASE_DEFENSE" || success)) || !aborted || (aborted && (*j)->isInExitArea(END_POINT)))
				{ // so game is not aborted or aborted and unit is on exit area
					UnitStats statIncrease;
					(*j)->postMissionProcedures(save, statIncrease);
					if ((*j)->getGeoscapeSoldier())
						_soldierStats.push_back(std::pair<std::wstring, UnitStats>((*j)->getGeoscapeSoldier()->getName(), statIncrease));
					playersInExitArea++;

					recoverItems((*j)->getInventory(), base);

					if (soldier != 0)
					{
						// calculate new statString
						soldier->calcStatString(_game->getMod()->getStatStrings(), (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getMod()->getPsiRequirements())));
					}
					else
					{ // non soldier player = tank
						base->getStorageItems()->addItem((*j)->getType());
						RuleItem *tankRule = _game->getMod()->getItem((*j)->getType(), true);
						if ((*j)->getItem("STR_RIGHT_HAND"))
						{
							BattleItem *ammoItem = (*j)->getItem("STR_RIGHT_HAND")->getAmmoItem();
							if (!tankRule->getCompatibleAmmo()->empty() && ammoItem != 0 && ammoItem->getAmmoQuantity() > 0)
							{
								int total = ammoItem->getAmmoQuantity();

								if (tankRule->getClipSize()) // meaning this tank can store multiple clips
								{
									total /= ammoItem->getRules()->getClipSize();
								}

								base->getStorageItems()->addItem(tankRule->getCompatibleAmmo()->front(), total);
							}
						}
						if ((*j)->getItem("STR_LEFT_HAND"))
						{
							RuleItem *secondaryRule = (*j)->getItem("STR_LEFT_HAND")->getRules();
							BattleItem *ammoItem = (*j)->getItem("STR_LEFT_HAND")->getAmmoItem();
							if (!secondaryRule->getCompatibleAmmo()->empty() && ammoItem != 0 && ammoItem->getAmmoQuantity() > 0)
							{
								int total = ammoItem->getAmmoQuantity();

								if (secondaryRule->getClipSize()) // meaning this tank can store multiple clips
								{
									total /= ammoItem->getRules()->getClipSize();
								}

								base->getStorageItems()->addItem(secondaryRule->getCompatibleAmmo()->front(), total);
							}
						}
					}
				}
				else
				{ // so game is aborted and unit is not on exit area
					addStat("STR_XCOM_OPERATIVES_MISSING_IN_ACTION", 1, -value);
					playersSurvived--;
					if (soldier != 0)
					{
						(*j)->updateGeoscapeStats(soldier);
						(*j)->getStatistics()->MIA = true;
						save->killSoldier(soldier);
					}
				}
			}
			else if (oldFaction == FACTION_HOSTILE && (!aborted || (*j)->isInExitArea(START_POINT)) && !_destroyBase
				// mind controlled units may as well count as unconscious
				&& faction == FACTION_PLAYER && (!(*j)->isOut() || (*j)->getStatus() == STATUS_IGNORE_ME))
			{
				if ((*j)->getTile())
				{
					for (std::vector<BattleItem*>::iterator k = (*j)->getInventory()->begin(); k != (*j)->getInventory()->end(); ++k)
					{
						if (!(*k)->getRules()->isFixed())
						{
							(*j)->getTile()->addItem(*k, _game->getMod()->getInventory("STR_GROUND", true));
						}
					}
				}
				recoverAlien(*j, base);
			}
			else if (oldFaction == FACTION_NEUTRAL)
			{
				// if mission fails, all civilians die
				if (aborted || playersSurvived == 0)
				{
					addStat("STR_CIVILIANS_KILLED_BY_ALIENS", 1, -(*j)->getValue());
				}
				else
				{
					addStat("STR_CIVILIANS_SAVED", 1, (*j)->getValue());
				}
			}
		}
	}
	if (craft != 0 && ((playersInExitArea == 0 && aborted) || (playersSurvived == 0)))
	{
		addStat("STR_XCOM_CRAFT_LOST", 1, -craft->getRules()->getScore());
		// Since this is not a base defense mission, we can safely erase the craft,
		// without worrying it's vehicles' destructor calling double (on base defense missions
		// all vehicle object in the craft is also referenced by base->getVehicles() !!)
		delete craft;
		craft = 0; // To avoid a crash down there!!
		base->getCrafts()->erase(craftIterator);
		_txtTitle->setText(tr("STR_CRAFT_IS_LOST"));
		playersSurvived = 0; // assuming you aborted and left everyone behind
		success = false;
	}
	if ((aborted || playersSurvived == 0) && target == "STR_BASE")
	{
		for (std::vector<Craft*>::iterator i = base->getCrafts()->begin(); i != base->getCrafts()->end(); ++i)
		{
			addStat("STR_XCOM_CRAFT_LOST", 1, -(*i)->getRules()->getScore());
		}
		playersSurvived = 0; // assuming you aborted and left everyone behind
		success = false;
	}
	if ((!aborted || success) && playersSurvived > 0) 	// RECOVER UFO : run through all tiles to recover UFO components and items
	{
		if (target == "STR_BASE")
		{
			_txtTitle->setText(tr("STR_BASE_IS_SAVED"));
		}
		else if (target == "STR_UFO")
		{
			_txtTitle->setText(tr("STR_UFO_IS_RECOVERED"));
		}
		else if (target == "STR_ALIEN_BASE")
		{
			_txtTitle->setText(tr("STR_ALIEN_BASE_DESTROYED"));
		}
		else
		{
			_txtTitle->setText(tr("STR_ALIENS_DEFEATED"));
			if (objectiveCompleteText != "")
			{
				int victoryStat = 0;
				if (ruleDeploy->getEscapeType() != ESCAPE_NONE)
				{
					if (ruleDeploy->getEscapeType() != ESCAPE_EXIT)
					{
						victoryStat += playersInEntryArea;
					}
					if (ruleDeploy->getEscapeType() != ESCAPE_ENTRY)
					{
						victoryStat += playersInExitArea;
					}
				}
				else
				{
					victoryStat = 1;
				}

				addStat(objectiveCompleteText, victoryStat, objectiveCompleteScore);
			}
		}

		if (!aborted)
		{
			// if this was a 2-stage mission, and we didn't abort (ie: we have time to clean up)
			// we can recover items from the earlier stages as well
			recoverItems(battle->getConditionalRecoveredItems(), base);
			int nonRecoverType = 0;
			if (ruleDeploy && ruleDeploy->getObjectiveType())
			{
				nonRecoverType = ruleDeploy->getObjectiveType();
			}
			for (int i = 0; i < battle->getMapSizeXYZ(); ++i)
			{
				// get recoverable map data objects from the battlescape map
				for (int part = O_FLOOR; part <= O_OBJECT; ++part)
				{
					TilePart tp = (TilePart)part;
					if (battle->getTiles()[i]->getMapData(tp))
					{
						int specialType = battle->getTiles()[i]->getMapData(tp)->getSpecialType();
						if (specialType != nonRecoverType && _recoveryStats.find(specialType) != _recoveryStats.end())
						{
							addStat(_recoveryStats[specialType]->name, 1, _recoveryStats[specialType]->value);
						}
					}
				}
				// recover items from the floor
				recoverItems(battle->getTiles()[i]->getInventory(), base);
			}
		}
		else
		{
			for (int i = 0; i < battle->getMapSizeXYZ(); ++i)
			{
				if (battle->getTiles()[i]->getMapData(O_FLOOR) && (battle->getTiles()[i]->getMapData(O_FLOOR)->getSpecialType() == START_POINT))
					recoverItems(battle->getTiles()[i]->getInventory(), base);
			}
		}
	}
	else
	{
		if (target == "STR_BASE")
		{
			_txtTitle->setText(tr("STR_BASE_IS_LOST"));
			_destroyBase = true;
		}
		else if (target == "STR_UFO")
		{
			_txtTitle->setText(tr("STR_UFO_IS_NOT_RECOVERED"));
		}
		else if (target == "STR_ALIEN_BASE")
		{
			_txtTitle->setText(tr("STR_ALIEN_BASE_STILL_INTACT"));
		}
		else
		{
			_txtTitle->setText(tr("STR_TERROR_CONTINUES"));
			if (objectiveFailedText != "")
			{
				addStat(objectiveFailedText, 1, objectiveFailedScore);
			}
		}

		if (playersSurvived > 0 && !_destroyBase)
		{
			// recover items from the craft floor
			for (int i = 0; i < battle->getMapSizeXYZ(); ++i)
			{
				if (battle->getTiles()[i]->getMapData(O_FLOOR) && (battle->getTiles()[i]->getMapData(O_FLOOR)->getSpecialType() == START_POINT))
					recoverItems(battle->getTiles()[i]->getInventory(), base);
			}
		}
	}

	// recover all our goodies
	if (playersSurvived > 0)
	{
		int aadivider = (target == "STR_UFO") ? 10 : 150;
		for (std::vector<DebriefingStat*>::iterator i = _stats.begin(); i != _stats.end(); ++i)
		{
			// alien alloys recovery values are divided by 10 or divided by 150 in case of an alien base
			if ((*i)->item == _recoveryStats[ALIEN_ALLOYS]->name)
			{
				(*i)->qty = (*i)->qty / aadivider;
				(*i)->score = (*i)->score / aadivider;
			}

			// recoverable battlescape tiles are now converted to items and put in base inventory
			if ((*i)->recovery && (*i)->qty > 0)
			{
				base->getStorageItems()->addItem((*i)->item, (*i)->qty);
			}
		}

		// assuming this was a multi-stage mission,
		// recover everything that was in the craft in the previous stage
		recoverItems(battle->getGuaranteedRecoveredItems(), base);
	}

	// calculate the clips for each type based on the recovered rounds.
	for (std::map<RuleItem*, int>::const_iterator i = _rounds.begin(); i != _rounds.end(); ++i)
	{
		int total_clips = i->second / i->first->getClipSize();
		if (total_clips > 0)
			base->getStorageItems()->addItem(i->first->getType(), total_clips);
	}

	// reequip craft after a non-base-defense mission (of course only if it's not lost already (that case craft=0))
	if (craft)
	{
		reequipCraft(base, craft, true);
	}

	if (target == "STR_BASE")
	{
		if (!_destroyBase)
		{
			// reequip crafts (only those on the base) after a base defense mission
			for (std::vector<Craft*>::iterator c = base->getCrafts()->begin(); c != base->getCrafts()->end(); ++c)
			{
				if ((*c)->getStatus() != "STR_OUT")
					reequipCraft(base, *c, false);
			}
			// Clear base->getVehicles() objects, they aren't needed anymore.
			for (std::vector<Vehicle*>::iterator i = base->getVehicles()->begin(); i != base->getVehicles()->end(); ++i)
				delete (*i);
			base->getVehicles()->clear();
		}
		else if (_game->getSavedGame()->getMonthsPassed() != -1)
		{
			for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
			{
				if ((*i) == base)
				{
					delete (*i);
					base = 0; // To avoid similar (potential) problems as with the deleted craft
					_game->getSavedGame()->getBases()->erase(i);
					break;
				}
			}
		}

		if (_region)
		{
			AlienMission* am = _game->getSavedGame()->findAlienMission(_region->getRules()->getType(), OBJECTIVE_RETALIATION);
			for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end();)
			{
				if ((*i)->getMission() == am)
				{
					delete *i;
					i = _game->getSavedGame()->getUfos()->erase(i);
				}
				else
				{
					++i;
				}
			}
			for (std::vector<AlienMission*>::iterator i = _game->getSavedGame()->getAlienMissions().begin();
				i != _game->getSavedGame()->getAlienMissions().end(); ++i)
			{
				if ((AlienMission*)(*i) == am)
				{
					delete (*i);
					_game->getSavedGame()->getAlienMissions().erase(i);
					break;
				}
			}
		}
	}
	_missionStatistics->success = success;

	// remember the base for later use (of course only if it's not lost already (in that case base=0))
	_base = base;
}

/**
 * Reequips a craft after a mission.
 * @param base Base to reequip from.
 * @param craft Craft to reequip.
 * @param vehicleItemsCanBeDestroyed Whether we can destroy the vehicles on the craft.
 */
void DebriefingState::reequipCraft(Base *base, Craft *craft, bool vehicleItemsCanBeDestroyed)
{
	std::map<std::string, int> craftItems = *craft->getItems()->getContents();
	for (std::map<std::string, int>::iterator i = craftItems.begin(); i != craftItems.end(); ++i)
	{
		int qty = base->getStorageItems()->getItem(i->first);
		if (qty >= i->second)
		{
			base->getStorageItems()->removeItem(i->first, i->second);
		}
		else
		{
			int missing = i->second - qty;
			base->getStorageItems()->removeItem(i->first, qty);
			craft->getItems()->removeItem(i->first, missing);
			ReequipStat stat = {i->first, missing, craft->getName(_game->getLanguage())};
			_missingItems.push_back(stat);
		}
	}

	// Now let's see the vehicles
	ItemContainer craftVehicles;
	for (std::vector<Vehicle*>::iterator i = craft->getVehicles()->begin(); i != craft->getVehicles()->end(); ++i)
		craftVehicles.addItem((*i)->getRules()->getType());
	// Now we know how many vehicles (separated by types) we have to read
	// Erase the current vehicles, because we have to reAdd them (cause we want to redistribute their ammo)
	if (vehicleItemsCanBeDestroyed)
		for (std::vector<Vehicle*>::iterator i = craft->getVehicles()->begin(); i != craft->getVehicles()->end(); ++i)
			delete (*i);
	craft->getVehicles()->clear();
	// Ok, now read those vehicles
	for (std::map<std::string, int>::iterator i = craftVehicles.getContents()->begin(); i != craftVehicles.getContents()->end(); ++i)
	{
		int qty = base->getStorageItems()->getItem(i->first);
		RuleItem *tankRule = _game->getMod()->getItem(i->first, true);
		int size = 4;
		if (_game->getMod()->getUnit(tankRule->getType()))
		{
			size = _game->getMod()->getArmor(_game->getMod()->getUnit(tankRule->getType())->getArmor(), true)->getSize();
			size *= size;
		}
		int canBeAdded = std::min(qty, i->second);
		if (qty < i->second)
		{ // missing tanks
			int missing = i->second - qty;
			ReequipStat stat = {i->first, missing, craft->getName(_game->getLanguage())};
			_missingItems.push_back(stat);
		}
		if (tankRule->getCompatibleAmmo()->empty())
		{ // so this tank does NOT require ammo
			for (int j = 0; j < canBeAdded; ++j)
				craft->getVehicles()->push_back(new Vehicle(tankRule, tankRule->getClipSize(), size));
			base->getStorageItems()->removeItem(i->first, canBeAdded);
		}
		else
		{ // so this tank requires ammo
			RuleItem *ammo = _game->getMod()->getItem(tankRule->getCompatibleAmmo()->front(), true);
			int ammoPerVehicle, clipSize;
			if (ammo->getClipSize() > 0 && tankRule->getClipSize() > 0)
			{
				clipSize = tankRule->getClipSize();
				ammoPerVehicle = clipSize / ammo->getClipSize();
			}
			else
			{
				clipSize = ammo->getClipSize();
				ammoPerVehicle = clipSize;
			}
			int baqty = base->getStorageItems()->getItem(ammo->getType()); // Ammo Quantity for this vehicle-type on the base
			if (baqty < i->second * ammoPerVehicle)
			{ // missing ammo
				int missing = (i->second * ammoPerVehicle) - baqty;
				ReequipStat stat = {ammo->getType(), missing, craft->getName(_game->getLanguage())};
				_missingItems.push_back(stat);
			}
			canBeAdded = std::min(canBeAdded, baqty / ammoPerVehicle);
			if (canBeAdded > 0)
			{
				for (int j = 0; j < canBeAdded; ++j)
				{
					craft->getVehicles()->push_back(new Vehicle(tankRule, clipSize, size));
					base->getStorageItems()->removeItem(ammo->getType(), ammoPerVehicle);
				}
				base->getStorageItems()->removeItem(i->first, canBeAdded);
			}
		}
	}
}

/**
 * Recovers items from the battlescape.
 *
 * Converts the battlescape inventory into a geoscape itemcontainer.
 * @param from Items recovered from the battlescape.
 * @param base Base to add items to.
 */
void DebriefingState::recoverItems(std::vector<BattleItem*> *from, Base *base)
{
	for (std::vector<BattleItem*>::iterator it = from->begin(); it != from->end(); ++it)
	{
		if ((*it)->getRules()->getName() == _game->getMod()->getAlienFuelName())
		{
			// special case of an item counted as a stat
			addStat(_game->getMod()->getAlienFuelName(), _game->getMod()->getAlienFuelQuantity(), (*it)->getRules()->getRecoveryPoints());
		}
		else
		{
			if ((*it)->getRules()->isRecoverable() && !(*it)->getXCOMProperty())
			{
				if ((*it)->getRules()->getBattleType() == BT_CORPSE)
				{
					BattleUnit *corpseUnit = (*it)->getUnit();
					if (corpseUnit->getStatus() == STATUS_DEAD)
					{
						base->getStorageItems()->addItem(corpseUnit->getArmor()->getCorpseGeoscape(), 1);
						addStat("STR_ALIEN_CORPSES_RECOVERED", 1, (*it)->getRules()->getRecoveryPoints());
					}
					else if (corpseUnit->getStatus() == STATUS_UNCONSCIOUS ||
							// or it's in timeout because it's unconscious from the previous stage
							// units can be in timeout and alive, and we assume they flee.
							(corpseUnit->getStatus() == STATUS_IGNORE_ME &&
							corpseUnit->getHealth() > 0 &&
							corpseUnit->getHealth() < corpseUnit->getStunlevel()))
					{
						if (corpseUnit->getOriginalFaction() == FACTION_HOSTILE)
						{
							recoverAlien(corpseUnit, base);
						}
						else if (corpseUnit->getOriginalFaction() == FACTION_NEUTRAL)
						{
							addStat("STR_CIVILIANS_SAVED", 1, corpseUnit->getValue());
						}
					}
				}
				// only add recovery points for unresearched items
				else if (!_game->getSavedGame()->isResearched((*it)->getRules()->getRequirements()))
				{
					addStat("STR_ALIEN_ARTIFACTS_RECOVERED", 1, (*it)->getRules()->getRecoveryPoints());
				}
			}

			// put items back in the base
			if (!(*it)->getRules()->isFixed() && (*it)->getRules()->isRecoverable())
			{
				switch ((*it)->getRules()->getBattleType())
				{
					case BT_CORPSE: // corpses are handled above, do not process them here.
						break;
					case BT_AMMO:
						// It's a clip, count any rounds left.
						_rounds[(*it)->getRules()] += (*it)->getAmmoQuantity();
						break;
					case BT_FIREARM:
					case BT_MELEE:
						// It's a weapon, count any rounds left in the clip.
						{
							BattleItem *clip = (*it)->getAmmoItem();
							if (clip && clip->getRules()->getClipSize() > 0 && clip != *it)
							{
								_rounds[clip->getRules()] += clip->getAmmoQuantity();
							}
						}
						// Fall-through, to recover the weapon itself.
					default:
						base->getStorageItems()->addItem((*it)->getRules()->getType(), 1);
				}
				if ((*it)->getRules()->getBattleType() == BT_NONE)
				{
					for (std::vector<Craft*>::iterator c = base->getCrafts()->begin(); c != base->getCrafts()->end(); ++c)
					{
						(*c)->reuseItem((*it)->getRules()->getType());
					}
				}
			}
		}
	}
}

/**
 * Recovers a live alien from the battlescape.
 * @param from Battle unit to recover.
 * @param base Base to add items to.
 */
void DebriefingState::recoverAlien(BattleUnit *from, Base *base)
{
	// Zombie handling: don't recover a zombie.
	if (!from->getSpawnUnit().empty())
	{
		// convert it, and mind control the resulting unit.
		// reason: zombies don't create unconscious bodies... ever.
		// the only way we can get into this situation is if psi-capture is enabled.
		// we can use that knowledge to our advantage to save having to make it unconscious and spawn a body item for it.
		BattleUnit *newUnit = _game->getSavedGame()->getSavedBattle()->getBattleGame()->convertUnit(from);
		newUnit->convertToFaction(FACTION_PLAYER);
		// don't process the zombie itself, our new unit just got added to the end of the vector we're iterating, and will be handled later.
		return;
	}
	std::string type = from->getType();
	if (base->getAvailableContainment() == 0 && _game->getSavedGame()->getMonthsPassed() > -1)
	{
		_noContainment = true;
		if (!from->getArmor()->getCorpseBattlescape().empty())
		{
			RuleItem *corpseRule = _game->getMod()->getItem(from->getArmor()->getCorpseBattlescape().front());
			if (corpseRule && corpseRule->isRecoverable())
			{
				addStat("STR_ALIEN_CORPSES_RECOVERED", 1, corpseRule->getRecoveryPoints());
				std::string corpseItem = from->getArmor()->getCorpseGeoscape();
				base->getStorageItems()->addItem(corpseItem, 1);
			}
		}
	}
	else
	{
		RuleResearch *research = _game->getMod()->getResearch(type);
		if (research != 0 && !_game->getSavedGame()->isResearched(type))
		{
			// more points if it's not researched
			addStat("STR_LIVE_ALIENS_RECOVERED", 1, from->getValue() * 2);
		}
		else
		{
			// 10 points for recovery
			addStat("STR_LIVE_ALIENS_RECOVERED", 1, 10);
		}

		base->getStorageItems()->addItem(type, 1);
		_manageContainment = base->getAvailableContainment() - (base->getUsedContainment() * _limitsEnforced) < 0;
	}
}

}
