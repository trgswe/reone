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

#include "../gui.h"

#include "ingame/abilities.h"
#include "ingame/character.h"
#include "ingame/equip.h"
#include "ingame/inventory.h"
#include "ingame/journal.h"
#include "ingame/map.h"
#include "ingame/messages.h"
#include "ingame/options.h"

namespace reone {

namespace kotor {

class InGameMenu : public GameGUI {
public:
    InGameMenu(KotOR &game, game::Services &services);

    void load() override;
    bool handle(const SDL_Event &event) override;
    void update(float dt) override;
    void draw() override;

    void openEquipment();
    void openInventory();
    void openCharacter();
    void openAbilities();
    void openMessages();
    void openJournal();
    void openMap();
    void openOptions();

    std::shared_ptr<gui::Button> getBtnChange2();
    std::shared_ptr<gui::Button> getBtnChange3();

private:
    struct Binding {
        std::shared_ptr<gui::Label> lblhEqu;
        std::shared_ptr<gui::Label> lblhInv;
        std::shared_ptr<gui::Label> lblhCha;
        std::shared_ptr<gui::Label> lblhAbi;
        std::shared_ptr<gui::Label> lblhMsg;
        std::shared_ptr<gui::Label> lblhJou;
        std::shared_ptr<gui::Label> lblhMap;
        std::shared_ptr<gui::Label> lblhOpt;

        std::shared_ptr<gui::Button> btnEqu;
        std::shared_ptr<gui::Button> btnInv;
        std::shared_ptr<gui::Button> btnChar;
        std::shared_ptr<gui::Button> btnAbi;
        std::shared_ptr<gui::Button> btnMsg;
        std::shared_ptr<gui::Button> btnJou;
        std::shared_ptr<gui::Button> btnMap;
        std::shared_ptr<gui::Button> btnOpt;
    } _binding;

    game::InGameMenuTab _tab {game::InGameMenuTab::None};

    std::unique_ptr<CharacterMenu> _character;
    std::unique_ptr<Equipment> _equip;
    std::unique_ptr<InventoryMenu> _inventory;
    std::unique_ptr<AbilitiesMenu> _abilities;
    std::unique_ptr<MessagesMenu> _messages;
    std::unique_ptr<JournalMenu> _journal;
    std::unique_ptr<MapMenu> _map;
    std::unique_ptr<OptionsMenu> _options;

    void bindControls();
    void updateTabButtons();
    void changeTab(game::InGameMenuTab tab);

    void loadCharacter();
    void loadEquipment();
    void loadInventory();
    void loadAbilities();
    void loadMessages();
    void loadJournal();
    void loadMap();
    void loadOptions();

    GUI *getActiveTabGUI() const;

    void setTabLabelsFocusable(bool focusable);
};

} // namespace kotor

} // namespace reone
