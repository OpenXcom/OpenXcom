/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "Inventory.h"
#include <cmath>
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleInventory.h"
#include "../Engine/Palette.h"
#include "../Engine/Game.h"
#include "../Interface/Text.h"
#include "../Interface/NumberText.h"
#include "../Engine/Font.h"
#include "../Engine/Language.h"
#include "../Engine/Options.h"
#include "../Resource/ResourcePack.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/SurfaceSet.h"
#include "../Savegame/BattleItem.h"
#include "../Ruleset/RuleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Engine/Action.h"
#include "../Engine/Sound.h"
#include "WarningMessage.h"
#include "../Savegame/Tile.h"
#include "PrimeGrenadeState.h"

namespace OpenXcom
{

/**
 * Sets up an inventory with the specified size and position.
 * @param game Pointer to core game.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Inventory::Inventory(Game *game, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _game(game), _selUnit(0), _selItem(0), _tu(true), _groundOffset(0)
{
	_grid = new Surface(width, height, x, y);
	_items = new Surface(width, height, x, y);
	_selection = new Surface(RuleInventory::HAND_W * RuleInventory::SLOT_W, RuleInventory::HAND_H * RuleInventory::SLOT_H, x, y);
	_warning = new WarningMessage(224, 24, 48, 176);
	_stackNumber = new NumberText(15, 15, 0, 0);

	_warning->setFonts(_game->getResourcePack()->getFont("Big.fnt"), _game->getResourcePack()->getFont("Small.fnt"));
	_warning->setColor(Palette::blockOffset(2));
	_warning->setTextColor(Palette::blockOffset(1)-1);
}

/**
 * Delete inventory surfaces.
 */
Inventory::~Inventory()
{
	delete _grid;
	delete _items;
	delete _selection;
	delete _warning;
	delete _stackNumber;
}

/**
 * Replaces a certain amount of colors in the inventory's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Inventory::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_grid->setPalette(colors, firstcolor, ncolors);
	_items->setPalette(colors, firstcolor, ncolors);
	_selection->setPalette(colors, firstcolor, ncolors);
	_warning->setPalette(colors, firstcolor, ncolors);
	_stackNumber->setPalette(getPalette());
}

/**
 * Changes the inventory's Time Units mode.
 * When True, inventory actions cost soldier time units (for battle).
 * When False, inventory actions don't cost anything (for pre-equip).
 * @param tu Time Units mode.
 */
void Inventory::setTuMode(bool tu)
{
	_tu = tu;
}

/**
 * Changes the unit to display the inventory of.
 * @param unit Pointer to battle unit.
 */
void Inventory::setSelectedUnit(BattleUnit *unit)
{
	_selUnit = unit;
	_groundOffset = 999;
	arrangeGround();
}

/**
 * Draws the inventory elements.
 */
void Inventory::draw()
{
	drawGrid();
	drawItems();
}

/**
 * Draws the inventory grid for item placement.
 */
void Inventory::drawGrid()
{
	_grid->clear();
	Text text = Text(80, 9, 0, 0);
	text.setPalette(_grid->getPalette());
	text.setFonts(_game->getResourcePack()->getFont("Big.fnt"), _game->getResourcePack()->getFont("Small.fnt"));
	text.setColor(Palette::blockOffset(4)-1);
	text.setHighContrast(true);

	Uint8 color = Palette::blockOffset(0)+8;
	for (std::map<std::string, RuleInventory*>::iterator i = _game->getRuleset()->getInventories()->begin(); i != _game->getRuleset()->getInventories()->end(); ++i)
	{
		// Draw grid
		if (i->second->getType() == INV_SLOT)
		{
			for (std::vector<RuleSlot>::iterator j = i->second->getSlots()->begin(); j != i->second->getSlots()->end(); ++j)
			{
				SDL_Rect r;
				r.x = i->second->getX() + RuleInventory::SLOT_W * j->x;
				r.y = i->second->getY() + RuleInventory::SLOT_H * j->y;
				r.w = RuleInventory::SLOT_W + 1;
				r.h = RuleInventory::SLOT_H + 1;
				_grid->drawRect(&r, color);
				r.x++;
				r.y++;
				r.w -= 2;
				r.h -= 2;
				_grid->drawRect(&r, 0);
			}
		}
		else if (i->second->getType() == INV_HAND)
		{
			SDL_Rect r;
			r.x = i->second->getX();
			r.y = i->second->getY();
			r.w = RuleInventory::HAND_W * RuleInventory::SLOT_W;
			r.h = RuleInventory::HAND_H * RuleInventory::SLOT_H;
			_grid->drawRect(&r, color);
			r.x++;
			r.y++;
			r.w -= 2;
			r.h -= 2;
			_grid->drawRect(&r, 0);
		}
		else if (i->second->getType() == INV_GROUND)
		{
			for (int x = i->second->getX(); x <= 320; x += RuleInventory::SLOT_W)
			{
				for (int y = i->second->getY(); y <= 200; y += RuleInventory::SLOT_H)
				{
					SDL_Rect r;
					r.x = x;
					r.y = y;
					r.w = RuleInventory::SLOT_W + 1;
					r.h = RuleInventory::SLOT_H + 1;
					_grid->drawRect(&r, color);
					r.x++;
					r.y++;
					r.w -= 2;
					r.h -= 2;
					_grid->drawRect(&r, 0);
				}
			}
		}

		// Draw label
		text.setX(i->second->getX());
		text.setY(i->second->getY() - text.getFont()->getHeight() - text.getFont()->getSpacing());
		text.setText(_game->getLanguage()->getString(i->second->getId()));
		text.blit(_grid);
	}
}

/**
 * Draws the items contained in the soldier's inventory.
 */
void Inventory::drawItems()
{
	_items->clear();
	if (_selUnit != 0)
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BIGOBS.PCK");
		// Soldier items
		for (std::vector<BattleItem*>::iterator i = _selUnit->getInventory()->begin(); i != _selUnit->getInventory()->end(); ++i)
		{
			if ((*i) == _selItem)
				continue;

			Surface *frame = texture->getFrame((*i)->getRules()->getBigSprite());
			if ((*i)->getSlot()->getType() == INV_SLOT)
			{
				frame->setX((*i)->getSlot()->getX() + (*i)->getSlotX() * RuleInventory::SLOT_W);
				frame->setY((*i)->getSlot()->getY() + (*i)->getSlotY() * RuleInventory::SLOT_H);
			}
			else if ((*i)->getSlot()->getType() == INV_HAND)
			{
				frame->setX((*i)->getSlot()->getX() + (RuleInventory::HAND_W - (*i)->getRules()->getInventoryWidth()) * RuleInventory::SLOT_W/2);
				frame->setY((*i)->getSlot()->getY() + (RuleInventory::HAND_H - (*i)->getRules()->getInventoryHeight()) * RuleInventory::SLOT_H/2);
			}
			texture->getFrame((*i)->getRules()->getBigSprite())->blit(_items);
		}
		Surface *stackLayer = new Surface(getWidth(), getHeight(), 0, 0);
		stackLayer->setPalette(getPalette());
		// Ground items
		for (std::vector<BattleItem*>::iterator i = _selUnit->getTile()->getInventory()->begin(); i != _selUnit->getTile()->getInventory()->end(); ++i)
		{
			// note that you can make items invisible by setting their width or height to 0 (for example used with tank corpse items)
			if ((*i) == _selItem || (*i)->getSlotX() < _groundOffset || (*i)->getRules()->getInventoryHeight() == 0 || (*i)->getRules()->getInventoryWidth() == 0)
				continue;
			Surface *frame = texture->getFrame((*i)->getRules()->getBigSprite());
			frame->setX((*i)->getSlot()->getX() + ((*i)->getSlotX() - _groundOffset) * RuleInventory::SLOT_W);
			frame->setY((*i)->getSlot()->getY() + (*i)->getSlotY() * RuleInventory::SLOT_H);
			texture->getFrame((*i)->getRules()->getBigSprite())->blit(_items);
			if (_stackLevel[(*i)->getSlotX()][(*i)->getSlotY()] > 1)
			{
				_stackNumber->setX(((*i)->getSlot()->getX() + (((*i)->getSlotX() + (*i)->getRules()->getInventoryWidth()) - _groundOffset) * RuleInventory::SLOT_W)-3);
				if (_stackLevel[(*i)->getSlotX()][(*i)->getSlotY()] > 9)
				{
					_stackNumber->setX(_stackNumber->getX()-4);
				}
				_stackNumber->setY(((*i)->getSlot()->getY() + ((*i)->getSlotY() + (*i)->getRules()->getInventoryHeight()) * RuleInventory::SLOT_H)-5);
				_stackNumber->setValue(_stackLevel[(*i)->getSlotX()][(*i)->getSlotY()]);
				_stackNumber->draw();
				_stackNumber->setColor(Palette::blockOffset(4)+2);
				_stackNumber->blit(stackLayer);
			}
		}

		// give it a border
		// this is the "darker" shade that goes in the corners.
		for (int x = -1; x <= 1; x += 2)
		{
			for (int y = -1; y <= 1; y += 2)
			{
				stackLayer->blitNShade(_items, x, y, 11);
			}
		}
		// this is the "slightly darker" version that goes in four cardinals.
		for (int z = -1; z <= 1; z += 2)
		{
			stackLayer->blitNShade(_items, z, 0, 8);
			stackLayer->blitNShade(_items, 0, z, 8);
		}
		// and finally the number itself
		stackLayer->blit(_items);
		delete stackLayer;
	}
}

/**
 * Moves an item to a specified slot in the
 * selected player's inventory.
 * @param item Pointer to battle item.
 * @param slot Inventory slot, or NULL if none.
 * @param x X position in slot.
 * @param y Y position in slot.
 */
void Inventory::moveItem(BattleItem *item, RuleInventory *slot, int x, int y)
{
	// Make items vanish (eg. ammo in weapons)
	if (slot == 0)
	{
		if (item->getSlot()->getType() == INV_GROUND)
		{
			_selUnit->getTile()->removeItem(item);
		}
		else
		{
			item->moveToOwner(0);
		}
	}
	else
	{
		// Handle dropping from/to ground.
		if (slot != item->getSlot())
		{
			if (slot->getType() == INV_GROUND)
			{
				item->moveToOwner(0);
				_selUnit->getTile()->addItem(item, item->getSlot());
				if (item->getUnit() && item->getUnit()->getStatus() == STATUS_UNCONSCIOUS)
				{
					item->getUnit()->setPosition(_selUnit->getPosition());
				}
			}
			else if (item->getSlot() == 0 || item->getSlot()->getType() == INV_GROUND)
			{
				item->moveToOwner(_selUnit);
				_selUnit->getTile()->removeItem(item);
				item->setTurnFlag(false);
				if (item->getUnit() && item->getUnit()->getStatus() == STATUS_UNCONSCIOUS)
				{
					item->getUnit()->setPosition(Position(-1,-1,-1));
				}
			}
		}
		item->setSlot(slot);
		item->setSlotX(x);
		item->setSlotY(y);
	}
}

/**
 * Checks if an item in a certain slot position would
 * overlap with any other inventory item.
 * @param item Pointer to battle item.
 * @param slot Inventory slot, or NULL if none.
 * @param x X position in slot.
 * @param y Y position in slot.
 * @return If there's overlap.
 */
bool Inventory::overlapItems(BattleItem *item, RuleInventory *slot, int x, int y) const
{
	if (slot->getType() != INV_GROUND)
	{
		for (std::vector<BattleItem*>::const_iterator i = _selUnit->getInventory()->begin(); i != _selUnit->getInventory()->end(); ++i)
		{
			if ((*i)->getSlot() == slot && (*i)->occupiesSlot(x, y, item))
			{
				return true;
			}
		}
	}
	else if (_selUnit->getTile() != 0)
	{
		for (std::vector<BattleItem*>::const_iterator i = _selUnit->getTile()->getInventory()->begin(); i != _selUnit->getTile()->getInventory()->end(); ++i)
		{
			if ((*i)->occupiesSlot(x, y, item))
			{
				return true;
			}
		}
	}
	return false;
}

/**
 * Gets the inventory slot located in the specified mouse position.
 * @param x Mouse X position. Returns the slot's X position.
 * @param y Mouse Y position. Returns the slot's Y position.
 * @return Slot rules, or NULL if none.
 */
RuleInventory *Inventory::getSlotInPosition(int *x, int *y) const
{
	for (std::map<std::string, RuleInventory*>::iterator i = _game->getRuleset()->getInventories()->begin(); i != _game->getRuleset()->getInventories()->end(); ++i)
	{
		if (i->second->checkSlotInPosition(x, y))
		{
			return i->second;
		}
	}
	return 0;
}

/**
 * Returns the item currently grabbed by the player.
 * @return Pointer to selected item, or NULL if none.
 */
BattleItem *Inventory::getSelectedItem() const
{
	return _selItem;
}

/**
 * Changes the item currently grabbed by the player.
 * @param item Pointer to selected item, or NULL if none.
 */
void Inventory::setSelectedItem(BattleItem *item)
{
	_selItem = (item && !item->getRules()->isFixed()) ? item : 0;
	if (_selItem == 0)
	{
		_selection->clear();
	}
	else
	{
		if (_selItem->getSlot()->getType() == INV_GROUND)
		{
			_stackLevel[_selItem->getSlotX()][_selItem->getSlotY()] -= 1;
		}
		_selItem->getRules()->drawHandSprite(_game->getResourcePack()->getSurfaceSet("BIGOBS.PCK"), _selection);
	}
	drawItems();
}

/**
 * Handle timers.
 */
void Inventory::think()
{
	_warning->think();
}

/**
 * Blits the inventory elements.
 * @param surface Pointer to surface to blit onto.
 */
void Inventory::blit(Surface *surface)
{
	clear();
	_grid->blit(this);
	_items->blit(this);
	_selection->blit(this);
	_warning->blit(this);
	Surface::blit(surface);
}

/**
 * Moves the selected item.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Inventory::mouseOver(Action *action, State *state)
{
	_selection->setX((int)floor(action->getAbsoluteXMouse()) - _selection->getWidth()/2 - _dx);
	_selection->setY((int)floor(action->getAbsoluteYMouse()) - _selection->getHeight()/2 - _dy);
	InteractiveSurface::mouseOver(action, state);
}

/**
 * Picks up / drops an item.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Inventory::mouseClick(Action *action, State *state)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (_selUnit == 0)
			return;
		// Pickup item
		if (_selItem == 0)
		{
			int x = (int)floor(action->getAbsoluteXMouse()) - _dx,
				y = (int)floor(action->getAbsoluteYMouse()) - _dy;
			RuleInventory *slot = getSlotInPosition(&x, &y);
			if (slot != 0)
			{
				if (slot->getType() == INV_GROUND)
				{
					x += _groundOffset;
				}
				BattleItem *item = _selUnit->getItem(slot, x, y);
				if (item != 0)
				{
					if ((SDL_GetModState() & KMOD_CTRL))
					{
						RuleInventory *newSlot = _game->getRuleset()->getInventory("STR_GROUND");
						std::string warning = "STR_NOT_ENOUGH_SPACE";
						bool placed = false;

						if (slot->getType() == INV_GROUND)
						{
							switch (item->getRules()->getBattleType())
							{
							case BT_FIREARM:
								newSlot = _game->getRuleset()->getInventory("STR_RIGHT_HAND");
								break;
							case BT_MINDPROBE:
							case BT_PSIAMP:
							case BT_MELEE:
							case BT_CORPSE:
								newSlot = _game->getRuleset()->getInventory("STR_LEFT_HAND");
								break;
							default:
								if (item->getRules()->getInventoryHeight() > 2)
								{
									newSlot = _game->getRuleset()->getInventory("STR_BACK_PACK");
								}
								else
								{
									newSlot = _game->getRuleset()->getInventory("STR_BELT");
								}
								break;
							}
						}

						if (newSlot->getType() != INV_GROUND)
						{
							_stackLevel[item->getSlotX()][item->getSlotY()] -= 1;

							placed = fitItem(newSlot, item, warning);

							if (!placed)
							{
								for (std::map<std::string, RuleInventory *>::const_iterator wildCard = _game->getRuleset()->getInventories()->begin(); wildCard != _game->getRuleset()->getInventories()->end() && !placed; ++wildCard)
								{
									newSlot = wildCard->second;
									if (newSlot->getType() == INV_GROUND)
									{
										continue;
									}
									placed = fitItem(newSlot, item, warning);
								}
							}
							if (!placed)
							{
								_stackLevel[item->getSlotX()][item->getSlotY()] += 1;
							}
						}
						else
						{
							if (!_tu || _selUnit->spendTimeUnits(item->getSlot()->getCost(newSlot)))
							{
								placed = true;
								moveItem(item, newSlot, 0, 0);
								_game->getResourcePack()->getSound("BATTLE.CAT", 38)->play();
								arrangeGround(false);
							}
							else
							{
								warning = "STR_NOT_ENOUGH_TIME_UNITS";
							}
						}

						if (!placed)
						{
							_warning->showMessage(_game->getLanguage()->getString(warning));
						}
					}
					else
					{
						setSelectedItem(item);
						if (item->getExplodeTurn() > 0)
						{
							_warning->showMessage(_game->getLanguage()->getString("STR_GRENADE_IS_ACTIVATED"));
						}
					}
				}
			}
		}
		// Drop item
		else
		{
			int x = _selection->getX() + (RuleInventory::HAND_W - _selItem->getRules()->getInventoryWidth()) * RuleInventory::SLOT_W/2 + RuleInventory::SLOT_W/2,
				y = _selection->getY() + (RuleInventory::HAND_H - _selItem->getRules()->getInventoryHeight()) * RuleInventory::SLOT_H/2 + RuleInventory::SLOT_H/2;
			RuleInventory *slot = getSlotInPosition(&x, &y);
			if (slot != 0)
			{
				if (slot->getType() == INV_GROUND)
				{
					x += _groundOffset;
				}
				BattleItem *item = _selUnit->getItem(slot, x, y);

				bool canStack = slot->getType() == INV_GROUND && canBeStacked(item, _selItem);

				// Put item in empty slot, or stack it, if possible.
				if (item == 0 || item == _selItem || canStack)
				{
					if (!overlapItems(_selItem, slot, x, y) && slot->fitItemInSlot(_selItem->getRules(), x, y))
					{
						if (!_tu || _selUnit->spendTimeUnits(_selItem->getSlot()->getCost(slot)))
						{
							moveItem(_selItem, slot, x, y);
							if (slot->getType() == INV_GROUND)
							{
								_stackLevel[x][y] += 1;
							}
							setSelectedItem(0);
							_game->getResourcePack()->getSound("BATTLE.CAT", 38)->play();
						}
						else
						{
							_warning->showMessage(_game->getLanguage()->getString("STR_NOT_ENOUGH_TIME_UNITS"));
						}
					}
					else if (canStack)
					{
						if (!_tu || _selUnit->spendTimeUnits(_selItem->getSlot()->getCost(slot)))
						{
							moveItem(_selItem, slot, item->getSlotX(), item->getSlotY());
							_stackLevel[item->getSlotX()][item->getSlotY()] += 1;
							setSelectedItem(0);
							_game->getResourcePack()->getSound("BATTLE.CAT", 38)->play();
						}
						else
						{
							_warning->showMessage(_game->getLanguage()->getString("STR_NOT_ENOUGH_TIME_UNITS"));
						}
					}
				}
				// Put item in weapon
				else
				{
					bool wrong = true;
					for (std::vector<std::string>::iterator i = item->getRules()->getCompatibleAmmo()->begin(); i != item->getRules()->getCompatibleAmmo()->end(); ++i)
					{
						if ((*i) == _selItem->getRules()->getType())
						{
							wrong = false;
							break;
						}
					}
					if (wrong)
					{
						_warning->showMessage(_game->getLanguage()->getString("STR_WRONG_AMMUNITION_FOR_THIS_WEAPON"));
					}
					else
					{
						if (item->getAmmoItem() != 0)
						{
							_warning->showMessage(_game->getLanguage()->getString("STR_WEAPON_IS_ALREADY_LOADED"));
						}
						else if (!_tu || _selUnit->spendTimeUnits(15))
						{
							moveItem(_selItem, 0, 0, 0);
							item->setAmmoItem(_selItem);
							_selItem->moveToOwner(0);
							setSelectedItem(0);
							_game->getResourcePack()->getSound("BATTLE.CAT", 17)->play();
							if (item->getSlot()->getType() == INV_GROUND)
							{
								arrangeGround(false);
							}
						}
						else
						{
							_warning->showMessage(_game->getLanguage()->getString("STR_NOT_ENOUGH_TIME_UNITS"));
						}
					}
				}
			}
			else
			{
				// try again, using the position of the mouse cursor, not the item (slightly more intuitive for stacking)
				x = (int)floor(action->getAbsoluteXMouse())-_dx;
				y = (int)floor(action->getAbsoluteYMouse())-_dy;
				slot = getSlotInPosition(&x, &y);
				if (slot != 0 && slot->getType() == INV_GROUND)
				{
					x += _groundOffset;
					BattleItem *item = _selUnit->getItem(slot, x, y);
					if (canBeStacked(item, _selItem))
					{
						if (!_tu || _selUnit->spendTimeUnits(_selItem->getSlot()->getCost(slot)))
						{
							moveItem(_selItem, slot, item->getSlotX(), item->getSlotY());
							_stackLevel[item->getSlotX()][item->getSlotY()] += 1;
							setSelectedItem(0);
							_game->getResourcePack()->getSound("BATTLE.CAT", 38)->play();
						}
						else
						{
							_warning->showMessage(_game->getLanguage()->getString("STR_NOT_ENOUGH_TIME_UNITS"));
						}
					}
				}
			}
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if (_selItem == 0)
		{
			if (!_tu)
			{
				int x = (int)floor(action->getAbsoluteXMouse()) - _dx,
					y = (int)floor(action->getAbsoluteYMouse()) - _dy;
				RuleInventory *slot = getSlotInPosition(&x, &y);
				if (slot != 0)
				{
					if (slot->getType() == INV_GROUND)
					{
						x += _groundOffset;
					}
					BattleItem *item = _selUnit->getItem(slot, x, y);
					if (item != 0)
					{
						BattleType itemType = item->getRules()->getBattleType();
						if (BT_GRENADE == itemType || BT_PROXIMITYGRENADE == itemType)
						{
							if (0 == item->getExplodeTurn())
							{
								// Prime that grenade!
								if (BT_PROXIMITYGRENADE == itemType) item->setExplodeTurn(1);
								else _game->pushState(new PrimeGrenadeState(_game, 0, true, item));
							}
							else item->setExplodeTurn(0);  // Unprime the grenade
						}
					}
				}
			}
		}
		else
		{
			if (_selItem->getSlot()->getType() == INV_GROUND)
			{
				_stackLevel[_selItem->getSlotX()][_selItem->getSlotY()] += 1;
			}
			// Return item to original position
			setSelectedItem(0);
		}
	}
	InteractiveSurface::mouseClick(action, state);
}

/**
 * Unloads the selected weapon, placing the gun
 * on the right hand and the ammo on the left hand.
 * @return The success of the weapon being unlaoded
 */
bool Inventory::unload()
{
	// Hands must be free
	for (std::vector<BattleItem*>::iterator i = _selUnit->getInventory()->begin(); i != _selUnit->getInventory()->end(); ++i)
	{
		if ((*i)->getSlot()->getType() == INV_HAND && (*i) != _selItem)
			return false;
	}

	if (!_tu || _selUnit->spendTimeUnits(8))
	{
		moveItem(_selItem->getAmmoItem(), _game->getRuleset()->getInventory("STR_LEFT_HAND"), 0, 0);
		_selItem->getAmmoItem()->moveToOwner(_selUnit);
		moveItem(_selItem, _game->getRuleset()->getInventory("STR_RIGHT_HAND"), 0, 0);
		_selItem->moveToOwner(_selUnit);
		_selItem->setAmmoItem(0);
		setSelectedItem(0);
	}
	else
	{
		_warning->showMessage(_game->getLanguage()->getString("STR_NOT_ENOUGH_TIME_UNITS"));
		return false;
	}

	return true;
}

/**
 * Arranges items on the ground for the inventory display.
 * Since items on the ground aren't assigned to anyone,
 * they don't actually have permanent slot positions.
 */
void Inventory::arrangeGround(bool alterOffset)
{
	RuleInventory *ground = _game->getRuleset()->getInventory("STR_GROUND");

	int slotsX = (320 - ground->getX()) / RuleInventory::SLOT_W;
	int slotsY = (200 - ground->getY()) / RuleInventory::SLOT_H;
	int x = 0;
	int y = 0;
	bool ok = false;
	int xMax = 0;
	_stackLevel.clear();

	if (_selUnit != 0)
	{
		// first move all items out of the way - a big number in X direction
		for (std::vector<BattleItem*>::iterator i = _selUnit->getTile()->getInventory()->begin(); i != _selUnit->getTile()->getInventory()->end(); ++i)
		{
			(*i)->setSlot(ground);
			(*i)->setSlotX(1000000);
			(*i)->setSlotY(0);
		}

		// now for each item, find the most topleft position that is not occupied and will fit
		for (std::vector<BattleItem*>::iterator i = _selUnit->getTile()->getInventory()->begin(); i != _selUnit->getTile()->getInventory()->end(); ++i)
		{
			x = 0;
			y = 0;
			ok = false;
			while (!ok)
			{
				ok = true; // assume we can put the item here, if one of the following checks fails, we can't.
				for (int xd = 0; xd < (*i)->getRules()->getInventoryWidth() && ok; xd++)
				{
					if ((x + xd) % slotsX < x % slotsX)
					{
						ok = false;
					}
					else
					{
						for (int yd = 0; yd < (*i)->getRules()->getInventoryHeight() && ok; yd++)
						{
							BattleItem *item = _selUnit->getItem(ground, x + xd, y + yd);
							ok = item == 0;
							if (canBeStacked(item, *i))
							{
								ok = true;
							}
						}
					}
				}
				if (ok)
				{
					(*i)->setSlotX(x);
					(*i)->setSlotY(y);
					// only increase the stack level if the item is actually visible.
					if ((*i)->getRules()->getInventoryWidth())
					{
						_stackLevel[x][y] += 1;
					}
					xMax = std::max(xMax, x + (*i)->getRules()->getInventoryWidth());
				}
				else
				{
					y++;
					if (y > slotsY - (*i)->getRules()->getInventoryHeight())
					{
						y = 0;
						x++;
					}
				}
			}
		}
	}
	if (alterOffset)
	{
		if (xMax >= _groundOffset + slotsX - 1)
		{
			_groundOffset += slotsX - 1;
		}
		else
		{
			_groundOffset = 0;
		}
	}
	drawItems();
}

bool Inventory::fitItem(RuleInventory *newSlot, BattleItem *item, std::string &warning)
{
	bool placed = false;
	for (int y2 = 0; y2 <= newSlot->getY() / RuleInventory::SLOT_H && !placed; ++y2)
	{
		for (int x2 = 0; x2 <= newSlot->getX() / RuleInventory::SLOT_W && !placed; ++x2)
		{
			if (!overlapItems(item, newSlot, x2, y2) && newSlot->fitItemInSlot(item->getRules(), x2, y2))
			{
				if (!_tu || _selUnit->spendTimeUnits(item->getSlot()->getCost(newSlot)))
				{
					placed = true;
					moveItem(item, newSlot, x2, y2);
					_game->getResourcePack()->getSound("BATTLE.CAT", 38)->play();
					drawItems();
				}
				else
				{
					warning = "STR_NOT_ENOUGH_TIME_UNITS";
				}
			}
		}
	}
	return placed;
}

/**
 * check if two items can be stacked on one another
 *
 */
bool Inventory::canBeStacked(BattleItem *itemA, BattleItem *itemB)
{
		//both items actually exist
	return (itemA != 0 && itemB != 0 &&
		//both items have the same ruleset
		itemA->getRules() == itemB->getRules() &&
		// either they both have no ammo
		((!itemA->getAmmoItem() && !itemB->getAmmoItem()) ||
		// or they both have ammo
		(itemA->getAmmoItem() && itemB->getAmmoItem() &&
		// and the same ammo type
		itemA->getAmmoItem()->getRules() == itemB->getAmmoItem()->getRules() &&
		// and the same ammo quantity
		itemA->getAmmoItem()->getAmmoQuantity() == itemB->getAmmoItem()->getAmmoQuantity())) &&
		// and neither is set to explode
		itemA->getExplodeTurn() == 0 && itemB->getExplodeTurn() == 0 &&
		// and neither is a corpse or unconscious unit
		itemA->getUnit() == 0 && itemB->getUnit() == 0);

}
}
