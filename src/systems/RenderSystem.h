//
// Created by ivanr on 5.6.2025 г..
//

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include "Renderer.h"
#include "components/BasicComponent.h"
#include "components/DrawingComponent.h"
#include "GameConfig.h"
#include <unordered_map>

class RenderSystem {
public:
    static void DrawPixelPerfect(entt::registry& registry) {
        const auto view = registry.view<PixelPerfectCameraComponent>();
        if (view.empty()) return;  // Early exit if no camera

        auto& camera = view.get<PixelPerfectCameraComponent>(view.front());
        
        // Cache screen dimensions
        const int screenWidth = GetScreenWidth();
        const int screenHeight = GetScreenHeight();
        
        // Calculate aspect ratios once
        static constexpr float virtualAspect = static_cast<float>(VIRTUAL_WIDTH) / static_cast<float>(VIRTUAL_HEIGHT);
        const float screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        
        // Calculate viewport dimensions to maintain aspect ratio
        int viewportWidth, viewportHeight;
        int offsetX = 0, offsetY = 0;
        
        if (screenAspect > virtualAspect) {
            // Screen is wider than virtual resolution
            viewportHeight = screenHeight;
            viewportWidth = static_cast<int>(viewportHeight * virtualAspect);
            offsetX = (screenWidth - viewportWidth) / 2;
        } else {
            // Screen is taller than virtual resolution
            viewportWidth = screenWidth;
            viewportHeight = static_cast<int>(viewportWidth / virtualAspect);
            offsetY = (screenHeight - viewportHeight) / 2;
        }
        
        // Update camera destination rectangle to maintain aspect ratio
        camera.destRec = {
            static_cast<float>(offsetX),
            static_cast<float>(offsetY),
            static_cast<float>(viewportWidth),
            static_cast<float>(viewportHeight)
        };
        
        // Begin rendering to the render texture
        BeginTextureMode(camera.target);
        {
            ClearBackground(RAYWHITE);
            
            // Draw the game world using the world space camera
            BeginMode2D(camera.worldSpaceCamera);
            {
                // Draw all rectangles in a single batch
                auto rectView = registry.view<TransformComponent, RectangleComponent>();
                for (auto rectEntity : rectView) {
                    const auto& transform = rectView.get<TransformComponent>(rectEntity);
                    const auto& rect = rectView.get<RectangleComponent>(rectEntity);
                    
                    render::DrawRectanglePro(
                        rect.rectangle,
                        { 0.0f, 0.0f },  // origin
                        transform.rotation,
                        rect.color
                    );
                }

                // Draw all sprites in a single batch
                auto spriteView = registry.view<TransformComponent, SpriteComponent>();
                for (auto spriteEntity : spriteView) {
                    const auto& transform = spriteView.get<TransformComponent>(spriteEntity);
                    const auto& sprite = spriteView.get<SpriteComponent>(spriteEntity);

                    if (sprite.texture != nullptr) {  // Check if texture pointer is valid
                        // Create source rectangle using the entire texture
                        static const Rectangle srcRec = {
                            0.0f, 0.0f,
                            static_cast<float>(sprite.texture->width),
                            static_cast<float>(sprite.texture->height)
                        };

                        // Create destination rectangle with fixed size
                        const Rectangle destRec = {
                            transform.position.x,  // Position at transform point
                            transform.position.y,  // Position at transform point
                            sprite.size.x,
                            sprite.size.y
                        };

                        render::DrawTexturePro(*sprite.texture,  // Dereference the pointer here
                            srcRec,
                            destRec,
                            Vector2{sprite.size.x / 2, sprite.size.y / 2},  // Set origin to center of sprite
                            transform.rotation,
                            sprite.tint);
                    }
                }

                // Draw all text components in a single batch
                auto textView = registry.view<TransformComponent, TextComponent>();
                for (auto textEntity : textView) {
                    const auto& transform = textView.get<TransformComponent>(textEntity);
                    const auto& text = textView.get<TextComponent>(textEntity);

                    // Draw the text with rotation
                    render::DrawText(str(text.text).c_str(),
                        static_cast<int>(transform.position.x), 
                        static_cast<int>(transform.position.y), 
                        text.fontSize, 
                        text.color);
                }

                // Draw all Pro text components in a single batch
                auto proTextView = registry.view<TransformComponent, TextComponentPro>();
                for (auto textEntity : proTextView) {
                    const auto& transform = proTextView.get<TransformComponent>(textEntity);
                    const auto& text = proTextView.get<TextComponentPro>(textEntity);
                    
                    // Draw the text with rotation
                    render::DrawTextPro(
                        text.font,
                        str(text.text).c_str(),
                        transform.position,
                        {0, 0},
                        transform.rotation,
                        text.fontSize,
                        text.spacing,
                        text.tint
                    );
                }

                // Draw all Pixel Perfect text components in a single batch
                auto pPerfectTextView = registry.view<TransformComponent, TextComponentPixelPerfect>();
                for (auto textEntity : pPerfectTextView) {
                    const auto& transform = pPerfectTextView.get<TransformComponent>(textEntity);
                    const auto& text = pPerfectTextView.get<TextComponentPixelPerfect>(textEntity);

                    // Draw the text with rotation
                    render::DrawTextPixelPerfect(
                        text.font,
                        str(text.text).c_str(),
                        transform.position,
                        text.fontSize,
                        text.spacing,
                        text.tint
                    );
                }
            }
            EndMode2D();
        }
        EndTextureMode();
        
        // Draw the render texture to the screen using the screen space camera
        BeginMode2D(camera.screenSpaceCamera);
        {
            render::DrawTexturePro(camera.target.texture, 
                camera.sourceRec, 
                camera.destRec,
                Vector2{0, 0}, 
                0.0f, 
                WHITE);
        }
        EndMode2D();
        
#ifdef GDEBUG
        // Draw debug info
        render::DrawText(TextFormat("Screen resolution: %ix%i", screenWidth, screenHeight), 10, 10, UI_DEFAULT_FONT_SIZE, DARKBLUE);
        render::DrawText(TextFormat("World resolution: %ix%i", VIRTUAL_WIDTH, VIRTUAL_HEIGHT), 10, 40, UI_DEFAULT_FONT_SIZE, DARKGREEN);
        render::DrawText(TextFormat("Viewport: %ix%i", viewportWidth, viewportHeight), 10, 70, UI_DEFAULT_FONT_SIZE, DARKGREEN);
        render::DrawFPS(screenWidth - 95, 10);
#endif
    }

private:
    static constexpr int DEFAULT_FONT_SIZE = UI_DEFAULT_FONT_SIZE;
};

#endif //RENDERSYSTEM_H
