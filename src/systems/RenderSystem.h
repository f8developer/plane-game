//
// Created by ivanr on 5.6.2025 г..
//

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include "Renderer.h"
#include "components/BasicComponent.h"
#include "components/DrawingComponent.h"
#include <unordered_map>

class RenderSystem {
public:
    static void DrawSprites(entt::registry& registry) {
        // Group sprites by texture for better batching
        std::unordered_map<Texture*, std::vector<std::pair<entt::entity, Vector2>>> textureGroups;
        
        const auto view = registry.view<SpriteComponent, TransformComponent>();
        for (auto entity : view) {
            auto& sprite = view.get<SpriteComponent>(entity);
            if (!sprite.texture) continue;  // Skip invalid textures silently in release

            auto& transform = view.get<TransformComponent>(entity);
            textureGroups[sprite.texture].emplace_back(entity, transform.position);
        }

        // Draw all sprites with the same texture in batch
        for (const auto& [texture, entities] : textureGroups) {
            for (const auto& [entity, position] : entities) {
                render::DrawTextureV(texture, position, WHITE);
            }
        }
    }

    static void DrawText(entt::registry& registry) {
        const auto view = registry.view<TextComponent, TransformComponent>();
        for (auto entity : view) {
            auto& text = view.get<TextComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);
            
            // TODO: Implement text rendering with proper font size and color
            render::DrawText(text.text, 
                static_cast<int>(transform.position.x),
                static_cast<int>(transform.position.y),
                20,  // Default font size
                WHITE);
        }
    }

private:
    static constexpr int DEFAULT_FONT_SIZE = 20;
};

#endif //RENDERSYSTEM_H
