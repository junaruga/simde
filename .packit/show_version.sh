#!/bin/bash
set -euo pipefail

major=$(grep '^#define SIMDE_VERSION_MAJOR' simde/simde-common.h \
  | cut -d ' ' -f3)
minor=$(grep '^#define SIMDE_VERSION_MINOR' simde/simde-common.h \
  | cut -d ' ' -f3)
micro=$(grep '^#define SIMDE_VERSION_MICRO' simde/simde-common.h \
  | cut -d ' ' -f3)
echo "${major}.${minor}.${micro}"
