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

#include "animationchannel.h"

#include <stdexcept>

#include "glm/common.hpp"

#include "node/modelnodescenenode.h"
#include "node/modelscenenode.h"
#include "types.h"

using namespace std;

using namespace reone::render;

namespace reone {

namespace scene {

AnimationChannel::AnimationChannel(ModelSceneNode *modelSceneNode, set<string> ignoreNodes) :
    _modelSceneNode(modelSceneNode),
    _ignoreNodes(move(ignoreNodes)) {

    if (!modelSceneNode) {
        throw invalid_argument("modelSceneNode must not be null");
    }
}

void AnimationChannel::reset() {
    _animation.reset();
    _time = 0.0f;
    _freeze = false;
    _finished = false;
}

void AnimationChannel::reset(shared_ptr<Animation> anim, AnimationProperties properties) {
    if (!anim) {
        throw invalid_argument("anim must not be null");
    }
    _animation = move(anim);
    _properties = move(properties);
    _time = 0.0f;
    _freeze = false;
    _finished = false;
}

void AnimationChannel::update(float dt) {
    if (!_animation || _freeze || _finished) return;

    float newTime = glm::min(_time + _properties.speed * dt, _animation->length());

    // Signal animation events between the previous time and the current time
    for (auto &event : _animation->events()) {
        if (_time < event.time && event.time <= newTime) {
            _modelSceneNode->signalEvent(event.name);
        }
    }

    computeLocalTransforms();

    _time = newTime;

    if (_time == _animation->length()) {
        bool loop = _properties.flags & AnimationFlags::loop;
        if (loop) {
            _time = 0.0f;
        } else {
            _finished = true;
        }
    }
}

void AnimationChannel::computeLocalTransforms() {
    _transformByNodeNumber.clear();
    computeLocalTransform(*_animation->rootNode());
}

void AnimationChannel::computeLocalTransform(const ModelNode &animNode) {
    if (_ignoreNodes.count(animNode.name()) == 0) {
        ModelNodeSceneNode *modelNodeSceneNode = _modelSceneNode->getModelNode(animNode.name());
        if (modelNodeSceneNode) {
            ModelNode *modelNode = modelNodeSceneNode->modelNode();
            glm::mat4 transform(1.0f);
            bool transformChanged = false;

            float scale;
            if (animNode.getScale(_time, scale)) {
                transform = glm::scale(transform, glm::vec3(scale));
                transformChanged = true;
            }

            glm::vec3 translation;
            if (animNode.getTranslation(_time, translation, _properties.scale)) {
                transform = glm::translate(transform, modelNode->position() + translation);
                transformChanged = true;
            } else {
                transform = glm::translate(transform, modelNode->position());
            }

            glm::quat orientation;
            if (animNode.getOrientation(_time, orientation)) {
                transform *= glm::mat4_cast(orientation);
                transformChanged = true;
            } else {
                transform *= glm::mat4_cast(modelNode->orientation());
            }

            if (transformChanged) {
                _transformByNodeNumber.insert(make_pair(modelNode->nodeNumber(), move(transform)));
            }
        }
    }

    for (auto &child : animNode.children()) {
        computeLocalTransform(*child);
    }
}

void AnimationChannel::freeze() {
    _freeze = true;
}

bool AnimationChannel::isSameAnimation(const Animation &anim, const AnimationProperties &properties) const {
    return _animation.get() == &anim && _properties == properties;
}

bool AnimationChannel::isActive() const {
    return _animation && !_finished;
}

bool AnimationChannel::isPastTransitionTime() const {
    return _animation && _time > _animation->transitionTime();
}

bool AnimationChannel::isFinished() const {
    return _animation && _finished;
}

bool AnimationChannel::getTransformByNodeNumber(uint16_t nodeNumber, glm::mat4 &transform) const {
    auto maybeTransform = _transformByNodeNumber.find(nodeNumber);
    if (maybeTransform != _transformByNodeNumber.end()) {
        transform = maybeTransform->second;
    }
    return maybeTransform != _transformByNodeNumber.end();
}

float AnimationChannel::getTransitionTime() const {
    return _animation ? _animation->transitionTime() : 0.0f;
}

void AnimationChannel::setTime(float time) {
    _time = time;
}

} // namespace scene

} // namespace reone
