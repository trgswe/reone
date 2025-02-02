/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "debug.h"

namespace reone {

namespace game {

static bool g_showWalkmesh = false;
static bool g_showTriggers = false;

bool isShowWalkmeshEnabled() {
    return g_showWalkmesh;
}

bool isShowTriggersEnabled() {
    return g_showTriggers;
}

void setShowWalkmesh(bool show) {
    g_showWalkmesh = show;
}

void setShowTriggers(bool show) {
    g_showTriggers = show;
}

} // namespace game

} // namespace reone
