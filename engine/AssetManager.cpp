//
// Created by ivanr on 4.6.2025 г..
//

#include "AssetManager.h"
#include "Log.h"

#include <ranges>

// Static member initialization
std::unordered_map<std::string_view, AssetManager::TextureData> AssetManager::textures;
std::unordered_map<i32, std::vector<std::string_view>> AssetManager::sceneOwnedTextures;

// Font management
std::unordered_map<std::string_view, Font> AssetManager::fonts;
std::unordered_map<i32, std::vector<std::string_view>> AssetManager::sceneOwnedFonts;

std::unordered_set<std::string> AssetManager::stringPool;

std::string_view AssetManager::InternString(std::string_view str) noexcept {
    // Convert string_view to string for lookup
    std::string strKey(str);
    auto it = stringPool.find(strKey);
    if (it != stringPool.end()) {
        return *it;
    }
    auto [inserted_it, _] = stringPool.emplace(std::move(strKey));
    return *inserted_it;
}

std::string AssetManager::GetAssetPath(std::string_view path) noexcept {
    std::string fullPath;
    fullPath.reserve(7 + path.length()); // "assets/" + path length
    fullPath = "assets/";
    fullPath.append(path);
    return fullPath;
}

void AssetManager::CalculateFramePositions(std::string_view name, const Texture& texture, const Vector2Int& gridSize) noexcept {
    const int framesPerRow = texture.width / gridSize.x;
    const int totalFrames = (texture.width / gridSize.x) * (texture.height / gridSize.y);
    
    auto& textureData = textures[InternString(name)];
    textureData.framePositions.clear();
    textureData.framePositions.reserve(totalFrames);
    
    for (int i = 0; i < totalFrames; ++i) {
        const int row = i / framesPerRow;
        const int col = i % framesPerRow;
        
        textureData.framePositions.emplace_back(
            static_cast<float>(col * gridSize.x),
            static_cast<float>(row * gridSize.y),
            static_cast<float>(gridSize.x),
            static_cast<float>(gridSize.y)
        );
    }
}

void AssetManager::CalculateTilePositions(std::string_view name, const Texture& texture, const Vector2Int& tileSize) noexcept {
    const int tilesPerRow = texture.width / tileSize.x;
    const int tilesPerCol = texture.height / tileSize.y;
    
    auto& textureData = textures[InternString(name)];
    textureData.tilePositions.clear();
    textureData.tilePositions.resize(tilesPerCol);
    
    for (int row = 0; row < tilesPerCol; ++row) {
        textureData.tilePositions[row].reserve(tilesPerRow);
        for (int col = 0; col < tilesPerRow; ++col) {
            textureData.tilePositions[row].emplace_back(
                static_cast<float>(col * tileSize.x),
                static_cast<float>(row * tileSize.y),
                static_cast<float>(tileSize.x),
                static_cast<float>(tileSize.y)
            );
        }
    }
}

void AssetManager::AddSceneTexture(std::string_view name, std::string_view path, i32 sceneIdentity) noexcept {
    std::string_view internedName = InternString(name);
    Texture texture = LoadTexture(GetAssetPath(path).c_str());
    textures[internedName] = TextureData{
        std::move(texture),
        TextureType::Single,
        Vector2Int{0, 0},
        {},
        {}
    };
    sceneOwnedTextures[sceneIdentity].push_back(internedName);
}

void AssetManager::AddSceneAnimatedTexture(std::string_view name, std::string_view path, i32 sceneIdentity, Vector2Int gridSquareSize) noexcept {
    std::string_view internedName = InternString(name);
    Texture texture = LoadTexture(GetAssetPath(path).c_str());
    textures[internedName] = TextureData{
        std::move(texture),
        TextureType::Animated,
        gridSquareSize,
        {},
        {}
    };
    CalculateFramePositions(name, textures[internedName].texture, gridSquareSize);
    sceneOwnedTextures[sceneIdentity].push_back(internedName);
}

void AssetManager::AddSceneTiledTexture(std::string_view name, std::string_view path, i32 sceneIdentity, Vector2Int tileSize) noexcept {
    std::string_view internedName = InternString(name);
    Texture texture = LoadTexture(GetAssetPath(path).c_str());
    textures[internedName] = TextureData{
        std::move(texture),
        TextureType::Tiled,
        tileSize,
        {},
        {}
    };
    CalculateTilePositions(name, textures[internedName].texture, tileSize);
    sceneOwnedTextures[sceneIdentity].push_back(internedName);
}

void AssetManager::AddSceneFont(std::string_view name, std::string_view path, i32 sceneIdentity, int fontSize) noexcept {
    Font font;
    if (fontSize > 0) {
        font = LoadFontEx(GetAssetPath(path).c_str(), fontSize, nullptr, 0);
    } else {
        font = LoadFont(GetAssetPath(path).c_str());
    }
    
    std::string_view internedName = InternString(name);
    fonts.emplace(internedName, std::move(font));
    sceneOwnedFonts[sceneIdentity].push_back(internedName);
}

void AssetManager::AddSceneFontWithCodepoints(std::string_view name, std::string_view path, i32 sceneIdentity, int fontSize, const std::vector<int>& codepoints) noexcept {
    Font font;
    if (fontSize > 0) {
        font = LoadFontEx(GetAssetPath(path).c_str(), fontSize, const_cast<int*>(codepoints.data()), static_cast<int>(codepoints.size()));
    } else {
        font = LoadFontEx(GetAssetPath(path).c_str(), 10, const_cast<int*>(codepoints.data()), static_cast<int>(codepoints.size()));
    }
    
    std::string_view internedName = InternString(name);
    fonts.emplace(internedName, std::move(font));
    sceneOwnedFonts[sceneIdentity].push_back(internedName);
}

const Texture& AssetManager::GetTexture(std::string_view name) noexcept {
    auto it = textures.find(InternString(name));
    if (it == textures.end()) {
        LOG_ERROR("Texture '%s' not found!", std::string(name).c_str());
        static Texture dummyTexture{};
        return dummyTexture;
    }
    return it->second.texture;
}

std::pair<const Texture&, Rectangle> AssetManager::GetTextureFrame(std::string_view name, const int frame) noexcept {
    std::string_view internedName = InternString(name);
    auto it = textures.find(internedName);
    if (it == textures.end()) {
        LOG_ERROR("Texture '%s' not found!", std::string(name).c_str());
        static Texture dummyTexture{};
        return {dummyTexture, {0,0,0,0}};
    }
    auto& textureData = it->second;
    
    // Default source rectangle (entire texture)
    Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(textureData.texture.width),
        static_cast<float>(textureData.texture.height)
    };
    
    // Check if this is an animated texture
    if (textureData.type == TextureType::Animated && !textureData.framePositions.empty()) {
        if (frame >= 0 && frame < textureData.framePositions.size()) {
            sourceRec = textureData.framePositions[frame];
        }
    }
    
    return {textureData.texture, sourceRec};
}

std::pair<const Texture&, Rectangle> AssetManager::GetTile(std::string_view name, int tileX, int tileY) noexcept {
    std::string_view internedName = InternString(name);
    auto it = textures.find(internedName);
    if (it == textures.end()) {
        LOG_ERROR("Texture '%s' not found!", std::string(name).c_str());
        static Texture dummyTexture{};
        return {dummyTexture, {0,0,0,0}};
    }
    auto& textureData = it->second;
    
    // Default source rectangle (entire texture)
    Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(textureData.texture.width),
        static_cast<float>(textureData.texture.height)
    };
    
    // Check if this is a tiled texture
    if (textureData.type == TextureType::Tiled && !textureData.tilePositions.empty()) {
        if (tileY >= 0 && tileY < textureData.tilePositions.size() && 
            tileX >= 0 && tileX < textureData.tilePositions[tileY].size()) {
            sourceRec = textureData.tilePositions[tileY][tileX];
        }
    }
    
    return {textureData.texture, sourceRec};
}

const Font& AssetManager::GetFont(std::string_view name) noexcept {
    auto it = fonts.find(InternString(name));
    if (it == fonts.end()) {
        LOG_ERROR("Font '%s' not found!", std::string(name).c_str());
        static Font dummyFont{};
        return dummyFont;
    }
    return it->second;
}

void AssetManager::RemoveSceneTextures(i32 sceneIdentity) noexcept {
    if (const auto it = sceneOwnedTextures.find(sceneIdentity); it != sceneOwnedTextures.end()) {
        for (const auto& textureName : it->second) {
            UnloadTexture(textureName);
            textures.erase(textureName);
        }
        sceneOwnedTextures.erase(it);
    }

    RemoveSceneFonts(sceneIdentity);
}

void AssetManager::UnloadTexture(std::string_view name) noexcept {
    auto it = textures.find(InternString(name));
    if (it == textures.end()) {
        LOG_ERROR("Tried to unload missing texture '%s'", std::string(name).c_str());
        return;
    }
    const Texture& texture = it->second.texture;
    ::UnloadTexture(texture);
    textures.erase(it);
}

void AssetManager::UnloadFont(std::string_view name) noexcept {
    auto it = fonts.find(InternString(name));
    if (it == fonts.end()) {
        LOG_ERROR("Tried to unload missing font '%s'", std::string(name).c_str());
        return;
    }
    const Font& font = it->second;
    ::UnloadFont(font);
    fonts.erase(it);
}

void AssetManager::RemoveSceneFonts(i32 sceneIdentity) noexcept {
    if (const auto it = sceneOwnedFonts.find(sceneIdentity); it != sceneOwnedFonts.end()) {
        for (const auto& fontName : it->second) {
            UnloadFont(fontName);
        }
        sceneOwnedFonts.erase(it);
    }
}