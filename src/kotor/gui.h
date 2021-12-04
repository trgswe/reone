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

#pragma once

#include "../game/types.h"
#include "../gui/gui.h"

namespace reone {

namespace game {

struct Services;

}

namespace kotor {

class KotOR;

/**
 * Encapsulates game-specific GUI configuration.
 */
class GameGUI : public gui::GUI {
protected:
    GameGUI(KotOR &game, game::Services &services);

    void initForGame();

    std::string getResRef(const std::string &base) const;

protected:
    KotOR &_game;
    game::Services &_services;

    void loadBackground(game::BackgroundType type);

private:
    void onClick(const std::string &control) override;
    void onFocusChanged(const std::string &control, bool focus) override;
};

} // namespace kotor

} // namespace reone