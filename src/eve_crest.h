//
//  EveCrest.h
//  EveIndustryEssentials
//
//  Created by Philippe Groarke on 2015-09-06.
//  Copyright (c) 2015 Philippe Groarke All rights reserved.
//

#pragma once

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>

struct SolarSystem {
	unsigned int id;
	std::string id_str;
	std::string url;
	std::string name;
	std::string alliance;
	std::string security_class;
	double x, y, z;

	friend std::ostream& operator <<(std::ostream& os, const SolarSystem& ss)
	{
		os << ss.name << std::endl
				<< "  " << ss.id << " (" << ss.id_str << ")" << std::endl
				<< "  " << ss.alliance << std::endl
				<< "  " << ss.security_class << std::endl
				<< "  " << "Position " << std::endl
				<< "    " << ss.x << std::endl
				<< "    " << ss.y << std::endl
				<< "    " << ss.z << std::endl << std::endl;
		return os;
	}
};

class EveCrest {
public:
	EveCrest();
	bool is_ready() const;

	void get_solar_systems(std::shared_ptr<std::vector<SolarSystem>> out_data);
private:
	void init();
	pplx::task<web::json::value> get_values(const web::uri& address);
	void parse_uris(web::json::object data);
	void parse_item_names_and_prices(web::json::array data);
	void get_item_names_and_prices();

	/* Members */
	std::map<std::string, std::string> _uri_map;
	std::atomic<bool> _is_ready{false};

};
