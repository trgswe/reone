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

#include "../effect.h"

namespace reone {

namespace game {

class SavingThrowDecreaseEffect : public Effect {
public:
    SavingThrowDecreaseEffect(int save, int value, SavingThrowType savingThrowType) :
        Effect(EffectType::SavingThrowDecrease),
        _save(save),
        _value(value),
        _savingThrowType(savingThrowType) {
    }

    void applyTo(Object &object) override;

private:
    int _save;
    int _value;
    SavingThrowType _savingThrowType;
};

} // namespace game

} // namespace reone
