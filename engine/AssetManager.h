//
// Created by ivanr on 4.6.2025 г..
//

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <unordered_map>
#include <vector>

#include "Defines.h"
#include "raylib.h"

class AssetManager {
public:
    DLLEX static void AddSceneTexture(const str& name, const str& path, i32 sceneIdentity);
    DLLEX static Texture& GetTexture(const str& name);
    DLLEX static void RemoveSceneTextures(i32 sceneIdentity);
private:
    static void UnloadTexture(const str& name);

    static std::unordered_map<str, Texture> textures;
    static std::unordered_map<i32, std::vector<str>> sceneOwnedTextures;
};



#endif //ASSETMANAGER_H
