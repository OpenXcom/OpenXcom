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
	_txtEngineers = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 51);
	_numEngineers = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 51);
	_txtScientists = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 61);
	_numScientists = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 61);

	_txtSpace = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 300, 9, 8, 72);
	// SOMETHING FUCKED HERE!!!
	_txtQuarters = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 83);
	_numQuarters = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 83);
	//_txtStores = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 93);
	//_numStores = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 93);
	//_txtLaboratories = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 103);
	//_numLaboratories = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 103);
	//_txtWorkshops = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 113);
	//_numWorkshops = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 113);
	//_txtHangars = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 123);
	//_numHangars = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 123);

	//_txtDefence = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 138);
	//_numDefence = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 138);
	//_txtShortRange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 153);
	//_numShortRange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 153);
	//_txtLongRange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 114, 9, 8, 163);
	//_numLongRange = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 40, 9, 126, 163);
	
	add(_bg);
	add(_btnOk);
	add(_btnTransfers);
	add(_btnStores);
	add(_btnMonthlyCosts);
	//
	//add(_txtBase);
	//add(_txtPersonnel);
	//add(_txtSoldiers);
	//add(_numSoldiers);
	//add(_txtEngineers);
	//add(_numEngineers);
	//add(_txtScientists);
	//add(_numScientists);

	//add(_txtSpace);
	//add(_txtQuarters);
	//add(_numQuarters);
	//add(_txtStores);
	//add(_numStores);
	//add(_txtLaboratories);
	//add(_numLaboratories);
	//add(_txtWorkshops);
	//add(_numWorkshops);
	//add(_txtHangars);
	//add(_numHangars);

	//add(_txtDefence);
	//add(_numDefence);
	//add(_txtShortRange);
	//add(_numShortRange);
	//add(_txtLongRange);
	//add(_numLongRange);
	
	// Set up objects
	_game->getSurface("BACK07.SCR")->blit(_bg);

	_btnOk->setColor(Palette::blockOffset(15)+9);
	_btnOk->setText(_game->getLanguage()->getString(76));
	_btnOk->onMouseClick((EventHandler)&BaseInfoState::btnOkClick);

	_btnTransfers->setColor(Palette::blockOffset(15)+9);
	_btnTransfers->setText(_game->getLanguage()->getString(140));

	_btnStores->setColor(Palette::blockOffset(15)+9);
	_btnStores->setText(_game->getLanguage()->getString(934));

	_btnMonthlyCosts->setColor(Palette::blockOffset(15)+9);
	_btnMonthlyCosts->setText(_game->getLanguage()->getString(954));

	/*_txtBase->setColor(Palette::blockOffset(15)+1);
	_txtBase->setBig();
	_txtBase->setText("Some Base");

	_txtPersonnel->setColor(Palette::blockOffset(15)+1);
	_txtPersonnel->setText(_game->getLanguage()->getString(128));

	_txtSoldiers->setColor(Palette::blockOffset(13)+5);
	_txtSoldiers->setText(_game->getLanguage()->getString(129));

	_numSoldiers->setColor(Palette::blockOffset(13));
	_numSoldiers->setText("0:0");

	_txtEngineers->setColor(Palette::blockOffset(13)+5);
	_txtEngineers->setText(_game->getLanguage()->getString(130));

	_numEngineers->setColor(Palette::blockOffset(13));
	_numEngineers->setText("0:0");

	_txtScientists->setColor(Palette::blockOffset(13)+5);
	_txtScientists->setText(_game->getLanguage()->getString(131));

	_numScientists->setColor(Palette::blockOffset(13));
	_numScientists->setText("0:0");

	_txtSpace->setColor(Palette::blockOffset(15)+1);
	_txtSpace->setText(_game->getLanguage()->getString(132));

	_txtQuarters->setColor(Palette::blockOffset(13)+5);
	_txtQuarters->setText(_game->getLanguage()->getString(133));

	_numQuarters->setColor(Palette::blockOffset(13));
	_numQuarters->setText("0:0");

	_txtStores->setColor(Palette::blockOffset(13)+5);
	_txtStores->setText(_game->getLanguage()->getString(134));

	_numStores->setColor(Palette::blockOffset(13));
	_numStores->setText("0:0");

	_txtLaboratories->setColor(Palette::blockOffset(13)+5);
	_txtLaboratories->setText(_game->getLanguage()->getString(135));

	_numLaboratories->setColor(Palette::blockOffset(13));
	_numLaboratories->setText("0:0");

	_txtWorkshops->setColor(Palette::blockOffset(13)+5);
	_txtWorkshops->setText(_game->getLanguage()->getString(136));

	_numWorkshops->setColor(Palette::blockOffset(13));
	_numWorkshops->setText("0:0");

	_txtHangars->setColor(Palette::blockOffset(13)+5);
	_txtHangars->setText(_game->getLanguage()->getString(137));

	_numHangars->setColor(Palette::blockOffset(13));
	_numHangars->setText("0:0");
	
	_txtDefence->setColor(Palette::blockOffset(13)+5);
	_txtDefence->setText(_game->getLanguage()->getString(139));

	_numDefence->setColor(Palette::blockOffset(13));
	_numDefence->setText("0:0");

	_txtShortRange->setColor(Palette::blockOffset(13)+5);
	_txtShortRange->setText(_game->getLanguage()->getString(138));

	_numShortRange->setColor(Palette::blockOffset(13));
	_numShortRange->setText("0:0");

	_txtLongRange->setColor(Palette::blockOffset(13)+5);
	_txtLongRange->setText(_game->getLanguage()->getString(933));

	_numLongRange->setColor(Palette::blockOffset(13));
	_numLongRange->setText("0:0");*/
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