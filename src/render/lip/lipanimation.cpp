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

#include "lipanimation.h"

#include <stdexcept>

using namespace std;

namespace reone {

namespace render {

LipAnimation::LipAnimation(float length, vector<Keyframe> keyframes) :
    _length(length), _keyframes(move(keyframes)) {
}

const LipAnimation::Keyframe &LipAnimation::getKeyframe(float time) const {
    if (_keyframes.empty()) {
        throw logic_error("keyframes is empty");
    }
    for (auto &frame : _keyframes) {
        if (time <= frame.time) return frame;
    }
    return _keyframes.back();
}

} // namespace render

} // namespace reone
