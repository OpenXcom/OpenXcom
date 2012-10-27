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
#ifndef OPENXCOM_PATHFINDINGOPENSET_H
#define OPENXCOM_PATHFINDINGOPENSET_H

#include <queue>

namespace OpenXcom
{

class PathfindingNode;

struct OpenSetEntry
{
	int _cost;
	PathfindingNode *_node;
};

/**
 * Helper class to compare entries through pointers.
 */
class EntryCompare
{
public:
 	/**
	 * Compare entries @a *a and @a *b.
         * @param a Pointer to first entry.
         * @param b Pointer to second entry.
	 * @return true If entry @a *b must come before @a *a.
	 */
	bool operator()(OpenSetEntry *a, OpenSetEntry *b)
	{
		return b->_cost < a->_cost;
	}
};

/**
 * A class that holds references to the nodes to be examined in pathfinding.
 */
class PathfindingOpenSet
{
public:
	/// Cleanup the set and free allocated memory.
	~PathfindingOpenSet();
	/// Get the next node to check.
	PathfindingNode *pop();
	/// Add a node in the set.
	void push(PathfindingNode *node);
	/// Is the set empty?
	bool empty() const { return _queue.empty(); }

private:
	std::priority_queue<OpenSetEntry*, std::vector<OpenSetEntry*>, EntryCompare> _queue;

	/// Remove reachable discarded entries.
	void removeDiscarded();
};

}

#endif
