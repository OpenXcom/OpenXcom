#pragma once
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
#include "../Engine/State.h"
#include <string>
#include <vector>
#include <map>
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class BattleItem;
class Craft;
class Base;
class Region;
class Country;
class RuleItem;
class BattleUnit;
struct UnitStats;

struct DebriefingStat { DebriefingStat(const std::string &_item, bool recovery) : item(_item), qty(0), score(0), recovery(recovery) {}; std::string item; int qty; int score; bool recovery; };

struct ReequipStat { std::string item; int qty; std::wstring craft; };

struct RecoveryItem { std::string name; int value; };

/**
 * Debriefing screen shown after a Battlescape
 * mission that displays the results.
 */
class DebriefingState : public State
{
private:
	typedef std::pair<std::wstring, UnitStats> SoldierStatsEntry;

	Region *_region;
	Country *_country;
	Base *_base;
	std::vector<DebriefingStat*> _stats;
	std::vector<SoldierStatsEntry> _soldierStats;
	TextButton *_btnOk, *_btnStats;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtScore, *_txtRecovery, *_txtRating,
	     *_txtSoldier, *_txtTU, *_txtStamina, *_txtHealth, *_txtBravery, *_txtReactions,
	     *_txtFiring, *_txtThrowing, *_txtMelee, *_txtStrength, *_txtPsiStrength, *_txtPsiSkill;
	TextList *_lstStats, *_lstRecovery, *_lstTotal, *_lstSoldierStats;
	std::string _currentTooltip;
	Text *_txtTooltip;
	std::vector<ReequipStat> _missingItems;
	std::map<RuleItem*, int> _rounds;
	std::map<int, RecoveryItem*> _recoveryStats;
	bool _positiveScore, _noContainment, _manageContainment, _destroyBase;
	int _limitsEnforced;
	MissionStatistics *_missionStatistics;
    std::vector<Soldier*> _soldiersCommended, _deadSoldiersCommended;
	/// Adds to the debriefing stats.
	void addStat(const std::string &name, int quantity, int score);
	/// Prepares debriefing.
	void prepareDebriefing();
	/// Recovers items from the battlescape.
	void recoverItems(std::vector<BattleItem*> *from, Base *base);
	/// Recovers an alien from the battlescape.
	void recoverAlien(BattleUnit *from, Base *base);
	/// Reequips a craft after a mission.
	void reequipCraft(Base *base, Craft *craft, bool vehicleItemsCanBeDestroyed);
	/// True when soldier stat improvements are shown rather than scores. Toggled with the corresponding button.
	bool _showSoldierStats;
	/// Sets the visibility according to the _showSoldierStats flag
	void applyVisibility();
	/// Creates a string for the soldier stats table from a stat difference value
	std::wstring makeSoldierString(int stat);
public:
	/// Creates the Debriefing state.
	DebriefingState();
	/// Cleans up the Debriefing state.
	~DebriefingState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	void init();
	/// Handler for clicking the STATS button.
	void btnStatsClick(Action *action);
	/// Handler for showing tooltip.
	void txtTooltipIn(Action *action);
	/// Handler for hiding tooltip.
	void txtTooltipOut(Action *action);
};

}
