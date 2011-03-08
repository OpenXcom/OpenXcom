/*
 * Copyright 2011 OpenXcom Developers.
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
#ifndef OPENXCOM_UFOPAEDIA_H
#define OPENXCOM_UFOPAEDIA_H

#include <vector>
#include <string>

namespace OpenXcom
{
	class Game;
	class ArticleDefinition;
	class ArticleState;
	
	/// definition of an article list
	typedef std::vector<ArticleDefinition *> ArticleDefinitionList;

	/// define Ufopaedia sections, which must be consistent
	static const std::string UFOPAEDIA_XCOM_CRAFT_ARMAMENT = "XCOM_CRAFT_ARMAMENT";
	static const std::string UFOPAEDIA_ALIEN_LIFE_FORMS = "ALIEN_LIFE_FORMS";
	static const std::string UFOPAEDIA_ALIEN_RESEARCH = "ALIEN_RESEARCH";
	
	/**
	 * This static class encapsulates all functions related to Ufopaedia
	 * for the game. It manages the relationship between the UfopaediaSaved 
	 * instance in SavedGame and the UfopaediaFactory in Ruleset.
	 * Main purpose is to open Ufopaedia from Geoscape, navigate between articles 
	 * and release new articles after successful research.
	 */
	
	class Ufopaedia
	{
	public:
		/// give the user access to a certain article (after research mostly).
		static void releaseArticle(Game *game, std::string &article_id);
		
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
		
		/// build a text string from a string template using current game settings.
		static std::wstring buildText(Game *game, std::string &str_template);
		
		/// TO BE REMOVED! run in test mode during development only.
		static void runStandalone(Game *game);
		
	protected:
		
		/// create a new state object from article definition.
		static ArticleState *createArticleState(Game *game, ArticleDefinition *article);
	};
}

#endif
