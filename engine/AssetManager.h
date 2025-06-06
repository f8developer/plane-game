//
// Created by ivanr on 4.6.2025 г..
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <unordered_map>
#include <vector>
#include <string_view>
#include <unordered_set>
#include <memory>

#include "Defines.h"
#include "raylib.h"

class AssetManager {
public:
    enum class TextureType {
        Single,     // Regular single texture
        Animated,   // Spritesheet animation
        Tiled       // Tiled texture
    };

    struct TextureData {
        Texture texture;
        TextureType type;
        Vector2Int gridSize;
        std::vector<Rectangle> framePositions;
        std::vector<std::vector<Rectangle>> tilePositions;
    };

    // Texture management
    DLLEX static void AddSceneTexture(std::string_view name, std::string_view path, i32 sceneIdentity) noexcept;
    DLLEX static void AddSceneAnimatedTexture(std::string_view name, std::string_view path, i32 sceneIdentity, Vector2Int gridSquareSize) noexcept;
    DLLEX static void AddSceneTiledTexture(std::string_view name, std::string_view path, i32 sceneIdentity, Vector2Int tileSize) noexcept;
    DLLEX static const Texture& GetTexture(std::string_view name) noexcept;
    DLLEX static std::pair<const Texture&, Rectangle> GetTextureFrame(std::string_view name, int frame) noexcept;
    DLLEX static std::pair<const Texture&, Rectangle> GetTile(std::string_view name, int tileX, int tileY) noexcept;
    DLLEX static void RemoveSceneTextures(i32 sceneIdentity) noexcept;

    // Font management
    DLLEX static void AddSceneFont(std::string_view name, std::string_view path, i32 sceneIdentity, int fontSize = 0) noexcept;
    DLLEX static void AddSceneFontWithCodepoints(std::string_view name, std::string_view path, i32 sceneIdentity, int fontSize, const std::vector<int>& codepoints) noexcept;
    DLLEX static const Font& GetFont(std::string_view name) noexcept;
    DLLEX static void RemoveSceneFonts(i32 sceneIdentity) noexcept;

private:
    static void UnloadTexture(std::string_view name) noexcept;
    static void UnloadFont(std::string_view name) noexcept;
    static void CalculateFramePositions(std::string_view name, const Texture& texture, const Vector2Int& gridSize) noexcept;
    static void CalculateTilePositions(std::string_view name, const Texture& texture, const Vector2Int& tileSize) noexcept;
    static std::string GetAssetPath(std::string_view path) noexcept;
    static std::string_view InternString(std::string_view str) noexcept;

    // Texture management
    static std::unordered_map<std::string_view, TextureData> textures;
    static std::unordered_map<i32, std::vector<std::string_view>> sceneOwnedTextures;

    // Font management
    static std::unordered_map<std::string_view, Font> fonts;
    static std::unordered_map<i32, std::vector<std::string_view>> sceneOwnedFonts;

    // String interning
    static std::unordered_set<std::string> stringPool;
};

#endif //ASSETMANAGER_H
