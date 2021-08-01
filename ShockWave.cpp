#include "ShockWave.h"

#include <random>
#include <execution>
#include <chrono>

#include "WaveMath.h"


void ShockWave::calculate_random_start_point() noexcept
{
    sf::Vector2u img_size = image.getSize();

    start_position = sf::Vector2u(rand() % img_size.x, rand() % img_size.y);
}

void ShockWave::recalculate_max_radius() noexcept
{
    sf::Vector2u img_size = image.getSize();

    const float distance_center_corner_0 = wave_math::distance(start_position, { 0, 0 });
    const float distance_center_corner_1 = wave_math::distance(start_position, { 0, img_size.y });
    const float distance_center_corner_2 = wave_math::distance(start_position, { img_size.x, 0 });
    const float distance_center_corner_3 = wave_math::distance(start_position, { img_size.x, img_size.y });

    const float max_distance_corners_01 = std::max(distance_center_corner_0, distance_center_corner_1);
    const float max_distance_corners_23 = std::max(distance_center_corner_2, distance_center_corner_3);

    max_radius = std::max(max_distance_corners_01, max_distance_corners_23) + shock_wave_parameters.ring_size / 2;
}


void ShockWave::precalculate_pixels_data_values() noexcept
{
    const int pixel_data_size = image.getSize().x * image.getSize().y;
    pixels_data = std::shared_ptr<unsigned int[]>(new unsigned int[pixel_data_size]);
}

void ShockWave::clean_pixels_data() noexcept
{
    pixels_data.reset();
}

bool ShockWave::check_in_ring(const float x, const float y, const float r_inner, const float r_outer) const noexcept
{
    const float dist = (x - start_position.x) * (x - start_position.x) + (y - start_position.y) * (y - start_position.y);
    return (r_inner * r_inner <= dist && dist <= r_outer * r_outer);
}

sf::Color ShockWave::new_color_for_pixel(const unsigned int i, const unsigned int j, const float radius, const float half_ring_size) const noexcept
{
    const float distance = wave_math::distance(start_position, { j, i });
    const float delta_rad = radius - distance;
    const float normalized = (delta_rad + half_ring_size) / (2.f * half_ring_size);
    const float normalized_on_real = shock_wave_parameters.most_left_x_value + normalized * (shock_wave_parameters.most_right_x_value - shock_wave_parameters.most_left_x_value);
    const float y0 = wave_math::cumulative_distribution_function(shock_wave_parameters.most_left_x_value, shock_wave_parameters.mean, shock_wave_parameters.variance);
    float y = wave_math::cumulative_distribution_function(-normalized_on_real, shock_wave_parameters.mean, shock_wave_parameters.variance);
    y -= y0;

    const float offset = shock_wave_parameters.magnitude * y;

    const float alpha = (distance - offset) / distance;
    const float pos_x = std::clamp(alpha * float(j) + (1.f - alpha) * start_position.x, 0.f, float(image.getSize().x - 1));
    const float pos_y = std::clamp(alpha * float(i) + (1.f - alpha) * start_position.y, 0.f, float(image.getSize().y - 1));

    return image.getPixel(unsigned int(pos_x), unsigned int(pos_y));
}

void ShockWave::sequential_algorithm(sf::Image &new_image, const float radius, const float half_ring_size) const noexcept
{
    for (unsigned int i = 0; i < new_image.getSize().y; ++i)
        for (unsigned int j = 0; j < new_image.getSize().x; ++j)
            if (check_in_ring(float(j), float(i), radius - half_ring_size, radius + half_ring_size))
                new_image.setPixel(j, i, new_color_for_pixel(i, j, radius, half_ring_size));
}

void ShockWave::parallel_algorithm(sf::Image &new_image, const float radius, const float half_ring_size) const noexcept
{
    const int pixels_data_size = image.getSize().x * image.getSize().y;
    for (int i = 0; i < pixels_data_size; ++i)
        pixels_data.get()[i] = i;

    std::for_each(
        std::execution::par_unseq, pixels_data.get(), pixels_data.get() + pixels_data_size,
        [this, radius, half_ring_size, &new_image](unsigned int &item)
        {
            const int width = image.getSize().x;
            const int index = item;
            const int i = index / width, j = index % width;
            sf::Color ans;
            if (check_in_ring(float(j), float(i), radius - half_ring_size, radius + half_ring_size))
                ans = new_color_for_pixel(i, j, radius, half_ring_size);
            else
                ans = new_image.getPixel(j, i);
            const int alpha_offset = 24, blue_offset = 16, green_offset = 8;
            item = (ans.a << alpha_offset) | (ans.b << blue_offset) | (ans.g << green_offset) | ans.r;
        });

    new_image.create(new_image.getSize().x, new_image.getSize().y, (sf::Uint8 *)pixels_data.get());
}

sf::Image ShockWave::copy_of_main_image() const noexcept
{
    sf::Image new_image;

    new_image.create(image.getSize().x, image.getSize().y, sf::Color(0, 0, 0));
    new_image.copy(image, 0, 0, sf::IntRect(0, 0, image.getSize().x, image.getSize().y));

    return new_image;
}

long long ShockWave::benchmark_function(const unsigned short int steps_number, const int algo_type) noexcept
{
    using namespace std::chrono;

    sf::Image new_image = copy_of_main_image();
    const float half_ring_size = shock_wave_parameters.ring_size / 2.f;
    auto start = high_resolution_clock::now();
    for (unsigned short int i = 0; i < steps_number; ++i)
    {
        const float current_radius = float(rand()) / float(RAND_MAX) * max_radius;
        if (algo_type == 1)
            parallel_algorithm(new_image, current_radius, half_ring_size);
        else
            sequential_algorithm(new_image, current_radius, half_ring_size);
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    return duration.count();
}

void ShockWave::draw(sf::RenderWindow &window, const sf::Image &new_image) noexcept
{
    sf::Texture texture;
    texture.loadFromImage(new_image);
    sf::Sprite sprite;
    sprite.setTexture(texture);
    window.draw(sprite);
}


bool ShockWave::load_image(const std::string &file_name) noexcept
{
    sf::Image loaded_image;
    if (!loaded_image.loadFromFile(file_name))
        return false;

    const int width = loaded_image.getSize().x;
    const int height = loaded_image.getSize().y;
    const float scale = float(IMAGE_MAX_HEIGHT_SIZE) / float(height);

    sf::Image new_image;
    new_image.create(unsigned int(width * scale), unsigned int(height * scale), sf::Color(0, 0, 0));

    wave_math::resize_image(loaded_image, new_image);

    image = new_image;

    return true;
}

void ShockWave::initialize_parameters() noexcept
{
    calculate_random_start_point();
    recalculate_max_radius();
    if (is_parallel)
        precalculate_pixels_data_values();
}

bool ShockWave::get_is_parallel() const noexcept
{
    return is_parallel;
}

ShockWaveParameters &ShockWave::get_shock_wave_parameters_ref() noexcept
{
    return shock_wave_parameters;
}

float ShockWave::radius_by_time(const float current_time) const noexcept
{
    const float scale_coefficient = 10.f;
    const float scaled_time = scale_coefficient * current_time;
    return (scaled_time * shock_wave_parameters.velocity);
}

bool ShockWave::is_finished(const float current_time) const noexcept
{
    const float current_radius = radius_by_time(current_time);
    return (current_radius > max_radius);
}

void ShockWave::switch_parallelism() noexcept
{
    is_parallel = !is_parallel;
    if (is_parallel)
        precalculate_pixels_data_values();
    else
        clean_pixels_data();
}

void ShockWave::show(sf::RenderWindow &window, const float current_time) const noexcept
{
    sf::Image new_image = copy_of_main_image();
    const float current_radius = radius_by_time(current_time);
    const float half_ring_size = shock_wave_parameters.ring_size / 2.f;

    if (is_parallel)
        parallel_algorithm(new_image, current_radius, half_ring_size);
    else
        sequential_algorithm(new_image, current_radius, half_ring_size);

    draw(window, new_image);
}


benchmark_data_t ShockWave::benchmark(const unsigned short int steps_number, const float image_scale) noexcept
{
    sf::Image original_image = copy_of_main_image();
    sf::Image new_image;
    new_image.create(unsigned int(image.getSize().x*image_scale), unsigned int(image.getSize().y*image_scale), sf::Color(0, 0, 0));
    
    wave_math::resize_image(image, new_image);
    image = new_image;

    const float miliseconds_to_seconds = 1000.f;
    const float sequential_algo_work_time = float(benchmark_function(steps_number, false)) / miliseconds_to_seconds;

    if (is_parallel)
        clean_pixels_data();

    precalculate_pixels_data_values();
    const float parallel_algo_work_time = float(benchmark_function(steps_number, true)) / miliseconds_to_seconds;
    clean_pixels_data();

    image = original_image;
    if (is_parallel)
        precalculate_pixels_data_values();
    return { sequential_algo_work_time, parallel_algo_work_time };
}