/*
 * Copyright 2010 Daniel Albano
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
#include "XcomRuleset.h"

XcomRuleset::XcomRuleset() : Ruleset()
{
}

XcomRuleset::~XcomRuleset()
{
}

SavedGame *XcomRuleset::newSave(GameDifficulty diff)
{
	SavedGame *save = new SavedGame(diff);

	save->getCountries()->insert(pair<LangString, Country*>(STR_USA, new Country(SavedGame::genRandom(600, 1200)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_RUSSIA, new Country(SavedGame::genRandom(230, 460)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_UK, new Country(SavedGame::genRandom(240, 480)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_FRANCE, new Country(SavedGame::genRandom(320, 640)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_GERMANY, new Country(SavedGame::genRandom(250, 500)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_ITALY, new Country(SavedGame::genRandom(160, 320)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_SPAIN, new Country(SavedGame::genRandom(140, 280)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_CHINA, new Country(SavedGame::genRandom(245, 490)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_JAPAN, new Country(SavedGame::genRandom(400, 800)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_INDIA, new Country(SavedGame::genRandom(150, 300)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_BRAZIL, new Country(SavedGame::genRandom(300, 600)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_AUSTRALIA, new Country(SavedGame::genRandom(280, 560)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_NIGERIA, new Country(SavedGame::genRandom(180, 360)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_SOUTH_AFRICA, new Country(SavedGame::genRandom(310, 620)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_EGYPT, new Country(SavedGame::genRandom(160, 320)*1000)));
	save->getCountries()->insert(pair<LangString, Country*>(STR_CANADA, new Country(SavedGame::genRandom(110, 220)*1000)));
	save->setFunds(save->getCountryFunding());
	
	Base *base = new Base(0.0, 0.0);
	base->setName("X-COM BASE 1");
	save->getBases()->push_back(base);
	
	return save;
}