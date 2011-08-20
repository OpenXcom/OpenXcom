#include "RuleResearchProject.h"

namespace OpenXcom
{

RuleResearchProject::RuleResearchProject(const std::string & name, int cost) : _name(name), _cost(cost)
{
}

void RuleResearchProject::addDependency (RuleResearchProject * rp)
{
	_dependencys.push_back(rp);
}

int RuleResearchProject::getCost() const
{
	return _cost;
}

const std::string & RuleResearchProject::getName () const
{
	return _name;
}

const std::vector<RuleResearchProject *> & RuleResearchProject::getDependencys () const
{
	return _dependencys;
}

bool RuleResearchProject::needItem() const
{
  return _needItem;
}

void RuleResearchProject::setNeedItem(bool b)
{
  _needItem = b;
}

}
