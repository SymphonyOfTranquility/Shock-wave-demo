#include "WaveMath.h"


namespace wave_math
{
    float get_distance(const sf::Vector2u a, const sf::Vector2u b) noexcept
    {
        const sf::Vector2f difference(sf::Vector2f(a) - sf::Vector2f(b));
        return std::sqrt(difference.x * difference.x + difference.y * difference.y);
    }

    void resize_image(const sf::Image &original_image, sf::Image &resized_image) noexcept
    {
        const sf::Vector2u original_size(original_image.getSize());
        const sf::Vector2u new_size(resized_image.getSize());
        for (unsigned int i = 0; i < new_size.y; ++i)
            for (unsigned int j = 0; j < new_size.x; ++j)
            {
                const unsigned int orig_j = unsigned int(float(j) / new_size.x * original_size.x);
                const unsigned int orig_i = unsigned int(float(i) / new_size.y * original_size.y);
                resized_image.setPixel(j, i, original_image.getPixel(orig_j, orig_i));
            }
    }

    float cumulative_distribution_function(const float x, const float mean, const float varience) noexcept
    {
        return 1.f / 2.f * (1.f + std::erf((x - mean) / (varience * std::sqrt(2.f))));
    }
}