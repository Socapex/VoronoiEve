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

struct Stargate {
	unsigned int id = 0;
	char href[256] = "";
};

struct SolarSystem {
	unsigned int id = 0;
	char href[256] = "";
	char name[256] = "";
	char alliance[256] = "";
	char security_class[256] = "";
	double security_status = 0;
	double x = 0;
	double y = 0;
	double z = 0;

	Stargate stargates[12];
	int stargates_size = 0;

	friend std::ostream& operator <<(std::ostream& os, const SolarSystem& ss)
	{
		os << ss.name << std::endl
				<< "  " << ss.id << std::endl
				<< "  " << ss.alliance << std::endl
				<< "  " << ss.security_class << std::endl
				<< "  " << "Position " << std::endl
				<< "    " << ss.x << std::endl
				<< "    " << ss.y << std::endl
				<< "    " << ss.z << std::endl
				<< "  " << "Stargates " << std::endl;
		for (int i = 0; i < ss.stargates_size; ++i) {
			os << "    " << ss.stargates[i].href << std::endl;
		}
		os << std::endl;

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
	web::http::client::http_client client;

};
