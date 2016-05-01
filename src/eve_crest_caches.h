#pragma once

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>

namespace EveCrestCache {
	using namespace std::chrono_literals;

	const std::string solar_system_cache_file = ".ss_cache.eec";

	bool solar_system_cache_is_good() {
		std::ifstream ifs(solar_system_cache_file,
				std::ios::in | std::ios::binary);

		if (!ifs.is_open()) {
			return false;
		}

		std::time_t file_time;
		ifs >> file_time;
		std::cout << std::ctime(&file_time);

		auto cache_valid_time = std::chrono::system_clock::from_time_t(file_time);
		if (std::chrono::system_clock::now() < cache_valid_time) {
			std::cout << "Cache is VALID." << std::endl;
		}
		ifs.close();
		return true;
	}

	void write_solar_system_cache(
			std::shared_ptr<std::vector<SolarSystem>> data)
	{
		std::ofstream ofs(solar_system_cache_file,
				std::ios::out | std::ios::binary);

		if (!ofs.is_open())
			return;

		auto cache_time = std::chrono::system_clock::now() + 12h;

		ofs << std::chrono::system_clock::to_time_t(cache_time);

		ofs.close();
	}
}
