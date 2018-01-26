#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <exception>
#include <json.hpp>

inline sf::Vector2f getDrawableArea(const nlohmann::json & configJSON) {
    sf::Vector2f drawableAreaSize = {};
    auto screenSize = sf::VideoMode::getDesktopMode();
    const float aspectRatio =
        (float)screenSize.width / (float)screenSize.height;
    try {
        for (auto it = configJSON["AspectRatios"].begin();
             it != configJSON["AspectRatios"].end(); ++it) {
            static const char delimiter = ':';
            const size_t splitPos = it.key().find(delimiter);
            const float wFactor =
                std::atoi(it.key().substr(0, splitPos).c_str());
            const float hFactor = std::atoi(
                it.key().substr(splitPos + 1, it.key().length()).c_str());
            const int width = it.value().find("Width")->get<int>();
            const int height = it.value().find("Height")->get<int>();
            if (wFactor / hFactor == aspectRatio) {
                drawableAreaSize.x = static_cast<float>(width);
                drawableAreaSize.y = static_cast<float>(height);
            } else if (hFactor / wFactor == aspectRatio) {
                drawableAreaSize.y = static_cast<float>(height);
                drawableAreaSize.x = static_cast<float>(width);
            }
        }
    } catch (const std::exception & ex) {
        throw std::runtime_error("JSON error: " + std::string(ex.what()));
    }
    if (drawableAreaSize == sf::Vector2f{}) {
        throw std::runtime_error("BlindJump cannot find configuration settings"
                                 " for your display. Please add an entry for "
                                 " your aspect ratio to config.json in the game\'s"
                                 " resources folder.");
    }
    return drawableAreaSize;
}
