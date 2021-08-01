#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include "ShockWave.h"
#include "UI_Setup.h"

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"


int main() noexcept
{
    ShockWave shock_wave;
    if (!shock_wave.load_image("Test Image Colored.jpg"))
    {
        MessageBox(nullptr, "No start image: \"Test Image Colored.jpg\"!", "Failed load image", MB_OK | MB_ICONEXCLAMATION);
        return 0;
    }
    shock_wave.initialize_parameters();

    const unsigned int window_width = 1066;
    const unsigned int window_height = 600;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Shock wave demo");
    window.setVerticalSyncEnabled(false);

    ImGui::SFML::Init(window);

    sf::Clock delta_clock;
    float current_time = 0.f;
    float speed_simulation = 1.f;
    benchmark_data_t benchmark_data = { NAN, NAN };

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }
        ImGui::SFML::Update(window, delta_clock.getElapsedTime());

        ui_setup::menus_setup(window, shock_wave, speed_simulation, benchmark_data);

        window.clear();

        shock_wave.show(window, current_time);

        current_time = std::max(current_time + delta_clock.getElapsedTime().asSeconds() * speed_simulation, 0.f);
        delta_clock.restart();

        if (shock_wave.is_finished(current_time))
        {
            current_time = 0.f;
            shock_wave.initialize_parameters();
        }

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
