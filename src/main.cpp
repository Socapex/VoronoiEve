#include <algorithm>
#include <bitset>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "ppm_image.hpp"
#include "eve_crest.h"

const int width = 2000;
//const int height = 2000;

using namespace std::chrono_literals;

int main(int argc, char** argv) {

	auto systems = std::make_shared<std::vector<SolarSystem>>();
	EveCrest test;
	while (!test.is_ready()) {
		std::this_thread::sleep_for(1s);
	}
	test.get_solar_systems(systems);
	while (!test.is_ready()) {
		std::this_thread::sleep_for(1s);
	}

	// Remove worm-holes. TEMPORARY.
	systems->erase(std::remove_if(systems->begin(), systems->end(),
		[](const SolarSystem& s) {
			return s.stargates_size == 0;
		}), systems->end());


	std::cout << std::endl << "Begin image generation." << std::endl;

	auto it = std::max_element(systems->begin(), systems->end(),
			[](const SolarSystem& a, const SolarSystem& b) {
		return (std::abs(a.x) < std::abs(b.x));
	});
	double max_x = std::abs((*it).x);
	double x_ratio = (double)(width / 2 - 1) / max_x;

	it = std::max_element(systems->begin(), systems->end(),
			[](const SolarSystem& a, const SolarSystem& b) {
		return (std::abs(a.z) < std::abs(b.z));
	});
	double max_z = std::abs((*it).z);
	double z_ratio = (double)(height / 2 - 1) / max_z;

	double height_diff = max_z / max_x;
	unsigned int height = width * height_diff;

	std::ofstream image_file("eve_map.ppm",
			std::ios::out | std::ios::binary);

	PPMImage image(width, height);
	image.fill(Pixel(0, 0, 0));

	for (const auto& x : *systems) {
		Pixel p(255, 255, 255);
		unsigned int x_pos = x.x * x_ratio + (double)(width / 2 - 1);
		unsigned int y_pos = (-1 * x.z) * z_ratio + (double)(height / 2 - 1);
		image.draw_pixel(p, x_pos, y_pos);
	}

	image_file << image;
	image_file.close();

	return 0;
}
