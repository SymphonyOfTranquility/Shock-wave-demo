#include "UI_Setup.h"

#include <windows.h>
#include <tchar.h>
#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"


namespace ui_setup
{
    void load_photo_menu(sf::RenderWindow &window, ShockWave &shock_wave, const bool on_start) noexcept
    {
        if (ImGui::Begin("Load photo"))
        {
            if (on_start)
            {
                ImVec2 window_pos(800, 10);
                ImGui::SetWindowPos(window_pos);
            }
            if (ImGui::Button("Load photo"))
            {
                OPENFILENAME open_file_name = { 0 };
                const int file_name_size = 260;
                TCHAR image_file_name[file_name_size] = { 0 };
                open_file_name.lStructSize = sizeof(open_file_name);
                open_file_name.hwndOwner = window.getSystemHandle();
                open_file_name.lpstrFile = image_file_name;
                open_file_name.nMaxFile = file_name_size;
                open_file_name.lpstrFilter = _T("Image\0*.jpg;*.png;*.jpeg;*.bmp;*.jfif");
                open_file_name.nFilterIndex = 1;
                open_file_name.lpstrFileTitle = NULL;
                open_file_name.nMaxFileTitle = 0;
                open_file_name.lpstrInitialDir = NULL;
                open_file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&open_file_name) == TRUE)
                {
                    if (!shock_wave.load_image(std::string(image_file_name)))
                    {
                        std::string error_message = std::string("No start image: ") + image_file_name + "!";
                        MessageBox(nullptr,  error_message.c_str(), "Failed load image", MB_OK | MB_ICONEXCLAMATION);
                    }
                    else
                        shock_wave.initialize_parameters();
                }
            }
        }
        ImGui::End();
    }

    void shock_wave_parameters_menu(ShockWave &shock_wave, float &simulation_speed, const bool on_start) noexcept
    {
        if (ImGui::Begin("Shock wave parameters"))
        {
            if (on_start)
            {
                ImVec2 window_pos(800, 100);
                ImGui::SetWindowPos(window_pos);
            }
            ShockWaveParameters &shock_wave_parameters = shock_wave.get_shock_wave_parameters_ref();
            ImGui::SliderFloat("Simulation speed", &simulation_speed, -1.f, 2.f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Velocity", &shock_wave_parameters.velocity, 1.f, 200.f);
            ImGui::SliderFloat("Magnitude", &shock_wave_parameters.magnitude, -100.f, 100.f);
            ImGui::SliderFloat("Mean", &shock_wave_parameters.mean, shock_wave_parameters.most_left_x_value, shock_wave_parameters.most_right_x_value);
            ImGui::SliderFloat("Variance", &shock_wave_parameters.variance, 0.f, 8.f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Ring size", &shock_wave_parameters.ring_size, 2.f, 200.f);
        }
        ImGui::End();
    }

    void parallel_menu(ShockWave &shock_wave, const bool on_start) noexcept
    {
        if (ImGui::Begin("Parallel"))
        {
            if (on_start)
            {
                ImVec2 window_pos(940, 10);
                ImGui::SetWindowPos(window_pos);
            }
            int is_parallel = shock_wave.get_is_parallel();
            ImGui::RadioButton("Sequencial", &is_parallel, 0);
            ImGui::RadioButton("Parallel", &is_parallel, 1);

            if (bool(is_parallel) != shock_wave.get_is_parallel())
                shock_wave.switch_parallelism();
        }
        ImGui::End();
    }

    void benchmark_menu(ShockWave &shock_wave, benchmark_data_t &benchmark_data, const bool on_start) noexcept
    {
        if (ImGui::Begin("Benchmark"))
        {
            if (on_start)
            {
                ImVec2 window_pos(790, 290);
                ImGui::SetWindowPos(window_pos);
            }
            static int steps_number = 100;
            static float image_scale = 1.f;
            ImGui::SliderInt("Steps number", &steps_number, 100, 10000, "%d", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Scale image", &image_scale, 0.1f, 10.f, "%.3f", ImGuiSliderFlags_Logarithmic);
            if (ImGui::Button("Start"))
                benchmark_data = shock_wave.benchmark(steps_number, image_scale);

            std::string label_sequencial = std::string("Sequencial algorithm time: ") + std::to_string(benchmark_data.first);
            std::string label_parallel = std::string("Parallel algorithm time: ") + std::to_string(benchmark_data.second);
            ImGui::Text(label_sequencial.c_str());
            ImGui::Text(label_parallel.c_str());
        }
        ImGui::End();
    }

    void menus_setup(sf::RenderWindow &window, ShockWave &shock_wave, float &simulation_speed, benchmark_data_t &benchmark_data) noexcept
    {
        static bool on_start = true;
        load_photo_menu(window, shock_wave, on_start);
        shock_wave_parameters_menu(shock_wave, simulation_speed, on_start);
        parallel_menu(shock_wave, on_start);
        benchmark_menu(shock_wave, benchmark_data, on_start);
        on_start = false;
    }
};

