#include "ResearchProject.h"
#include "../Ruleset/RuleResearchProject.h"

namespace OpenXcom
{

ResearchProject::ResearchProject(RuleResearchProject * p) : _project(p), _spent(0), _assigned(0)
{
}

bool ResearchProject::step()
{
	_spent += _assigned;
	if (_spent >= _project->getCost())
	{
		_project->setDiscovered(true);
		return true;
	}
	return false;
}
  
void ResearchProject::setAssigned (int nb)
{
	_assigned = nb;
}

const RuleResearchProject * ResearchProject::getRuleResearchProject () const
{
	return _project;
}

int ResearchProject::getAssigned () const
{
	return _assigned;
}

}
