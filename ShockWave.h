#pragma once
#include <SFML\Graphics.hpp>
#include <thread>


using benchmark_data_t = std::pair<float, float>;


struct ShockWaveParameters
{
    float velocity = 80.f;
    float magnitude = 15.f;
    float mean = 19.5f;
    float ring_size = 70.f;
    float variance = 1.1f;

    // range of valid x coordinates
    const float most_left_x_value = -20;
    const float most_right_x_value = 20;
};


class ShockWave
{
    const int IMAGE_MAX_WIDTH_SIZE = 600;

    sf::Image image;
    std::shared_ptr<unsigned int[]> pixels_data;
    ShockWaveParameters shock_wave_parameters;
    sf::Vector2u start_position;
    float max_radius;
    bool is_parallel;

    void calculate_random_start_point() noexcept;
    void recalculate_max_radius() noexcept;

    void precalculate_pixels_data_values() noexcept;
    void clean_pixels_data() noexcept;

    bool check_in_ring(const float x, const float y, const float r_inner, const float r_outer) const noexcept;
    sf::Color new_color_for_pixel(const unsigned int i, const unsigned int j, const float radius, const float ring_size) const noexcept;

    void sequencial_algorithm(sf::Image &new_image, const float radius, const float half_ring_size) const noexcept;
    void parallel_algorithm(sf::Image &new_image, const float radius, const float half_ring_size) const noexcept;

    sf::Image copy_of_main_image() const noexcept;
    long long benchmark_function(const unsigned short int steps_number, const bool parallel_check);

    static void draw(sf::RenderWindow &window, const sf::Image &new_image) noexcept;

public:
    bool load_image(const std::string &file_name) noexcept;
    void initialize_parameters() noexcept;

    bool get_is_parallel() const noexcept;
    ShockWaveParameters &get_shock_wave_parameters_ref() noexcept;

    float radius_by_time(const float current_time) const noexcept;

    bool is_finished(const float current_time) const noexcept;

    void switch_parallelism() noexcept;

    void show(sf::RenderWindow &window, const float current_time) const noexcept;

    benchmark_data_t benchmark(const unsigned short int steps_number, const float image_scale);
};

