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

BaseInfoState::BaseInfoState(Game *game) : State(game)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_btnOk = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 30, 14, 10, 180);
	_btnTransfers = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 80, 14, 46, 180);
	_btnStores = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 80, 14, 132, 180);
	_btnMonthlyCosts = new Button(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 92, 14, 218, 180);
	
	_txtBase = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 170, 16, 8, 8);
	_txtPersonnel = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 9, 8, 30);
	_txtSoldiers = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 41);
	_numSoldiers = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 41);
	_barSoldiers = new Bar(150, 5, 165, 43);
	_txtEngineers = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 51);
	_numEngineers = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 51);
	_barEngineers = new Bar(150, 5, 165, 53);
	_txtScientists = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 61);
	_numScientists = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 61);
	_barScientists = new Bar(150, 5, 165, 63);

	_txtSpace = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 9, 8, 72);
	_txtQuarters = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 83);
	_numQuarters = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 83);
	_barQuarters = new Bar(150, 5, 165, 85);
	_txtStores = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 93);
	_numStores = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 93);
	_barStores = new Bar(150, 5, 165, 95);
	_txtLaboratories = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 103);
	_numLaboratories = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 103);
	_barLaboratories = new Bar(150, 5, 165, 105);
	_txtWorkshops = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 113);
	_numWorkshops = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 113);
	_barWorkshops = new Bar(150, 5, 165, 115);
	_txtHangars = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 123);
	_numHangars = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 123);
	_barHangars = new Bar(150, 5, 165, 125);

	_txtDefence = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 138);
	_numDefence = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 138);
	_barDefence = new Bar(150, 5, 165, 140);
	_txtShortRange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 130, 9, 8, 153);
	_numShortRange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 153);
	_barShortRange = new Bar(150, 5, 165, 155);
	_txtLongRange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 130, 9, 8, 163);
	_numLongRange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 163);
	_barLongRange = new Bar(150, 5, 165, 165);
	
	add(_bg);
	add(_btnOk);
	add(_btnTransfers);
	add(_btnStores);
	add(_btnMonthlyCosts);
	
	add(_txtBase);
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
	_game->getSurface("BACK07.SCR")->blit(_bg);

	_btnOk->setColor(Palette::blockOffset(15)+9);
	_btnOk->setText(_game->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&BaseInfoState::btnOkClick);

	_btnTransfers->setColor(Palette::blockOffset(15)+9);
	_btnTransfers->setText(_game->getLanguage()->getString(STR_TRANSFERS));

	_btnStores->setColor(Palette::blockOffset(15)+9);
	_btnStores->setText(_game->getLanguage()->getString(STR_STORES_UC));

	_btnMonthlyCosts->setColor(Palette::blockOffset(15)+9);
	_btnMonthlyCosts->setText(_game->getLanguage()->getString(STR_MONTHLY_COSTS));

	_txtBase->setColor(Palette::blockOffset(15)+1);
	_txtBase->setBig();
	_txtBase->setText("Some Base");


	_txtPersonnel->setColor(Palette::blockOffset(15)+1);
	_txtPersonnel->setText(_game->getLanguage()->getString(STR_PERSONNEL_AVAILABLE));

	_txtSoldiers->setColor(Palette::blockOffset(13)+5);
	_txtSoldiers->setText(_game->getLanguage()->getString(STR_SOLDIERS));

	_numSoldiers->setColor(Palette::blockOffset(13));
	_numSoldiers->setText("0:0");

	_barSoldiers->setColor(Palette::blockOffset(1));
	_barSoldiers->setScale(1.0);
	_barSoldiers->setMax(100.0);
	_barSoldiers->setValue(50.0);

	_txtEngineers->setColor(Palette::blockOffset(13)+5);
	_txtEngineers->setText(_game->getLanguage()->getString(STR_ENGINEERS));

	_numEngineers->setColor(Palette::blockOffset(13));
	_numEngineers->setText("0:0");

	_barEngineers->setColor(Palette::blockOffset(1));
	_barEngineers->setScale(1.0);
	_barEngineers->setMax(100.0);
	_barEngineers->setValue(50.0);

	_txtScientists->setColor(Palette::blockOffset(13)+5);
	_txtScientists->setText(_game->getLanguage()->getString(STR_SCIENTISTS));

	_numScientists->setColor(Palette::blockOffset(13));
	_numScientists->setText("0:0");

	_barScientists->setColor(Palette::blockOffset(1));
	_barScientists->setScale(1.0);
	_barScientists->setMax(100.0);
	_barScientists->setValue(50.0);


	_txtSpace->setColor(Palette::blockOffset(15)+1);
	_txtSpace->setText(_game->getLanguage()->getString(STR_SPACE_USED));

	_txtQuarters->setColor(Palette::blockOffset(13)+5);
	_txtQuarters->setText(_game->getLanguage()->getString(STR_LIVING_QUARTERS));

	_numQuarters->setColor(Palette::blockOffset(13));
	_numQuarters->setText("0:0");

	_barQuarters->setColor(Palette::blockOffset(3));
	_barQuarters->setScale(0.5);
	_barQuarters->setMax(100.0);
	_barQuarters->setValue(50.0);

	_txtStores->setColor(Palette::blockOffset(13)+5);
	_txtStores->setText(_game->getLanguage()->getString(STR_STORES));

	_numStores->setColor(Palette::blockOffset(13));
	_numStores->setText("0:0");

	_barStores->setColor(Palette::blockOffset(3));
	_barStores->setScale(0.5);
	_barStores->setMax(100.0);
	_barStores->setValue(50.0);

	_txtLaboratories->setColor(Palette::blockOffset(13)+5);
	_txtLaboratories->setText(_game->getLanguage()->getString(STR_LABORATORIES));

	_numLaboratories->setColor(Palette::blockOffset(13));
	_numLaboratories->setText("0:0");

	_barLaboratories->setColor(Palette::blockOffset(3));
	_barLaboratories->setScale(0.5);
	_barLaboratories->setMax(100.0);
	_barLaboratories->setValue(50.0);

	_txtWorkshops->setColor(Palette::blockOffset(13)+5);
	_txtWorkshops->setText(_game->getLanguage()->getString(STR_WORK_SHOPS));

	_numWorkshops->setColor(Palette::blockOffset(13));
	_numWorkshops->setText("0:0");

	_barWorkshops->setColor(Palette::blockOffset(3));
	_barWorkshops->setScale(0.5);
	_barWorkshops->setMax(100.0);
	_barWorkshops->setValue(50.0);

	_txtHangars->setColor(Palette::blockOffset(13)+5);
	_txtHangars->setText(_game->getLanguage()->getString(STR_HANGARS));

	_numHangars->setColor(Palette::blockOffset(13));
	_numHangars->setText("0:0");

	_barHangars->setColor(Palette::blockOffset(3));
	_barHangars->setScale(18.0);
	_barHangars->setMax(5.0);
	_barHangars->setValue(2.0);
	

	_txtDefence->setColor(Palette::blockOffset(13)+5);
	_txtDefence->setText(_game->getLanguage()->getString(STR_DEFENCE_STRENGTH));

	_numDefence->setColor(Palette::blockOffset(13));
	_numDefence->setText("0");

	_barDefence->setColor(Palette::blockOffset(2));
	_barDefence->setScale(0.125);
	_barDefence->setMax(500.0);
	_barDefence->setValue(200.0);

	_txtShortRange->setColor(Palette::blockOffset(13)+5);
	_txtShortRange->setText(_game->getLanguage()->getString(STR_SHORT_RANGE_DETECTION));

	_numShortRange->setColor(Palette::blockOffset(13));
	_numShortRange->setText("0");

	_barShortRange->setColor(Palette::blockOffset(8));
	_barShortRange->setScale(25.0);
	_barShortRange->setMax(5.0);
	_barShortRange->setValue(2.0);

	_txtLongRange->setColor(Palette::blockOffset(13)+5);
	_txtLongRange->setText(_game->getLanguage()->getString(STR_LONG_RANGE_DETECTION));

	_numLongRange->setColor(Palette::blockOffset(13));
	_numLongRange->setText("0");

	_barLongRange->setColor(Palette::blockOffset(8));
	_barLongRange->setScale(25.0);
	_barLongRange->setMax(5.0);
	_barLongRange->setValue(2.0);
}

BaseInfoState::~BaseInfoState()
{
	
}

void BaseInfoState::think()
{
}

void BaseInfoState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->setState(new BasescapeState(_game));
}