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

/** @file
 *  Implementation of time-related routines.
 */

#include "../declarations.h"

#include "../../../../../script/types.h"

#include "../../../game.h"
#include "../../../script/routine/argutil.h"
#include "../../../script/routine/context.h"

using namespace std;

using namespace reone::script;

namespace reone {

namespace game {

namespace routine {

Variable setTime(const vector<Variable> &args, const RoutineContext &ctx) {
    int hour = getInt(args, 0);
    int minute = getInt(args, 1);
    int second = getInt(args, 2);
    int millisecond = getInt(args, 3);

    ctx.game.module()->setTime(hour, minute, second, millisecond);

    return Variable::ofNull();
}

Variable getTimeHour(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->time().hour;
    return Variable::ofInt(result);
}

Variable getTimeMinute(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->time().minute;
    return Variable::ofInt(result);
}

Variable getTimeSecond(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->time().second;
    return Variable::ofInt(result);
}

Variable getTimeMillisecond(const vector<Variable> &args, const RoutineContext &ctx) {
    int result = ctx.game.module()->time().millisecond;
    return Variable::ofInt(result);
}

Variable getIsDay(const vector<Variable> &args, const RoutineContext &ctx) {
    shared_ptr<Module> module(ctx.game.module());
    bool result = module->time().hour > module->info().dawnHour && module->time().hour < module->info().duskHour;

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsNight(const vector<Variable> &args, const RoutineContext &ctx) {
    shared_ptr<Module> module(ctx.game.module());
    bool result = module->time().hour < module->info().dawnHour || module->time().hour > module->info().duskHour;

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsDawn(const vector<Variable> &args, const RoutineContext &ctx) {
    shared_ptr<Module> module(ctx.game.module());
    bool result = module->time().hour == module->info().dawnHour;

    return Variable::ofInt(static_cast<int>(result));
}

Variable getIsDusk(const vector<Variable> &args, const RoutineContext &ctx) {
    shared_ptr<Module> module(ctx.game.module());
    bool result = module->time().hour == module->info().duskHour;

    return Variable::ofInt(static_cast<int>(result));
}

} // namespace routine

} // namespace game

} // namespace reone