/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include <assert.h>
#include "PathfindingOpenSet.h"
#include "PathfindingNode.h"

namespace OpenXcom
{

/**
 * Cleans up all the entries still in set.
 */
PathfindingOpenSet::~PathfindingOpenSet()
{
	while (!_queue.empty())
	{
		OpenSetEntry *entry = _queue.top();
		_queue.pop();
		delete entry;
	}
}

/**
 * Keeps removing all discarded entries that have come to the top of the queue.
 */
void PathfindingOpenSet::removeDiscarded()
{
	while (!_queue.empty() && !_queue.top()->_node)
	{
		OpenSetEntry *entry = _queue.top();
		_queue.pop();
		delete entry;
	}
}

/**
 * Gets the node with the least cost.
 * After this call, the node is no longer in the set. It is an error to call this when the set is empty.
 * @return A pointer to the node which had the least cost.
 */
PathfindingNode *PathfindingOpenSet::pop()
{
	assert(!empty());
	OpenSetEntry *entry = _queue.top();
	PathfindingNode *nd = entry->_node;
	_queue.pop();
	delete entry;
	nd->_openentry = 0;

	// Discarded entries might be visible now.
	removeDiscarded();
	return nd;
}

/**
 * Places the node in the set.
 * If the node was already in the set, the previous entry is discarded.
 * It is the caller's responsibility to never re-add a node with a worse cost.
 * @param node A pointer to the node to add.
 */
void PathfindingOpenSet::push(PathfindingNode *node)
{
	OpenSetEntry *entry = new OpenSetEntry;
	entry->_node = node;
	entry->_cost = node->getTUCost(false) + node->getTUGuess();
	if (node->_openentry)
		node->_openentry->_node = 0;
	node->_openentry = entry;
	_queue.push(entry);
}


}
