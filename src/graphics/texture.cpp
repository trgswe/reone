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

#include "texture.h"

#include "pixelutil.h"
#include "textureutil.h"

using namespace std;

namespace reone {

namespace graphics {

static bool isMipmapFilter(Texture::Filtering filter) {
    switch (filter) {
    case Texture::Filtering::NearestMipmapNearest:
    case Texture::Filtering::LinearMipmapNearest:
    case Texture::Filtering::NearestMipmapLinear:
    case Texture::Filtering::LinearMipmapLinear:
        return true;
    default:
        return false;
    }
}

static uint32_t getPixelFormatGL(PixelFormat format) {
    switch (format) {
    case PixelFormat::Grayscale:
        return GL_RED;
    case PixelFormat::RGB:
        return GL_RGB;
    case PixelFormat::RGBA:
    case PixelFormat::DXT1:
    case PixelFormat::DXT5:
        return GL_RGBA;
    case PixelFormat::BGR:
        return GL_BGR;
    case PixelFormat::BGRA:
        return GL_BGRA;
    case PixelFormat::Depth:
        return GL_DEPTH_COMPONENT;
    case PixelFormat::DepthStencil:
        return GL_DEPTH_STENCIL;
    default:
        throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(format)));
    }
}

static uint32_t getPixelTypeGL(PixelFormat format) {
    switch (format) {
    case PixelFormat::Grayscale:
    case PixelFormat::RGB:
    case PixelFormat::RGBA:
    case PixelFormat::BGR:
    case PixelFormat::BGRA:
        return GL_UNSIGNED_BYTE;
    case PixelFormat::Depth:
        return GL_FLOAT;
    case PixelFormat::DepthStencil:
        return GL_UNSIGNED_INT_24_8;
    default:
        throw logic_error("Unsupported pixel format: " + to_string(static_cast<int>(format)));
    }
}

static uint32_t getFilterGL(Texture::Filtering filter) {
    switch (filter) {
    case Texture::Filtering::Nearest:
        return GL_NEAREST;
    case Texture::Filtering::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case Texture::Filtering::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    case Texture::Filtering::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case Texture::Filtering::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case Texture::Filtering::Linear:
    default:
        return GL_LINEAR;
    }
}

void Texture::init() {
    if (_inited) {
        return;
    }
    glGenTextures(1, &_nameGL);
    bind();
    configure();
    refresh();
    _inited = true;
}

void Texture::deinit() {
    if (!_inited) {
        return;
    }
    glDeleteTextures(1, &_nameGL);
    _inited = false;
}

void Texture::bind() {
    auto target = getTargetGL();
    glBindTexture(target, _nameGL);
}

void Texture::unbind() {
    auto target = getTargetGL();
    glBindTexture(target, 0);
}

void Texture::configure() {
    if (isCubeMap()) {
        configureCubeMap();
    } else {
        configure2D();
    }
}

void Texture::configureCubeMap() {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, getFilterGL(_properties.minFilter));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, getFilterGL(_properties.maxFilter));

    switch (_properties.wrap) {
    case Wrapping::ClampToBorder:
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &_properties.borderColor[0]);
        break;
    case Wrapping::ClampToEdge:
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        break;
    case Wrapping::Repeat:
    default:
        // Wrap is GL_REPEAT by default in OpenGL
        break;
    }
}

void Texture::configure2D() {
    auto target = getTargetGL();
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, getFilterGL(_properties.minFilter));
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, getFilterGL(_properties.maxFilter));

    switch (_properties.wrap) {
    case Wrapping::ClampToBorder:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, &_properties.borderColor[0]);
        break;
    case Wrapping::ClampToEdge:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    case Wrapping::Repeat:
    default:
        // Wrap is GL_REPEAT by default in OpenGL
        break;
    }
}

void Texture::refresh() {
    if (isCubeMap()) {
        refreshCubeMap();
    } else if (isMultilayer()) {
        refresh2DArray();
    } else {
        refresh2D();
    }
}

void Texture::refreshCubeMap() {
    for (int i = 0; i < kNumCubeFaces; ++i) {
        if (i < _layers.size() && !_layers[i].mipMaps.empty()) {
            const MipMap &mipMap = _layers[i].mipMaps.front();
            fillTarget2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mipMap.width, mipMap.height, mipMap.pixels->data(), static_cast<int>(mipMap.pixels->size()));
        } else {
            fillTarget2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, _width, _height);
        }
    }

    // Generate mip maps, if required
    if (isMipmapFilter(_properties.minFilter)) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
}

void Texture::refresh2DArray() {
    int numLayers = static_cast<int>(_layers.size());
    fillTarget3D(_width, _height, numLayers);
}

void Texture::refresh2D() {
    GLenum target = isMultisample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    int numMipMaps = !_layers.empty() ? static_cast<int>(_layers[0].mipMaps.size()) : 0;
    if (numMipMaps > 0) {
        for (int i = 0; i < numMipMaps; ++i) {
            const MipMap &mipMap = _layers[0].mipMaps[i];
            fillTarget2D(target, i, mipMap.width, mipMap.height, mipMap.pixels->data(), static_cast<int>(mipMap.pixels->size()));
        }
        if (numMipMaps > 1) {
            glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, numMipMaps - 1);
        } else if (isMipmapFilter(_properties.minFilter)) {
            glGenerateMipmap(target);
        }
    } else {
        fillTarget2D(target, 0, _width, _height);
    }
}

void Texture::clear(int w, int h, PixelFormat format, int numLayers, bool refresh) {
    _width = w;
    _height = h;
    _pixelFormat = format;

    _layers.clear();
    _layers.resize(numLayers);

    if (refresh) {
        this->refresh();
    }
}

void Texture::setPixels(int w, int h, PixelFormat format, shared_ptr<ByteArray> pixels, bool refresh) {
    MipMap mipMap;
    mipMap.width = w;
    mipMap.height = h;
    mipMap.pixels = move(pixels);

    Layer layer;
    layer.mipMaps.push_back(move(mipMap));

    setPixels(w, h, format, vector<Layer> {layer}, refresh);
}

void Texture::setPixels(int w, int h, PixelFormat format, vector<Layer> layers, bool refresh) {
    if (layers.empty()) {
        throw invalid_argument("layers is empty");
    }
    _width = w;
    _height = h;
    _pixelFormat = format;
    _layers = move(layers);

    if (refresh) {
        this->refresh();
    }
}

void Texture::flushGPUToCPU() {
    if (isMultilayer()) {
        throw logic_error("Flushing cubemap or array textures is not supported");
    }
    if (isMultisample()) {
        throw logic_error("Flushing multisample textures is not supported");
    }
    Layer layer;
    layer.mipMaps.resize(1);

    MipMap &mipMap = layer.mipMaps[0];
    mipMap.width = _width;
    mipMap.height = _height;
    mipMap.pixels = make_shared<ByteArray>();
    mipMap.pixels->resize(3 * _width * _height);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, &(*mipMap.pixels)[0]);

    _layers.clear();
    _layers.push_back(move(layer));
}

glm::vec4 Texture::sample(float s, float t) const {
    int x = glm::round(glm::fract(s) * (_width - 1));
    int y = glm::round(glm::fract(t) * (_height - 1));
    return sample(x, y);
}

glm::vec4 Texture::sample(int x, int y) const {
    if (isMultilayer()) {
        throw logic_error("Sampling cubemap or array textures is not supported");
    }
    if (isMultisample()) {
        throw logic_error("Sampling multisample textures is not supported");
    }
    if (isCompressed(_pixelFormat)) {
        throw logic_error("Sampling compressed textures is not supported");
    }
    bool grayscale = isGrayscale();
    bool alpha = _pixelFormat == PixelFormat::RGBA || _pixelFormat == PixelFormat::BGRA;
    int bpp = grayscale ? 1 : (alpha ? 4 : 3);

    auto &pixels = *_layers.front().mipMaps.front().pixels;
    auto pixel = &pixels[bpp * (y * _width + x)];

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
    switch (_pixelFormat) {
    case PixelFormat::Grayscale:
        r = static_cast<uint8_t>(pixel[0]) / 255.0f;
        g = r;
        b = r;
        break;
    case PixelFormat::RGB:
        r = static_cast<uint8_t>(pixel[0]) / 255.0f;
        g = static_cast<uint8_t>(pixel[1]) / 255.0f;
        b = static_cast<uint8_t>(pixel[2]) / 255.0f;
        break;
    case PixelFormat::RGBA:
        r = static_cast<uint8_t>(pixel[0]) / 255.0f;
        g = static_cast<uint8_t>(pixel[1]) / 255.0f;
        b = static_cast<uint8_t>(pixel[2]) / 255.0f;
        a = static_cast<uint8_t>(pixel[3]) / 255.0f;
        break;
    case PixelFormat::BGR:
        r = static_cast<uint8_t>(pixel[2]) / 255.0f;
        g = static_cast<uint8_t>(pixel[1]) / 255.0f;
        b = static_cast<uint8_t>(pixel[0]) / 255.0f;
        break;
    case PixelFormat::BGRA:
        r = static_cast<uint8_t>(pixel[2]) / 255.0f;
        g = static_cast<uint8_t>(pixel[1]) / 255.0f;
        b = static_cast<uint8_t>(pixel[0]) / 255.0f;
        a = static_cast<uint8_t>(pixel[3]) / 255.0f;
        break;
    default:
        throw logic_error("Unsupported texture format: " + to_string(static_cast<int>(_pixelFormat)));
    }

    return glm::vec4(r, g, b, a);
}

void Texture::fillTarget2D(uint32_t target, int level, int width, int height, const void *pixels, int size) {
    switch (_pixelFormat) {
    case PixelFormat::DXT1:
    case PixelFormat::DXT5:
        glCompressedTexImage2D(target, level, getInternalPixelFormatGL(_pixelFormat), width, height, 0, size, pixels);
        break;
    default:
        if (isMultisample()) {
            // Multisample textures can only be used as color buffers
            glTexImage2DMultisample(target, _properties.numSamples, getInternalPixelFormatGL(_pixelFormat), width, height, GL_TRUE);
        } else {
            glTexImage2D(target, level, getInternalPixelFormatGL(_pixelFormat), width, height, 0, getPixelFormatGL(_pixelFormat), getPixelTypeGL(_pixelFormat), pixels);
        }
        break;
    }
}

void Texture::fillTarget3D(int width, int height, int depth) {
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, getInternalPixelFormatGL(_pixelFormat), width, height, depth, 0, getPixelFormatGL(_pixelFormat), getPixelTypeGL(_pixelFormat), nullptr);
}

uint32_t Texture::getTargetGL() const {
    if (isCubeMap()) {
        return GL_TEXTURE_CUBE_MAP;
    } else if (isMultilayer()) {
        return GL_TEXTURE_2D_ARRAY;
    } else {
        return isMultisample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }
}

} // namespace graphics

} // namespace reone
