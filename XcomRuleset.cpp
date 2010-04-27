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

int XcomRuleset::genFunding(int min, int max)
{
	return (rand() % (max - min) + min) * 1000;
}

SavedGame *XcomRuleset::newSave(GameDifficulty diff)
{
	SavedGame *save = new SavedGame(diff);
	save->setSeed(time(NULL));
	save->getCountries()->insert(pair<LangString, Country*>(STR_USA, new Country(genFunding(600, 1200))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_RUSSIA, new Country(genFunding(230, 460))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_UK, new Country(genFunding(240, 480))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_FRANCE, new Country(genFunding(320, 640))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_GERMANY, new Country(genFunding(250, 500))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_ITALY, new Country(genFunding(160, 320))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_SPAIN, new Country(genFunding(140, 280))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_CHINA, new Country(genFunding(245, 490))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_JAPAN, new Country(genFunding(400, 800))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_INDIA, new Country(genFunding(150, 300))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_BRAZIL, new Country(genFunding(300, 600))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_AUSTRALIA, new Country(genFunding(280, 560))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_NIGERIA, new Country(genFunding(180, 360))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_SOUTH_AFRICA, new Country(genFunding(310, 620))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_EGYPT, new Country(genFunding(160, 320))));
	save->getCountries()->insert(pair<LangString, Country*>(STR_CANADA, new Country(genFunding(110, 220))));
	save->setFunds(save->getCountryFunding());
	return save;
}