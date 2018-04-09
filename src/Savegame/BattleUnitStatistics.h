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
#include <string>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include "BattleUnit.h"
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Container for battle unit kills statistics.
 */
struct BattleUnitKills
{
	// Variables
	std::wstring name;
	std::string type, rank, race, weapon, weaponAmmo;
	UnitFaction faction;
	UnitStatus status;
	int mission, turn, id;
	UnitSide side;
	UnitBodyPart bodypart;

	// Functions
	/// Make turn unique across all kills
	int makeTurnUnique()
	{
		return turn += mission * 300; // Maintains divisibility by 3 as well
	}

	/// Check to see if turn was on HOSTILE side
	bool hostileTurn() const
	{
		if ((turn - 1) % 3 == 0) return true;
		return false;
	}

	/// Make turn unique across mission
	void setTurn(int unitTurn, UnitFaction unitFaction)
	{
		turn = unitTurn * 3 + (int)unitFaction;
	}

	/// Load
	void load(const YAML::Node &node)
	{
		if (const YAML::Node n = node["name"])
		{
			name = Language::utf8ToWstr(n.as<std::string>());
		}
		type = node["type"].as<std::string>(type);
		rank = node["rank"].as<std::string>(rank);
		race = node["race"].as<std::string>(race);
		weapon = node["weapon"].as<std::string>(weapon);
		weaponAmmo = node["weaponAmmo"].as<std::string>(weaponAmmo);
		status = (UnitStatus)node["status"].as<int>();
		faction = (UnitFaction)node["faction"].as<int>();
		mission = node["mission"].as<int>(mission);
		turn = node["turn"].as<int>(turn);
		side = (UnitSide)node["side"].as<int>();
		bodypart = (UnitBodyPart)node["bodypart"].as<int>();
		id = node["id"].as<int>(id);
	}

	/// Save
	YAML::Node save() const
	{
		YAML::Node node;
		if (!name.empty())
			node["name"] = Language::wstrToUtf8(name);
		if (!type.empty())
			node["type"] = type;
		node["rank"] = rank;
		node["race"] = race;
		node["weapon"] = weapon;
		node["weaponAmmo"] = weaponAmmo;
		node["status"] = (int)status;
		node["faction"] = (int)faction;
		node["mission"] = mission;
		node["turn"] = turn;
		node["side"] = (int)side;
		node["bodypart"] = (int)bodypart;
		node["id"] = id;
		return node;
	}

	/// Convert kill Status to string.
	std::string getKillStatusString() const
	{
		switch (status)
		{
		case STATUS_DEAD:           return "STR_KILLED";
		case STATUS_UNCONSCIOUS:    return "STR_STUNNED";
		case STATUS_PANICKING:		return "STR_PANICKED";
		case STATUS_TURNING:		return "STR_MINDCONTROLLED";
		default:                    return "status error";
		}
	}

	/// Convert victim Status to string.
	std::string getUnitStatusString() const
	{
		switch (status)
		{
		case STATUS_DEAD:           return "STATUS_DEAD";
		case STATUS_UNCONSCIOUS:    return "STATUS_UNCONSCIOUS";
		case STATUS_PANICKING:		return "STATUS_PANICKING";
		case STATUS_TURNING:		return "STATUS_TURNING";
		default:                    return "status error";
		}
	}

	/// Convert victim Faction to string.
	std::string getUnitFactionString() const
	{
		switch (faction)
		{
		case FACTION_PLAYER:    return "FACTION_PLAYER";
		case FACTION_HOSTILE:   return "FACTION_HOSTILE";
		case FACTION_NEUTRAL:   return "FACTION_NEUTRAL";
		default:                return "faction error";
		}
	}

	/// Convert victim Side to string.
	std::string getUnitSideString() const
	{
		switch (side)
		{
		case SIDE_FRONT:    return "SIDE_FRONT";
		case SIDE_LEFT:     return "SIDE_LEFT";
		case SIDE_RIGHT:    return "SIDE_RIGHT";
		case SIDE_REAR:     return "SIDE_REAR";
		case SIDE_UNDER:    return "SIDE_UNDER";
		default:            return "side error";
		}
	}

	/// Convert victim Body part to string.
	std::string getUnitBodyPartString() const
	{
		switch (bodypart)
		{
		case BODYPART_HEAD:     return "BODYPART_HEAD";
		case BODYPART_TORSO:    return "BODYPART_TORSO";
		case BODYPART_RIGHTARM: return "BODYPART_RIGHTARM";
		case BODYPART_LEFTARM:  return "BODYPART_LEFTARM";
		case BODYPART_RIGHTLEG: return "BODYPART_RIGHTLEG";
		case BODYPART_LEFTLEG:  return "BODYPART_LEFTLEG";
		default:                return "body part error";
		}
	}

	/// Get human-readable victim name.
	std::wstring getUnitName(Language *lang) const
	{
		if (!name.empty())
		{
			return name;
		}
		else if (!type.empty())
		{
			return lang->getString(type);
		}
		else
		{
			std::wostringstream ss;
			ss << lang->getString(race) << L" " << lang->getString(rank);
			return ss.str();
		}
	}

	/// Decide victim name, race and rank.
	void setUnitStats(BattleUnit *unit)
	{
		name = L"";
		type = "";
		if (unit->getGeoscapeSoldier())
		{
			name = unit->getGeoscapeSoldier()->getName();
		}
		else
		{
			type = unit->getType();
		}

		if (unit->getOriginalFaction() == FACTION_PLAYER)
		{
			// Soldiers
			if (unit->getGeoscapeSoldier())
			{
				if (!unit->getGeoscapeSoldier()->getRankString().empty())
				{
					rank = unit->getGeoscapeSoldier()->getRankString();
				}
				else
				{
					rank = "STR_SOLDIER";
				}
				if (unit->getUnitRules() != 0 && !unit->getUnitRules()->getRace().empty())
				{
					race = unit->getUnitRules()->getRace();
				}
				else
				{
					race = "STR_FRIENDLY";
				}
			}
			// HWPs
			else
			{
				if (unit->getUnitRules() != 0 && !unit->getUnitRules()->getRank().empty())
				{
					rank = unit->getUnitRules()->getRank();
				}
				else
				{
					rank = "STR_HWPS";
				}
				if (unit->getUnitRules() != 0 && !unit->getUnitRules()->getRace().empty())
				{
					race = unit->getUnitRules()->getRace();
				}
				else
				{
					race = "STR_FRIENDLY";
				}
			}
		}
		// Aliens
		else if (unit->getOriginalFaction() == FACTION_HOSTILE)
		{
			if (unit->getUnitRules() != 0 && !unit->getUnitRules()->getRank().empty())
			{
				rank = unit->getUnitRules()->getRank();
			}
			else
			{
				rank = "STR_LIVE_SOLDIER";
			}
			if (unit->getUnitRules() != 0 && !unit->getUnitRules()->getRace().empty())
			{
				race = unit->getUnitRules()->getRace();
			}
			else
			{
				race = "STR_HOSTILE";
			}
		}
		// Civilians
		else if (unit->getOriginalFaction() == FACTION_NEUTRAL)
		{
			if (unit->getUnitRules() != 0 && !unit->getUnitRules()->getRank().empty())
			{
				rank = unit->getUnitRules()->getRank();
			}
			else
			{
				rank = "STR_CIVILIAN";
			}
			if (unit->getUnitRules() != 0 && !unit->getUnitRules()->getRace().empty())
			{
				race = unit->getUnitRules()->getRace();
			}
			else
			{
				race = "STR_NEUTRAL";
			}
		}
		// Error
		else
		{
			rank = "STR_UNKNOWN";
			race = "STR_UNKNOWN";
		}
	}

	BattleUnitKills(const YAML::Node& node) { load(node); }
	BattleUnitKills(): faction(FACTION_HOSTILE), status(STATUS_IGNORE_ME), mission(0), turn(0), id(0), side(SIDE_FRONT), bodypart(BODYPART_HEAD) { }
	~BattleUnitKills() { }
};

/**
 * Container for battle unit statistics.
 */
struct BattleUnitStatistics
{
	// Variables
	bool wasUnconcious;                  ///< Tracks if the soldier fell unconcious
	int shotAtCounter;                   ///< Tracks how many times the unit was shot at
	int hitCounter;                      ///< Tracks how many times the unit was hit
	int shotByFriendlyCounter;           ///< Tracks how many times the unit was hit by a friendly
	int shotFriendlyCounter;             ///< Tracks how many times the unit was hit a friendly
	bool loneSurvivor;                   ///< Tracks if the soldier was the only survivor
	bool ironMan;                        ///< Tracks if the soldier was the only soldier on the mission
	int longDistanceHitCounter;          ///< Tracks how many long distance shots were landed
	int lowAccuracyHitCounter;           ///< Tracks how many times the unit landed a low probability shot
	int shotsFiredCounter;               ///< Tracks how many times a unit has shot
	int shotsLandedCounter;              ///< Tracks how many times a unit has hit his target
	std::vector<BattleUnitKills*> kills; ///< Tracks kills
	int daysWounded;                     ///< Tracks how many days the unit was wounded for
	bool KIA;                            ///< Tracks if the soldier was killed in battle
	bool nikeCross;                      ///< Tracks if a soldier killed every alien or killed and stunned every alien
	bool mercyCross;                     ///< Tracks if a soldier stunned every alien
	int woundsHealed;                    ///< Tracks how many times a fatal wound was healed by this unit
	UnitStats delta;                     ///< Tracks the increase in unit stats (is not saved, only used during debriefing)
	int appliedStimulant;                ///< Tracks how many times this soldier applied stimulant
	int appliedPainKill;                 ///< Tracks how many times this soldier applied pain killers
	int revivedSoldier;                  ///< Tracks how many times this soldier revived another unit
	bool MIA;                            ///< Tracks if the soldier was left behind :(
	int martyr;                          ///< Tracks how many kills the soldier landed on the turn of his death
	int slaveKills;                      ///< Tracks how many kills the soldier landed thanks to a mind controlled unit.

	// Functions
	/// Duplicate entry check
	bool duplicateEntry(UnitStatus status, int id) const
	{
		for (std::vector<BattleUnitKills*>::const_iterator i = kills.begin(); i != kills.end(); ++i)
		{
			if ((*i)->id == id && (*i)->status == status)
			{
				return true;
			}
		}
		return false;
	}

	/// Friendly fire check
	bool hasFriendlyFired() const
	{
		for (std::vector<BattleUnitKills*>::const_iterator i = kills.begin(); i != kills.end(); ++i)
		{
			if ((*i)->faction == FACTION_PLAYER)
				return true;
		}
		return false;
	}

	/// Load function
	void load(const YAML::Node& node)
	{
		wasUnconcious = node["wasUnconcious"].as<bool>(wasUnconcious);
		if (const YAML::Node &YAMLkills = node["kills"])
		{
			for (YAML::const_iterator i = YAMLkills.begin(); i != YAMLkills.end(); ++i)
				kills.push_back(new BattleUnitKills(*i));
		}
		shotAtCounter = node["shotAtCounter"].as<int>(shotAtCounter);
		hitCounter = node["hitCounter"].as<int>(hitCounter);
		shotByFriendlyCounter = node["shotByFriendlyCounter"].as<int>(shotByFriendlyCounter);
		shotFriendlyCounter = node["shotFriendlyCounter"].as<int>(shotFriendlyCounter);
		loneSurvivor = node["loneSurvivor"].as<bool>(loneSurvivor);
		ironMan = node["ironMan"].as<bool>(ironMan);
		longDistanceHitCounter = node["longDistanceHitCounter"].as<int>(longDistanceHitCounter);
		lowAccuracyHitCounter = node["lowAccuracyHitCounter"].as<int>(lowAccuracyHitCounter);
		shotsFiredCounter = node["shotsFiredCounter"].as<int>(shotsFiredCounter);
		shotsLandedCounter = node["shotsLandedCounter"].as<int>(shotsLandedCounter);
		nikeCross = node["nikeCross"].as<bool>(nikeCross);
		mercyCross = node["mercyCross"].as<bool>(mercyCross);
		woundsHealed = node["woundsHealed"].as<int>(woundsHealed);
		appliedStimulant = node["appliedStimulant"].as<int>(appliedStimulant);
		appliedPainKill = node["appliedPainKill"].as<int>(appliedPainKill);
		revivedSoldier = node["revivedSoldier"].as<int>(revivedSoldier);
		martyr = node["martyr"].as<int>(martyr);
		slaveKills = node["slaveKills"].as<int>(slaveKills);
	}

	/// Save function
	YAML::Node save() const
	{
		YAML::Node node;
		node["wasUnconcious"] = wasUnconcious;
		if (!kills.empty())
		{
			for (std::vector<BattleUnitKills*>::const_iterator i = kills.begin(); i != kills.end(); ++i)
				node["kills"].push_back((*i)->save());
		}
		if (shotAtCounter) node["shotAtCounter"] = shotAtCounter;
		if (hitCounter) node["hitCounter"] = hitCounter;
		if (shotByFriendlyCounter) node["shotByFriendlyCounter"] = shotByFriendlyCounter;
		if (shotFriendlyCounter) node["shotFriendlyCounter"] = shotFriendlyCounter;
		if (loneSurvivor) node["loneSurvivor"] = loneSurvivor;
		if (ironMan) node["ironMan"] = ironMan;
		if (longDistanceHitCounter) node["longDistanceHitCounter"] = longDistanceHitCounter;
		if (lowAccuracyHitCounter) node["lowAccuracyHitCounter"] = lowAccuracyHitCounter;
		if (shotsFiredCounter) node["shotsFiredCounter"] = shotsFiredCounter;
		if (shotsLandedCounter) node["shotsLandedCounter"] = shotsLandedCounter;
		if (nikeCross) node["nikeCross"] = nikeCross;
		if (mercyCross) node["mercyCross"] = mercyCross;
		if (woundsHealed) node["woundsHealed"] = woundsHealed;
		if (appliedStimulant) node["appliedStimulant"] = appliedStimulant;
		if (appliedPainKill) node["appliedPainKill"] = appliedPainKill;
		if (revivedSoldier) node["revivedSoldier"] = revivedSoldier;
		if (martyr) node["martyr"] = martyr;
		if (slaveKills) node["slaveKills"] = slaveKills;
		return node;
	}

	BattleUnitStatistics(const YAML::Node& node) { load(node); }
	BattleUnitStatistics() : wasUnconcious(false), shotAtCounter(0), hitCounter(0), shotByFriendlyCounter(0), shotFriendlyCounter(0), loneSurvivor(false), ironMan(false), longDistanceHitCounter(0), lowAccuracyHitCounter(0), shotsFiredCounter(0), shotsLandedCounter(0), kills(), daysWounded(0), KIA(false), nikeCross(false), mercyCross(false), woundsHealed(0), appliedStimulant(0), appliedPainKill(0), revivedSoldier(0), MIA(false), martyr(0), slaveKills(0) { }
	~BattleUnitStatistics() { }
};

}
