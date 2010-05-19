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

/**
 * Initializes the X-Com ruleset with all the rules
 * mimicking the original game.
 */
XcomRuleset::XcomRuleset() : Ruleset()
{
	SoldierNamePool *american = new SoldierNamePool();
	american->addMaleName("Austin");
	american->addMaleName("Calvin");
	american->addMaleName("Carl");
	american->addMaleName("Clarence");
	american->addMaleName("Donald");
	american->addMaleName("Dwight");
	american->addMaleName("Ed");
	american->addMaleName("Kevin");
	american->addMaleName("Lester");
	american->addMaleName("Mark");
	american->addMaleName("Oscar");
	american->addMaleName("Samuel");
	american->addMaleName("Spencer");
	american->addMaleName("Tom");
	american->addMaleName("Virgil");
	american->addFemaleName("Barbara");
	american->addFemaleName("Catherine");
	american->addFemaleName("Evelyn");
	american->addFemaleName("Patricia");
	american->addFemaleName("Sigourney");
	american->addLastName("Bradley");
	american->addLastName("Bryant");
	american->addLastName("Carr");
	american->addLastName("Crossett");
	american->addLastName("Dodge");
	american->addLastName("Gallagher");
	american->addLastName("Homburger");
	american->addLastName("Horton");
	american->addLastName("Hudson");
	american->addLastName("Johnson");
	american->addLastName("Kemp");
	american->addLastName("King");
	american->addLastName("McNeil");
	american->addLastName("Miller");
	american->addLastName("Mitchell");
	american->addLastName("Nash");
	american->addLastName("Stephens");
	american->addLastName("Stoddard");
	american->addLastName("Thompson");
	american->addLastName("Webb");
	_names.push_back(american);

	SoldierNamePool *british = new SoldierNamePool();
	british->addMaleName("Adam");
	british->addMaleName("Alan");
	british->addMaleName("Arthur");
	british->addMaleName("Brett");
	british->addMaleName("Damien");
	british->addMaleName("David");
	british->addMaleName("Frank");
	british->addMaleName("James");
	british->addMaleName("John");
	british->addMaleName("Michael");
	british->addMaleName("Neil");
	british->addMaleName("Patrick");
	british->addMaleName("Paul");
	british->addMaleName("Robert");
	british->addMaleName("Scott");
	british->addFemaleName("Andrea");
	british->addFemaleName("Helen");
	british->addFemaleName("Jane");
	british->addFemaleName("Maria");
	british->addFemaleName("Sarah");
	british->addLastName("Bailey");
	british->addLastName("Blake");
	british->addLastName("Davies");
	british->addLastName("Day");
	british->addLastName("Evans");
	british->addLastName("Hill");
	british->addLastName("Jones");
	british->addLastName("Jonlan");
	british->addLastName("Martin");
	british->addLastName("Parker");
	british->addLastName("Pearce");
	british->addLastName("Reynolds");
	british->addLastName("Robinson");
	british->addLastName("Sharpe");
	british->addLastName("Smith");
	british->addLastName("Stewart");
	british->addLastName("Taylor");
	british->addLastName("Watson");
	british->addLastName("White");
	british->addLastName("Wright");
	_names.push_back(british);

	SoldierNamePool *french = new SoldierNamePool();
	french->addMaleName("Armand");
	french->addMaleName("Bernard");
	french->addMaleName("Claude");
	french->addMaleName("Emile");
	french->addMaleName("Gaston");
	french->addMaleName("Gerard");
	french->addMaleName("Henri");
	french->addMaleName("Jacques");
	french->addMaleName("Jean");
	french->addMaleName("Leon");
	french->addMaleName("Louis");
	french->addMaleName("Marc");
	french->addMaleName("Marcel");
	french->addMaleName("Pierre");
	french->addMaleName("Rene");
	french->addFemaleName("Danielle");
	french->addFemaleName("Jacqueline");
	french->addFemaleName("Marielle");
	french->addFemaleName("Micheline");
	french->addFemaleName("Sylvie");
	french->addLastName("Bouissou");
	french->addLastName("Bouton");
	french->addLastName("Buchard");
	french->addLastName("Coicard");
	french->addLastName("Collignon");
	french->addLastName("Cuvelier");
	french->addLastName("Dagallier");
	french->addLastName("Dreyfus");
	french->addLastName("Dujardin");
	french->addLastName("Gaudin");
	french->addLastName("Gautier");
	french->addLastName("Gressier");
	french->addLastName("Guerin");
	french->addLastName("Laroyenne");
	french->addLastName("Lecointe");
	french->addLastName("Lefevre");
	french->addLastName("Luget");
	french->addLastName("Marcelle");
	french->addLastName("Pecheux");
	french->addLastName("Revenu");
	_names.push_back(french);

	SoldierNamePool *german = new SoldierNamePool();
	german->addMaleName("Dieter");
	german->addMaleName("Franz");
	german->addMaleName("Gerhard");
	german->addMaleName("Gunter");
	german->addMaleName("Hans");
	german->addMaleName("Jurgen");
	german->addMaleName("Klaus");
	german->addMaleName("Manfred");
	german->addMaleName("Matthias");
	german->addMaleName("Otto");
	german->addMaleName("Rudi");
	german->addMaleName("Siegfried");
	german->addMaleName("Stefan");
	german->addMaleName("Werner");
	german->addMaleName("Wolfgang");
	german->addFemaleName("Christel");
	german->addFemaleName("Gudrun");
	german->addFemaleName("Helga");
	german->addFemaleName("Karin");
	german->addFemaleName("Uta");
	german->addLastName("Berger");
	german->addLastName("Brehme");
	german->addLastName("Esser");
	german->addLastName("Faerber");
	german->addLastName("Geisler");
	german->addLastName("Gunkel");
	german->addLastName("Hafner");
	german->addLastName("Heinsch");
	german->addLastName("Keller");
	german->addLastName("Krause");
	german->addLastName("Mederow");
	german->addLastName("Meyer");
	german->addLastName("Richter");
	german->addLastName("Schultz");
	german->addLastName("Seidler");
	german->addLastName("Steinbach");
	german->addLastName("Ulbricht");
	german->addLastName("Unger");
	german->addLastName("Vogel");
	german->addLastName("Zander");
	_names.push_back(german);

	SoldierNamePool *japanese = new SoldierNamePool();
	japanese->addMaleName("Akinori");
	japanese->addMaleName("Isao");
	japanese->addMaleName("Jungo");
	japanese->addMaleName("Kenji");
	japanese->addMaleName("Masaharu");
	japanese->addMaleName("Masanori");
	japanese->addMaleName("Naohiro");
	japanese->addMaleName("Shigeo");
	japanese->addMaleName("Shigeru");
	japanese->addMaleName("Shuji");
	japanese->addMaleName("Tatsuo");
	japanese->addMaleName("Toshio");
	japanese->addMaleName("Yasuaki");
	japanese->addMaleName("Yataka");
	japanese->addMaleName("Yuzo");
	japanese->addFemaleName("Mariko");
	japanese->addFemaleName("Michiko");
	japanese->addFemaleName("Sata");
	japanese->addFemaleName("Sumie");
	japanese->addFemaleName("Yoko");
	japanese->addLastName("Akira");
	japanese->addLastName("Fujimoto");
	japanese->addLastName("Ishii");
	japanese->addLastName("Iwahara");
	japanese->addLastName("Iwasaki");
	japanese->addLastName("Kojima");
	japanese->addLastName("Koyama");
	japanese->addLastName("Matsumara");
	japanese->addLastName("Morita");
	japanese->addLastName("Noguchi");
	japanese->addLastName("Okabe");
	japanese->addLastName("Okamoto");
	japanese->addLastName("Sato");
	japanese->addLastName("Shimaoka");
	japanese->addLastName("Shoji");
	japanese->addLastName("Tanida");
	japanese->addLastName("Tanikawa");
	japanese->addLastName("Yamanaka");
	japanese->addLastName("Yamashita");
	japanese->addLastName("Yamazaki");
	_names.push_back(japanese);

	SoldierNamePool *russian = new SoldierNamePool();
	russian->addMaleName("Anatoly");
	russian->addMaleName("Andrei");
	russian->addMaleName("Boris");
	russian->addMaleName("Dmitriy");
	russian->addMaleName("Gennadi");
	russian->addMaleName("Grigoriy");
	russian->addMaleName("Igor");
	russian->addMaleName("Ivan");
	russian->addMaleName("Leonid");
	russian->addMaleName("Mikhail");
	russian->addMaleName("Nikolai");
	russian->addMaleName("Sergei");
	russian->addMaleName("Victor");
	russian->addMaleName("Vladimir");
	russian->addMaleName("Yuri");
	russian->addFemaleName("Astra");
	russian->addFemaleName("Galina");
	russian->addFemaleName("Lyudmila");
	russian->addFemaleName("Olga");
	russian->addFemaleName("Tatyana");
	russian->addLastName("Andianov");
	russian->addLastName("Belov");
	russian->addLastName("Chukarin");
	russian->addLastName("Gorokhova");
	russian->addLastName("Kolotov");
	russian->addLastName("Korkia");
	russian->addLastName("Likhachev");
	russian->addLastName("Maleev");
	russian->addLastName("Mikhailov");
	russian->addLastName("Petrov");
	russian->addLastName("Ragulin");
	russian->addLastName("Romanov");
	russian->addLastName("Samusenko");
	russian->addLastName("Scharov");
	russian->addLastName("Shadrin");
	russian->addLastName("Shalimov");
	russian->addLastName("Torban");
	russian->addLastName("Voronin");
	russian->addLastName("Yakubik");
	russian->addLastName("Zhdanovich");
	_names.push_back(russian);
}

/**
 *
 */
XcomRuleset::~XcomRuleset()
{
}

/**
 * Generates a brand new saved game with the default countries
 * and a base with all the starting equipment.
 * @param diff Difficulty for the save.
 * @return New saved game.
 */
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
	for (int i = 0; i < 8; i++)
	{
		base->getSoldiers()->push_back(new Soldier(&_names));
	}

	save->getBases()->push_back(base);
	
	return save;
}