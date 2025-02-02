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

#include "../../../game/format/ltrreader.h"
#include "../../../gui/textinput.h"

#include "../../gui.h"

namespace reone {

namespace gui {

class Button;

}

namespace kotor {

class CharacterGeneration;

class NameEntry : public GameGUI {
public:
    NameEntry(
        CharacterGeneration &charGen,
        KotOR &game,
        game::Services &services);

    void load() override;
    bool handle(const SDL_Event &event) override;

    void loadRandomName();

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> btnRandom;
        std::shared_ptr<gui::Button> endBtn;
        std::shared_ptr<gui::Control> nameBoxEdit;
    } _binding;

    CharacterGeneration &_charGen;
    gui::TextInput _input;
    game::LtrReader _maleLtr;
    game::LtrReader _femaleLtr;
    game::LtrReader _lastNameLtr;

    void bindControls();
    void loadLtrFile(const std::string &resRef, game::LtrReader &ltr);

    std::string getRandomName() const;
};

} // namespace kotor

} // namespace reone
