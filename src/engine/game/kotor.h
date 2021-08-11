
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

#include "game.h"

namespace reone {

namespace game {

/**
 * KotOR game.
 */
class KotOR : public Game {
public:
    KotOR(
        GameID gameId,
        boost::filesystem::path path,
        Options options,
        resource::ResourceServices &resource,
        graphics::GraphicsServices &graphics,
        audio::AudioServices &audio,
        scene::SceneServices &scene,
        script::ScriptServices &script);

    void initResourceProviders() override;
};

} // namespace game

} // namespace reone