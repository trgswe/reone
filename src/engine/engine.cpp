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

#include "engine.h"

#include "../common/logutil.h"

#include "di/ioccontainer.h"
#include "gameprobe.h"
#include "optionsparser.h"

using namespace reone::game;

namespace reone {

int Engine::run() {
    OptionsParser optionsParser(_argc, _argv);
    Options gameOptions(optionsParser.parse());

    setLogLevel(gameOptions.logLevel);
    setLogToFile(gameOptions.logToFile);
    setLogChannels(gameOptions.logChannels);

    GameProbe gameProbe(gameOptions.gamePath);
    GameID gameId = gameProbe.probe();

    IocContainer container(gameId, gameOptions);
    container.init();

    return container.getGame().run();
}

} // namespace reone
