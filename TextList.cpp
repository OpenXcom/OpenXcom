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

TextList::TextList(Font *big, Font *small, int width, int height, int x, int y) : Surface(width, height, x, y), _big(big), _small(small), _texts(), _columns(), _rowY(0), _color(0)
{

}

TextList::~TextList()
{
	for (vector<vector<Text*>>::iterator u = _texts.begin(); u < _texts.end(); u++) {
		for (vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); v++) {
			delete (*v);
		}
	}

	Surface::~Surface();
}

Text* TextList::getCell(int row, int col)
{
	return _texts[row][col];
}

void TextList::addRow(int num, ...)
{
	va_list args;
	va_start(args, num);
	vector<Text*> temp;
	int rowX = 0;

	for (int i = 0; i < num; i++) {
		Text* txt = new Text(_big, _small, _columns[i], _small->getHeight());
		txt->setX(rowX);
		txt->setY(_rowY);
		txt->setPalette(this->getPalette());
		txt->setText(va_arg(args, char*));
		txt->setColor(_color);
		txt->setSmall();
		temp.push_back(txt);
		rowX += _columns[i];
	}
	_texts.push_back(temp);
	_rowY += _small->getHeight();

	va_end(args);
}

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

void TextList::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	for (vector<vector<Text*>>::iterator u = _texts.begin(); u < _texts.end(); u++) {
		for (vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); v++) {
			(*v)->setPalette(colors, firstcolor, ncolors);
		}
	}
}

void TextList::setColor(Uint8 color)
{
	_color = color;
}

void TextList::blit(Surface *surface)
{
	for (vector<vector<Text*>>::iterator u = _texts.begin(); u < _texts.end(); u++) {
		for (vector<Text*>::iterator v = (*u).begin(); v < (*u).end(); v++) {
            (*v)->blit(this);
        }
    }
	Surface::blit(surface);
}
