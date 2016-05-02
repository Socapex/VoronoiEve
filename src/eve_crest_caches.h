#pragma once

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>

namespace EveCrestCache {
	using namespace std::chrono_literals;
	const std::string solar_system_cache_file = ".ss_cache.eec";

	template <typename T>
	void load_data(std::ifstream& ifs,
			std::shared_ptr<std::vector<T>> data) {

		std::size_t vec_size;
		ifs.read(reinterpret_cast<char*>(&vec_size), sizeof(vec_size));

		data->resize(vec_size);
		ifs.read(reinterpret_cast<char*>(&(*data)[0]), vec_size * sizeof(T));

//		for (const auto& x : *data) {
//			std::cout << x;
//		}
		std::cout << "Loaded " << vec_size << " cached SolarSystems." << std::endl;
	}


	template <typename T>
	void write_data(std::ofstream& ofs,
			std::shared_ptr<std::vector<T>> data) {

		const std::size_t vec_size = data->size();
		ofs.write(reinterpret_cast<char const*>(&vec_size),
				sizeof(vec_size));
		ofs.write(reinterpret_cast<char const*>(data->data()),
				vec_size * sizeof(T));
	}

	bool read_solar_system_cache(std::shared_ptr<std::vector<SolarSystem>> data) {
		std::ifstream ifs(solar_system_cache_file, std::ios::binary);
		if (!ifs.is_open()) {
			return false;
		}

		std::chrono::system_clock::rep file_time_rep;
		if (!ifs.read(reinterpret_cast<char*>(&file_time_rep),
				sizeof(file_time_rep))) {
			ifs.close();
			return false;
		}

		std::chrono::system_clock::time_point const cache_valid_time{
				std::chrono::system_clock::duration{file_time_rep}};

		if (std::chrono::system_clock::now() > cache_valid_time) {
			std::cout << "Cache is not valid, regenerating." << std::endl;
			std::time_t t = std::chrono::system_clock::to_time_t(cache_valid_time);
			std::cout << std::ctime(&t) << std::endl;
			ifs.close();
			return false;
		}

		std::cout << "Found valid cache, loading from file." << std::endl;
		load_data<SolarSystem>(ifs, data);
		ifs.close();
		return true;
	}

	void write_solar_system_cache(
			std::shared_ptr<std::vector<SolarSystem>> data)
	{
		std::ofstream ofs(solar_system_cache_file, std::ios::binary);
		if (!ofs.is_open())
			return;

		auto const expiration = (std::chrono::system_clock::now() + 720h);
		auto const cache_time = expiration.time_since_epoch().count();

		ofs.write(reinterpret_cast<char const*>(&cache_time), sizeof(cache_time));
		write_data<SolarSystem>(ofs, data);
		ofs.close();
	}

}
