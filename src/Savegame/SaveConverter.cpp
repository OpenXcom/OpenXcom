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
#define _USE_MATH_DEFINES
#include "SaveConverter.h"
#include "SaveConverterXcom1.h"
#include <yaml-cpp/yaml.h>
#include <SDL_endian.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include "../Engine/Options.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Exception.h"
#include "../Engine/Language.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "SavedGame.h"
#include "GameTime.h"
#include "Country.h"
#include "Region.h"
#include "Base.h"
#include "BaseFacility.h"
#include "ItemContainer.h"
#include "Ufo.h"
#include "Craft.h"
#include "AlienBase.h"
#include "Waypoint.h"
#include "TerrorSite.h"
#include "CraftWeapon.h"
#include "Transfer.h"
#include "Vehicle.h"
#include "AlienStrategy.h"
#include "AlienMission.h"
#include "../Ruleset/RuleResearch.h"
#include "../Ruleset/ArticleDefinition.h"
#include "ResearchProject.h"
#include "../Ruleset/RuleManufacture.h"
#include "Production.h"

namespace OpenXcom
{

enum TargetType { TARGET_NONE, TARGET_UFO, TARGET_CRAFT, TARGET_XBASE, TARGET_ABASE, TARGET_CRASH, TARGET_LANDED, TARGET_WAYPOINT, TARGET_TERROR };
const char *xcomAltitudes[] = { "STR_GROUND", "STR_VERY_LOW", "STR_LOW", "STR_HIGH", "STR_VERY_HIGH" };
const char *xcomStatus[] = { "STR_READY", "STR_OUT", "STR_REPAIRS", "STR_REFUELLING", "STR_REARMING" };

// Helper functions
template <typename T> T load(char* data) { return *(T*)data; }
template <> Uint16 load(char* data) { return SDL_SwapLE16(*(Uint16*)data); }
template <> Sint16 load(char* data) { return SDL_SwapLE16(*(Sint16*)data); }
template <> int load(char* data) { return SDL_SwapLE32(*(int*)data); }
template <> unsigned int load(char* data) { return SDL_SwapLE32(*(unsigned int*)data); }
template <> std::string load(char* data) { return data; }
template <> std::wstring load(char* data) { return Language::utf8ToWstr(data); }

#define SET_ID(a, b, n) { b = a; *n = sizeof(a)/sizeof(a[0]); }

char *SaveConverter::binaryBuffer(const std::string &filename, std::vector<char> &buffer)
{
	std::string s = _savePath + "/" + filename;
	std::ifstream datFile(s.c_str(), std::ios::in | std::ios::binary);
	if (!datFile)
	{
		throw Exception(filename + " not found");
	}
	buffer = std::vector<char>((std::istreambuf_iterator<char>(datFile)), (std::istreambuf_iterator<char>()));
	datFile.close();
	return &buffer[0];
}

/**
 * Creates a new converter for the given save folder.
 * @param save Number of the save folder GAME_#
 * @param rule Ruleset to associate with this save.
 */
SaveConverter::SaveConverter(int save, Ruleset *rule) : _rule(rule)
{
	std::stringstream ssFolder, ssPath;
	ssFolder << "GAME_" << save;
	ssPath << Options::getUserFolder() << "/" << ssFolder.str();
	_saveName = ssFolder.str();
	_savePath = ssPath.str();
	ssPath << "/SAVEINFO.DAT";
	if (!CrossPlatform::fileExists(ssPath.str()))
	{
		throw Exception(_saveName + " is not a valid save folder");
	}
}

SaveConverter::~SaveConverter()
{
}

/**
 * Gets all the info of the saves found in the user folder.
 * @param lang Loaded language.
 * @param info Returned list of saves info.
 */
void SaveConverter::getList(Language *lang, SaveOriginal info[NUM_SAVES])
{
	for (int i = 0; i < NUM_SAVES; ++i)
	{
		SaveOriginal save;
		save.id = 0;

		int id = i + 1;
		std::stringstream ss;
		ss << Options::getUserFolder() << "/GAME_" << id << "/SAVEINFO.DAT";
		std::ifstream datFile(ss.str().c_str(), std::ios::in | std::ios::binary);
		if (datFile)
		{
			std::vector<char> buffer((std::istreambuf_iterator<char>(datFile)), (std::istreambuf_iterator<char>()));
			char *data = &buffer[0];

			std::wstring name = load<std::wstring>(data + 0x02);
			int year = load<Uint16>(data + 0x1C);
			int month = load<Uint16>(data + 0x1E);
			int day = load<Uint16>(data + 0x20);
			int hour = load<Uint16>(data + 0x22);
			int minute = load<Uint16>(data + 0x24);
			bool tactical = load<char>(data + 0x26) != 0;

			GameTime time = GameTime(0, day, month + 1, year, hour, minute, 0);

			std::wstringstream ssDate, ssTime;
			ssDate << time.getDayString(lang) << L"  " << lang->getString(time.getMonthString()) << L"  " << time.getYear();
			ssTime << time.getHour() << L":" << std::setfill(L'0') << std::setw(2) << time.getMinute();

			save.id = id;
			save.name = name;
			save.date = ssDate.str();
			save.time = ssTime.str();
			save.tactical = tactical;

			datFile.close();
		}
		info[i] = save;
	}
}

/**
 * Converts an UFO: Enemy Unknown / X-COM: UFO Defense save into an OpenXcom save.
 * @return New OpenXcom save.
 */
SavedGame *SaveConverter::loadXcom1()
{
	SET_ID(xcom1Markers, _idMarkers, &_nMarkers);
	SET_ID(xcom1Countries, _idCountries, &_nCountries);
	SET_ID(xcom1Regions, _idRegions, &_nRegions);
	SET_ID(xcom1Facilities, _idFacilities, &_nFacilities);
	SET_ID(xcom1Items, _idItems, &_nItems);
	SET_ID(xcom1Races, _idRaces, &_nRaces);
	SET_ID(xcom1Crafts, _idCrafts, &_nCrafts);
	SET_ID(xcom1Ufos, _idUfos, &_nUfos);
	SET_ID(xcom1CraftWeapons, _idCraftWeapons, &_nCraftWeapons);
	SET_ID(xcom1Missions, _idMissions, &_nMissions);
	SET_ID(xcom1Armor, _idArmor, &_nArmor);
	SET_ID(xcom1LiveAliens, _idLiveAliens, &_nLiveAliens);
	SET_ID(xcom1LiveRanks, _idLiveRanks, &_nLiveRanks);
	SET_ID(xcom1Research, _idResearch, &_nResearch);
	SET_ID(xcom1Manufacture, _idManufacture, &_nManufacture);

	_save = new SavedGame();

	// Load globe data
	_save->getIncomes().clear();
	for (size_t i = 0; i < _nCountries; ++i)
	{
		Country *country = new Country(_rule->getCountry(_idCountries[i]));
		country->getActivityAlien().clear();
		country->getActivityXcom().clear();
		country->getFunding().clear();
		_save->getCountries()->push_back(country);
	}
	for (size_t i = 0; i < _nRegions; ++i)
	{
		Region *region = new Region(_rule->getRegion(_idRegions[i]));
		region->getActivityAlien().clear();
		region->getActivityXcom().clear();
		_save->getRegions()->push_back(region);
	}
	loadDatXcom();
	loadDatAlien();
	loadDatDiplom();
	loadDatLease();

	// Load graph data
	_save->getExpenditures().clear();
	_save->getMaintenances().clear();
	_save->getFundsList().clear();
	_save->getResearchScores().clear();
	loadDatLIGlob();
	loadDatUIGlob();
	loadDatIGlob();

	// Load alien data
	loadDatZonal();
	loadDatActs();
	loadDatMissions();

	// Load player data
	loadDatLoc();
	loadDatBase();
	loadDatAStore();
	loadDatCraft();
	loadDatSoldier();
	loadDatTransfer();
	loadDatResearch();
	loadDatUp();
	loadDatProject();
	loadDatBProd();
	loadDatXBases();

	return _save;
}

/**
 * Corrects vectors of graph data.
 * Original X-COM uses months as array indexes,
 * while OpenXcom stores month data in a linear fashion.
 * @param vector Vector of graph data.
 * @param month Current month.
 * @param year Has game gone longer than a year?
 */
template <typename T>
void SaveConverter::graphVector(std::vector<T> &vector, int month, bool year)
{
	if (year)
	{
		std::vector<T> newVector;
		int i = month;
		do 
		{
			newVector.push_back(vector[i]);
			i = (i + 1) % vector.size();
		}
		while (i != month);
		vector = newVector;
	}
	else
	{
		vector.resize(month);
	}
}

/**
 * Loads the IGLOB.DAT file.
 * Contains game date, time and difficulty.
 */
void SaveConverter::loadDatIGlob()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("IGLOB.DAT", buffer);

	int month = load<int>(data + 0x00) + 1;
	int weekday = load<int>(data + 0x04) + 1;
	int day = load<int>(data + 0x08);
	int hour = load<int>(data + 0x0C);
	int minute = load<int>(data + 0x10);
	int second = load<int>(data + 0x14);
	_save->setTime(GameTime(weekday, day, month, _year, hour, minute, second));

	int difficulty = load<int>(data + 0x3C);
	_save->setDifficulty((GameDifficulty)difficulty);

	// Fix up the months
	int monthsPassed = month + (_year - _rule->getStartingTime().getYear()) * 12;
	for (int i = 0; i < monthsPassed - 1; ++i)
	{
		_save->addMonth();
	}
	graphVector(_save->getIncomes(), month, _year != _rule->getStartingTime().getYear());
	graphVector(_save->getExpenditures(), month, _year != _rule->getStartingTime().getYear());
	graphVector(_save->getMaintenances(), month, _year != _rule->getStartingTime().getYear());
	graphVector(_save->getFundsList(), month, _year != _rule->getStartingTime().getYear());
	graphVector(_save->getResearchScores(), month, _year != _rule->getStartingTime().getYear());
	for (size_t i = 0; i < _nCountries; ++i)
	{
		Country *country = _save->getCountries()->at(i);
		graphVector(country->getActivityAlien(), month, _year != _rule->getStartingTime().getYear());
		graphVector(country->getActivityXcom(), month, _year != _rule->getStartingTime().getYear());
		graphVector(country->getFunding(), month, _year != _rule->getStartingTime().getYear());
	}
	for (size_t i = 0; i < _nRegions; ++i)
	{
		Region *region = _save->getRegions()->at(i);
		graphVector(region->getActivityAlien(), month, _year != _rule->getStartingTime().getYear());
		graphVector(region->getActivityXcom(), month, _year != _rule->getStartingTime().getYear());
	}
	_save->getFundsList().back() = _funds;
}

/**
 * Loads the LIGLOB.DAT file.
 * Contains financial data.
 */
void SaveConverter::loadDatLIGlob()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("LIGLOB.DAT", buffer);

	// 12 months of data - 0x04 * 12 = 0x30
	for (size_t i = 0x04; i < 0x04 + 0x30; i += 4)
	{
		int expenditure = load<int>(data + i);
		_save->getExpenditures().push_back(expenditure);
	}
	for (size_t i = 0x34; i < 0x34 + 0x30; i += 4)
	{
		int maintenance = load<int>(data + i);
		_save->getMaintenances().push_back(maintenance);
	}
	for (size_t i = 0x64; i < 0x64 + 0x30; i += 4)
	{
		int balance = load<int>(data + i);
		_save->getFundsList().push_back(balance);
	}

	_funds = load<int>(data + 0x00);
}

/**
 * Loads the UIGLOB.DAT file.
 * Contains Geoscape number IDs and scores.
 */
void SaveConverter::loadDatUIGlob()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("UIGLOB.DAT", buffer);

	std::map<std::string, int> ids;
	// IDs are 2 bytes each
	for (size_t i = 0; i < _nMarkers; ++i)
	{
		ids[_idMarkers[i]] = load<Uint16>(data + i * 2);
	}
	ids["STR_CRASH_SITE"] = ids["STR_LANDING_SITE"] = ids["STR_UFO"];
	_save->setIds(ids);

	_year = load<Uint16>(data + 0x16);

	// 12 months of data - 0x04 * 12 = 0x30
	for (int i = 0x18; i < 0x18 + 0x30; i += 2)
	{
		int score = load<Sint16>(data + i);
		_save->getResearchScores().push_back(score);
	}
}

/**
 * Loads the LEASE.DAT file.
 * Contains globe camera settings.
 */
void SaveConverter::loadDatLease()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("LEASE.DAT", buffer);

	double lat = -load<Sint16>(data + 0x00) * 0.125 * M_PI / 180;
	double lon = -load<Sint16>(data + 0x06) * 0.125 * M_PI / 180;
	int zoom = load<Sint16>(data + 0x0C);

	_save->setGlobeLongitude(lon);
	_save->setGlobeLatitude(lat);
	switch (zoom)
	{
	case 90: zoom = 0; break;
	case 120: zoom = 1; break;
	case 180: zoom = 2; break;
	case 360: zoom = 3; break;
	case 450: zoom = 4; break;
	case 720: zoom = 5; break;
	}
	_save->setGlobeZoom(zoom);
}

/**
 * Loads the XCOM.DAT file.
 * Contains X-COM graph info.
 */
void SaveConverter::loadDatXcom()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("XCOM.DAT", buffer);

	// 12 months of data
	size_t n = _nCountries + _nRegions;
	for (size_t i = 0; i < n * 12; ++i)
	{
		int score = load<int>(data + i * 4);
		size_t j = i % n;
		// country
		if (j < _nCountries)
		{
			_save->getCountries()->at(j)->getActivityXcom().push_back(score);
		}
		// region
		else
		{
			j -= _nCountries;
			_save->getRegions()->at(j)->getActivityXcom().push_back(score);
		}
	}
}

/**
 * Loads the ALIEN.DAT file.
 * Contains Alien graph info.
 */
void SaveConverter::loadDatAlien()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("ALIEN.DAT", buffer);

	// 12 months of data
	size_t n = _nCountries + _nRegions;
	for (size_t i = 0; i < n * 12; ++i)
	{
		int score = load<int>(data + i * 4);
		size_t j = i % n;
		// country
		if (j < _nCountries)
		{
			_save->getCountries()->at(j)->getActivityAlien().push_back(score);
		}
		// region
		else
		{
			j -= _nCountries;
			_save->getRegions()->at(j)->getActivityAlien().push_back(score);
		}
	}
}

/**
 * Loads the DIPLOM.DAT file.
 * Contains country status.
 */
void SaveConverter::loadDatDiplom()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("DIPLOM.DAT", buffer);

	std::vector<int64_t> income;
	for (size_t i = 0; i < 12; ++i)
	{
		income.push_back(0);
	}

	// each country is 36 bytes
	for (size_t i = 0; i < _nCountries; ++i)
	{
		char *cdata = (data + i * 36);
		Country *country = _save->getCountries()->at(i);

		int satisfaction = load<Sint16>(cdata + 0x02);
		for (size_t j = 0; j < 12; ++j)
		{
			int funding = load<Sint16>(cdata + 0x04 + j * 2);
			funding *= 1000;
			income[j] += funding;
			country->getFunding().push_back(funding);
		}
		bool pact = satisfaction == 0;
		bool newPact = load<Sint16>(cdata + 0x1E) != 0;
		
		if (pact)
			country->setPact();
		if (newPact)
			country->setNewPact();
	}
	_save->getIncomes() = income;
}

/**
 * Loads the ZONAL.DAT file.
 * Contains alien region chances.
 */
void SaveConverter::loadDatZonal()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("ZONAL.DAT", buffer);

	std::map<std::string, int> chances;
	const size_t nRegions = 12;
	for (size_t i = 0; i < nRegions; ++i)
	{
		chances[_idRegions[i]] = load<Uint8>(data + i);
	}
	YAML::Node node;
	node["regions"] = chances;
	_save->getAlienStrategy().load(_rule, node);
}

/**
 * Loads the ACTS.DAT file.
 * Contains alien mission chances.
 */
void SaveConverter::loadDatActs()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("ACTS.DAT", buffer);

	std::map < std::string, std::map<std::string, int> > chances;
	const size_t nRegions = 12;
	const size_t nMissions = 7;
	for (size_t i = 0; i < nRegions * nMissions; ++i)
	{
		size_t mission = i % nMissions;
		size_t region = i / nMissions;

		chances[_idRegions[region]][_idMissions[mission]] = load<Uint8>(data + i);
	}

	YAML::Node node;
	for (std::map < std::string, std::map<std::string, int> >::iterator i = chances.begin(); i != chances.end(); ++i)
	{
		YAML::Node subnode;
		subnode["region"] = i->first;
		subnode["missions"] = i->second;
		node["possibleMissions"].push_back(subnode);
	}
	_save->getAlienStrategy().load(_rule, node);
}

/**
 * Loads the MISSIONS.DAT file.
 * Contains ongoing alien missions.
 */
void SaveConverter::loadDatMissions()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("MISSIONS.DAT", buffer);
	const int nRegions = 12;
	const int nMissions = 7;
	for (size_t i = 0; i < nRegions * nMissions; ++i)
	{
		char *mdata = (data + i * 8);
		int wave = load<Uint16>(mdata + 0x00);
		if (wave != 0xFFFF)
		{			int ufoCounter = load<Uint16>(mdata + 0x02);
			int spawn = load<Uint16>(mdata + 0x04);
			int race = load<Uint16>(mdata + 0x06);
			int mission = i % nMissions;
			int region = i / nMissions;

			YAML::Node node;
			AlienMission *m = new AlienMission(*_rule->getAlienMission(_idMissions[mission]));
			node["region"] = _idRegions[region];
			node["race"] = _idRaces[race];
			node["nextWave"] = wave * 30;
			node["nextUfoCounter"] = ufoCounter;
			node["spawnCountdown"] = spawn;
			node["uniqueID"] = _save->getId("ALIEN_MISSIONS");
			m->load(node, *_save);
			_save->getAlienMissions().push_back(m);
			_missions[std::make_pair(mission, region)] = m;
		}
	}
}

/**
 * Loads the LOC.DAT file.
 * Contains globe markers.
 */
void SaveConverter::loadDatLoc()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("LOC.DAT", buffer);

	// 50 records - 20 bytes each
	for (size_t i = 0; i < 50; ++i)
	{
		char *tdata = (data + i * 20);
		TargetType type = (TargetType)load<Uint8>(tdata);

		int dat = load<Uint8>(tdata + 0x01);
		double lon = load<Sint16>(tdata + 0x02) * 0.125 * M_PI / 180;
		double lat = load<Sint16>(tdata + 0x04) * 0.125 * M_PI / 180;
		int timer = load<Sint16>(tdata + 0x06);
		int id = load<Sint16>(tdata + 0x0A);
		std::bitset<3> visibility(load<int>(tdata + 0x10));

		// can't declare variables in switches :(
		Target *target = 0;
		Ufo *ufo = 0;
		Craft *craft = 0;
		Base *xbase = 0;
		AlienBase *abase = 0;
		Waypoint *waypoint = 0;
		TerrorSite *terror = 0;
		switch (type)
		{
		case TARGET_NONE:
			target = 0;
			break;
		case TARGET_UFO:
		case TARGET_CRASH:
		case TARGET_LANDED:
			ufo = new Ufo(_rule->getUfo(_idUfos[0]));
			ufo->setId(id);
			ufo->setCrashId(id);
			ufo->setLandId(id);
			ufo->setSecondsRemaining(timer);
			ufo->setDetected(!visibility.test(0));
			target = ufo;
			break;
		case TARGET_CRAFT:
			craft = new Craft(_rule->getCraft(_idCrafts[0]), 0, id);
			target = craft;
			break;
		case TARGET_XBASE:
			xbase = new Base(_rule);
			target = xbase;
			break;
		case TARGET_ABASE:
			abase = new AlienBase();
			abase->setId(id);
			abase->setAlienRace(_idRaces[dat]);
			abase->setDiscovered(!visibility.test(0));
			_save->getAlienBases()->push_back(abase);
			target = abase;
			break;
		case TARGET_WAYPOINT:
			waypoint = new Waypoint();
			waypoint->setId(id);
			_save->getWaypoints()->push_back(waypoint);
			target = waypoint;
			break;
		case TARGET_TERROR:
			terror = new TerrorSite();
			terror->setId(id);
			terror->setAlienRace(_idRaces[dat]);
			terror->setSecondsRemaining(timer * 3600);
			_save->getTerrorSites()->push_back(terror);
			target = terror;
			break;
		}
		if (target != 0)
		{
			target->setLongitude(lon);
			target->setLatitude(lat);
		}
		_targets.push_back(target);
		_targetDat.push_back(dat);
	}
}

/**
 * Loads the BASE.DAT file.
 * Contains X-COM base contents.
 */
void SaveConverter::loadDatBase()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("BASE.DAT", buffer);

	std::vector<Base*> bases(8, (Base*)0);
	for (size_t i = 0; i != _targets.size(); ++i)
	{
		Base *base = dynamic_cast<Base*>(_targets[i]);
		if (base != 0)
		{
			int j = _targetDat[i];
			char *bdata = (data + j * 0x124);
			std::wstring name = load<std::wstring>(bdata);
			// 36 facilities
			for (int j = 0; j < 36; ++j)
			{
				size_t facilityType = *(bdata + 0x16 + j);
				if (facilityType < _nFacilities)
				{
					BaseFacility *facility = new BaseFacility(_rule->getBaseFacility(_idFacilities[facilityType]), base);
					int x = j % 6;
					int y = j / 6;
					int days = load<Uint8>(bdata + 0x3A + j);
					facility->setX(x);
					facility->setY(y);
					facility->setBuildTime(days);
					base->getFacilities()->push_back(facility);
				}
			}
			int engineers = load<Uint8>(bdata + 0x5E);
			int scientists = load<Uint8>(bdata + 0x5F);
			// items
			for (int j = 0; j < _nItems; ++j)
			{
				int qty = load<Uint16>(bdata + 0x60 + j * 2);
				if (qty != 0 && _idItems[j] != 0)
				{
					base->getItems()->addItem(_idItems[j], qty);
				}
			}
			base->setEngineers(engineers);
			base->setScientists(scientists);
			base->setName(name);
			bases[j] = base;
		}
	}

	for (std::vector<Base*>::iterator i = bases.begin(); i != bases.end(); ++i)
	{
		if (*i != 0)
		{
			_save->getBases()->push_back(*i);
		}
	}
}

/**
 * Loads the ASTORE.DAT file.
 * Contains X-COM alien containment.
 */
void SaveConverter::loadDatAStore()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("ASTORE.DAT", buffer);

	// 50 entries - 12 bytes each
	for (size_t i = 0; i < 50; ++i)
	{
		char *adata = (data + i * 12);
		int race = load<Uint8>(adata + 0x00);
		std::string liveAlien;
		if (race != 0)
		{
			int rank = load<Uint8>(adata + 0x01);
			int base = load<Uint8>(adata + 0x02);
			liveAlien = _idLiveAliens[race];
			liveAlien += _idLiveRanks[rank];
			if (base != 0xFF)
			{
				Base *b = dynamic_cast<Base*>(_targets[base]);
				b->getItems()->addItem(liveAlien);
			}
		}
		_aliens.push_back(liveAlien);
	}
}

/**
 * Loads the TRANSFER.DAT file.
 * Contains transfers to X-COM bases.
 */
void SaveConverter::loadDatTransfer()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("TRANSFER.DAT", buffer);

	// 100 entries - 8 bytes each
	for (size_t i = 0; i < 100; ++i)
	{
		char *tdata = (data + i * 8);
		int qty = load<Uint8>(tdata + 0x06);
		if (qty != 0)
		{
			int base = load<Uint8>(tdata + 0x01);
			Base *b = dynamic_cast<Base*>(_targets[base]);
			int hours = load<Uint8>(tdata + 0x02);
			TransferType type = (TransferType)load<Uint8>(tdata + 0x03);
			int dat = load<Uint8>(tdata + 0x04);

			Transfer *transfer = new Transfer(hours);
			switch (type)
			{
			case TRANSFER_CRAFT:
				transfer->setCraft(dynamic_cast<Craft*>(_targets[dat]));
				break;
			case TRANSFER_SOLDIER:
				transfer->setSoldier(_soldiers[dat]);
				break;
			case TRANSFER_SCIENTIST:
				transfer->setScientists(qty);
				break;
			case TRANSFER_ENGINEER:
				transfer->setEngineers(qty);
				break;
			default:
				if (type == TRANSFER_ITEM)
					transfer->setItems(_idItems[dat], qty);
				else
					transfer->setItems(_aliens[dat]);
				break;
			}

			b->getTransfers()->push_back(transfer);
		}
	}
}


/**
 * Loads the CRAFT.DAT file.
 * Contains X-COM craft and Alien UFOs.
 */
void SaveConverter::loadDatCraft()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("CRAFT.DAT", buffer);

	for (size_t i = 0; i != _targets.size(); ++i)
	{
		int j = _targetDat[i];
		char *cdata = (data + j * 0x68);
		int type = load<Uint8>(cdata);
		if (type != 0xFF)
		{
			YAML::Node node;
			Craft *craft = dynamic_cast<Craft*>(_targets[i]);
			if (craft != 0)
			{
				craft->changeRules(_rule->getCraft(_idCrafts[type]));

				int lweapon = load<Uint8>(cdata + 0x01);
				int lammo = load<Uint16>(cdata + 0x02);
				if (lweapon != 0xFF)
				{
					CraftWeapon *cw = new CraftWeapon(_rule->getCraftWeapon(_idCraftWeapons[lweapon]), lammo);
					craft->getWeapons()->at(0) = cw;
				}
				int flight = load<Uint8>(cdata + 0x04);
				int rweapon = load<Uint8>(cdata + 0x05);
				int rammo = load<Uint8>(cdata + 0x06);
				if (rweapon != 0xFF)
				{
					CraftWeapon *cw = new CraftWeapon(_rule->getCraftWeapon(_idCraftWeapons[rweapon]), rammo);
					craft->getWeapons()->at(1) = cw;
				}

				node["damage"] = (int)load<Uint16>(cdata + 0x0A);
				node["speed"] = (int)load<Uint16>(cdata + 0x0E);
				int dest = load<Uint16>(cdata + 0x10);
				node["fuel"] = (int)load<Uint16>(cdata + 0x18);
				int base = load<Uint16>(cdata + 0x1A);
				node["status"] = xcomStatus[load<Uint16>(cdata + 0x2A)];

				// vehicles
				for (size_t j = 0; j < 5; ++j)
				{
					int qty = load<Uint8>(cdata + 0x2C + j);
					for (int v = 0; v < qty; ++v)
					{
						RuleItem *rule = _rule->getItem(_idItems[j + 10]);
						craft->getVehicles()->push_back(new Vehicle(rule, rule->getClipSize(), 4));
					}
				}
				// items
				for (size_t j = 5; j < 55; ++j)
				{
					int qty = load<Uint8>(cdata + 0x2C + j);
					if (qty != 0 && _idItems[j + 10] != 0)
					{
						craft->getItems()->addItem(_idItems[j + 10], qty);
					}
				}

				std::bitset<7> status(load<int>(cdata + 0x64));
				node["lowFuel"] = status.test(1);

				craft->load(node, _rule, _save);

				if (flight != 0 && dest != 0xFF)
				{
					Target *t = _targets[dest];
					craft->setDestination(t);
				}
				if (base != 0xFF)
				{
					Base *b = dynamic_cast<Base*>(_targets[base]);
					craft->setBase(b, false);
					b->getCrafts()->push_back(craft);
				}
			}
			Ufo *ufo = dynamic_cast<Ufo*>(_targets[i]);
			if (ufo != 0)
			{
				ufo->changeRules(_rule->getUfo(_idUfos[type - 5]));
				node["damage"] = (int)load<Uint16>(cdata + 0x0A);
				node["altitude"] = xcomAltitudes[load<Uint16>(cdata + 0x0C)];
				node["speed"] = (int)load<Uint16>(cdata + 0x0E);
				node["dest"]["lon"] = load<Sint16>(cdata + 0x14) * 0.125 * M_PI / 180;
				node["dest"]["lat"] = load<Sint16>(cdata + 0x16) * 0.125 * M_PI / 180;

				int mission = load<Uint16>(cdata + 0x1C);
				int region = load<Uint16>(cdata + 0x1E);
				AlienMission *m = _missions[std::make_pair(mission, region)];
				node["mission"] = m->getId();
				m->increaseLiveUfos();
				std::stringstream trajectory;
				trajectory << "P" << load<Uint16>(cdata + 0x22);
				node["trajectory"] = trajectory.str();
				node["trajectoryPoint"] = (int)load<Uint16>(cdata + 0x20);
				std::bitset<7> status(load<int>(cdata + 0x64));
				node["hyperDetected"] = status.test(6);

				ufo->load(node, *_rule, *_save);
				ufo->setSpeed(ufo->getSpeed());
				if (ufo->getStatus() == Ufo::CRASHED)
				{
					ufo->setSecondsRemaining(ufo->getSecondsRemaining() * 3600);
				}
				else if (ufo->getStatus() == Ufo::LANDED)
				{
					ufo->setSecondsRemaining(ufo->getSecondsRemaining() * 5);
				}
				else
				{
					ufo->setSecondsRemaining(0);
				}

				_save->getUfos()->push_back(ufo);
			}
		}
	}
}

/**
 * Loads the SOLDIER.DAT file.
 * Contains X-COM soldiers.
 */
void SaveConverter::loadDatSoldier()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("SOLDIER.DAT", buffer);

	// 250 soldier records - 68 bytes each
	for (size_t i = 0; i < 250; ++i)
	{
		char *sdata = (data + i * 68);
		int rank = load<Uint16>(sdata);
		if (rank != 0xFFFF)
		{
			YAML::Node node;
			int base = load<Uint16>(sdata + 0x02);
			int craft = load<Uint16>(sdata + 0x04);
			node["missions"] = (int)load<Sint16>(sdata + 0x08);
			node["kills"] = (int)load<Sint16>(sdata + 0x0A);
			node["recovery"] = (int)load<Sint16>(sdata + 0x0C);
			node["name"] = load<std::string>(sdata + 0x10);
			node["rank"] = rank;

			UnitStats initial;
			initial.tu = load<Uint8>(sdata + 0x2A);
			initial.health = load<Uint8>(sdata + 0x2B);
			initial.stamina = load<Uint8>(sdata + 0x2C);
			initial.reactions = load<Uint8>(sdata + 0x2D);
			initial.strength = load<Uint8>(sdata + 0x2E);
			initial.firing = load<Uint8>(sdata + 0x2F);
			initial.throwing = load<Uint8>(sdata + 0x30);
			initial.melee = load<Uint8>(sdata + 0x31);
			initial.psiStrength = load<Uint8>(sdata + 0x32);
			initial.psiSkill = load<Uint8>(sdata + 0x33);
			initial.bravery = 110 - (10 * load<Uint8>(sdata + 0x34));
			node["initialStats"] = initial;

			UnitStats current;
			current.tu = load<Uint8>(sdata + 0x35);
			current.health = load<Uint8>(sdata + 0x36);
			current.stamina = load<Uint8>(sdata + 0x37);
			current.reactions = load<Uint8>(sdata + 0x38);
			current.strength = load<Uint8>(sdata + 0x39);
			current.firing = load<Uint8>(sdata + 0x3A);
			current.throwing = load<Uint8>(sdata + 0x3B);
			current.melee = load<Uint8>(sdata + 0x3C);
			current.psiStrength = 0;
			current.psiSkill = 0;
			current.bravery = 10 * load<Uint8>(sdata + 0x3D);
			current += initial;
			node["currentStats"] = current;

			int armor = load<Uint8>(sdata + 0x3E);
			node["armor"] = _idArmor[armor];
			node["improvement"] = (int)load<Uint8>(sdata + 0x3F);
			node["psiTraining"] = (int)load<char>(sdata + 0x40) != 0;
			node["gender"] = (int)load<Uint8>(sdata + 0x42);
			node["look"] = (int)load<Uint8>(sdata + 0x43);
			node["id"] = _save->getId("STR_SOLDIER");

			Soldier *soldier = new Soldier(_rule->getSoldier("XCOM"), _rule->getArmor("STR_NONE_UC"));
			soldier->load(node, _rule, _save);
			if (base != 0xFFFF)
			{
				Base *b = dynamic_cast<Base*>(_targets[base]);
				b->getSoldiers()->push_back(soldier);
			}
			if (craft != 0xFFFF)
			{
				Craft *c = dynamic_cast<Craft*>(_targets[craft]);
				soldier->setCraft(c);
			}
			_soldiers.push_back(soldier);
		}
		else
		{
			_soldiers.push_back(0);
		}
	}
}

/**
 * Loads the RESEARCH.DAT file.
 * Contains X-COM research progress.
 */
void SaveConverter::loadDatResearch()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("RESEARCH.DAT", buffer);

	// 22 bytes for each entry
	for (size_t i = 0; i < _nResearch; ++i)
	{
		char *rdata = (data + i * 22);
		if (_idResearch[i] != 0)
		{
			RuleResearch *research = _rule->getResearch(_idResearch[i]);
			if (research != 0)
			{
				bool discovered = load<Uint8>(rdata + 0x0A) != 0;
				bool popped = load<Uint8>(rdata + 0x12) != 0;
				if (discovered)
				{
					_save->addFinishedResearch(research, _rule, false);
				}
				else if (popped)
				{
					_save->addPoppedResearch(research);
				}
			}
		}
	}
}

/**
 * Loads the UP.DAT file.
 * Contains X-COM Ufopaedia progress.
 */
void SaveConverter::loadDatUp()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("UP.DAT", buffer);

	// 12 bytes for each entry
	for (size_t i = 0; i < _rule->getUfopaediaList().size(); ++i)
	{
		char *rdata = (data + i * 12);
		ArticleDefinition *article = _rule->getUfopaediaArticle(_rule->getUfopaediaList().at(i));
		if (article != 0)
		{
			bool discovered = load<Uint8>(rdata + 0x08) == 2;
			if (discovered)
			{
				std::vector<std::string> requires = article->requires;
				for (std::vector<std::string>::const_iterator r = requires.begin(); r != requires.end(); ++r)
				{
					RuleResearch *research = _rule->getResearch(*r);
					if (research->getCost() == 0)
					{
						_save->addFinishedResearch(research, _rule);
					}
				}
			}
		}
	}
}

/**
 * Loads the PROJECT.DAT file.
 * Contains X-COM research projects.
 */
void SaveConverter::loadDatProject()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("PROJECT.DAT", buffer);
	// 8 bases - 288 bytes each
	for (size_t i = 0; i < _save->getBases()->size(); ++i)
	{
		Base *base = _save->getBases()->at(i);
		for (size_t j = 0; j < _nResearch; ++j)
		{
			char *pdata = (data + i * 288);
			int remaining = load<Uint16>(pdata + j * 2);
			int scientists = load<Uint8>(pdata + 0xC0 + j);
			if (remaining != 0 && _idResearch[j] != 0)
			{
				RuleResearch *research = _rule->getResearch(_idResearch[j]);
				if (research != 0)
				{
					ResearchProject *project = new ResearchProject(research, research->getCost());
					project->setAssigned(scientists);
					project->setSpent(project->getCost() - remaining);
					base->addResearch(project);
					base->setScientists(base->getScientists() - scientists);
				}
			}
		}
	}
}

/**
 * Loads the BPROD.DAT file.
 * Contains X-COM manufacture projects.
 */
void SaveConverter::loadDatBProd()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("BPROD.DAT", buffer);
	// 8 bases - 350 bytes each
	for (size_t i = 0; i < _save->getBases()->size(); ++i)
	{
		Base *base = _save->getBases()->at(i);
		for (size_t j = 0; j < _nManufacture; ++j)
		{
			char *pdata = (data + i * 350);
			int remaining = load<int>(pdata + j * 4);
			int engineers = load<Uint16>(pdata + 0x8C + j * 2);
			int total = load<Uint16>(pdata + 0xD2 + j * 2);
			int produced = load<Uint16>(pdata + 0x118 + j * 2);
			if (remaining != 0 && _idManufacture[j] != 0)
			{
				RuleManufacture *manufacture = _rule->getManufacture(_idManufacture[j]);
				if (manufacture != 0)
				{
					Production *project = new Production(manufacture, total);
					project->setAssignedEngineers(engineers);
					project->setTimeSpent(produced * manufacture->getManufactureTime() + manufacture->getManufactureTime() - remaining);
					base->addProduction(project);
					base->setEngineers(base->getEngineers() - engineers);
				}
			}
		}
	}
}

/**
 * Loads the XBASES.DAT file.
 * Contains targeted X-COM bases.
 */
void SaveConverter::loadDatXBases()
{
	std::vector<char> buffer;
	char *data = binaryBuffer("XBASES.DAT", buffer);
	const size_t nRegions = 12;
	for (size_t i = 0; i < nRegions; ++i)
	{
		char *bdata = (data + i * 4);
		bool detected = load<Uint16>(bdata + 0x00) != 0;
		if (detected)
		{
			int loc = load<Uint16>(bdata + 0x02);
			Base *base = dynamic_cast<Base*>(_targets[loc]);
			if (base != 0)
			{
				base->setRetaliationTarget(true);
			}
		}
	}
	
}

}
