#include <bitset>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "ppm_image.hpp"
#include "eve_crest.h"

const int width = 42;
const int height = 42;

using namespace std::chrono_literals;

int main(int argc, char** argv) {

	auto systems = std::make_shared<std::vector<SolarSystem>>();
	EveCrest test;
	while (!test.is_ready()) {
		std::this_thread::sleep_for(1s);
	}
	test.get_solar_systems(systems);

	std::ofstream image_file("eve_map.ppm",
			std::ios::out | std::ios::binary);

	PPMImage image(42, 42);
	image.fill(Pixel(255, 0, 0));

	image_file << image;
	image_file.close();

	while (true) {
		std::this_thread::sleep_for(1s);
	}

	return 0;
}
