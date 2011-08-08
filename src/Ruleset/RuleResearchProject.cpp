#include "RuleResearchProject.h"

namespace OpenXcom
{

ResearchProject::ResearchProject(const std::wstring & name, int cost) : _name(name), _cost(cost)
{
}

bool ResearchProject::isAvailable () const
{
	std::vector<ResearchProject *>::const_iterator iter = _dependencys.begin ();
	while (iter != _dependencys.end ())
	{
		if (!(*iter)->isDiscovered())
			return false;
		iter++;
	}

	return true;
}

void ResearchProject::addDependency (ResearchProject * rp)
{
	_dependencys.push_back(rp);
}

bool ResearchProject::isDiscovered () const
{
	return _discovered;
}

int ResearchProject::getCost() const
{
	return _cost;
}

void ResearchProject::setDiscovered (bool b)
{
	_discovered = b;
}

const std::wstring & ResearchProject::getName () const
{
	return _name;
}

}
