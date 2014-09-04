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
#ifndef OPENXCOM_RULESET_H
#define OPENXCOM_RULESET_H

#include <map>
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include "../Savegame/GameTime.h"

namespace OpenXcom
{

class SavedGame;
class SoldierNamePool;
class Soldier;
class RuleCountry;
class RuleRegion;
class RuleBaseFacility;
class RuleCraft;
class RuleCraftWeapon;
class RuleItem;
class RuleUfo;
class RuleTerrain;
class MapDataSet;
class ResourcePack;
class RuleSoldier;
class Unit;
class Armor;
class ArticleDefinition;
class RuleInventory;
class RuleResearch;
class RuleManufacture;
class RuleScriptedEvent;
class RuleMissionSequence;
class AlienRace;
class AlienDeployment;
class UfoTrajectory;
class RuleAlienMission;
class City;
class Base;
class MCDPatch;
class ExtraSprites;
class ExtraSounds;
class ExtraStrings;
class StatString;
class RuleInterface;
class RuleGlobe;
class SoundDefinition;

/**
 * Set of rules and stats for a game.
 * A ruleset holds all the constant info that never changes
 * throughout a game, like stats of all the in-game items,
 * countries, research tree, soldier names, starting base, etc.
 */
class Ruleset
{
protected:
	std::vector<SoldierNamePool*> _names;
	std::map<std::string, RuleCountry*> _countries;
	std::map<std::string, RuleRegion*> _regions;
	std::map<std::string, RuleBaseFacility*> _facilities;
	std::map<std::string, RuleCraft*> _crafts;
	std::map<std::string, RuleCraftWeapon*> _craftWeapons;
	std::map<std::string, RuleItem*> _items;
	std::map<std::string, RuleUfo*> _ufos;
	std::map<std::string, RuleTerrain*> _terrains;
	std::map<std::string, MapDataSet*> _mapDataSets;
	std::map<std::string, RuleSoldier*> _soldiers;
	std::map<std::string, Unit*> _units;
	std::map<std::string, AlienRace*> _alienRaces;
	std::map<std::string, AlienDeployment*> _alienDeployments;
	std::map<std::string, Armor*> _armors;
	std::map<std::string, ArticleDefinition*> _ufopaediaArticles;
	std::map<std::string, RuleInventory*> _invs;
	std::map<std::string, RuleResearch *> _research;
	std::map<std::string, RuleManufacture *> _manufacture;
	std::map<std::string, RuleScriptedEvent *> _scriptedEvents;
	std::map<std::string, RuleMissionSequence *> _missionSequences;
	std::map<std::string, UfoTrajectory *> _ufoTrajectories;
	std::map<std::string, RuleAlienMission *> _alienMissions;
	std::map<std::string, RuleInterface *> _interfaces;
	std::map<std::string, SoundDefinition *> _soundDefs;
	std::map<std::string, MCDPatch *> _MCDPatches;
	std::vector<std::pair<std::string, ExtraSprites *> > _extraSprites;
	std::vector<std::pair<std::string, ExtraSounds *> > _extraSounds;
	std::map<std::string, ExtraStrings *> _extraStrings;
	std::vector<StatString*> _statStrings;
	RuleGlobe *_globe;
	int _costSoldier, _costEngineer, _costScientist, _timePersonnel, _initialFunding;
	std::string _alienFuel;
	YAML::Node _startingBase;
	GameTime _startingTime;
	std::vector<std::string> _countriesIndex, _regionsIndex, _facilitiesIndex, _craftsIndex, _craftWeaponsIndex, _itemsIndex, _invsIndex, _ufosIndex;
	std::vector<std::string> _aliensIndex, _deploymentsIndex, _armorsIndex, _ufopaediaIndex, _researchIndex, _manufactureIndex, _MCDPatchesIndex;
	std::vector<std::string> _alienMissionsIndex, _terrainIndex, _extraSpritesIndex, _extraSoundsIndex, _extraStringsIndex;
	std::vector<std::string> _scriptedEventsIndex, _missionSequencesIndex;
	std::vector<std::vector<int> > _alienItemLevels;
	int _modIndex, _facilityListOrder, _craftListOrder, _itemListOrder, _researchListOrder,  _manufactureListOrder, _ufopaediaListOrder, _invListOrder;
	std::vector<std::string> _psiRequirements; // it's a cache for psiStrengthEval
	/// Loads a ruleset from a YAML file.
	void loadFile(const std::string &filename);
	/// Loads all ruleset files from a directory.
	void loadFiles(const std::string &dirname);
	/// Loads a ruleset element.
	template <typename T>
	T *loadRule(const YAML::Node &node, std::map<std::string, T*> *map, std::vector<std::string> *index = 0, const std::string &key = "type");
public:
	/// Creates a blank ruleset.
	Ruleset();
	/// Cleans up the ruleset.
	~Ruleset();
	/// Loads a ruleset from the given source.
	void load(const std::string &source);
	/// Generates the starting saved game.
	SavedGame *newSave() const;
	/// Gets the pool list for soldier names.
	const std::vector<SoldierNamePool*> &getPools() const;
	/// Gets the ruleset for a country type.
	RuleCountry *getCountry(const std::string &id) const;
	/// Gets the available countries.
	const std::vector<std::string> &getCountriesList() const;
	/// Gets the ruleset for a region type.
	RuleRegion *getRegion(const std::string &id) const;
	/// Gets the available regions.
	const std::vector<std::string> &getRegionsList() const;
	/// Gets the ruleset for a facility type.
	RuleBaseFacility *getBaseFacility(const std::string &id) const;
	/// Gets the available facilities.
	const std::vector<std::string> &getBaseFacilitiesList() const;
	/// Gets the ruleset for a craft type.
	RuleCraft *getCraft(const std::string &id) const;
	/// Gets the available crafts.
	const std::vector<std::string> &getCraftsList() const;
	/// Gets the ruleset for a craft weapon type.
	RuleCraftWeapon *getCraftWeapon(const std::string &id) const;
	/// Gets the available craft weapons.
	const std::vector<std::string> &getCraftWeaponsList() const;
	/// Gets the ruleset for an item type.
	RuleItem *getItem(const std::string &id) const;
	/// Gets the available items.
	const std::vector<std::string> &getItemsList() const;
	/// Gets the ruleset for a UFO type.
	RuleUfo *getUfo(const std::string &id) const;
	/// Gets the available UFOs.
	const std::vector<std::string> &getUfosList() const;
	/// Gets the available terrains.
	const std::vector<std::string> &getTerrainList() const;
	/// Gets terrains for battlescape games.
	RuleTerrain *getTerrain(const std::string &name) const;
	/// Gets mapdatafile for battlescape games.
	MapDataSet *getMapDataSet(const std::string &name);
	/// Gets soldier unit rules.
	RuleSoldier *getSoldier(const std::string &name) const;
	/// Gets the scripted events
	RuleScriptedEvent *getScriptedEvent(const std::string &name) const;
	/// Gets the scripted event list
	const std::vector<std::string> getScriptedEventList() const;
	/// Gets a mission sequences
	RuleMissionSequence *getMissionSequence(const std::string &name) const;
	/// Gets the mission sequences list
	const std::vector<std::string> getMissionSequenceList() const;
	/// Gets generated unit rules.
	Unit *getUnit(const std::string &name) const;
	/// Gets alien race rules.
	AlienRace *getAlienRace(const std::string &name) const;
	/// Gets the available alien races.
	const std::vector<std::string> &getAlienRacesList() const;
	/// Gets deployment rules.
	AlienDeployment *getDeployment(const std::string &name) const;
	/// Gets the available alien deployments.
	const std::vector<std::string> &getDeploymentsList() const;
	/// Gets armor rules.
	Armor *getArmor(const std::string &name) const;
	/// Gets the available armors.
	const std::vector<std::string> &getArmorsList() const;
	/// Gets Ufopaedia article definition.
	ArticleDefinition *getUfopaediaArticle(const std::string &name) const;
	/// Gets the available articles.
	const std::vector<std::string> &getUfopaediaList() const;
	/// Gets the inventory list.
	std::map<std::string, RuleInventory*> *getInventories();
	/// Gets the ruleset for a specific inventory.
	RuleInventory *getInventory(const std::string &id) const;
	/// Gets the cost of a soldier.
	int getSoldierCost() const;
	/// Gets the cost of an engineer.
	int getEngineerCost() const;
	/// Gets the cost of a scientist.
	int getScientistCost() const;
	/// Gets the transfer time of personnel.
	int getPersonnelTime() const;
	/// Gets the ruleset for a specific research project.
	RuleResearch *getResearch (const std::string &id) const;
	/// Gets the list of all research projects.
	const std::vector<std::string> &getResearchList () const;
	/// Gets the ruleset for a specific manufacture project.
	RuleManufacture *getManufacture (const std::string &id) const;
	/// Gets the list of all manufacture projects.
	const std::vector<std::string> &getManufactureList () const;
	/// Gets facilities for custom bases.
	std::vector<OpenXcom::RuleBaseFacility*> getCustomBaseFacilities() const;
	/// Gets a specific UfoTrajectory.
	const UfoTrajectory *getUfoTrajectory(const std::string &id) const;
	/// Gets the ruleset for a specific alien mission.
	const RuleAlienMission *getAlienMission(const std::string &id) const;
	/// Gets the list of all alien missions.
	const std::vector<std::string> &getAlienMissionList() const;
	/// Gets the city at the specified coordinates.
	const City *locateCity(double lon, double lat) const;
	/// Gets the alien item level table.
	const std::vector<std::vector<int> > &getAlienItemLevels() const;
	/// Gets the Defined starting base.
	const YAML::Node &getStartingBase();
	/// Gets an MCDPatch.
	MCDPatch *getMCDPatch(const std::string name) const;
	/// Gets the list of external Sprites.
	std::vector<std::pair<std::string, ExtraSprites *> > getExtraSprites() const;
	/// Gets the list of external Sounds.
	std::vector<std::pair<std::string, ExtraSounds *> > getExtraSounds() const;
	/// Gets the list of external Strings.
	std::map<std::string, ExtraStrings *> getExtraStrings() const;
	/// Gets the list of StatStrings.
    std::vector<StatString *> getStatStrings() const;    
	/// Sorts all our lists according to their weight.
	void sortLists();
	/// Gets the research-requirements for Psi-Lab (it's a cache for psiStrengthEval)
	std::vector<std::string> getPsiRequirements() const;
	/// Returns the sorted list of inventories.
	const std::vector<std::string> &getInvsList () const;
	/// Generates a new soldier.
	Soldier *genSoldier(SavedGame *save) const;
	/// Gets the item to be used as fuel for ships.
	const std::string getAlienFuel() const;
	/// Gets information on an interface element.
	RuleInterface *getInterface(const std::string id) const;
	/// Gets the ruleset for the globe
	RuleGlobe *getGlobe() const;
	/// Gets the list of selective files for insertion into our cat files.
	const std::map<std::string, SoundDefinition *> *getSoundDefinitions() const;
};

}

#endif
