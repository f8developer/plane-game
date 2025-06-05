//
// Created by ivanr on 4.6.2025 г..
//

#include "AssetManager.h"

#include <ranges>

std::unordered_map<str, Texture> AssetManager::textures;
std::unordered_map<i32, std::vector<str>> AssetManager::sceneOwnedTextures;

void AssetManager::AddSceneTexture(const str& name, const str& path, const i32 sceneIdentity) {
    Texture texture = LoadTexture(("assets/"+path).c_str() );
    textures.emplace(name, texture);
    sceneOwnedTextures[sceneIdentity].push_back(name);  // Add to scene's texture list
}

Texture& AssetManager::GetTexture(const str& name) {
    return textures.at(name);
}

void AssetManager::RemoveSceneTextures(const i32 sceneIdentity) {
    if (const auto it = sceneOwnedTextures.find(sceneIdentity); it != sceneOwnedTextures.end()) {
        for (const auto& textureName : it->second) {
            UnloadTexture(textureName);
        }
        sceneOwnedTextures.erase(it);
    }
}

void AssetManager::UnloadTexture(const str& name) {
    const Texture& texture = GetTexture(name);
    ::UnloadTexture(texture);
    textures.erase(name);
}