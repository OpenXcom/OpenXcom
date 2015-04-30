/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_SAVECONVERTER_H
#define OPENXCOM_SAVECONVERTER_H

#include <string>
#include <vector>
#include <map>
#include <utility>

namespace OpenXcom
{

/**
 * Container for savegame info displayed on listings.
 */
struct SaveOriginal
{
	int id;
	std::wstring name, date, time;
	bool tactical;
};

class SavedGame;
class Ruleset;
class Language;
class Target;
class Soldier;
class AlienMission;

/**
 * Handles conversion operations for original X-COM savegames.
 * @sa http://ufopaedia.org/index.php?title=Saved_Game_Files
 */
class SaveConverter
{
private:
	std::string _saveName, _savePath;
	std::vector<std::string> _idMarkers, _idCountries, _idRegions, _idFacilities, _idItems, _idCrews, _idCrafts, _idUfos, _idCraftWeapons, _idMissions, _idArmor, _idAlienRaces, _idAlienRanks, _idResearch, _idManufacture, _idUfopaedia;
	SavedGame *_save;
	Ruleset *_rule;
	int _year, _funds;
	std::vector<Target*> _targets;
	std::vector<int> _targetDat;
	std::vector<Soldier*> _soldiers;
	std::vector<std::string> _aliens;
	std::map<std::pair<int, int>, AlienMission*> _missions;

	/// Loads a binary file into a vector.
	char *binaryBuffer(const std::string &filename, std::vector<char> &buffer);
	/// Corrects a vector of graph data.
	template <typename T> void graphVector(std::vector<T> &vector, int month, bool year);

	/// Load XCOM.DAT
	void loadDatXcom();
	/// Load ALIEN.DAT
	void loadDatAlien();
	/// Load DIPLOM.DAT
	void loadDatDiplom();
	/// Load LEASE.DAT
	void loadDatLease();
	/// Load LIGLOB.DAT
	void loadDatLIGlob();
	/// Load UIGLOB.DAT
	void loadDatUIGlob();
	/// Load IGLOB.DAT
	void loadDatIGlob();
	/// Load ZONAL.DAT
	void loadDatZonal();
	/// Load ACTS.DAT
	void loadDatActs();
	/// Load MISSIONS.DAT
	void loadDatMissions();
	/// Load LOC.DAT
	void loadDatLoc();
	/// Load BASE.DAT
	void loadDatBase();
	/// Load ASTORE.DAT
	void loadDatAStore();
	/// Load CRAFT.DAT
	void loadDatCraft();
	/// Load SOLDIER.DAT
	void loadDatSoldier();
	/// Load TRANSFER.DAT
	void loadDatTransfer();
	/// Load RESEARCH.DAT
	void loadDatResearch();
	/// Load UP.DAT
	void loadDatUp();
	/// Load PROJECT.DAT
	void loadDatProject();
	/// Load BPROD.DAT
	void loadDatBProd();
	/// Load XBASES.DAT
	void loadDatXBases();
public:
	static const int NUM_SAVES = 10;

	/// Creates a converter for the specified save.
	SaveConverter(int save, Ruleset *rule);
	/// Cleans up the converter.
	~SaveConverter();
	/// Gets list of saves in the user directory.
	static void getList(Language *lang, SaveOriginal info[NUM_SAVES]);
	/// Loads an original X-COM save.
	SavedGame *loadOriginal();
};

}

#endif
