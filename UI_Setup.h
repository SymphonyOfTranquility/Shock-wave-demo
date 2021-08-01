#pragma once
#include <SFML\Graphics.hpp>

#include "ShockWave.h"


namespace ui_setup
{
    void menus_setup(sf::RenderWindow &window, ShockWave &shock_wave, float &simulation_speed, benchmark_data_t &benchmark_data) noexcept;
};

