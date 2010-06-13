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

/**
 * Initializes all the elements in the Basescape screen.
 * @param game Pointer to the core game.
 */
BasescapeState::BasescapeState(Game *game) : State(game)
{
	_base = _game->getSavedGame()->getBases()->front();

	// Create objects
	_view = new BaseView(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 192, 0, 8);
	_mini = new MiniBaseView(128, 16, 192, 41);
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
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());

	add(_view);
	add(_mini);
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
	_view->setTexture(_game->getResourcePack()->getSurfaceSet("BASEBITS.PCK"));
	_view->onMouseClick((EventHandler)&BasescapeState::viewClick);

	_mini->setTexture(_game->getResourcePack()->getSurfaceSet("BASEBITS.PCK"));
	_mini->setBases(_game->getSavedGame()->getBases());

	_txtFacility->setColor(Palette::blockOffset(13)+10);
	_txtFacility->setText("");

	_txtBase->setColor(Palette::blockOffset(15)+1);
	_txtBase->setBig();

	_txtLocation->setColor(Palette::blockOffset(15)+6);

	_txtFunds->setColor(Palette::blockOffset(13)+10);
	
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
	_btnFacilities->onMouseClick((EventHandler)&BasescapeState::btnFacilitiesClick);

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

/**
 *
 */
BasescapeState::~BasescapeState()
{
	
}

/**
 * The player can change the selected base
 * or change info on other screens.
 */
void BasescapeState::init()
{
	_view->setBase(_base);
	_mini->draw();
	_txtBase->setText(_base->getName());
	_txtLocation->setText("Some Location");

	string s = _game->getResourcePack()->getLanguage()->getString(STR_FUNDS_);
	s.erase(s.size()-1, 1);
	s += Text::formatFunding(_game->getSavedGame()->getFunds());
	_txtFunds->setText(s);
}

/**
 * The facility name can change
 * whenever the cursor is moved.
 */
void BasescapeState::think()
{
	BaseFacility *f = _view->getSelectedFacility();
	if (f == 0)
		_txtFacility->setText("");
	else
		_txtFacility->setText(_game->getResourcePack()->getLanguage()->getString(f->getRules()->getType()));
}

/**
 * Goes to the Base Info screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnBaseInfoClick(SDL_Event *ev, int scale)
{
	_game->pushState(new BaseInfoState(_game, _base));
}

/**
 * Goes to the Soldiers screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnSoldiersClick(SDL_Event *ev, int scale)
{
	_game->pushState(new SoldiersState(_game, _base));
}

/**
 * Goes to the Crafts screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnCraftsClick(SDL_Event *ev, int scale)
{
	_game->pushState(new CraftsState(_game, _base));
}

/**
 * Opens the Build Facilities window.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnFacilitiesClick(SDL_Event *ev, int scale)
{
	_game->pushState(new BuildFacilitiesState(_game));
}

/**
 * Goes to the Research screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnResearchClick(SDL_Event *ev, int scale)
{
	_game->pushState(new ResearchState(_game));
}

/**
 * Goes to the Manufacture screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnManufactureClick(SDL_Event *ev, int scale)
{
	_game->pushState(new ManufactureState(_game));
}

/**
 * Goes to the Purchase screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnPurchaseClick(SDL_Event *ev, int scale)
{
	_game->pushState(new PurchaseState(_game));
}

/**
 * Goes to the Sell screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnSellClick(SDL_Event *ev, int scale)
{
	_game->pushState(new SellState(_game));
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::btnGeoscapeClick(SDL_Event *ev, int scale)
{
	_game->popState();
}

/**
 * Processes clicking on facilities.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void BasescapeState::viewClick(SDL_Event *ev, int scale)
{
	BaseFacility *fac = _view->getSelectedFacility();
	if (fac != 0)
	{
		if (fac->getRules()->getLift() == true ||
			_base->getAvailableQuarters() - fac->getRules()->getPersonnel() < _base->getUsedQuarters() ||
			_base->getAvailableStores() - fac->getRules()->getStorage() < _base->getUsedStores() ||
			_base->getAvailableLaboratories() - fac->getRules()->getLaboratories() < _base->getUsedLaboratories() ||
			_base->getAvailableWorkshops() - fac->getRules()->getWorkshops() < _base->getUsedWorkshops() ||
			_base->getAvailableHangars() - fac->getRules()->getCrafts() < _base->getUsedHangars())
		{
			_game->pushState(new BasescapeErrorState(_game, STR_FACILITY_IN_USE));
		}
		else
		{
			_game->pushState(new BasescapeDismantleState(_game, _base, fac));
		}
	}
}