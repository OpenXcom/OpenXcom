#pragma once

/* SURELY THIS WILL WORK */

#include "AIModule.h"
#include <janet/janet.h>

namespace OpenXcom
{
  class JanetAIModule : public AIModule
  {
    JanetAIModule(SavedBattleGame *save, BattleUnit *unit, Node *node);
  };
}
