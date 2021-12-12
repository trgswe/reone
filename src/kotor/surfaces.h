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

#include "../game/surface.h"
#include "../game/surfaces.h"

namespace reone {

namespace resource {

class TwoDas;

}

namespace kotor {

class Surfaces : public game::ISurfaces {
public:
    Surfaces(resource::TwoDas &twoDas) :
        _twoDas(twoDas) {
    }

    void init();

    bool isWalkable(int index) const override;

    const game::Surface &getSurface(int index) const override;

    std::set<uint32_t> getGrassSurfaces() const override;
    std::set<uint32_t> getWalkableSurfaces() const override;
    std::set<uint32_t> getWalkcheckSurfaces() const override;

private:
    resource::TwoDas &_twoDas;

    std::vector<game::Surface> _surfaces;

    inline std::set<uint32_t> getSurfaceIndices(const std::function<bool(const game::Surface &)> &pred) const;
};

} // namespace kotor

} // namespace reone
