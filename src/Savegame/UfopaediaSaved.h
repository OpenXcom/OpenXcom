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
#ifndef OPENXCOM_UFOPAEDIASAVED_H
#define OPENXCOM_UFOPAEDIASAVED_H

#include <vector>
#include <string>
#include "../Ufopaedia/Ufopaedia.h"

namespace OpenXcom
{
	class ArticleDefinition;
	
	/**
	 * This class encapsulates the saved game properties concerning Ufopaedia.
	 * It holds a list of available articles in sorted order, it can deliver a
	 * list of articles belonging to a certain section for UfopaediaSelectItemState and
	 * keeps track of the current viewed article index for prev/next navigation.
	 * Save game state can be stored/loaded.
	 */
	
	class UfopaediaSaved
	{
	protected:
		/// article ids that are currently visible to the user.
		ArticleDefinitionList _visible_articles;
		
		/// current selected article index (for prev/next navigation).
		unsigned int _current_index;
		
	public:
		/// Ufopaedia constructor.
		UfopaediaSaved();
		
		/// Ufopaedia destructor.
		virtual ~UfopaediaSaved();
		
		/// insert article into _visible_articles.
		void insertArticle(ArticleDefinition *article);
		
		/// check if an article is available.
		bool isArticleAvailable(const std::string &article_id);
		
		/// open Ufopaedia with selection dialog.
		void setCurrentArticle(ArticleDefinition *article);
		
		/// article navigation to next article.
		ArticleDefinition *goNextArticle();
		
		/// article navigation to previous article.
		ArticleDefinition *goPrevArticle();
		
		/// load a vector with article ids that are currently visible of a given section.
		void getSectionList(const std::string &section, ArticleDefinitionList &data);
		
		/// TODO: get current visible article list as storeable data (for save).
		void getSavegameProperties(const std::string &info) const;
		
		/// TODO: set current visible article list from stored data (on load).
		void setSavegameProperties(const std::string &info);

	protected:
		
		/// get index of the given article id in the visible list.
		int getArticleIndex(const std::string &article_id);
	};
	
}

#endif
