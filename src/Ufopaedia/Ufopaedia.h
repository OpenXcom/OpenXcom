#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <vector>
#include <string>

namespace OpenXcom
{
	class Game;
	class SavedGame;
	class Mod;
	class ArticleDefinition;
	class ArticleState;

	/// definition of an article list
	typedef std::vector<ArticleDefinition *> ArticleDefinitionList;

	// This section is meant for articles, that have to be activated,
	// but have no own entry in a list. E.g. Ammunition items.
	// Maybe others as well, that should just not be selectable.
	static const std::string UFOPAEDIA_NOT_AVAILABLE = "STR_NOT_AVAILABLE";

	/**
	 * This static class encapsulates all functions related to Ufopaedia
	 * for the game.
	 * Main purpose is to open Ufopaedia from Geoscape, navigate between articles
	 * and release new articles after successful research.
	 */

	class Ufopaedia
	{
	public:
		/// check, if a specific article is currently available.
		static bool isArticleAvailable(SavedGame *save, ArticleDefinition *article);

		/// open Ufopaedia on a certain entry.
		static void openArticle(Game *game, std::string &article_id);

		/// open Ufopaedia article from a given article definition.
		static void openArticle(Game *game, ArticleDefinition *article);

		/// open Ufopaedia with selection dialog.
		static void open(Game *game);

		/// article navigation to next article.
		static void next(Game *game);

		/// article navigation to previous article.
		static void prev(Game *game);

		/// load a vector with article ids that are currently visible of a given section.
		static void list(SavedGame *save, Mod *rule, const std::string &section, ArticleDefinitionList &data);

	protected:

		/// current selected article index (for prev/next navigation).
		static size_t _current_index;

		/// get index of the given article id in the visible list.
		static size_t getArticleIndex(SavedGame *save, Mod *rule, std::string &article_id);

		/// get list of researched articles
		static ArticleDefinitionList getAvailableArticles(SavedGame *save, Mod *rule);

		/// create a new state object from article definition.
		static ArticleState *createArticleState(ArticleDefinition *article);
	};
}
