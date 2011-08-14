#include "RuleResearchProject.h"

namespace OpenXcom
{

RuleResearchProject::RuleResearchProject(const std::string & name, int cost) : _name(name), _cost(cost), _discovered(false)
{
}

bool RuleResearchProject::isAvailable () const
{
	std::vector<RuleResearchProject *>::const_iterator iter = _dependencys.begin ();
	while (iter != _dependencys.end ())
	{
		if (!(*iter)->isDiscovered())
			return false;
		iter++;
	}

	return true;
}

void RuleResearchProject::addDependency (RuleResearchProject * rp)
{
	_dependencys.push_back(rp);
}

bool RuleResearchProject::isDiscovered () const
{
	return _discovered;
}

int RuleResearchProject::getCost() const
{
	return _cost;
}

void RuleResearchProject::setDiscovered (bool b)
{
	_discovered = b;
}

const std::string & RuleResearchProject::getName () const
{
	return _name;
}

const std::vector<RuleResearchProject *> & RuleResearchProject::getDependencys () const
{
	return _dependencys;
}
}
