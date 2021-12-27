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

#include "world.h"

#include "../camera/perspective.h"
#include "../context.h"
#include "../format/tgawriter.h"
#include "../mesh.h"
#include "../meshes.h"
#include "../renderbuffer.h"
#include "../scene.h"
#include "../shaders.h"
#include "../texture.h"
#include "../textures.h"
#include "../textureutil.h"
#include "../window.h"

using namespace std;

namespace reone {

namespace graphics {

static constexpr float kPointLightShadowsFOV = glm::radians(90.0f);
static constexpr float kPointLightShadowsNearPlane = 0.1f;
static constexpr float kPointLightShadowsFarPlane = 10000.0f;

static constexpr int kScreenshotResolution = 256;

static const vector<float> g_shadowCascadeDivisors {
    0.005f,
    0.015f,
    0.035f,
    0.075f,
    0.155f,
    0.315f,
    0.635f};

static vector<glm::vec4> computeFrustumCornersWorldSpace(const glm::mat4 &projection, const glm::mat4 &view) {
    auto inv = glm::inverse(projection * view);

    vector<glm::vec4> corners;
    for (auto x = 0; x < 2; ++x) {
        for (auto y = 0; y < 2; ++y) {
            for (auto z = 0; z < 2; ++z) {
                auto pt = inv * glm::vec4(
                                    2.0f * x - 1.0f,
                                    2.0f * y - 1.0f,
                                    2.0f * z - 1.0f,
                                    1.0f);
                corners.push_back(pt / pt.w);
            }
        }
    }

    return move(corners);
}

static glm::mat4 computeDirectionalLightSpaceMatrix(
    float fov,
    float aspect,
    float near, float far,
    const glm::vec3 &lightDir,
    const glm::mat4 &cameraView) {

    auto projection = glm::perspective(fov, aspect, near, far);

    glm::vec3 center(0.0f);
    auto corners = computeFrustumCornersWorldSpace(projection, cameraView);
    for (auto &v : corners) {
        center += glm::vec3(v);
    }
    center /= corners.size();

    auto lightView = glm::lookAt(center - lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = numeric_limits<float>::max();
    float maxX = numeric_limits<float>::min();
    float minY = numeric_limits<float>::max();
    float maxY = numeric_limits<float>::min();
    float minZ = numeric_limits<float>::max();
    float maxZ = numeric_limits<float>::min();
    for (auto &v : corners) {
        auto trf = lightView * v;
        minX = min(minX, trf.x);
        maxX = max(maxX, trf.x);
        minY = min(minY, trf.y);
        maxY = max(maxY, trf.y);
        minZ = min(minZ, trf.z);
        maxZ = max(maxZ, trf.z);
    }
    float zMult = 10.0f;
    if (minZ < 0.0f) {
        minZ *= zMult;
    } else {
        minZ /= zMult;
    }
    if (maxZ < 0.0f) {
        maxZ /= zMult;
    } else {
        maxZ *= zMult;
    }

    auto lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

static glm::mat4 getPointLightView(const glm::vec3 &lightPos, CubeMapFace face) {
    switch (face) {
    case CubeMapFace::PositiveX:
        return glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    case CubeMapFace::NegativeX:
        return glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    case CubeMapFace::PositiveY:
        return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
    case CubeMapFace::NegativeY:
        return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
    case CubeMapFace::PositiveZ:
        return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
    case CubeMapFace::NegativeZ:
        return glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
    default:
        throw invalid_argument("side is invalid");
    }
}

void WorldPipeline::init() {
    // Common depth buffers

    _dbCommon = make_unique<Renderbuffer>();
    _dbCommon->configure(_options.width, _options.height, PixelFormat::Depth);
    _dbCommon->init();

    _dbCommonMS = make_unique<Renderbuffer>(_options.aaSamples);
    _dbCommonMS->configure(_options.width, _options.height, PixelFormat::Depth);
    _dbCommonMS->init();

    // Multi-sample geometry framebuffer

    _cbGeometry1MS = make_unique<Texture>("geometry_color1_ms", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    _cbGeometry1MS->clear(_options.width, _options.height, PixelFormat::RGB);
    _cbGeometry1MS->init();

    _cbGeometry2MS = make_unique<Texture>("geometry_color2_ms", getTextureProperties(TextureUsage::ColorBuffer, _options.aaSamples));
    _cbGeometry2MS->clear(_options.width, _options.height, PixelFormat::RGB);
    _cbGeometry2MS->init();

    _fbGeometryMS = make_shared<Framebuffer>();
    _fbGeometryMS->attachColorsDepth(_cbGeometry1MS, _cbGeometry2MS, _dbCommonMS);
    _fbGeometryMS->init();

    // Geometry framebuffer

    _cbGeometry1 = make_unique<Texture>("geometry_color1", getTextureProperties(TextureUsage::ColorBuffer));
    _cbGeometry1->clear(_options.width, _options.height, PixelFormat::RGB);
    _cbGeometry1->init();

    _cbGeometry2 = make_unique<Texture>("geometry_color2", getTextureProperties(TextureUsage::ColorBuffer));
    _cbGeometry2->clear(_options.width, _options.height, PixelFormat::RGB);
    _cbGeometry2->init();

    _fbGeometry = make_shared<Framebuffer>();
    _fbGeometry->attachColorsDepth(_cbGeometry1, _cbGeometry2, _dbCommon);
    _fbGeometry->init();

    // Vertical blur framebuffer

    _cbVerticalBlur = make_unique<Texture>("vertical_blur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _cbVerticalBlur->clear(_options.width, _options.height, PixelFormat::RGB);
    _cbVerticalBlur->init();

    _fbVerticalBlur = make_shared<Framebuffer>();
    _fbVerticalBlur->attachColorDepth(_cbVerticalBlur, _dbCommon);
    _fbVerticalBlur->init();

    // Horizontal blur framebuffer

    _cbHorizontalBlur = make_unique<Texture>("horizontal_blur_color", getTextureProperties(TextureUsage::ColorBuffer));
    _cbHorizontalBlur->clear(_options.width, _options.height, PixelFormat::RGB);
    _cbHorizontalBlur->init();

    _fbHorizontalBlur = make_shared<Framebuffer>();
    _fbHorizontalBlur->attachColorDepth(_cbHorizontalBlur, _dbCommon);
    _fbHorizontalBlur->init();

    // Directional light shadows framebuffer

    _dbDirectionalLightShadows = make_unique<Texture>("point_light_shadows_color", getTextureProperties(TextureUsage::DepthBuffer));
    _dbDirectionalLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth, kNumShadowCascades);
    _dbDirectionalLightShadows->init();

    _fbDirectionalLightShadows = make_shared<Framebuffer>();
    _fbDirectionalLightShadows->attachDepth(_dbDirectionalLightShadows);
    _fbDirectionalLightShadows->init();

    // Point light shadows framebuffer

    _dbPointLightShadows = make_unique<Texture>("directional_light_shadows_color", getTextureProperties(TextureUsage::DepthBufferCubeMap));
    _dbPointLightShadows->clear(_options.shadowResolution, _options.shadowResolution, PixelFormat::Depth);
    _dbPointLightShadows->init();

    _fbPointLightShadows = make_shared<Framebuffer>();
    _fbPointLightShadows->attachDepth(_dbPointLightShadows);
    _fbPointLightShadows->init();

    // Screenshot framebuffer

    _cbScreenshot = make_unique<Texture>("screenshot_color", getTextureProperties(TextureUsage::ColorBuffer));
    _cbScreenshot->clear(kScreenshotResolution, kScreenshotResolution, PixelFormat::RGB);
    _cbScreenshot->init();

    _fbScreenshot = make_shared<Framebuffer>();
    _fbScreenshot->attachColorDepth(_cbScreenshot, _dbCommon);
    _fbScreenshot->init();
}

void WorldPipeline::draw() {
    if (!_scene->camera()) {
        return;
    }
    computeLightSpaceMatrices();

    drawShadows();
    drawGeometry();
    drawHorizontalBlur();
    drawVerticalBlur();
    presentWorld();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WorldPipeline::computeLightSpaceMatrices() {
    if (!_scene->hasShadowLight()) {
        return;
    }
    if (_scene->isShadowLightDirectional()) {
        auto camera = static_pointer_cast<PerspectiveCamera>(_scene->camera());
        auto lightDir = glm::normalize(camera->position() - _scene->shadowLightPosition());
        float fovy = camera->fovy();
        float aspect = camera->aspect();
        float cameraNear = camera->zNear();
        float cameraFar = camera->zFar();
        for (int i = 0; i < kNumShadowCascades; ++i) {
            float near = cameraNear;
            if (i > 0) {
                near = cameraFar * g_shadowCascadeDivisors[i - 1];
            }
            float far = cameraFar;
            if (i < kNumShadowCascades - 1) {
                far *= g_shadowCascadeDivisors[i];
            }
            _shadowLightSpace[i] = computeDirectionalLightSpaceMatrix(fovy, aspect, near, far, lightDir, camera->view());
            _shadowCascadeFarPlanes[i / 4][i % 4] = far;
        }
    } else {
        glm::mat4 projection(glm::perspective(kPointLightShadowsFOV, 1.0f, kPointLightShadowsNearPlane, kPointLightShadowsFarPlane));
        for (int i = 0; i < kNumCubeFaces; ++i) {
            glm::mat4 lightView(getPointLightView(_scene->shadowLightPosition(), static_cast<CubeMapFace>(i)));
            _shadowLightSpace[i] = projection * lightView;
        }
    }
}

void WorldPipeline::drawShadows() {
    if (!_scene->hasShadowLight()) {
        return;
    }

    // Set global uniforms
    glm::vec4 lightPosition(
        _scene->shadowLightPosition(),
        _scene->isShadowLightDirectional() ? 0.0f : 1.0f);
    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.shadowLightPosition = move(lightPosition);
    for (int i = 0; i < kNumShadowLightSpace; ++i) {
        uniforms.general.shadowLightSpace[i] = _shadowLightSpace[i];
    }

    // Bind shadows framebuffer
    auto framebuffer = _scene->isShadowLightDirectional() ? _fbDirectionalLightShadows : _fbPointLightShadows;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->nameGL());
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);

    _graphicsContext.withViewport(glm::ivec4(0, 0, _options.shadowResolution, _options.shadowResolution), [this]() {
        _graphicsContext.clearDepth();
        _scene->drawShadows();
    });
}

void WorldPipeline::drawGeometry() {
    static constexpr GLenum colors[] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

    auto camera = _scene->camera();

    // Set global uniforms

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.projection = camera->projection();
    uniforms.general.view = camera->view();
    uniforms.general.cameraPosition = glm::vec4(camera->position(), 1.0f);
    uniforms.general.worldAmbientColor = glm::vec4(_scene->ambientLightColor(), 1.0f);
    uniforms.general.fogNear = _scene->fogNear();
    uniforms.general.fogFar = _scene->fogFar();
    uniforms.general.fogColor = glm::vec4(_scene->fogColor(), 1.0f);

    if (_scene->hasShadowLight()) {
        glm::vec4 lightPosition(
            _scene->shadowLightPosition(),
            _scene->isShadowLightDirectional() ? 0.0f : 1.0f);
        uniforms.general.shadowLightPosition = move(lightPosition);
        uniforms.general.shadowStrength = _scene->shadowStrength();
        uniforms.general.shadowRadius = _scene->shadowRadius();
        for (int i = 0; i < kNumShadowLightSpace; ++i) {
            uniforms.general.shadowLightSpace[i] = _shadowLightSpace[i];
        }
        for (int i = 0; i < 2; ++i) {
            uniforms.general.shadowCascadeFarPlanes[i] = _shadowCascadeFarPlanes[i];
        }
    }

    // Draw scene to multi-sample framebuffer

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbGeometryMS->nameGL());
    glDrawBuffers(2, colors);

    if (_scene->hasShadowLight()) {
        if (_scene->isShadowLightDirectional()) {
            _textures.bind(*_dbDirectionalLightShadows, TextureUnits::shadowMap);
        } else {
            _textures.bind(*_dbPointLightShadows, TextureUnits::cubeShadowMap);
        }
    }

    _graphicsContext.clearColorDepth();
    _scene->draw();

    // Blit multi-sample geometry framebuffer to geometry framebuffer

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbGeometryMS->nameGL());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbGeometry->nameGL());
    for (int i = 0; i < 2; ++i) {
        glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glBlitFramebuffer(0, 0, _options.width, _options.height, 0, 0, _options.width, _options.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

void WorldPipeline::drawHorizontalBlur() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbHorizontalBlur->nameGL());

    // Set uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.featureMask = UniformsFeatureFlags::blur;
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(1.0f, 0.0f);

    // Draw quad with second geometry color buffer as texture

    _shaders.use(_shaders.blur(), true);
    _textures.bind(*_cbGeometry2);

    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void WorldPipeline::drawVerticalBlur() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbVerticalBlur->nameGL());

    // Set shader uniforms

    float w = static_cast<float>(_options.width);
    float h = static_cast<float>(_options.height);

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();
    uniforms.general.featureMask = UniformsFeatureFlags::blur;
    uniforms.general.blurResolution = glm::vec2(w, h);
    uniforms.general.blurDirection = glm::vec2(0.0f, 1.0f);

    // Draw quad with horizontal blur color buffer as texture

    _shaders.use(_shaders.blur(), true);
    _textures.bind(*_cbHorizontalBlur);

    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();
}

void WorldPipeline::presentWorld() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Reset uniforms

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetGlobals();
    uniforms.general.resetLocals();

    // Draw quad with geometry color buffer and vertical blur color buffer as textures

    _shaders.use(_shaders.presentWorld(), true);

    _textures.bind(*_cbGeometry1);
    _textures.bind(*_cbVerticalBlur, TextureUnits::bloom);

    _graphicsContext.clearColorDepth();
    _meshes.quadNDC().draw();

    // Render to texture

    if (_takeScreenshot) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbScreenshot->nameGL());

        _graphicsContext.withViewport(glm::ivec4(0, 0, kScreenshotResolution, kScreenshotResolution), [this]() {
            _graphicsContext.clearColorDepth();
            _meshes.quadNDC().draw();
        });

        _textures.bind(*_cbScreenshot);
        _cbScreenshot->flushGPUToCPU();
        _takeScreenshot = false;
    }
}

} // namespace graphics

} // namespace reone