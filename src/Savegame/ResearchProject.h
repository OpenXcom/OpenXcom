#ifndef OPENXCOM_RESEARCHPROJECT
#define OPENXCOM_RESEARCHPROJECT
#include "yaml.h"

namespace OpenXcom
{
class RuleResearchProject;
class Ruleset;

class ResearchProject
{
  RuleResearchProject * _project;
  int _assigned;
  float _spent;
public:
  ResearchProject(RuleResearchProject * p);

  bool step();

  void setAssigned (int nb);

  int getAssigned () const;

  float getSpent () const;
  void setSpent (float f);

  const RuleResearchProject * getRuleResearchProject () const;

  void load(const YAML::Node& node, const Ruleset *rule);
  void save(YAML::Emitter& out) const;
};
}
#endif
