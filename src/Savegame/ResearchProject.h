#ifndef OPENXCOM_RESEARCHPROJECT
#define OPENXCOM_RESEARCHPROJECT

namespace OpenXcom
{
class RuleResearchProject;

class ResearchProject
{
  RuleResearchProject * _project;
  int _assigned;
  float _spent;
public:
  ResearchProject(RuleResearchProject * p);

  bool step();

  void setAssigned (int nb);

  const RuleResearchProject * GetRuleResearchProject () const;
};
}
#endif
