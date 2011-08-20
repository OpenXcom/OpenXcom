#include "ResearchProject.h"
#include "../Ruleset/RuleResearchProject.h"
#include "../Ruleset/Ruleset.h"
#include <algorithm>

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

float ResearchProject::getSpent () const
{
	return _spent;
}

void ResearchProject::setSpent (float f)
{
	_spent = f;
}

void ResearchProject::load(const YAML::Node& node, const Ruleset *rule)
{
	int assigned;
	float spent;
	node["assigned"] >> assigned;
	node["spent"] >> spent;
	setAssigned(assigned);
	setSpent(spent);
}

void ResearchProject::save(YAML::Emitter& out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "project" << YAML::Value << getRuleResearchProject ()->getName ();
	out << YAML::Key << "assigned" << YAML::Value << getAssigned ();
	out << YAML::Key << "spent" << YAML::Value << getSpent ();
	out << YAML::EndMap;
}
}
