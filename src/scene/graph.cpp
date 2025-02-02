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

#include "graph.h"

#include "../graphics/context.h"
#include "../graphics/mesh.h"
#include "../graphics/meshes.h"
#include "../graphics/shaders.h"
#include "../graphics/uniforms.h"
#include "../graphics/walkmesh.h"

#include "collision.h"
#include "node/camera.h"
#include "node/emitter.h"
#include "node/grass.h"
#include "node/grasscluster.h"
#include "node/light.h"
#include "node/mesh.h"
#include "node/model.h"
#include "node/particle.h"
#include "node/sound.h"
#include "node/trigger.h"
#include "node/walkmesh.h"

using namespace std;

using namespace reone::graphics;

namespace reone {

namespace scene {

static constexpr int kMaxFlareLights = 4;
static constexpr int kMaxSoundCount = 4;

static constexpr float kShadowFadeSpeed = 2.0f;
static constexpr float kElevationTestZ = 1024.0f;

static constexpr float kLightRadiusBias = 64.0f;
static constexpr float kLightRadiusBias2 = kLightRadiusBias * kLightRadiusBias;

static constexpr float kMaxCollisionDistanceWalk = 8.0f;
static constexpr float kMaxCollisionDistanceWalk2 = kMaxCollisionDistanceWalk * kMaxCollisionDistanceWalk;

static constexpr float kMaxCollisionDistanceLineOfSight = 16.0f;
static constexpr float kMaxCollisionDistanceLineOfSight2 = kMaxCollisionDistanceLineOfSight * kMaxCollisionDistanceLineOfSight;

void SceneGraph::clear() {
    _modelRoots.clear();
    _walkmeshRoots.clear();
    _soundRoots.clear();
    _grassRoots.clear();
    _activeLights.clear();
}

void SceneGraph::addRoot(shared_ptr<ModelSceneNode> node) {
    _modelRoots.insert(move(node));
}

void SceneGraph::addRoot(shared_ptr<WalkmeshSceneNode> node) {
    _walkmeshRoots.insert(move(node));
}

void SceneGraph::addRoot(shared_ptr<TriggerSceneNode> node) {
    _triggerRoots.insert(move(node));
}

void SceneGraph::addRoot(shared_ptr<GrassSceneNode> node) {
    _grassRoots.insert(move(node));
}

void SceneGraph::addRoot(shared_ptr<SoundSceneNode> node) {
    _soundRoots.insert(move(node));
}

void SceneGraph::removeRoot(const shared_ptr<ModelSceneNode> &node) {
    for (auto it = _activeLights.begin(); it != _activeLights.end();) {
        if (&(*it)->model() == node.get()) {
            it = _activeLights.erase(it);
        } else {
            ++it;
        }
    }

    _modelRoots.erase(node);
}

void SceneGraph::removeRoot(const shared_ptr<WalkmeshSceneNode> &node) {
    _walkmeshRoots.erase(node);
}

void SceneGraph::removeRoot(const shared_ptr<TriggerSceneNode> &node) {
    _triggerRoots.erase(node);
}

void SceneGraph::removeRoot(const shared_ptr<GrassSceneNode> &node) {
    _grassRoots.erase(node);
}

void SceneGraph::removeRoot(const shared_ptr<SoundSceneNode> &node) {
    _soundRoots.erase(node);
}

void SceneGraph::update(float dt) {
    if (_updateRoots) {
        for (auto &root : _modelRoots) {
            root->update(dt);
        }
        for (auto &root : _grassRoots) {
            root->update(dt);
        }
        for (auto &root : _soundRoots) {
            root->update(dt);
        }
    }
    if (!_activeCamera) {
        return;
    }
    cullRoots();
    refresh();
    updateLighting();
    updateShadowLight(dt);
    updateFlareLights();
    updateSounds();
    prepareOpaqueLeafs();
    prepareTransparentLeafs();
}

void SceneGraph::cullRoots() {
    for (auto &root : _modelRoots) {
        bool culled =
            !root->isEnabled() ||
            root->getSquareDistanceTo(*_activeCamera) > root->drawDistance() * root->drawDistance() ||
            (root->isCullable() && !_activeCamera->isInFrustum(*root));

        root->setCulled(culled);
    }
}

void SceneGraph::updateLighting() {
    // Find closest lights and create a lookup
    auto closestLights = computeClosestLights(kMaxLights, [](auto &light, float distance2) {
        float radius2 = light.radius() * light.radius();
        return distance2 < radius2 * radius2 + kLightRadiusBias2;
    });
    set<LightSceneNode *> lookup;
    for (auto &light : closestLights) {
        lookup.insert(light);
    }
    // De-activate active lights, unless found in a lookup. Active lights are removed from the lookup
    for (auto &light : _activeLights) {
        if (lookup.count(light) == 0) {
            light->setActive(false);
        } else {
            lookup.erase(light);
        }
    }
    // Remove active lights that are inactive and completely faded
    for (auto it = _activeLights.begin(); it != _activeLights.end();) {
        auto light = *it;
        if ((!light->isActive() && light->strength() == 0.0f) || (!light->model().isEnabled())) {
            it = _activeLights.erase(it);
        } else {
            ++it;
        }
    }
    // Add closest lights to active lights
    for (auto &light : lookup) {
        if (_activeLights.size() >= kMaxLights) {
            return;
        }
        light->setActive(true);
        _activeLights.push_back(light);
    }
}

void SceneGraph::updateShadowLight(float dt) {
    auto closestLights = computeClosestLights(1, [this](auto &light, float distance2) {
        if (!light.modelNode().light()->shadow) {
            return false;
        }
        float radius = light.radius();
        return distance2 < radius * radius;
    });
    if (_shadowLight) {
        if (closestLights.empty() || _shadowLight != closestLights.front()) {
            _shadowActive = false;
        }
        if (_shadowActive) {
            _shadowStrength = glm::min(1.0f, _shadowStrength + kShadowFadeSpeed * dt);
        } else {
            _shadowStrength = glm::max(0.0f, _shadowStrength - kShadowFadeSpeed * dt);
            if (_shadowStrength == 0.0f) {
                _shadowLight = nullptr;
            }
        }
    }
    if (!_shadowLight && !closestLights.empty()) {
        _shadowLight = closestLights.front();
        _shadowActive = true;
    }
}

void SceneGraph::updateFlareLights() {
    _flareLights = computeClosestLights(kMaxFlareLights, [](auto &light, float distance2) {
        if (light.modelNode().light()->flares.empty()) {
            return false;
        }
        float radius = light.modelNode().light()->flareRadius;
        return distance2 < radius * radius;
    });
}

void SceneGraph::updateSounds() {
    vector<pair<SoundSceneNode *, float>> distances;
    glm::vec3 cameraPos(_activeCamera->localTransform()[3]);

    // For each sound, calculate its distance to the camera
    for (auto &root : _soundRoots) {
        root->setAudible(false);
        if (!root->isEnabled()) {
            continue;
        }
        float dist2 = root->getSquareDistanceTo(cameraPos);
        float maxDist2 = root->maxDistance() * root->maxDistance();
        if (dist2 > maxDist2) {
            continue;
        }
        distances.push_back(make_pair(root.get(), dist2));
    }

    // Take up to N most closest sounds to the camera
    sort(distances.begin(), distances.end(), [](auto &left, auto &right) {
        int leftPriority = left.first->priority();
        int rightPriority = right.first->priority();
        if (leftPriority < rightPriority) {
            return true;
        }
        if (leftPriority > rightPriority) {
            return false;
        }
        return left.second < right.second;
    });
    if (distances.size() > kMaxSoundCount) {
        distances.erase(distances.begin() + kMaxSoundCount, distances.end());
    }

    // Mark closest sounds as audible
    for (auto &pair : distances) {
        pair.first->setAudible(true);
    }
}

void SceneGraph::refresh() {
    _opaqueMeshes.clear();
    _transparentMeshes.clear();
    _shadowMeshes.clear();
    _lights.clear();
    _emitters.clear();

    for (auto &root : _modelRoots) {
        refreshFromNode(root);
    }
}

void SceneGraph::refreshFromNode(const shared_ptr<SceneNode> &node) {
    bool propagate = true;

    switch (node->type()) {
    case SceneNodeType::Model: {
        // Ignore models that have been culled
        auto model = static_pointer_cast<ModelSceneNode>(node);
        if (model->isCulled()) {
            propagate = false;
        }
        break;
    }
    case SceneNodeType::Mesh: {
        // For model nodes, determine whether they should be rendered and cast shadows
        auto modelNode = static_pointer_cast<MeshSceneNode>(node);
        if (modelNode->shouldRender()) {
            // Sort model nodes into transparent and opaque
            if (modelNode->isTransparent()) {
                _transparentMeshes.push_back(modelNode.get());
            } else {
                _opaqueMeshes.push_back(modelNode.get());
            }
        }
        if (modelNode->shouldCastShadows()) {
            _shadowMeshes.push_back(modelNode.get());
        }
        break;
    }
    case SceneNodeType::Light:
        _lights.push_back(static_pointer_cast<LightSceneNode>(node).get());
        break;
    case SceneNodeType::Emitter:
        _emitters.push_back(static_pointer_cast<EmitterSceneNode>(node).get());
        break;
    default:
        break;
    }

    if (propagate) {
        for (auto &child : node->children()) {
            refreshFromNode(child);
        }
    }
}

void SceneGraph::prepareOpaqueLeafs() {
    _opaqueLeafs.clear();

    vector<SceneNode *> bucket;
    auto camera = _activeCamera->camera();

    // Group grass clusters into buckets without sorting
    for (auto &grass : _grassRoots) {
        if (!grass->isEnabled()) {
            continue;
        }
        for (auto &child : grass->children()) {
            if (child->type() != SceneNodeType::GrassCluster) {
                continue;
            }
            auto cluster = static_cast<GrassClusterSceneNode *>(child.get());
            if (!camera->isInFrustum(cluster->getOrigin())) {
                continue;
            }
            if (bucket.size() >= kMaxGrassClusters) {
                _opaqueLeafs.push_back(make_pair(grass.get(), bucket));
                bucket.clear();
            }
            bucket.push_back(cluster);
        }
        if (!bucket.empty()) {
            _opaqueLeafs.push_back(make_pair(grass.get(), bucket));
            bucket.clear();
        }
    }
}

void SceneGraph::prepareTransparentLeafs() {
    _transparentLeafs.clear();

    auto camera = _activeCamera->camera();

    // Add meshes and emitters to transparent leafs
    vector<SceneNode *> leafs;
    for (auto &mesh : _transparentMeshes) {
        leafs.push_back(mesh);
    }
    for (auto &emitter : _emitters) {
        for (auto &child : emitter->children()) {
            if (child->type() != SceneNodeType::Particle) {
                continue;
            }
            auto particle = static_cast<ParticleSceneNode *>(child.get());
            if (!camera->isInFrustum(particle->getOrigin())) {
                continue;
            }
            leafs.push_back(particle);
        }
    }

    // Group transparent leafs into buckets
    SceneNode *bucketParent = nullptr;
    vector<SceneNode *> bucket;
    for (auto leaf : leafs) {
        SceneNode *parent = leaf->parent();
        if (leaf->type() == SceneNodeType::Mesh) {
            parent = &static_cast<MeshSceneNode *>(leaf)->model();
        }
        if (!bucket.empty()) {
            int maxCount = 1;
            if (parent->type() == SceneNodeType::Emitter) {
                maxCount = kMaxParticles;
            } else if (parent->type() == SceneNodeType::Grass) {
                maxCount = kMaxGrassClusters;
            }
            if (bucketParent != parent || bucket.size() >= maxCount) {
                _transparentLeafs.push_back(make_pair(bucketParent, bucket));
                bucket.clear();
            }
        }
        bucketParent = parent;
        bucket.push_back(leaf);
    }
    if (bucketParent && !bucket.empty()) {
        _transparentLeafs.push_back(make_pair(bucketParent, bucket));
    }
}

void SceneGraph::drawShadows() {
    if (!_activeCamera) {
        return;
    }
    _graphicsContext.withFaceCulling(CullFaceMode::Front, [this]() {
        for (auto &mesh : _shadowMeshes) {
            mesh->drawShadow();
        }
    });
}

void SceneGraph::drawOpaque() {
    if (!_activeCamera) {
        return;
    }
    if (_drawWalkmeshes || _drawTriggers) {
        _uniforms.setWalkmesh([this](auto &walkmesh) {
            for (int i = 0; i < kMaxWalkmeshMaterials - 1; ++i) {
                walkmesh.materials[i] = _walkableSurfaces.count(i) > 0 ? glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
            walkmesh.materials[kMaxWalkmeshMaterials - 1] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // triggers
        });
    }
    if (_drawWalkmeshes) {
        // Draw walkmeshes if enabled
        for (auto &walkmesh : _walkmeshRoots) {
            if (walkmesh->isEnabled()) {
                walkmesh->draw();
            }
        }
    } else {
        // Draw opaque meshes
        for (auto &mesh : _opaqueMeshes) {
            mesh->draw();
        }
        // Draw opaque leafs
        for (auto &[node, leafs] : _opaqueLeafs) {
            node->drawLeafs(leafs);
        }
    }
    if (_drawTriggers) {
        for (auto &trigger : _triggerRoots) {
            trigger->draw();
        }
    }
}

void SceneGraph::drawTransparent() {
    if (!_activeCamera || _drawWalkmeshes) {
        return;
    }
    // Draw transparent leafs (incl. meshes)
    for (auto &[node, leafs] : _transparentLeafs) {
        node->drawLeafs(leafs);
    }
}

void SceneGraph::drawLensFlares() {
    // Draw lens flares
    if (_flareLights.empty() || _drawWalkmeshes) {
        return;
    }
    _graphicsContext.withDepthTest(DepthTestMode::None, [this]() {
        for (auto &light : _flareLights) {
            Collision collision;
            if (testLineOfSight(_activeCamera->getOrigin(), light->getOrigin(), collision)) {
                continue;
            }
            light->drawLensFlare(light->modelNode().light()->flares.front());
        }
    });
}

vector<LightSceneNode *> SceneGraph::computeClosestLights(int count, const function<bool(const LightSceneNode &, float)> &pred) const {
    // Compute distance from each light to the camera
    vector<pair<LightSceneNode *, float>> distances;
    for (auto &light : _lights) {
        float distance2 = light->getSquareDistanceTo(*_activeCamera);
        if (!pred(*light, distance2)) {
            continue;
        }
        distances.push_back(make_pair(light, distance2));
    }

    // Sort lights by distance to the camera. Directional lights are prioritizied
    sort(distances.begin(), distances.end(), [](auto &a, auto &b) {
        auto aLight = a.first;
        auto bLight = b.first;
        if (aLight->isDirectional() && !bLight->isDirectional()) {
            return true;
        }
        if (!aLight->isDirectional() && bLight->isDirectional()) {
            return false;
        }
        float aDistance = a.second;
        float bDistance = b.second;
        return aDistance < bDistance;
    });

    // Keep up to maximum number of lights
    if (distances.size() > count) {
        distances.erase(distances.begin() + count, distances.end());
    }

    vector<LightSceneNode *> lights;
    for (auto &light : distances) {
        lights.push_back(light.first);
    }
    return move(lights);
}

bool SceneGraph::testElevation(const glm::vec2 &position, Collision &outCollision) const {
    static glm::vec3 down(0.0f, 0.0f, -1.0f);

    glm::vec3 origin(position, kElevationTestZ);

    for (auto &root : _walkmeshRoots) {
        if (!root->isEnabled()) {
            continue;
        }
        if (!root->walkmesh().isAreaWalkmesh()) {
            float distance2 = root->getSquareDistanceTo2D(position);
            if (distance2 > kMaxCollisionDistanceWalk2) {
                continue;
            }
        }
        auto objSpaceOrigin = glm::vec3(root->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        float distance = 0.0f;
        auto face = root->walkmesh().raycast(_walkcheckSurfaces, objSpaceOrigin, down, 2.0f * kElevationTestZ, distance);
        if (face) {
            if (_walkableSurfaces.count(face->material) == 0) {
                // non-walkable
                return false;
            }
            outCollision.user = root->user();
            outCollision.intersection = origin + distance * down;
            outCollision.normal = root->absoluteTransform() * glm::vec4(face->normal, 0.0f);
            outCollision.material = face->material;
            return true;
        }
    }

    return false;
}

bool SceneGraph::testLineOfSight(const glm::vec3 &origin, const glm::vec3 &dest, Collision &outCollision) const {
    glm::vec3 originToDest(dest - origin);
    glm::vec3 dir(glm::normalize(originToDest));
    float maxDistance = glm::length(originToDest);
    float minDistance = numeric_limits<float>::max();

    for (auto &root : _walkmeshRoots) {
        if (!root->isEnabled()) {
            continue;
        }
        if (!root->walkmesh().isAreaWalkmesh()) {
            float distance2 = root->getSquareDistanceTo(origin);
            if (distance2 > kMaxCollisionDistanceLineOfSight2) {
                continue;
            }
        }
        glm::vec3 objSpaceOrigin(root->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        glm::vec3 objSpaceDir(root->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance = 0.0f;
        auto face = root->walkmesh().raycast(_lineOfSightSurfaces, objSpaceOrigin, objSpaceDir, maxDistance, distance);
        if (!face || distance > minDistance) {
            continue;
        }
        outCollision.user = root->user();
        outCollision.intersection = origin + distance * dir;
        outCollision.normal = root->absoluteTransform() * glm::vec4(face->normal, 0.0f);
        outCollision.material = face->material;
        minDistance = distance;
    }

    return minDistance != numeric_limits<float>::max();
}

bool SceneGraph::testWalk(const glm::vec3 &origin, const glm::vec3 &dest, const IUser *excludeUser, Collision &outCollision) const {
    glm::vec3 originToDest(dest - origin);
    glm::vec3 dir(glm::normalize(originToDest));
    float maxDistance = glm::length(originToDest);
    float minDistance = numeric_limits<float>::max();

    for (auto &root : _walkmeshRoots) {
        if (!root->isEnabled() || root->user() == excludeUser) {
            continue;
        }
        if (!root->walkmesh().isAreaWalkmesh()) {
            float distance2 = root->getSquareDistanceTo(origin);
            if (distance2 > kMaxCollisionDistanceWalk2) {
                continue;
            }
        }
        glm::vec3 objSpaceOrigin(root->absoluteTransformInverse() * glm::vec4(origin, 1.0f));
        glm::vec3 objSpaceDir(root->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance = 0.0f;
        auto face = root->walkmesh().raycast(_walkcheckSurfaces, objSpaceOrigin, objSpaceDir, kMaxCollisionDistanceWalk, distance);
        if (!face || distance > maxDistance || distance > minDistance) {
            continue;
        }
        outCollision.user = root->user();
        outCollision.intersection = origin + distance * dir;
        outCollision.normal = root->absoluteTransform() * glm::vec4(face->normal, 0.0f);
        outCollision.material = face->material;
        minDistance = distance;
    }

    return minDistance != numeric_limits<float>::max();
}

shared_ptr<ModelSceneNode> SceneGraph::pickModelAt(int x, int y, IUser *except) const {
    if (!_activeCamera) {
        return nullptr;
    }

    auto camera = _activeCamera->camera();
    glm::vec4 viewport(0.0f, 0.0f, _options.width, _options.height);
    glm::vec3 start(glm::unProject(glm::vec3(x, _options.height - y, 0.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 end(glm::unProject(glm::vec3(x, _options.height - y, 1.0f), camera->view(), camera->projection(), viewport));
    glm::vec3 dir(glm::normalize(end - start));

    vector<pair<shared_ptr<ModelSceneNode>, float>> distances;
    for (auto &model : _modelRoots) {
        if (!model->isPickable() || model->user() == except) {
            continue;
        }
        if (model->getSquareDistanceTo(start) > kMaxCollisionDistanceLineOfSight2) {
            continue;
        }
        glm::vec3 objSpaceStart(model->absoluteTransformInverse() * glm::vec4(start, 1.0f));
        glm::vec3 objSpaceDir(model->absoluteTransformInverse() * glm::vec4(dir, 0.0f));
        float distance;
        if (model->aabb().raycast(objSpaceStart, objSpaceDir, kMaxCollisionDistanceLineOfSight, distance) && distance > 0.0f) {
            Collision collision;
            if (testLineOfSight(start, start + distance * dir, collision) && collision.user != model->user()) {
                continue;
            }
            distances.push_back(make_pair(model, distance));
        }
    }
    if (distances.empty()) {
        return nullptr;
    }
    sort(distances.begin(), distances.end(), [](auto &left, auto &right) { return left.second < right.second; });

    return distances[0].first;
}

void SceneGraph::fillLightingUniforms() {
    _uniforms.setLighting([this](auto &lighting) {
        lighting.numLights = static_cast<int>(_activeLights.size());
        for (size_t i = 0; i < _activeLights.size(); ++i) {
            LightUniforms &shaderLight = lighting.lights[i];
            shaderLight.position = glm::vec4(_activeLights[i]->getOrigin(), _activeLights[i]->isDirectional() ? 0.0f : 1.0f);
            shaderLight.color = glm::vec4(_activeLights[i]->color(), 1.0f);
            shaderLight.multiplier = _activeLights[i]->multiplier() * _activeLights[i]->strength();
            shaderLight.radius = _activeLights[i]->radius();
            shaderLight.ambientOnly = static_cast<int>(_activeLights[i]->modelNode().light()->ambientOnly);
            shaderLight.dynamicType = _activeLights[i]->modelNode().light()->dynamicType;
        }
    });
}

unique_ptr<DummySceneNode> SceneGraph::newDummy(shared_ptr<ModelNode> modelNode) {
    return make_unique<DummySceneNode>(move(modelNode), *this, _graphicsContext, _meshes, _shaders, _textures, _uniforms);
}

unique_ptr<WalkmeshSceneNode> SceneGraph::newWalkmesh(shared_ptr<Walkmesh> walkmesh) {
    return make_unique<WalkmeshSceneNode>(move(walkmesh), *this, _graphicsContext, _shaders, _uniforms);
}

unique_ptr<TriggerSceneNode> SceneGraph::newTrigger(vector<glm::vec3> geometry) {
    return make_unique<TriggerSceneNode>(move(geometry), *this, _graphicsContext, _shaders, _uniforms);
}

unique_ptr<CameraSceneNode> SceneGraph::newCamera() {
    return make_unique<CameraSceneNode>(*this);
}

unique_ptr<SoundSceneNode> SceneGraph::newSound() {
    return make_unique<SoundSceneNode>(*this, _audioPlayer);
}

unique_ptr<ModelSceneNode> SceneGraph::newModel(shared_ptr<Model> model, ModelUsage usage, IAnimationEventListener *animEventListener) {
    return make_unique<ModelSceneNode>(
        move(model),
        usage,
        *this,
        _graphicsContext,
        _meshes,
        _shaders,
        _textures,
        _uniforms,
        animEventListener);
}

unique_ptr<GrassSceneNode> SceneGraph::newGrass(float density, float quadSize, glm::vec4 probabilities, set<uint32_t> materials, shared_ptr<Texture> texture, shared_ptr<ModelNode> aabbNode) {
    return make_unique<GrassSceneNode>(
        density,
        quadSize,
        move(probabilities),
        move(materials),
        move(texture),
        move(aabbNode),
        *this,
        _graphicsContext,
        _meshes,
        _shaders,
        _textures,
        _uniforms);
}

} // namespace scene

} // namespace reone
