#pragma once

/* SURELY THIS WILL WORK */

#include "AIModule.h"
#include <janet/janet.h>

namespace OpenXcom
{
  class JanetAIModule : public AIModule
  {
  public:
    static JanetTable* janet_env;

    static void SetUp() {
      janet_init();
      janet_env = janet_core_env(NULL);
    }

    static void TearDown() {
      janet_deinit();
      janet_env = NULL;
    }

    JanetAIModule(SavedBattleGame *save, BattleUnit *unit, Node *node);
  };
}
