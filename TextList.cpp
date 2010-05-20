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
#include "TextList.h"

/**
 * Sets up a blank list with the specified size and position.
 * @param big Pointer to the big-size font.
 * @param small Pointer to the small-size font.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
TextList::TextList(Font *big, Font *small, int width, int height, int x, int y) : Surface(width, height, x, y), _texts(), _columns(), _big(big), _small(small), _rowY(0), _color(0), _dot(false)
{

}

/**
 * Deletes all the child Text's contained by the list.
 */
TextList::~TextList()
{
	for (vector< vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); u++) {
		for (vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); v++) {
			delete (*v);
		}
	}

	
}

/**
 * Returns a pointer to a certain Text object in the list.
 * Useful for customizing or getting values off an individual cell.
 * @param row Row number.
 * @param col Column number.
 * @return Pointer to the requested Text.
 */
Text* TextList::getCell(int row, int col)
{
	return _texts[row][col];
}

/**
 * Adds a new row of text to the list, automatically creating
 * the required Text objects lined up where they need to be.
 * @param num Number of columns.
 * @param ... Text for each cell in the new row.
 */
void TextList::addRow(int num, ...)
{
	va_list args;
	va_start(args, num);
	vector<Text*> temp;
	int rowX = 0;

	for (int i = 0; i < num; i++)
	{
		Text* txt = new Text(_big, _small, _columns[i], _small->getHeight(), rowX, _rowY);
		txt->setPalette(this->getPalette());
		
		string buf = va_arg(args, char*);
		// Places dots between text
		if (_dot && i < num - 1)
		{
			int w = 0;
			for (string::iterator c = buf.begin(); c < buf.end(); c++)
			{
				if (*c == ' ')
					w += _small->getWidth() / 2;
				else
					w += _small->getChar(*c)->getCrop()->w + _small->getSpacing();
			}
			while (w < _columns[i])
			{
				w += _small->getChar('.')->getCrop()->w + _small->getSpacing();
				buf += '.';
			}
		}
		txt->setText(buf);

		txt->setColor(_color);
		txt->setSmall();
		temp.push_back(txt);
		rowX += _columns[i];
	}
	_texts.push_back(temp);
	_rowY += _small->getHeight() + _small->getSpacing();

	va_end(args);
}

/**
 * Changes the number of columns that the list contains.
 * While rows can be unlimited, columns need to be specified
 * since they can have variable widths for lining up the text.
 * @param num Number of columns.
 * @param ... Width of each column.
 */
void TextList::setColumns(int num, ...)
{
	va_list args;
	va_start(args, num);

	for (int i = 0; i < num; i++)
	{
		_columns.push_back(va_arg(args, int));
	}

	va_end(args);
}

/**
 * Replaces a certain amount of colors in the palette of all
 * the text contained in the list.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void TextList::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	for (vector< vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); u++) {
		for (vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); v++) {
			(*v)->setPalette(colors, firstcolor, ncolors);
		}
	}
}

/**
 * Changes the color of the text in the list. This doesn't change
 * the color of existing text, just the color of text added from then on.
 * @param color Color value.
 */
void TextList::setColor(Uint8 color)
{
	_color = color;
}

/**
 * If enabled, the text in different columns will be separated by dots.
 * Otherwise, it will only be separated by blank space.
 * @param dot True for dots, False for spaces.
 */
void TextList::setDot(bool dot)
{
	_dot = dot;
}

/**
 * Draws the text list and all the text contained within onto another surface.
 * @param surface Pointer to surface to blit onto.
 */
void TextList::blit(Surface *surface)
{
	for (vector< vector<Text*> >::iterator u = _texts.begin(); u < _texts.end(); u++) {
		for (vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); v++) {
            (*v)->blit(this);
        }
    }
	Surface::blit(surface);
}
