/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_DEBRIEFINGSTATE_H
#define OPENXCOM_DEBRIEFINGSTATE_H

#include "../Engine/State.h"
#include <string>
#include <vector>
#include <map>

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
class RuleScriptedEvent;

struct DebriefingStat { DebriefingStat(std::string _item, bool recovery) : item(_item), qty(0), score(0), recovery(recovery) {}; std::string item; int qty; int score; bool recovery; };

struct ReequipStat { std::string item; int qty; std::wstring craft; };

/**
 * Debriefing screen shown after a Battlescape
 * mission that displays the results.
 */
class DebriefingState : public State
{
private:
	Region *_region;
	Country *_country;
	Base *_base;
	std::vector<DebriefingStat*> _stats;
	TextButton *_btnOk;
	Window *_window;
	RuleScriptedEvent *_scriptedEvent;
	std::string _titleText;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtScore, *_txtRecovery, *_txtRating;
	TextList *_lstStats, *_lstRecovery, *_lstTotal;
	std::vector<ReequipStat> _missingItems;
	std::map<RuleItem*, int> _rounds;
	int _playerInExitArea; // if this stays 0 the craft is lost...
	int _playersSurvived; // if this stays 0 the craft is lost...
	int _playersUnconscious;
	/// Adds to the debriefing stats.
	void addStat(const std::string &name, int quantity, int score);
	/// Prepares debriefing.
	void prepareDebriefing();
	/// Recovers items from the battlescape.
	void recoverItems(std::vector<BattleItem*> *from, Base *base);
	/// Reequips a craft after a mission.
	void reequipCraft(Base *base, Craft *craft, bool vehicleItemsCanBeDestroyed);
	bool _noContainment, _manageContainment, _destroyBase;
	int _limitsEnforced;
	/// Calculate the number of player units surviving
	void calculateSurvivingPlayers();
	/// Add items from a scripted event to the rewards
	void rewardItems(const std::map<std::string, int> &items, Base *base);
	/// Immediate research following a scripted event
	void rewardResearch(std::string research, Base *base);
	int getScoreForItem(std::string item);
public:
	/// Creates the Debriefing state.
	DebriefingState();
	/// Cleans up the Debriefing state.
	~DebriefingState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
};

}

#endif
