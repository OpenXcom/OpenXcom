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

#include "Ufopaedia.h"
#include "UfopaediaStartState.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/ArticleDefinition.h"
#include "ArticleState.h"
#include "ArticleStateBaseFacility.h"
#include "ArticleStateCraft.h"
#include "ArticleStateCraftWeapon.h"
#include "ArticleStateItem.h"
#include "ArticleStateArmor.h"
#include "ArticleStateText.h"
#include "ArticleStateTextImage.h"
#include "ArticleStateUfo.h"
#include "ArticleStateVehicle.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"

// these are for runStandalone...
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{
	size_t Ufopaedia::_current_index = 0;

	/**
	 * Adds a new article to the visible list, mainly after a successful research.
	 * @param game Pointer to actual game.
	 * @param article_id Article id to release.
	 */
	void Ufopaedia::releaseArticle(Game *game, std::string &article_id)
	{
		// TODO: get definition from Ruleset and add it to UPSaved...
	}

	/**
	 * Checks, if an article has already been released.
	 * @param game Pointer to actual game.
	 * @param article_id Article id to release.
	 * @returns true, if the article is available.
	 */
	bool Ufopaedia::isArticleAvailable(Game *game, std::string &article_id)
	{
		return game->getSavedGame()->isResearched(article_id);
	}

	/**
	 * Gets the index of the selected article_id in the visible list.
	 * If the id is not found, returns -1.
	 * @param article_id Article id to find.
	 * @returns Index of the given article id in the internal list, -1 if not found.
	 */
	size_t Ufopaedia::getArticleIndex(Game *game, const std::string &article_id)
	{
		ArticleDefinitionList articles = getAvailableArticles(game);
		for (size_t it=0; it<articles.size(); ++it)
		{
			if (articles[it]->id == article_id)
			{
				return it;
			}
		}
		return -1;
	}

	/**
	 * Creates a new article state dependent on the given article definition.
	 * @param game Pointer to actual game.
	 * @param article Article definition to create from.
	 * @returns Article state object if created, 0 otherwise.
	 */
	ArticleState *Ufopaedia::createArticleState(Game *game, ArticleDefinition *article)
	{
		switch(article->getType())
		{
			case UFOPAEDIA_TYPE_CRAFT:
				return new ArticleStateCraft(game, static_cast<ArticleDefinitionCraft *> (article));
				break;
			case UFOPAEDIA_TYPE_CRAFT_WEAPON:
				return new ArticleStateCraftWeapon(game, static_cast<ArticleDefinitionCraftWeapon *> (article));
				break;
			case UFOPAEDIA_TYPE_VEHICLE:
				return new ArticleStateVehicle(game, static_cast<ArticleDefinitionVehicle *> (article));
				break;
			case UFOPAEDIA_TYPE_ITEM:
				return new ArticleStateItem(game, static_cast<ArticleDefinitionItem *> (article));
				break;
			case UFOPAEDIA_TYPE_ARMOR:
				return new ArticleStateArmor(game, static_cast<ArticleDefinitionArmor *> (article));
				break;
			case UFOPAEDIA_TYPE_BASE_FACILITY:
				return new ArticleStateBaseFacility(game, static_cast<ArticleDefinitionBaseFacility *> (article));
				break;
			case UFOPAEDIA_TYPE_TEXT:
				return new ArticleStateText(game, static_cast<ArticleDefinitionText *> (article));
				break;
			case UFOPAEDIA_TYPE_TEXTIMAGE:
				return new ArticleStateTextImage(game, static_cast<ArticleDefinitionTextImage *> (article));
				break;
			case UFOPAEDIA_TYPE_UFO:
				return new ArticleStateUfo(game, static_cast<ArticleDefinitionUfo *> (article));
				break;
			default: break;
		}
		return 0;
	}

	/**
	 * Set UPSaved index and open the new state.
	 * @param game Pointer to actual game.
	 * @param article Article definition of the article to open.
	 */
	void Ufopaedia::openArticle(Game *game, ArticleDefinition *article)
	{
		_current_index = getArticleIndex(game, article->id);
		game->pushState(createArticleState(game, article));
	}

	/**
	 * Checks if selected article_id is available -> if yes, open it.
	 * Otherwise, open start state!
	 * @param game Pointer to actual game.
	 * @param article_id Article id to find.
	 */
	void Ufopaedia::openArticle(Game *game, std::string &article_id)
	{
		// TODO: look if article id is available!
	}

	/**
	 * Open Ufopaedia start state, presenting the section selection buttons.
	 * @param game Pointer to actual game.
	 */
	void Ufopaedia::open(Game *game)
	{
		game->pushState(new UfopaediaStartState(game));
	}

	/**
	 * Open the next article in the list. Loops to the first.
	 * @param game Pointer to actual game.
	 */
	void Ufopaedia::next(Game *game)
	{
		ArticleDefinitionList articles = getAvailableArticles(game);
		if (_current_index >= articles.size() - 1)
		{
			// goto first
			_current_index = 0;
		}
		else
		{
			_current_index++;
		}
		game->popState();
		game->pushState(createArticleState(game, articles[_current_index]));
	}

	/**
	 * Open the previous article in the list. Loops to the last.
	 * @param game Pointer to actual game.
	 */
	void Ufopaedia::prev(Game *game)
	{
		ArticleDefinitionList articles = getAvailableArticles(game);
		if (_current_index == 0)
		{
			// goto last
			_current_index = articles.size() - 1;
		}
		else
		{
			_current_index--;
		}
		game->popState();
		game->pushState(createArticleState(game, articles[_current_index]));
	}

	/**
	 * Fill an ArticleList with the currently visible ArticleIds of the given section.
	 * @param game Pointer to actual game.
	 * @param section Article section to find, e.g. "XCOM Crafts & Armaments", "Alien Lifeforms", etc.
	 * @param data Article definition list object to fill data in.
	 */
	void Ufopaedia::list(Game *game, const std::string &section, ArticleDefinitionList &data)
	{
		ArticleDefinitionList articles = getAvailableArticles(game);
		for (ArticleDefinitionList::iterator it=articles.begin(); it!=articles.end(); ++it)
		{
			if ((*it)->section == section)
			{
				data.push_back(*it);
			}
		}
	}

	/**
	 * Return an ArticleList with all the currently visible ArticleIds.
	 * @param game Pointer to actual game.
	 */
	ArticleDefinitionList Ufopaedia::getAvailableArticles(Game *game)
	{
		std::vector<std::string> list = game->getRuleset()->getUfopaediaList();
		ArticleDefinitionList articles;
		for (std::vector<std::string>::iterator it=list.begin(); it!=list.end(); ++it)
		{
			ArticleDefinition *article = game->getRuleset()->getUfopaediaArticle(*it);
			if (/*isArticleAvailable(game, *it) && */article->section != UFOPAEDIA_NOT_AVAILABLE)
			{
				articles.push_back(article);
			}
		}
		return articles;
	}

	/**
	 * Build a string from a string template. A template can be a concatenation of string ids,
	 * f.i. "STR_SECTOID + STR_AUTOPSY". Maybe must add constant for whitespace also.
	 * @param game Pointer to actual game.
	 * @param str_template String containing the text constants
	 * @returns The string built using the text constant(s).
	 */
	std::wstring Ufopaedia::buildText(Game *game, std::string &str_template)
	{
		// TODO: actually parse the template string.
		return game->getLanguage()->getString(str_template);
	}

	/**
	 * Open Ufopaedia to test it without starting a whole game.
	 * @param game Pointer to actual game.
	 */
	void Ufopaedia::runStandalone(Game *game)
	{
		// set game language
		game->loadLanguage("English");

		// init game
		game->loadRuleset();
		game->setSavedGame(game->getRuleset()->newSave());

		// open Ufopaedia
		open(game);
	}

}
