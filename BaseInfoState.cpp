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
#include "BaseInfoState.h"

/**
 * Initializes all the elements in the Base Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
BaseInfoState::BaseInfoState(Game *game, Base *base) : State(game), _base(base)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_btnOk = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 30, 14, 10, 180);
	_btnTransfers = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 80, 14, 46, 180);
	_btnStores = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 80, 14, 132, 180);
	_btnMonthlyCosts = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 92, 14, 218, 180);
	_edtBase = new TextEdit(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 170, 16, 8, 8);

	_txtPersonnel = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 9, 8, 30);
	_txtSoldiers = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 41);
	_numSoldiers = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 41);
	_barSoldiers = new Bar(150, 5, 165, 43);
	_txtEngineers = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 51);
	_numEngineers = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 51);
	_barEngineers = new Bar(150, 5, 165, 53);
	_txtScientists = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 61);
	_numScientists = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 61);
	_barScientists = new Bar(150, 5, 165, 63);

	_txtSpace = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 9, 8, 72);
	_txtQuarters = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 83);
	_numQuarters = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 83);
	_barQuarters = new Bar(150, 5, 165, 85);
	_txtStores = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 93);
	_numStores = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 93);
	_barStores = new Bar(150, 5, 165, 95);
	_txtLaboratories = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 103);
	_numLaboratories = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 103);
	_barLaboratories = new Bar(150, 5, 165, 105);
	_txtWorkshops = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 113);
	_numWorkshops = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 113);
	_barWorkshops = new Bar(150, 5, 165, 115);
	_txtHangars = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 123);
	_numHangars = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 123);
	_barHangars = new Bar(150, 5, 165, 125);

	_txtDefence = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 8, 138);
	_numDefence = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 138);
	_barDefence = new Bar(150, 5, 165, 140);
	_txtShortRange = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 130, 9, 8, 153);
	_numShortRange = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 153);
	_barShortRange = new Bar(150, 5, 165, 155);
	_txtLongRange = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 130, 9, 8, 163);
	_numLongRange = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 40, 9, 126, 163);
	_barLongRange = new Bar(150, 5, 165, 165);
	
	add(_bg);
	add(_btnOk);
	add(_btnTransfers);
	add(_btnStores);
	add(_btnMonthlyCosts);
	add(_edtBase);

	add(_txtPersonnel);
	add(_txtSoldiers);
	add(_numSoldiers);
	add(_barSoldiers);
	add(_txtEngineers);
	add(_numEngineers);
	add(_barEngineers);
	add(_txtScientists);
	add(_numScientists);
	add(_barScientists);

	add(_txtSpace);
	add(_txtQuarters);
	add(_numQuarters);
	add(_barQuarters);
	add(_txtStores);
	add(_numStores);
	add(_barStores);
	add(_txtLaboratories);
	add(_numLaboratories);
	add(_barLaboratories);
	add(_txtWorkshops);
	add(_numWorkshops);
	add(_barWorkshops);
	add(_txtHangars);
	add(_numHangars);
	add(_barHangars);

	add(_txtDefence);
	add(_numDefence);
	add(_barDefence);
	add(_txtShortRange);
	add(_numShortRange);
	add(_barShortRange);
	add(_txtLongRange);
	add(_numLongRange);
	add(_barLongRange);
	
	// Set up objects
	_game->getResourcePack()->getSurface("BACK07.SCR")->blit(_bg);

	_btnOk->setColor(Palette::blockOffset(15)+9);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&BaseInfoState::btnOkClick);

	_btnTransfers->setColor(Palette::blockOffset(15)+9);
	_btnTransfers->setText(_game->getResourcePack()->getLanguage()->getString(STR_TRANSFERS));

	_btnStores->setColor(Palette::blockOffset(15)+9);
	_btnStores->setText(_game->getResourcePack()->getLanguage()->getString(STR_STORES_UC));

	_btnMonthlyCosts->setColor(Palette::blockOffset(15)+9);
	_btnMonthlyCosts->setText(_game->getResourcePack()->getLanguage()->getString(STR_MONTHLY_COSTS));

	_edtBase->setColor(Palette::blockOffset(15)+1);
	_edtBase->setBig();
	_edtBase->setText(_base->getName());


	_txtPersonnel->setColor(Palette::blockOffset(15)+1);
	_txtPersonnel->setText(_game->getResourcePack()->getLanguage()->getString(STR_PERSONNEL_AVAILABLE));

	_txtSoldiers->setColor(Palette::blockOffset(13)+5);
	_txtSoldiers->setText(_game->getResourcePack()->getLanguage()->getString(STR_SOLDIERS));

	_numSoldiers->setColor(Palette::blockOffset(13));
	stringstream ss;
	ss << _base->getAvailableSoldiers() << ":" << _base->getTotalSoldiers();
	_numSoldiers->setText(ss.str());

	_barSoldiers->setColor(Palette::blockOffset(1));
	_barSoldiers->setScale(1.0);
	_barSoldiers->setMax(_base->getTotalSoldiers());
	_barSoldiers->setValue(_base->getAvailableSoldiers());

	_txtEngineers->setColor(Palette::blockOffset(13)+5);
	_txtEngineers->setText(_game->getResourcePack()->getLanguage()->getString(STR_ENGINEERS));

	_numEngineers->setColor(Palette::blockOffset(13));
	stringstream ss2;
	ss2 << _base->getAvailableEngineers() << ":" << _base->getTotalEngineers();
	_numEngineers->setText(ss2.str());

	_barEngineers->setColor(Palette::blockOffset(1));
	_barEngineers->setScale(1.0);
	_barEngineers->setMax(_base->getTotalEngineers());
	_barEngineers->setValue(_base->getAvailableEngineers());

	_txtScientists->setColor(Palette::blockOffset(13)+5);
	_txtScientists->setText(_game->getResourcePack()->getLanguage()->getString(STR_SCIENTISTS));

	_numScientists->setColor(Palette::blockOffset(13));
	stringstream ss3;
	ss3 << _base->getAvailableScientists() << ":" << _base->getTotalScientists();
	_numScientists->setText(ss3.str());

	_barScientists->setColor(Palette::blockOffset(1));
	_barScientists->setScale(1.0);
	_barScientists->setMax(_base->getTotalScientists());
	_barScientists->setValue(_base->getAvailableScientists());


	_txtSpace->setColor(Palette::blockOffset(15)+1);
	_txtSpace->setText(_game->getResourcePack()->getLanguage()->getString(STR_SPACE_USED));

	_txtQuarters->setColor(Palette::blockOffset(13)+5);
	_txtQuarters->setText(_game->getResourcePack()->getLanguage()->getString(STR_LIVING_QUARTERS));

	_numQuarters->setColor(Palette::blockOffset(13));
	stringstream ss4;
	ss4 << _base->getUsedQuarters() << ":" << _base->getAvailableQuarters();
	_numQuarters->setText(ss4.str());

	_barQuarters->setColor(Palette::blockOffset(3));
	_barQuarters->setScale(0.5);
	_barQuarters->setMax(_base->getAvailableQuarters());
	_barQuarters->setValue(_base->getUsedQuarters());

	_txtStores->setColor(Palette::blockOffset(13)+5);
	_txtStores->setText(_game->getResourcePack()->getLanguage()->getString(STR_STORES));

	_numStores->setColor(Palette::blockOffset(13));
	stringstream ss5;
	ss5 << _base->getUsedStores() << ":" << _base->getAvailableStores();
	_numStores->setText(ss5.str());

	_barStores->setColor(Palette::blockOffset(3));
	_barStores->setScale(0.5);
	_barStores->setMax(_base->getAvailableStores());
	_barStores->setValue(_base->getUsedStores());

	_txtLaboratories->setColor(Palette::blockOffset(13)+5);
	_txtLaboratories->setText(_game->getResourcePack()->getLanguage()->getString(STR_LABORATORIES));

	_numLaboratories->setColor(Palette::blockOffset(13));
	stringstream ss6;
	ss6 << _base->getUsedLaboratories() << ":" << _base->getAvailableLaboratories();
	_numLaboratories->setText(ss6.str());

	_barLaboratories->setColor(Palette::blockOffset(3));
	_barLaboratories->setScale(0.5);
	_barLaboratories->setMax(_base->getAvailableLaboratories());
	_barLaboratories->setValue(_base->getUsedLaboratories());

	_txtWorkshops->setColor(Palette::blockOffset(13)+5);
	_txtWorkshops->setText(_game->getResourcePack()->getLanguage()->getString(STR_WORK_SHOPS));

	_numWorkshops->setColor(Palette::blockOffset(13));
	stringstream ss7;
	ss7 << _base->getUsedWorkshops() << ":" << _base->getAvailableWorkshops();
	_numWorkshops->setText(ss7.str());

	_barWorkshops->setColor(Palette::blockOffset(3));
	_barWorkshops->setScale(0.5);
	_barWorkshops->setMax(_base->getAvailableWorkshops());
	_barWorkshops->setValue(_base->getUsedWorkshops());

	_txtHangars->setColor(Palette::blockOffset(13)+5);
	_txtHangars->setText(_game->getResourcePack()->getLanguage()->getString(STR_HANGARS));

	_numHangars->setColor(Palette::blockOffset(13));
	stringstream ss8;
	ss8 << _base->getUsedHangars() << ":" << _base->getAvailableHangars();
	_numHangars->setText(ss8.str());

	_barHangars->setColor(Palette::blockOffset(3));
	_barHangars->setScale(18.0);
	_barHangars->setMax(_base->getAvailableHangars());
	_barHangars->setValue(_base->getUsedHangars());
	

	_txtDefence->setColor(Palette::blockOffset(13)+5);
	_txtDefence->setText(_game->getResourcePack()->getLanguage()->getString(STR_DEFENCE_STRENGTH));

	_numDefence->setColor(Palette::blockOffset(13));
	stringstream ss9;
	ss9 << _base->getDefenceValue();
	_numDefence->setText(ss9.str());

	_barDefence->setColor(Palette::blockOffset(2));
	_barDefence->setScale(0.125);
	_barDefence->setMax(_base->getDefenceValue());
	_barDefence->setValue(_base->getDefenceValue());

	_txtShortRange->setColor(Palette::blockOffset(13)+5);
	_txtShortRange->setText(_game->getResourcePack()->getLanguage()->getString(STR_SHORT_RANGE_DETECTION));

	_numShortRange->setColor(Palette::blockOffset(13));
	stringstream ss10;
	ss10 << _base->getShortRangeDetection();
	_numShortRange->setText(ss10.str());
	if (_game->getResourcePack()->getLanguageName() == "GERMAN.DAT")
	{
		_numShortRange->setX(137);
	}

	_barShortRange->setColor(Palette::blockOffset(8));
	_barShortRange->setScale(25.0);
	_barShortRange->setMax(_base->getShortRangeDetection());
	_barShortRange->setValue(_base->getShortRangeDetection());

	_txtLongRange->setColor(Palette::blockOffset(13)+5);
	_txtLongRange->setText(_game->getResourcePack()->getLanguage()->getString(STR_LONG_RANGE_DETECTION));

	_numLongRange->setColor(Palette::blockOffset(13));
	stringstream ss11;
	ss11 << _base->getLongRangeDetection();
	_numLongRange->setText(ss11.str());
	if (_game->getResourcePack()->getLanguageName() == "GERMAN.DAT")
	{
		_numLongRange->setX(137);
	}

	_barLongRange->setColor(Palette::blockOffset(8));
	_barLongRange->setScale(25.0);
	_barLongRange->setMax(_base->getLongRangeDetection());
	_barLongRange->setValue(_base->getLongRangeDetection());
}

/**
 *
 */
BaseInfoState::~BaseInfoState()
{
	
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BaseInfoState::btnOkClick(SDL_Event *ev, int scale)
{
	_base->setName(_edtBase->getText());
	_game->popState();
}