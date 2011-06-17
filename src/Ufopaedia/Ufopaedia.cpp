/*
 * Copyright 2010 OpenXcom Developers.
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
#include "../Savegame/UfopaediaSaved.h"
#include "../Ruleset/ArticleDefinition.h"
#include "ArticleState.h"
#include "ArticleStateCraft.h"
#include "ArticleStateCraftWeapon.h"
#include "ArticleStateText.h"
#include "ArticleStateTextImage.h"
#include "ArticleStateUfo.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"

// these are for runStandalone...
#include "../Resource/ResourcePack.h"
#include "../Ruleset/XcomRuleset.h"
#include <sstream>

namespace OpenXcom
{
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
			case UFOPAEDIA_TYPE_TEXT:
				return new ArticleStateText(game, static_cast<ArticleDefinitionText *> (article));
				break;
			case UFOPAEDIA_TYPE_TEXTIMAGE:
				return new ArticleStateTextImage(game, static_cast<ArticleDefinitionTextImage *> (article));
				break;
			case UFOPAEDIA_TYPE_UFO:
				return new ArticleStateUfo(game, static_cast<ArticleDefinitionUfo *> (article));
				break;
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
		game->getSavedGame()->getUfopaedia()->setCurrentArticle(article);
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
		ArticleDefinition *article = game->getSavedGame()->getUfopaedia()->goNextArticle();
		if (article)
		{
			game->popState();
			game->pushState(createArticleState(game, article));
		}
	}
	
	/**
	 * Open the previous article in the list. Loops to the last.
	 * @param game Pointer to actual game.
	 */
	void Ufopaedia::prev(Game *game)
	{
		ArticleDefinition *article = game->getSavedGame()->getUfopaedia()->goPrevArticle();
		if (article)
		{
			game->popState();
			game->pushState(createArticleState(game, article));
		}
	}
	
	/**
	 * Build a string from a string template. A template can be a concatenation of string ids, 
	 * f.i. "STR_SECTOID + STR_AUTOPSY". Maybe must add constant for whitepace also.
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
		std::stringstream ss;
		ss << game->getResourcePack()->getFolder() << "Language/English.lng";
		Language *l = new Language();
		l->loadLng(ss.str());
		game->setLanguage(l);
		
		// init game
		game->setRuleset(new XcomRuleset());
		game->setSavedGame(game->getRuleset()->newSave(DIFF_BEGINNER));
		
		// open Ufopaedia
		open(game);
	}
	
}
