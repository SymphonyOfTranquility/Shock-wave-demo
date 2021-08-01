#pragma once
#include <SFML\Graphics.hpp>


namespace wave_math
{
    float get_distance(const sf::Vector2u a, const sf::Vector2u b) noexcept;
    void resize_image(const sf::Image &original_image, sf::Image &resized_image) noexcept;
    float cumulative_distribution_function(const float x, const float mean, const float varience) noexcept;
};

