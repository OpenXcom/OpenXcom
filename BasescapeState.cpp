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
#include "BasescapeState.h"

BasescapeState::BasescapeState(Game *game) : State(game)
{
	// Create objects
	_txtFacility = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 9, 0, 0);
	_txtBase = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 127, 17, 193, 0);
	_txtLocation = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 126, 9, 194, 16);
	_txtFunds = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 126, 9, 194, 24);
	_btnNewBase = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 58);
	_btnBaseInfo = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 71);
	_btnSoldiers = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 84);
	_btnCrafts = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 97);
	_btnFacilities = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 110);
	_btnResearch = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 123);
	_btnManufacture = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 136);
	_btnTransfer = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 149);
	_btnPurchase = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 162);
	_btnSell = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 175);
	_btnGeoscape = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 128, 12, 192, 188);

	_base = _game->getSavedGame()->getBases()->front();
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());

	add(_txtFacility);
	add(_txtBase);
	add(_txtLocation);
	add(_txtFunds);
	add(_btnNewBase);
	add(_btnBaseInfo);
	add(_btnSoldiers);
	add(_btnCrafts);
	add(_btnFacilities);
	add(_btnResearch);
	add(_btnManufacture);
	add(_btnTransfer);
	add(_btnPurchase);
	add(_btnSell);
	add(_btnGeoscape);

	// Set up objects
	_txtFacility->setColor(Palette::blockOffset(13)+10);
	_txtFacility->setText("Some Facility");

	_txtBase->setColor(Palette::blockOffset(15)+1);
	_txtBase->setBig();
	_txtBase->setText(_base->getName());

	_txtLocation->setColor(Palette::blockOffset(15)+6);
	_txtLocation->setText("Some Location");

	string s = _game->getResourcePack()->getLanguage()->getString(STR_FUNDS_);
	s.erase(s.size()-1, 1);
	s += Text::formatFunding(_game->getSavedGame()->getFunds());
	_txtFunds->setColor(Palette::blockOffset(13)+10);
	_txtFunds->setText(s);
	
	_btnNewBase->setColor(Palette::blockOffset(13)+8);
	_btnNewBase->setText(_game->getResourcePack()->getLanguage()->getString(STR_BUILD_NEW_BASE));

	_btnBaseInfo->setColor(Palette::blockOffset(13)+8);
	_btnBaseInfo->setText(_game->getResourcePack()->getLanguage()->getString(STR_BASE_INFORMATION));
	_btnBaseInfo->onMouseClick((EventHandler)&BasescapeState::btnBaseInfoClick);

	_btnSoldiers->setColor(Palette::blockOffset(13)+8);
	_btnSoldiers->setText(_game->getResourcePack()->getLanguage()->getString(STR_SOLDIERS_UC));
	_btnSoldiers->onMouseClick((EventHandler)&BasescapeState::btnSoldiersClick);

	_btnCrafts->setColor(Palette::blockOffset(13)+8);
	_btnCrafts->setText(_game->getResourcePack()->getLanguage()->getString(STR_EQUIP_CRAFT));
	_btnCrafts->onMouseClick((EventHandler)&BasescapeState::btnCraftsClick);

	_btnFacilities->setColor(Palette::blockOffset(13)+8);
	_btnFacilities->setText(_game->getResourcePack()->getLanguage()->getString(STR_BUILD_FACILITIES));

	_btnResearch->setColor(Palette::blockOffset(13)+8);
	_btnResearch->setText(_game->getResourcePack()->getLanguage()->getString(STR_RESEARCH));
	_btnResearch->onMouseClick((EventHandler)&BasescapeState::btnResearchClick);

	_btnManufacture->setColor(Palette::blockOffset(13)+8);
	_btnManufacture->setText(_game->getResourcePack()->getLanguage()->getString(STR_MANUFACTURE));
	_btnManufacture->onMouseClick((EventHandler)&BasescapeState::btnManufactureClick);

	_btnTransfer->setColor(Palette::blockOffset(13)+8);
	_btnTransfer->setText(_game->getResourcePack()->getLanguage()->getString(STR_TRANSFER));

	_btnPurchase->setColor(Palette::blockOffset(13)+8);
	_btnPurchase->setText(_game->getResourcePack()->getLanguage()->getString(STR_PURCHASE_RECRUIT));
	_btnPurchase->onMouseClick((EventHandler)&BasescapeState::btnPurchaseClick);

	_btnSell->setColor(Palette::blockOffset(13)+8);
	_btnSell->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELL_SACK_UC));
	_btnSell->onMouseClick((EventHandler)&BasescapeState::btnSellClick);

	_btnGeoscape->setColor(Palette::blockOffset(13)+8);
	_btnGeoscape->setText(_game->getResourcePack()->getLanguage()->getString(STR_GEOSCAPE));
	_btnGeoscape->onMouseClick((EventHandler)&BasescapeState::btnGeoscapeClick);
}

BasescapeState::~BasescapeState()
{
	
}

void BasescapeState::init()
{
	_txtBase->setText(_base->getName());

	_txtLocation->setText("Some Location");

	string s = _game->getResourcePack()->getLanguage()->getString(STR_FUNDS_);
	s.erase(s.size()-1, 1);
	s += Text::formatFunding(_game->getSavedGame()->getFunds());
	_txtFunds->setText(s);
}

void BasescapeState::btnBaseInfoClick(SDL_Event *ev, int scale)
{
	_game->pushState(new BaseInfoState(_game, _base));
}

void BasescapeState::btnSoldiersClick(SDL_Event *ev, int scale)
{
	_game->pushState(new SoldiersState(_game, _base));
}

void BasescapeState::btnCraftsClick(SDL_Event *ev, int scale)
{
	_game->pushState(new CraftsState(_game));
}

void BasescapeState::btnResearchClick(SDL_Event *ev, int scale)
{
	_game->pushState(new ResearchState(_game));
}

void BasescapeState::btnManufactureClick(SDL_Event *ev, int scale)
{
	_game->pushState(new ManufactureState(_game));
}

void BasescapeState::btnPurchaseClick(SDL_Event *ev, int scale)
{
	_game->pushState(new PurchaseState(_game));
}

void BasescapeState::btnSellClick(SDL_Event *ev, int scale)
{
	_game->pushState(new SellState(_game));
}

void BasescapeState::btnGeoscapeClick(SDL_Event *ev, int scale)
{
	_game->popState();
}