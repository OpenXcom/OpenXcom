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
#include <map>
#include <vector>
#include <string>
#include <SDL.h>
#include <yaml-cpp/yaml.h>
#include "../Engine/Options.h"
#include "../Savegame/GameTime.h"
#include "Unit.h"
#include "RuleAlienMission.h"

namespace OpenXcom
{

class Surface;
class SurfaceSet;
class Font;
class Palette;
class Music;
class SoundSet;
class Sound;
class CatFile;
class GMCatFile;
class Music;
class Palette;
class SavedGame;
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
class RuleSoldier;
class Unit;
class Armor;
class ArticleDefinition;
class RuleInventory;
class RuleResearch;
class RuleManufacture;
class AlienRace;
class AlienDeployment;
class UfoTrajectory;
class RuleAlienMission;
class Base;
class MCDPatch;
class ExtraSprites;
class ExtraSounds;
class ExtraStrings;
class RuleCommendations;
class StatString;
class RuleInterface;
class RuleGlobe;
class RuleConverter;
class SoundDefinition;
class MapScript;
class ModInfo;
class RuleVideo;
class RuleMusic;
class RuleMissionScript;
struct StatAdjustment;

/**
 * Mod data used when loading resources
 */
struct ModData
{
	/// Mod name
	std::string name;
	/// Optional info about mod
	const ModInfo* info;
	/// Offset that mod use is common sets
	size_t offset;
	/// Maximum size allowed by mod in common sets
	size_t size;
};

/**
 * Contains all the game-specific static data that never changes
 * throughout the game, like rulesets and resources.
 */
class Mod
{
private:
	Music *_muteMusic;
	Sound *_muteSound;
	std::string _playingMusic;

	std::map<std::string, Palette*> _palettes;
	std::map<std::string, Font*> _fonts;
	std::map<std::string, Surface*> _surfaces;
	std::map<std::string, SurfaceSet*> _sets;
	std::map<std::string, SoundSet*> _sounds;
	std::map<std::string, Music*> _musics;
	std::vector<Uint16> _voxelData;
	std::vector<std::vector<Uint8> > _transparencyLUTs;

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
	std::map<std::string, UfoTrajectory *> _ufoTrajectories;
	std::map<std::string, RuleAlienMission *> _alienMissions;
	std::map<std::string, RuleInterface *> _interfaces;
	std::map<std::string, SoundDefinition *> _soundDefs;
	std::map<std::string, RuleVideo *>_videos;
	std::map<std::string, MCDPatch *> _MCDPatches;
	std::map<std::string, std::vector<MapScript *> > _mapScripts;
	std::map<std::string, RuleCommendations *> _commendations;
	std::map<std::string, RuleMissionScript*> _missionScripts;
	std::map<std::string, std::vector<ExtraSprites *> > _extraSprites;
	std::vector<std::pair<std::string, ExtraSounds *> > _extraSounds;
	std::map<std::string, ExtraStrings *> _extraStrings;
	std::vector<StatString*> _statStrings;
	std::map<std::string, RuleMusic *> _musicDefs;
	RuleGlobe *_globe;
	RuleConverter *_converter;
	int _costEngineer, _costScientist, _timePersonnel, _initialFunding, _turnAIUseGrenade, _turnAIUseBlaster, _defeatScore, _defeatFunds;
	bool _difficultyDemigod;
	std::pair<std::string, int> _alienFuel;
	std::string _fontName, _finalResearch;
	YAML::Node _startingBase;
	GameTime _startingTime;
	StatAdjustment _statAdjustment[5];

	std::map<std::string, int> _ufopaediaSections;
	std::vector<std::string> _countriesIndex, _regionsIndex, _facilitiesIndex, _craftsIndex, _craftWeaponsIndex, _itemsIndex, _invsIndex, _ufosIndex;
	std::vector<std::string> _soldiersIndex, _aliensIndex, _deploymentsIndex, _armorsIndex, _ufopaediaIndex, _ufopaediaCatIndex, _researchIndex, _manufactureIndex;
	std::vector<std::string> _alienMissionsIndex, _terrainIndex, _missionScriptIndex;
	std::vector<std::vector<int> > _alienItemLevels;
	std::vector<SDL_Color> _transparencies;
	int _facilityListOrder, _craftListOrder, _itemListOrder, _researchListOrder,  _manufactureListOrder, _ufopaediaListOrder, _invListOrder;
	std::vector<ModData> _modData;
	ModData* _modCurrent;
	SDL_Color *_statePalette;
	std::vector<std::string> _psiRequirements; // it's a cache for psiStrengthEval

	/// Loads a ruleset from a YAML file that have basic resources configuration.
	void loadResourceConfigFile(const std::string &filename);
	void loadConstants(const YAML::Node &node);
	/// Loads a ruleset from a YAML file.
	void loadFile(const std::string &filename);
	/// Loads a ruleset element.
	template <typename T>
	T *loadRule(const YAML::Node &node, std::map<std::string, T*> *map, std::vector<std::string> *index = 0, const std::string &key = "type") const;
	/// Gets a ruleset element.
	template <typename T>
	T *getRule(const std::string &id, const std::string &name, const std::map<std::string, T*> &map, bool error) const;
	/// Gets a random music. This is private to prevent access, use playMusic(name, true) instead.
	Music *getRandomMusic(const std::string &name) const;
	/// Gets a particular sound set. This is private to prevent access, use getSound(name, id) instead.
	SoundSet *getSoundSet(const std::string &name, bool error = true) const;
	/// Loads battlescape specific resources.
	void loadBattlescapeResources();
	/// Loads a specified music file.
	Music *loadMusic(MusicFormat fmt, RuleMusic *rule, CatFile *adlibcat, CatFile *aintrocat, GMCatFile *gmcat) const;
	/// Creates a transparency lookup table for a given palette.
	void createTransparencyLUT(Palette *pal);
	/// Loads a specified mod content.
	void loadMod(const std::vector<std::string> &rulesetFiles);
	/// Loads resources from vanilla.
	void loadVanillaResources();
	/// Loads resources from extra rulesets.
	void loadExtraResources();
	/// Loads surfaces on demand.
	void lazyLoadSurface(const std::string &name);
	/// Loads an external sprite.
	void loadExtraSprite(ExtraSprites *spritePack);
	/// Applies mods to vanilla resources.
	void modResources();
	/// Sorts all our lists according to their weight.
	void sortLists();
public:
	static int DOOR_OPEN;
	static int SLIDING_DOOR_OPEN;
	static int SLIDING_DOOR_CLOSE;
	static int SMALL_EXPLOSION;
	static int LARGE_EXPLOSION;
	static int EXPLOSION_OFFSET;
	static int SMOKE_OFFSET;
	static int UNDERWATER_SMOKE_OFFSET;
	static int ITEM_DROP;
	static int ITEM_THROW;
	static int ITEM_RELOAD;
	static int WALK_OFFSET;
	static int FLYING_SOUND;
	static int BUTTON_PRESS;
	static int WINDOW_POPUP[3];
	static int UFO_FIRE;
	static int UFO_HIT;
	static int UFO_CRASH;
	static int UFO_EXPLODE;
	static int INTERCEPTOR_HIT;
	static int INTERCEPTOR_EXPLODE;
	static int GEOSCAPE_CURSOR;
	static int BASESCAPE_CURSOR;
	static int BATTLESCAPE_CURSOR;
	static int UFOPAEDIA_CURSOR;
	static int GRAPHS_CURSOR;
	static int DAMAGE_RANGE;
	static int EXPLOSIVE_DAMAGE_RANGE;
	static int FIRE_DAMAGE_RANGE[2];
	static std::string DEBRIEF_MUSIC_GOOD;
	static std::string DEBRIEF_MUSIC_BAD;
	static int DIFFICULTY_COEFFICIENT[5];
	// reset all the statics in all classes to default values
	static void resetGlobalStatics();
	/// Creates a blank mod.
	Mod();
	/// Cleans up the mod.
	~Mod();

	/// For internal use only
	const std::map<std::string, int> &getUfopaediaSections() const { return _ufopaediaSections; }

	/// Gets a particular font.
	Font *getFont(const std::string &name, bool error = true) const;
	/// Gets a particular surface.
	Surface *getSurface(const std::string &name, bool error = true);
	/// Gets a particular surface set.
	SurfaceSet *getSurfaceSet(const std::string &name, bool error = true);
	/// Gets a particular music.
	Music *getMusic(const std::string &name, bool error = true) const;
	/// Plays a particular music.
	void playMusic(const std::string &name, int id = 0);
	/// Gets a particular sound.
	Sound *getSound(const std::string &set, unsigned int sound, bool error = true) const;
	/// Gets a particular palette.
	Palette *getPalette(const std::string &name, bool error = true) const;
	/// Sets a new palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Gets list of voxel data.
	std::vector<Uint16> *getVoxelData();
	/// Returns a specific sound from either the land or underwater sound set.
	Sound *getSoundByDepth(unsigned int depth, unsigned int sound, bool error = true) const;
	/// Gets list of LUT data.
	const std::vector<std::vector<Uint8> > *getLUTs() const;

	/// Gets the mod offset.
	int getModOffset() const;
	/// Get offset and index for sound set or sprite set.
	void loadOffsetNode(const std::string &parent, int& offset, const YAML::Node &node, int shared, const std::string &set, size_t multiplier, size_t sizeScale = 1) const;
	/// Gets the mod offset for a certain sprite.
	void loadSpriteOffset(const std::string &parent, int& sprite, const YAML::Node &node, const std::string &set, size_t multiplier = 1) const;
	/// Gets the mod offset for a certain sound.
	void loadSoundOffset(const std::string &parent, int& sound, const YAML::Node &node, const std::string &set) const;
	/// Gets the mod offset array for a certain sound.
	void loadSoundOffset(const std::string &parent, std::vector<int>& sounds, const YAML::Node &node, const std::string &set) const;
	/// Gets the mod offset array for a certain transparency index.
	void loadTransparencyOffset(const std::string &parent, int& index, const YAML::Node &node) const;
	/// Gets the mod offset for a generic value.
	int getOffset(int id, int max) const;

	/// Loads a list of mods.
	void loadAll(const std::vector< std::pair< std::string, std::vector<std::string> > > &mods);
	/// Generates the starting saved game.
	SavedGame *newSave() const;
	/// Gets the ruleset for a country type.
	RuleCountry *getCountry(const std::string &id, bool error = false) const;
	/// Gets the available countries.
	const std::vector<std::string> &getCountriesList() const;
	/// Gets the ruleset for a region type.
	RuleRegion *getRegion(const std::string &id, bool error = false) const;
	/// Gets the available regions.
	const std::vector<std::string> &getRegionsList() const;
	/// Gets the ruleset for a facility type.
	RuleBaseFacility *getBaseFacility(const std::string &id, bool error = false) const;
	/// Gets the available facilities.
	const std::vector<std::string> &getBaseFacilitiesList() const;
	/// Gets the ruleset for a craft type.
	RuleCraft *getCraft(const std::string &id, bool error = false) const;
	/// Gets the available crafts.
	const std::vector<std::string> &getCraftsList() const;
	/// Gets the ruleset for a craft weapon type.
	RuleCraftWeapon *getCraftWeapon(const std::string &id, bool error = false) const;
	/// Gets the available craft weapons.
	const std::vector<std::string> &getCraftWeaponsList() const;
	/// Gets the ruleset for an item type.
	RuleItem *getItem(const std::string &id, bool error = false) const;
	/// Gets the available items.
	const std::vector<std::string> &getItemsList() const;
	/// Gets the ruleset for a UFO type.
	RuleUfo *getUfo(const std::string &id, bool error = false) const;
	/// Gets the available UFOs.
	const std::vector<std::string> &getUfosList() const;
	/// Gets terrains for battlescape games.
	RuleTerrain *getTerrain(const std::string &name, bool error = false) const;
	/// Gets the available terrains.
	const std::vector<std::string> &getTerrainList() const;
	/// Gets mapdatafile for battlescape games.
	MapDataSet *getMapDataSet(const std::string &name);
	/// Gets soldier unit rules.
	RuleSoldier *getSoldier(const std::string &name, bool error = false) const;
	/// Gets the available soldiers.
	const std::vector<std::string> &getSoldiersList() const;
	/// Gets commendation rules.
	RuleCommendations *getCommendation(const std::string &id, bool error = false) const;
	/// Gets the available commendations.
	const std::map<std::string, RuleCommendations *> &getCommendationsList() const;
	/// Gets generated unit rules.
	Unit *getUnit(const std::string &name, bool error = false) const;
	/// Gets alien race rules.
	AlienRace *getAlienRace(const std::string &name, bool error = false) const;
	/// Gets the available alien races.
	const std::vector<std::string> &getAlienRacesList() const;
	/// Gets deployment rules.
	AlienDeployment *getDeployment(const std::string &name, bool error = false) const;
	/// Gets the available alien deployments.
	const std::vector<std::string> &getDeploymentsList() const;
	/// Gets armor rules.
	Armor *getArmor(const std::string &name, bool error = false) const;
	/// Gets the available armors.
	const std::vector<std::string> &getArmorsList() const;
	/// Gets Ufopaedia article definition.
	ArticleDefinition *getUfopaediaArticle(const std::string &name, bool error = false) const;
	/// Gets the available articles.
	const std::vector<std::string> &getUfopaediaList() const;
	/// Gets the available article categories.
	const std::vector<std::string> &getUfopaediaCategoryList() const;
	/// Gets the inventory list.
	std::map<std::string, RuleInventory*> *getInventories();
	/// Gets the ruleset for a specific inventory.
	RuleInventory *getInventory(const std::string &id, bool error = false) const;
	/// Gets the cost of an engineer.
	int getEngineerCost() const;
	/// Gets the cost of a scientist.
	int getScientistCost() const;
	/// Gets the transfer time of personnel.
	int getPersonnelTime() const;
	/// Gets the ruleset for a specific research project.
	RuleResearch *getResearch (const std::string &id, bool error = false) const;
	/// Gets the list of all research projects.
	const std::vector<std::string> &getResearchList() const;
	/// Gets the ruleset for a specific manufacture project.
	RuleManufacture *getManufacture (const std::string &id, bool error = false) const;
	/// Gets the list of all manufacture projects.
	const std::vector<std::string> &getManufactureList() const;
	/// Gets facilities for custom bases.
	std::vector<RuleBaseFacility*> getCustomBaseFacilities() const;
	/// Gets a specific UfoTrajectory.
	const UfoTrajectory *getUfoTrajectory(const std::string &id, bool error = false) const;
	/// Gets the ruleset for a specific alien mission.
	const RuleAlienMission *getAlienMission(const std::string &id, bool error = false) const;
	/// Gets the ruleset for a random alien mission.
	const RuleAlienMission *getRandomMission(MissionObjective objective, size_t monthsPassed) const;
	/// Gets the list of all alien missions.
	const std::vector<std::string> &getAlienMissionList() const;
	/// Gets the alien item level table.
	const std::vector<std::vector<int> > &getAlienItemLevels() const;
	/// Gets the player starting base.
	const YAML::Node &getStartingBase() const;
	/// Gets the game starting time.
	const GameTime &getStartingTime() const;
	/// Gets an MCDPatch.
	MCDPatch *getMCDPatch(const std::string &id) const;
	/// Gets the list of external Sprites.
	const std::map<std::string, std::vector<ExtraSprites *> > &getExtraSprites() const;
	/// Gets the list of external Sounds.
	const std::vector<std::pair<std::string, ExtraSounds *> > &getExtraSounds() const;
	/// Gets the list of external Strings.
	const std::map<std::string, ExtraStrings *> &getExtraStrings() const;
	/// Gets the list of StatStrings.
	const std::vector<StatString *> &getStatStrings() const;
	/// Gets the research-requirements for Psi-Lab (it's a cache for psiStrengthEval)
	const std::vector<std::string> &getPsiRequirements() const;
	/// Returns the sorted list of inventories.
	const std::vector<std::string> &getInvsList() const;
	/// Generates a new soldier.
	Soldier *genSoldier(SavedGame *save, std::string type = "") const;
	/// Gets the item to be used as fuel for ships.
	std::string getAlienFuelName() const;
	/// Gets the amount of alien fuel to recover
	int getAlienFuelQuantity() const;
	/// Gets the font name.
	std::string getFontName() const;
	/// Gets first turn when AI can use grenade.
	int getTurnAIUseGrenade() const {return _turnAIUseGrenade;}
	/// Gets first turn when AI can use Blaster launcher.
	int getTurnAIUseBlaster() const {return _turnAIUseBlaster;}
	/// Gets the minimum radar's range.
	int getMinRadarRange() const;
	/// Gets information on an interface element.
	RuleInterface *getInterface(const std::string &id, bool error = true) const;
	/// Gets the ruleset for the globe.
	RuleGlobe *getGlobe() const;
	/// Gets the ruleset for the converter.
	RuleConverter *getConverter() const;
	/// Gets the list of selective files for insertion into our cat files.
	const std::map<std::string, SoundDefinition *> *getSoundDefinitions() const;
	/// Gets the list of transparency colors,
	const std::vector<SDL_Color> *getTransparencies() const;
	const std::vector<MapScript*> *getMapScript(const std::string& id) const;
	/// Gets a video for intro/outro etc.
	RuleVideo *getVideo(const std::string &id, bool error = false) const;
	const std::map<std::string, RuleMusic *> *getMusic() const;
	const std::vector<std::string> *getMissionScriptList() const;
	RuleMissionScript *getMissionScript(const std::string &name, bool error = false) const;
	std::string getFinalResearch() const;
	StatAdjustment *getStatAdjustment(int difficulty);
	int getDefeatScore() const;
	int getDefeatFunds() const;
	bool isDemigod() const;
};

}
