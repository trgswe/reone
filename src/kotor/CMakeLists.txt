# Copyright (c) 2020-2021 The reone project contributors

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

set(KOTOR_HEADERS
    cursors.h
    gui.h
    gui/barkbubble.h
    gui/chargen.h
    gui/chargen/abilities.h
    gui/chargen/classselect.h
    gui/chargen/custom.h
    gui/chargen/feats.h
    gui/chargen/levelup.h
    gui/chargen/nameentry.h
    gui/chargen/portraitselect.h
    gui/chargen/quick.h
    gui/chargen/quickorcustom.h
    gui/chargen/skills.h
    gui/computer.h
    gui/console.h
    gui/container.h
    gui/conversation.h
    gui/dialog.h
    gui/hud.h
    gui/ingame.h
    gui/ingame/abilities.h
    gui/ingame/character.h
    gui/ingame/equip.h
    gui/ingame/inventory.h
    gui/ingame/journal.h
    gui/ingame/map.h
    gui/ingame/messages.h
    gui/ingame/options.h
    gui/loadscreen.h
    gui/mainmenu.h
    gui/map.h
    gui/partyselect.h
    gui/profileoverlay.h
    gui/saveload.h
    gui/selectoverlay.h
    kotor.h
    script/routine/argutil.h
    script/routine/context.h
    script/routine/declarations.h
    script/routine/objectutil.h
    script/routines.h)

set(KOTOR_SOURCES
    cursors.cpp
    gui.cpp
    gui/barkbubble.cpp
    gui/chargen.cpp
    gui/chargen/abilities.cpp
    gui/chargen/classselect.cpp
    gui/chargen/custom.cpp
    gui/chargen/feats.cpp
    gui/chargen/levelup.cpp
    gui/chargen/nameentry.cpp
    gui/chargen/portraitselect.cpp
    gui/chargen/quick.cpp
    gui/chargen/quickorcustom.cpp
    gui/chargen/skills.cpp
    gui/computer.cpp
    gui/console.cpp
    gui/container.cpp
    gui/conversation.cpp
    gui/dialog.cpp
    gui/hud.cpp
    gui/ingame.cpp
    gui/ingame/abilities.cpp
    gui/ingame/character.cpp
    gui/ingame/equip.cpp
    gui/ingame/inventory.cpp
    gui/ingame/journal.cpp
    gui/ingame/map.cpp
    gui/ingame/messages.cpp
    gui/ingame/options.cpp
    gui/loadscreen.cpp
    gui/mainmenu.cpp
    gui/map.cpp
    gui/partyselect.cpp
    gui/profileoverlay.cpp
    gui/saveload.cpp
    gui/selectoverlay.cpp
    kotor.cpp
    script/routine/argutil.cpp
    script/routine/impl/action.cpp
    script/routine/impl/effect.cpp
    script/routine/impl/main.cpp
    script/routine/impl/minigame.cpp
    script/routines.cpp)

add_library(kotor STATIC ${KOTOR_HEADERS} ${KOTOR_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(kotor PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(kotor PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(kotor PUBLIC game)
