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
#include "Position.h"

#include <yaml-cpp/yaml.h>

namespace YAML{
        template<>
        struct convert<OpenXcom::Position>{
                static Node encode(const OpenXcom::Position& rhs){
                        Node node;
                        node.push_back(rhs.x);
                        node.push_back(rhs.y);
                        node.push_back(rhs.z);
                        return node;
                }

                static bool decode(const Node& node, OpenXcom::Position& rhs){
                        if (!node.IsSequence() || node.size() != 3)
                                return false;

                        rhs.x = node[0].as<int>();
                        rhs.y = node[1].as<int>();
                        rhs.z = node[2].as<int>();
                        return true;
                }
        };
}
