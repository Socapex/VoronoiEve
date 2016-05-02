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
const int height = 2000;

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

			bool ret = false;
			if (strlen(s.security_class) == 0) {
				ret = true;
				std::cout << s.name << std::endl;
			}
			return ret;
		}), systems->end());


	std::cout << std::endl << "Begin image generation." << std::endl;

	auto max_x = std::max_element(systems->begin(), systems->end(),
			[](const SolarSystem& a, const SolarSystem& b) {
		return (std::abs(a.x) < std::abs(b.x));
	});
	double x_ratio = (double)(width / 2 - 1) / (*max_x).x;

	auto max_y = std::max_element(systems->begin(), systems->end(),
			[](const SolarSystem& a, const SolarSystem& b) {
		return (std::abs(a.y) < std::abs(b.y));
	});
	double y_ratio = (double)(height / 2 - 1) / (*max_y).y;

	std::cout << std::fixed << "max x : " << (*max_x).x << std::endl;
	std::cout << std::setprecision(30) << "  ratio : " << x_ratio << std::endl;
	std::cout << std::fixed << "max y : " << (*max_y).y << std::endl;
	std::cout << std::setprecision(30) << "  ratio : " << y_ratio << std::endl;

	std::ofstream image_file("eve_map.ppm",
			std::ios::out | std::ios::binary);

	PPMImage image(width, height);
	image.fill(Pixel(0, 0, 0));

	for (const auto& x : *systems) {
		Pixel p(255, 255, 255);
//		if (x.name[0] == 'J') {
//			p = Pixel(255,0,0);
//		} else {
//			continue;
//			p = Pixel(255, 255, 255);
//		}

		unsigned int x_pos = x.x * x_ratio + (double)(width / 2 - 1);
		unsigned int y_pos = x.y * y_ratio + (double)(height / 2 - 1);
		image.set_pixel(p, x_pos, y_pos);
	}

	image_file << image;
	image_file.close();

	return 0;
}
