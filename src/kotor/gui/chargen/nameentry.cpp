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

#include "nameentry.h"

#include "../../../common/streamutil.h"
#include "../../../gui/control/button.h"
#include "../../../resource/resources.h"

#include "../../kotor.h"

#include "../chargen.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace kotor {

NameEntry::NameEntry(
    CharacterGeneration &charGen,
    KotOR &game,
    Services &services) :
    GameGUI(game, services),
    _charGen(charGen),
    _input(TextInputFlags::lettersWhitespace) {

    _resRef = getResRef("name");

    initForGame();
}

void NameEntry::load() {
    GUI::load();
    bindControls();

    loadLtrFile("humanm", _maleLtr);
    loadLtrFile("humanf", _femaleLtr);
    loadLtrFile("humanl", _lastNameLtr);

    _binding.nameBoxEdit->setTextMessage("");

    _binding.btnRandom->setOnClick([this]() {
        loadRandomName();
    });
    _binding.endBtn->setOnClick([this]() {
        _charGen.goToNextStep();
        _charGen.openSteps();
    });
    _binding.btnBack->setOnClick([this]() {
        _charGen.openSteps();
    });
}

void NameEntry::bindControls() {
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.btnRandom = getControl<Button>("BTN_RANDOM");
    _binding.endBtn = getControl<Button>("END_BTN");
    _binding.nameBoxEdit = getControl<Control>("NAME_BOX_EDIT");
}

void NameEntry::loadLtrFile(const string &resRef, LtrReader &ltr) {
    shared_ptr<ByteArray> data(_resources.get(resRef, ResourceType::Ltr));
    ltr.load(wrap(data));
}

bool NameEntry::handle(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && _input.handle(event)) {
        _binding.nameBoxEdit->setTextMessage(_input.text());
        return true;
    }
    return GUI::handle(event);
}

void NameEntry::loadRandomName() {
    _binding.nameBoxEdit->setTextMessage(getRandomName());
}

string NameEntry::getRandomName() const {
    Gender gender = _charGen.character().gender;
    const LtrReader &nameLtr = gender == Gender::Female ? _femaleLtr : _maleLtr;
    return nameLtr.getRandomName(8) + " " + _lastNameLtr.getRandomName(8);
}

} // namespace kotor

} // namespace reone
