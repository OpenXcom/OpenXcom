/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "DebriefingState.h"
#include <sstream>
#include <map>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Music.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Base.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Tile.h"
#include "../Savegame/ItemContainer.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/RuleItem.h"
#include "../Savegame/Vehicle.h"
#include "../Savegame/TerrorSite.h"
#include "PromotionsState.h"
#include "CannotReequipState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Debriefing screen.
 * @param game Pointer to the core game.
 */
DebriefingState::DebriefingState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(40, 12, 16, 180);
	_txtTitle = new Text(280, 16, 16, 8);
	_txtItem = new Text(180, 9, 16, 24);
	_txtQuantity = new Text(60, 9, 200, 24);
	_txtScore = new Text(50, 9, 260, 24);
	_txtUfoRecovery = new Text(180, 9, 16, 60);
	_txtRating = new Text(120, 9, 64, 180);
	_lstStats = new TextList(280, 80, 16, 32);
	_lstUfoRecovery = new TextList(280, 80, 16, 32);
	_lstTotal = new TextList(280, 9, 16, 12);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtItem);
	add(_txtQuantity);
	add(_txtScore);
	add(_txtUfoRecovery);
	add(_txtRating);
	add(_lstStats);
	add(_lstUfoRecovery);
	add(_lstTotal);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)-1);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&DebriefingState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();

	_txtItem->setColor(Palette::blockOffset(8)+5);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtQuantity->setColor(Palette::blockOffset(8)+5);
	_txtQuantity->setText(_game->getLanguage()->getString("STR_QUANTITY_UC"));

	_txtScore->setColor(Palette::blockOffset(8)+5);
	_txtScore->setText(_game->getLanguage()->getString("STR_SCORE"));

	_txtUfoRecovery->setColor(Palette::blockOffset(8)+5);
	_txtUfoRecovery->setText(_game->getLanguage()->getString("STR_UFO_RECOVERY"));

	_txtRating->setColor(Palette::blockOffset(8)+5);

	_lstStats->setColor(Palette::blockOffset(15)-1);
	_lstStats->setSecondaryColor(Palette::blockOffset(8)+10);
	_lstStats->setColumns(3, 184, 60, 64);
	_lstStats->setDot(true);

	_lstUfoRecovery->setColor(Palette::blockOffset(15)-1);
	_lstUfoRecovery->setSecondaryColor(Palette::blockOffset(8)+10);
	_lstUfoRecovery->setColumns(3, 184, 60, 64);
	_lstUfoRecovery->setDot(true);

	_lstTotal->setColor(Palette::blockOffset(8)+5);
	_lstTotal->setColumns(2, 244, 64);
	_lstTotal->setDot(true);

	prepareDebriefing();

	int total = 0, statsY = 0, recoveryY = 0;
	for (std::vector<DebriefingStat*>::iterator i = _stats.begin(); i != _stats.end(); ++i)
	{
		if ((*i)->qty == 0)
			continue;

		std::wstringstream ss, ss2;
		ss << L'\x01' << (*i)->qty << L'\x01';
		ss2 << L'\x01' << (*i)->score;
		total += (*i)->score;
		if ((*i)->recovery)
		{
			_lstUfoRecovery->addRow(3, _game->getLanguage()->getString((*i)->item).c_str(), ss.str().c_str(), ss2.str().c_str());
			recoveryY += 8;
		}
		else
		{
			_lstStats->addRow(3, _game->getLanguage()->getString((*i)->item).c_str(), ss.str().c_str(), ss2.str().c_str());
			statsY += 8;
		}
	}
	std::wstringstream ss3;
	ss3 << total;
	_lstTotal->addRow(2, _game->getLanguage()->getString("STR_TOTAL_UC").c_str(), ss3.str().c_str());

	if (recoveryY > 0)
	{
		_txtUfoRecovery->setY(_lstStats->getY() + statsY + 5);
		_lstUfoRecovery->setY(_txtUfoRecovery->getY() + 8);
		_lstTotal->setY(_lstUfoRecovery->getY() + recoveryY + 5);
	}
	else
	{
		_txtUfoRecovery->setText(L"");
		_lstTotal->setY(_lstStats->getY() + statsY + 5);
	}

	// Calculate rating
	std::wstring rating = _game->getLanguage()->getString("STR_RATING");
	if (total <= -200)
	{
		rating += _game->getLanguage()->getString("STR_RATING_TERRIBLE");
	}
	else if (total <= 0)
	{
		rating += _game->getLanguage()->getString("STR_RATING_POOR");
	}
	else if (total <= 200)
	{
		rating += _game->getLanguage()->getString("STR_RATING_OK");
	}
	else if (total <= 500)
	{
		rating += _game->getLanguage()->getString("STR_RATING_GOOD");
	}
	else
	{
		rating += _game->getLanguage()->getString("STR_RATING_EXCELLENT");
	}
	_txtRating->setText(rating);

	// Set music
	_game->getResourcePack()->getMusic("GMMARS")->play();
}

/**
 *
 */
DebriefingState::~DebriefingState()
{
	for (std::vector<DebriefingStat*>::iterator i = _stats.begin(); i != _stats.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void DebriefingState::btnOkClick(Action *action)
{
	_game->getSavedGame()->setBattleGame(0);
	_game->popState();
	if (_game->getSavedGame()->handlePromotions())
	{
		_game->pushState(new PromotionsState(_game));
	}
	if (!_missingItems.empty())
	{
		_game->pushState(new CannotReequipState(_game, _missingItems));
	}
}



/**
 * Add to debriefing stats.
 * @param name The untranslated name of the stat.
 * @param quantity The quantity to add.
 * @param score The score to add.
 */
void DebriefingState::addStat(const std::string &name, int quantity, int score)
{
	for (std::vector<DebriefingStat*>::iterator i = _stats.begin(); i != _stats.end(); ++i)
	{
		if ((*i)->item == name)
		{
			(*i)->qty = (*i)->qty + quantity;
			(*i)->score = (*i)->score + score;
			break;
		}
	}
}

/**
 * Prepares debriefing: gathers Aliens, Corpses, Artefacts, UFO Components.
 * Adds the items to the craft.
 * Also calculates the soldiers experience, and possible promotions.
 * If aborted, only the things on the exit area are recovered.
 */
void DebriefingState::prepareDebriefing()
{
	_stats.push_back(new DebriefingStat("STR_ALIENS_KILLED", false));
	_stats.push_back(new DebriefingStat("STR_ALIEN_CORPSES_RECOVERED", false));
	_stats.push_back(new DebriefingStat("STR_LIVE_ALIENS_RECOVERED", false));
	_stats.push_back(new DebriefingStat("STR_ALIEN_ARTIFACTS_RECOVERED", false));
	_stats.push_back(new DebriefingStat("STR_ALIEN_BASE_CONTROL_DESTROYED", false));
	_stats.push_back(new DebriefingStat("STR_CIVILIANS_KILLED_BY_ALIENS", false));
	_stats.push_back(new DebriefingStat("STR_CIVILIANS_KILLED_BY_XCOM_OPERATIVES", false));
	_stats.push_back(new DebriefingStat("STR_CIVILIANS_SAVED", false));
	_stats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_KILLED", false));
	//_stats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_RETIRED_THROUGH_INJURY", false));
	_stats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_MISSING_IN_ACTION", false));
	_stats.push_back(new DebriefingStat("STR_TANKS_DESTROYED", false));
	_stats.push_back(new DebriefingStat("STR_XCOM_CRAFT_LOST", false));
	_stats.push_back(new DebriefingStat("STR_UFO_POWER_SOURCE", true));
	_stats.push_back(new DebriefingStat("STR_UFO_NAVIGATION", true));
	_stats.push_back(new DebriefingStat("STR_UFO_CONSTRUCTION", true));
	_stats.push_back(new DebriefingStat("STR_ALIEN_FOOD", true));
	_stats.push_back(new DebriefingStat("STR_ALIEN_REPRODUCTION", true));
	_stats.push_back(new DebriefingStat("STR_ALIEN_ENTERTAINMENT", true));
	_stats.push_back(new DebriefingStat("STR_ALIEN_SURGERY", true));
	_stats.push_back(new DebriefingStat("STR_EXAMINATION_ROOM", true));
	_stats.push_back(new DebriefingStat("STR_ALIEN_ALLOYS", true));
	_stats.push_back(new DebriefingStat("STR_ELERIUM_115", true));

	SavedGame *save = _game->getSavedGame();
	SavedBattleGame *battle = save->getBattleGame();
	bool aborted = battle->isAborted();
	Craft* craft = 0;
	std::vector<Craft*>::iterator craftIterator;
	Base* base = 0;

	int playerInExitArea = 0; // if this stays 0 the craft is lost...
	int playersSurvived = 0; // if this stays 0 the craft is lost...

	for (std::vector<Base*>::iterator i = save->getBases()->begin(); i != save->getBases()->end(); ++i)
	{
		// in case we have a craft - check which craft it is about
		for (std::vector<Craft*>::iterator j = (*i)->getCrafts()->begin(); j != (*i)->getCrafts()->end(); ++j)
		{
			if ((*j)->isInBattlescape())
			{
				craft = (*j);
				base = (*i);
				craftIterator = j;
				craft->returnToBase();
				craft->setLowFuel(true);
				craft->setInBattlescape(false);
			}
		}
	}

	// UFO crash/landing site disappears
	for (std::vector<Ufo*>::iterator i = save->getUfos()->begin(); i != save->getUfos()->end(); ++i)
	{
		if ((*i)->isInBattlescape())
		{
			delete *i;
			save->getUfos()->erase(i);
			break;
		}
	}

	// terror site disappears
	for (std::vector<TerrorSite*>::iterator i = save->getTerrorSites()->begin(); i != save->getTerrorSites()->end(); ++i)
	{
		if ((*i)->isInBattlescape())
		{
			delete *i;
			save->getTerrorSites()->erase(i);
			break;
		}
	}

	// lets see what happens with units
	for (std::vector<BattleUnit*>::iterator j = battle->getUnits()->begin(); j != battle->getUnits()->end(); ++j)
	{
		UnitStatus status = (*j)->getStatus();
		UnitFaction faction = (*j)->getFaction();
		int value = (*j)->getValue();
		Soldier *soldier = save->getSoldier((*j)->getId());
		std::string type = (*j)->getType();

		if (faction == FACTION_NEUTRAL)
		{
			// civilians are all dead if mission fails
			if (aborted || playersSurvived == 0 || status == STATUS_DEAD)
			{
				addStat("STR_CIVILIANS_KILLED_BY_ALIENS", 1, -value);
			}
			// otherwise save the live ones
			else
			{
				addStat("STR_CIVILIANS_SAVED", 1, value);
			}
		}


		if (status == STATUS_DEAD)
		{
			if (faction == FACTION_HOSTILE)
			{
				addStat("STR_ALIENS_KILLED", 1, value);
			}
			else if (faction == FACTION_PLAYER)
			{
				if (soldier != 0)
				{
					addStat("STR_XCOM_OPERATIVES_KILLED", 1, -value);
					for (std::vector<Soldier*>::iterator i = base->getSoldiers()->begin(); i != base->getSoldiers()->end(); ++i)
					{
						if ((*i) == soldier)
						{
							delete (*i);
							base->getSoldiers()->erase(i);
							break;
						}
					}
				}
				else
				{
					// non soldier player = tank
					addStat("STR_TANKS_DESTROYED", 1, -value);
					for (std::vector<Vehicle*>::iterator i = craft->getVehicles()->begin(); i != craft->getVehicles()->end(); ++i)
					{
						if ((*i)->getRules()->getType() == "STR_" + type)
						{
							delete (*i);
							craft->getVehicles()->erase(i);
							break;
						}
					}
				}
			}
		}
		else if (status == STATUS_UNCONSCIOUS)
		{
			if (faction == FACTION_HOSTILE && (!aborted || (*j)->isInExitArea()))
			{
				addStat("STR_LIVE_ALIENS_RECOVERED", 1, value*2);
			}
		}
		else
		{
			if (faction == FACTION_PLAYER)
			{
				playersSurvived++;
				if ((*j)->isInExitArea() || !aborted)
				{
					playerInExitArea++;
					(*j)->postMissionProcedures(save);
					recoverItems((*j)->getInventory(), base);		
				}
				else
				{
					addStat("STR_XCOM_OPERATIVES_MISSING_IN_ACTION", 1, -value);
					if (soldier != 0)
					{
						for (std::vector<Soldier*>::iterator i = base->getSoldiers()->begin(); i != base->getSoldiers()->end(); ++i)
						{
							if ((*i) == soldier)
							{
								delete (*i);
								base->getSoldiers()->erase(i);
								break;
							}
						}
					}
					else
					{
						// non soldier player = tank
						for (std::vector<Vehicle*>::iterator i = craft->getVehicles()->begin(); i != craft->getVehicles()->end(); ++i)
						{
							if ((*i)->getRules()->getType() == "STR_" + type)
							{
								delete (*i);
								craft->getVehicles()->erase(i);
								break;
							}
						}
					}
				}
			}
		}
	}
	if (((playerInExitArea == 0 && aborted) || (playersSurvived == 0)) && craft != 0)
	{
		addStat("STR_XCOM_CRAFT_LOST", 1, -craft->getRules()->getScore());
		delete craft;
		base->getCrafts()->erase(craftIterator);
		for (std::vector<Soldier*>::iterator i = base->getSoldiers()->begin(); i != base->getSoldiers()->end();)
		{
			if ((*i)->getCraft() == craft)
			{
				delete (*i);
				i = base->getSoldiers()->erase(i);
			}
			else
			{
				 ++i;
			}
		}
		_txtTitle->setText(_game->getLanguage()->getString("STR_CRAFT_IS_LOST"));
		return;
	}

	if (!aborted && playersSurvived > 0) 	// RECOVER UFO : run through all tiles to recover UFO components and items
	{
		if (battle->getMissionType() == "STR_BASE_DEFENSE")
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_BASE_IS_SAVED"));
		}
		else if (battle->getMissionType() == "STR_TERROR_MISSION")
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_ALIENS_DEFEATED"));
		}
		else if (battle->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_ALIEN_BASE_DESTROYED"));
		}
		else
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_IS_RECOVERED"));
		}

		for (int i = 0; i < battle->getHeight() * battle->getLength() * battle->getWidth(); ++i)
		{
			// get recoverable map data objects from the battlescape map
			for (int part = 0; part < 4; part++)
			{
				if (battle->getTiles()[i]->getMapData(part))
				{
					switch (battle->getTiles()[i]->getMapData(part)->getSpecialType())
					{
					case UFO_POWER_SOURCE:
						addStat("STR_UFO_POWER_SOURCE", 1, 20); break;
					case UFO_NAVIGATION:
						addStat("STR_UFO_NAVIGATION", 1, 5); break;
					case UFO_CONSTRUCTION:
						addStat("STR_UFO_CONSTRUCTION", 1, 2); break;
					case ALIEN_FOOD:
						addStat("STR_ALIEN_FOOD", 1, 2); break;
					case ALIEN_REPRODUCTION:
						addStat("STR_ALIEN_REPRODUCTION", 1, 2); break;
					case ALIEN_ENTERTAINMENT:
						addStat("STR_ALIEN_ENTERTAINMENT", 1, 2); break;
					case ALIEN_SURGERY:
						addStat("STR_ALIEN_SURGERY", 1, 2); break;
					case EXAM_ROOM:
						addStat("STR_EXAMINATION_ROOM", 1, 2); break;
					case ALIEN_ALLOYS:
						addStat("STR_ALIEN_ALLOYS", 1, 1); break;
					case ALIEN_HABITAT:
						addStat("STR_ALIEN_HABITAT", 1, 1); break;
					case MUST_DESTROY: break; // this is the brain
					default: break;
					}

				}
			}
			// recover items from the floor
			recoverItems(battle->getTiles()[i]->getInventory(), base);		
		}

		int aadivider = battle->getMissionType()=="STR_ALIEN_BASE_ASSAULT"?150:10;
		for (std::vector<DebriefingStat*>::iterator i = _stats.begin(); i != _stats.end(); ++i)
		{
			// alien alloys recovery values are divided by 10 or divided by 150 in case of an alien base
			if ((*i)->item == "STR_ALIEN_ALLOYS")
			{
				(*i)->qty = (*i)->qty / aadivider;
				(*i)->score = (*i)->score / aadivider;
			}

			// recoverable battlescape tiles are now converted to items and put in base inventory
			if ((*i)->recovery && (*i)->qty > 0)
			{
				base->getItems()->addItem((*i)->item, (*i)->qty);
			}
		}
	}
	else
	{
		if (battle->getMissionType() == "STR_BASE_DEFENSE")
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_BASE_IS_LOST"));
		}
		else if (battle->getMissionType() == "STR_TERROR_MISSION")
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_TERROR_CONTINUES"));
		}
		else if (battle->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_ALIEN_BASE_STILL_INTACT"));
		}
		else
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_IS_NOT_RECOVERED"));
		}

		if (playersSurvived > 0)
		{
			// recover items from the craft floor
			for (int i = 0; i < battle->getHeight() * battle->getLength() * battle->getWidth(); ++i)
			{
				if (battle->getTiles()[i]->getMapData(MapData::O_FLOOR) && (battle->getTiles()[i]->getMapData(MapData::O_FLOOR)->getSpecialType() == START_POINT))
					recoverItems(battle->getTiles()[i]->getInventory(), base);		
			}

		}
	}

	// reequip crafts
	if (craft)
	{
		reequipCraft(base, craft);
	}
	else
	{
		for (std::vector<Craft*>::iterator c = base->getCrafts()->begin(); c != base->getCrafts()->end(); ++c)
		{
			if ((*c)->getStatus() != "STR_OUT")
				reequipCraft(base, *c);
		}
	}
}

void DebriefingState::reequipCraft(Base *base, Craft *craft)
{
	std::map<std::string, int> craftItems = *craft->getItems()->getContents();
	for (std::map<std::string, int>::iterator i = craftItems.begin(); i != craftItems.end(); ++i)
	{
		int qty = base->getItems()->getItem(i->first);
		if (qty >= i->second)
		{
			base->getItems()->removeItem(i->first, i->second);
		}
		else
		{
			int missing = i->second - qty;
			base->getItems()->removeItem(i->first, qty);
			craft->getItems()->removeItem(i->first, missing);
			ReequipStat stat = {i->first, missing, craft->getName(_game->getLanguage())};
			_missingItems.push_back(stat);
		}
	}
}

/* converts battlescape inventory into geoscape itemcontainer */
void DebriefingState::recoverItems(std::vector<BattleItem*> *from, Base *base)
{
	std::map<RuleItem*, int> rounds;
	for (std::vector<BattleItem*>::iterator it = from->begin(); it != from->end(); ++it)
	{
		if ((*it)->getRules()->getName() == "STR_ELERIUM_115")
		{
			// special case of an item counted as a stat
			addStat("STR_ELERIUM_115", 50, 5);
		}
		else
		{
			// only "recover" unresearched items
			if ((*it)->getRules()->getRecoveryPoints() && !(*it)->getXCOMProperty() && _game->getSavedGame()->isResearched((*it)->getRules()->getRequirements()))
			{
				if ((*it)->getRules()->getBattleType() == BT_CORPSE && (*it)->getUnit()->getStatus() == STATUS_DEAD)
				{
					addStat("STR_ALIEN_CORPSES_RECOVERED", 1, (*it)->getRules()->getRecoveryPoints());
				}
				else
				{
					addStat("STR_ALIEN_ARTIFACTS_RECOVERED", 1, (*it)->getRules()->getRecoveryPoints());
				}
			}
			// put items back in the base
			if ((*it)->getRules()->isRecoverable() && !(*it)->getRules()->isFixed())
			{
				switch ((*it)->getRules()->getBattleType())
				{
					case BT_AMMO:
						// It's a clip, count any rounds left.
						rounds[(*it)->getRules()] += (*it)->getAmmoQuantity();
						break;
					case BT_FIREARM:
					case BT_MELEE:
						// It's a weapon, count any rounds left in the clip.
						{
							BattleItem *clip = (*it)->getAmmoItem();
							if (clip && (*it)->getRules()->getClipSize() != -1)
							{
								rounds[clip->getRules()] += clip->getAmmoQuantity();
							}
						}
						// Fall-through, to recover the weapon itself.
					default:
						base->getItems()->addItem((*it)->getRules()->getType(), 1);
				}
			}
		}
	}

	// Now calculate the clips for each type based on the recovered rounds.
	for (std::map<RuleItem*, int>::const_iterator rl = rounds.begin(); rl != rounds.end(); ++rl)
	{
		//Count half-full clips as full.
		int total_clips = (rl->second + rl->first->getClipSize()/2) / rl->first->getClipSize();
		base->getItems()->addItem(rl->first->getType(), total_clips);
	}
}


}
