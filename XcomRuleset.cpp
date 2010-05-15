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
	SoldierNamePool *american = new SoldierNamePool();
	american->getMaleNames()->push_back("Austin");
	american->getMaleNames()->push_back("Calvin");
	american->getMaleNames()->push_back("Carl");
	american->getMaleNames()->push_back("Clarence");
	american->getMaleNames()->push_back("Donald");
	american->getMaleNames()->push_back("Dwight");
	american->getMaleNames()->push_back("Ed");
	american->getMaleNames()->push_back("Kevin");
	american->getMaleNames()->push_back("Lester");
	american->getMaleNames()->push_back("Mark");
	american->getMaleNames()->push_back("Oscar");
	american->getMaleNames()->push_back("Samuel");
	american->getMaleNames()->push_back("Spencer");
	american->getMaleNames()->push_back("Tom");
	american->getMaleNames()->push_back("Virgil");
	american->getFemaleNames()->push_back("Barbara");
	american->getFemaleNames()->push_back("Catherine");
	american->getFemaleNames()->push_back("Evelyn");
	american->getFemaleNames()->push_back("Patricia");
	american->getFemaleNames()->push_back("Sigourney");
	american->getLastNames()->push_back("Bradley");
	american->getLastNames()->push_back("Bryant");
	american->getLastNames()->push_back("Carr");
	american->getLastNames()->push_back("Crossett");
	american->getLastNames()->push_back("Dodge");
	american->getLastNames()->push_back("Gallagher");
	american->getLastNames()->push_back("Homburger");
	american->getLastNames()->push_back("Horton");
	american->getLastNames()->push_back("Hudson");
	american->getLastNames()->push_back("Johnson");
	american->getLastNames()->push_back("Kemp");
	american->getLastNames()->push_back("King");
	american->getLastNames()->push_back("McNeil");
	american->getLastNames()->push_back("Miller");
	american->getLastNames()->push_back("Mitchell");
	american->getLastNames()->push_back("Nash");
	american->getLastNames()->push_back("Stephens");
	american->getLastNames()->push_back("Stoddard");
	american->getLastNames()->push_back("Thompson");
	american->getLastNames()->push_back("Webb");
	_names.push_back(american);

	SoldierNamePool *british = new SoldierNamePool();
	british->getMaleNames()->push_back("Adam");
	british->getMaleNames()->push_back("Alan");
	british->getMaleNames()->push_back("Arthur");
	british->getMaleNames()->push_back("Brett");
	british->getMaleNames()->push_back("Damien");
	british->getMaleNames()->push_back("David");
	british->getMaleNames()->push_back("Frank");
	british->getMaleNames()->push_back("James");
	british->getMaleNames()->push_back("John");
	british->getMaleNames()->push_back("Michael");
	british->getMaleNames()->push_back("Neil");
	british->getMaleNames()->push_back("Patrick");
	british->getMaleNames()->push_back("Paul");
	british->getMaleNames()->push_back("Robert");
	british->getMaleNames()->push_back("Scott");
	british->getFemaleNames()->push_back("Andrea");
	british->getFemaleNames()->push_back("Helen");
	british->getFemaleNames()->push_back("Jane");
	british->getFemaleNames()->push_back("Maria");
	british->getFemaleNames()->push_back("Sarah");
	british->getLastNames()->push_back("Bailey");
	british->getLastNames()->push_back("Blake");
	british->getLastNames()->push_back("Davies");
	british->getLastNames()->push_back("Day");
	british->getLastNames()->push_back("Evans");
	british->getLastNames()->push_back("Hill");
	british->getLastNames()->push_back("Jones");
	british->getLastNames()->push_back("Jonlan");
	british->getLastNames()->push_back("Martin");
	british->getLastNames()->push_back("Parker");
	british->getLastNames()->push_back("Pearce");
	british->getLastNames()->push_back("Reynolds");
	british->getLastNames()->push_back("Robinson");
	british->getLastNames()->push_back("Sharpe");
	british->getLastNames()->push_back("Smith");
	british->getLastNames()->push_back("Stewart");
	british->getLastNames()->push_back("Taylor");
	british->getLastNames()->push_back("Watson");
	british->getLastNames()->push_back("White");
	british->getLastNames()->push_back("Wright");
	_names.push_back(british);

	SoldierNamePool *french = new SoldierNamePool();
	french->getMaleNames()->push_back("Armand");
	french->getMaleNames()->push_back("Bernard");
	french->getMaleNames()->push_back("Claude");
	french->getMaleNames()->push_back("Emile");
	french->getMaleNames()->push_back("Gaston");
	french->getMaleNames()->push_back("Gerard");
	french->getMaleNames()->push_back("Henri");
	french->getMaleNames()->push_back("Jacques");
	french->getMaleNames()->push_back("Jean");
	french->getMaleNames()->push_back("Leon");
	french->getMaleNames()->push_back("Louis");
	french->getMaleNames()->push_back("Marc");
	french->getMaleNames()->push_back("Marcel");
	french->getMaleNames()->push_back("Pierre");
	french->getMaleNames()->push_back("Rene");
	french->getFemaleNames()->push_back("Danielle");
	french->getFemaleNames()->push_back("Jacqueline");
	french->getFemaleNames()->push_back("Marielle");
	french->getFemaleNames()->push_back("Micheline");
	french->getFemaleNames()->push_back("Sylvie");
	french->getLastNames()->push_back("Bouissou");
	french->getLastNames()->push_back("Bouton");
	french->getLastNames()->push_back("Buchard");
	french->getLastNames()->push_back("Coicard");
	french->getLastNames()->push_back("Collignon");
	french->getLastNames()->push_back("Cuvelier");
	french->getLastNames()->push_back("Dagallier");
	french->getLastNames()->push_back("Dreyfus");
	french->getLastNames()->push_back("Dujardin");
	french->getLastNames()->push_back("Gaudin");
	french->getLastNames()->push_back("Gautier");
	french->getLastNames()->push_back("Gressier");
	french->getLastNames()->push_back("Guerin");
	french->getLastNames()->push_back("Laroyenne");
	french->getLastNames()->push_back("Lecointe");
	french->getLastNames()->push_back("Lefevre");
	french->getLastNames()->push_back("Luget");
	french->getLastNames()->push_back("Marcelle");
	french->getLastNames()->push_back("Pecheux");
	french->getLastNames()->push_back("Revenu");
	_names.push_back(french);

	SoldierNamePool *german = new SoldierNamePool();
	german->getMaleNames()->push_back("Dieter");
	german->getMaleNames()->push_back("Franz");
	german->getMaleNames()->push_back("Gerhard");
	german->getMaleNames()->push_back("Gunter");
	german->getMaleNames()->push_back("Hans");
	german->getMaleNames()->push_back("Jurgen");
	german->getMaleNames()->push_back("Klaus");
	german->getMaleNames()->push_back("Manfred");
	german->getMaleNames()->push_back("Matthias");
	german->getMaleNames()->push_back("Otto");
	german->getMaleNames()->push_back("Rudi");
	german->getMaleNames()->push_back("Siegfried");
	german->getMaleNames()->push_back("Stefan");
	german->getMaleNames()->push_back("Werner");
	german->getMaleNames()->push_back("Wolfgang");
	german->getFemaleNames()->push_back("Christel");
	german->getFemaleNames()->push_back("Gudrun");
	german->getFemaleNames()->push_back("Helga");
	german->getFemaleNames()->push_back("Karin");
	german->getFemaleNames()->push_back("Uta");
	german->getLastNames()->push_back("Berger");
	german->getLastNames()->push_back("Brehme");
	german->getLastNames()->push_back("Esser");
	german->getLastNames()->push_back("Faerber");
	german->getLastNames()->push_back("Geisler");
	german->getLastNames()->push_back("Gunkel");
	german->getLastNames()->push_back("Hafner");
	german->getLastNames()->push_back("Heinsch");
	german->getLastNames()->push_back("Keller");
	german->getLastNames()->push_back("Krause");
	german->getLastNames()->push_back("Mederow");
	german->getLastNames()->push_back("Meyer");
	german->getLastNames()->push_back("Richter");
	german->getLastNames()->push_back("Schultz");
	german->getLastNames()->push_back("Seidler");
	german->getLastNames()->push_back("Steinbach");
	german->getLastNames()->push_back("Ulbricht");
	german->getLastNames()->push_back("Unger");
	german->getLastNames()->push_back("Vogel");
	german->getLastNames()->push_back("Zander");
	_names.push_back(german);

	SoldierNamePool *japanese = new SoldierNamePool();
	japanese->getMaleNames()->push_back("Akinori");
	japanese->getMaleNames()->push_back("Isao");
	japanese->getMaleNames()->push_back("Jungo");
	japanese->getMaleNames()->push_back("Kenji");
	japanese->getMaleNames()->push_back("Masaharu");
	japanese->getMaleNames()->push_back("Masanori");
	japanese->getMaleNames()->push_back("Naohiro");
	japanese->getMaleNames()->push_back("Shigeo");
	japanese->getMaleNames()->push_back("Shigeru");
	japanese->getMaleNames()->push_back("Shuji");
	japanese->getMaleNames()->push_back("Tatsuo");
	japanese->getMaleNames()->push_back("Toshio");
	japanese->getMaleNames()->push_back("Yasuaki");
	japanese->getMaleNames()->push_back("Yataka");
	japanese->getMaleNames()->push_back("Yuzo");
	japanese->getFemaleNames()->push_back("Mariko");
	japanese->getFemaleNames()->push_back("Michiko");
	japanese->getFemaleNames()->push_back("Sata");
	japanese->getFemaleNames()->push_back("Sumie");
	japanese->getFemaleNames()->push_back("Yoko");
	japanese->getLastNames()->push_back("Akira");
	japanese->getLastNames()->push_back("Fujimoto");
	japanese->getLastNames()->push_back("Ishii");
	japanese->getLastNames()->push_back("Iwahara");
	japanese->getLastNames()->push_back("Iwasaki");
	japanese->getLastNames()->push_back("Kojima");
	japanese->getLastNames()->push_back("Koyama");
	japanese->getLastNames()->push_back("Matsumara");
	japanese->getLastNames()->push_back("Morita");
	japanese->getLastNames()->push_back("Noguchi");
	japanese->getLastNames()->push_back("Okabe");
	japanese->getLastNames()->push_back("Okamoto");
	japanese->getLastNames()->push_back("Sato");
	japanese->getLastNames()->push_back("Shimaoka");
	japanese->getLastNames()->push_back("Shoji");
	japanese->getLastNames()->push_back("Tanida");
	japanese->getLastNames()->push_back("Tanikawa");
	japanese->getLastNames()->push_back("Yamanaka");
	japanese->getLastNames()->push_back("Yamashita");
	japanese->getLastNames()->push_back("Yamazaki");
	_names.push_back(japanese);

	SoldierNamePool *russian = new SoldierNamePool();
	russian->getMaleNames()->push_back("Anatoly");
	russian->getMaleNames()->push_back("Andrei");
	russian->getMaleNames()->push_back("Boris");
	russian->getMaleNames()->push_back("Dmitriy");
	russian->getMaleNames()->push_back("Gennadi");
	russian->getMaleNames()->push_back("Grigoriy");
	russian->getMaleNames()->push_back("Igor");
	russian->getMaleNames()->push_back("Ivan");
	russian->getMaleNames()->push_back("Leonid");
	russian->getMaleNames()->push_back("Mikhail");
	russian->getMaleNames()->push_back("Nikolai");
	russian->getMaleNames()->push_back("Sergei");
	russian->getMaleNames()->push_back("Victor");
	russian->getMaleNames()->push_back("Vladimir");
	russian->getMaleNames()->push_back("Yuri");
	russian->getFemaleNames()->push_back("Astra");
	russian->getFemaleNames()->push_back("Galina");
	russian->getFemaleNames()->push_back("Lyudmila");
	russian->getFemaleNames()->push_back("Olga");
	russian->getFemaleNames()->push_back("Tatyana");
	russian->getLastNames()->push_back("Andianov");
	russian->getLastNames()->push_back("Belov");
	russian->getLastNames()->push_back("Chukarin");
	russian->getLastNames()->push_back("Gorokhova");
	russian->getLastNames()->push_back("Kolotov");
	russian->getLastNames()->push_back("Korkia");
	russian->getLastNames()->push_back("Likhachev");
	russian->getLastNames()->push_back("Maleev");
	russian->getLastNames()->push_back("Mikhailov");
	russian->getLastNames()->push_back("Petrov");
	russian->getLastNames()->push_back("Ragulin");
	russian->getLastNames()->push_back("Romanov");
	russian->getLastNames()->push_back("Samusenko");
	russian->getLastNames()->push_back("Scharov");
	russian->getLastNames()->push_back("Shadrin");
	russian->getLastNames()->push_back("Shalimov");
	russian->getLastNames()->push_back("Torban");
	russian->getLastNames()->push_back("Voronin");
	russian->getLastNames()->push_back("Yakubik");
	russian->getLastNames()->push_back("Zhdanovich");
	_names.push_back(russian);
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
	for (int i = 0; i < 8; i++)
	{
		base->getSoldiers()->push_back(new Soldier(&_names));
	}

	save->getBases()->push_back(base);
	
	return save;
}