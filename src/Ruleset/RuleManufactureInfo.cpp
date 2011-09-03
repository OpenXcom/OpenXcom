#include "RuleManufactureInfo.h"

namespace OpenXcom
{
RuleManufactureInfo::RuleManufactureInfo(int workspace, int time, int cost) : _requiredWorkspace(workspace), _manufactureTime(time), _manufactureCost(cost)
{
}

void RuleManufactureInfo::setRequiredSpace (int workspace)
{
	_requiredWorkspace = workspace;
}

int RuleManufactureInfo::getRequiredSpace () const
{
	return _requiredWorkspace;
}

void RuleManufactureInfo::setManufactureTime (int time)
{
	_manufactureTime = time;
}

int RuleManufactureInfo::getManufactureTime () const
{
	return _manufactureTime;
}

void RuleManufactureInfo::setManufactureCost (int cost)
{
	_manufactureCost = cost;
}

int RuleManufactureInfo::getManufactureCost () const
{
	return _manufactureCost;
}

const std::map<std::string, int> & RuleManufactureInfo::getNeededItems() const
{
	return _neededItems;
}

void RuleManufactureInfo::addNeededItem (std::string type, int quantity)
{
	_neededItems[type] = quantity;
}
}
