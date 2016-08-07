#include <algorithm>
#include <bitset>
#include <chrono>
#include <cmath>
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

struct Point {
	unsigned int x;
	unsigned int y;

	friend bool operator==(const Point& l, const Point& r)
	{
		return l.x == r.x && l.y == r.y;
	}
};

struct VoronoiCell {
	Point origin;
	std::vector<Point> other_stars;
	Pixel color;
	std::vector<Point> points;
	std::string alliance;
	double security;

	friend bool operator<(const VoronoiCell& l, const VoronoiCell& r)
	{
		return l.alliance < r.alliance;
	}

	friend bool operator==(const VoronoiCell& l, const VoronoiCell& r)
	{
		bool ret = true;
		ret &= l.origin == r.origin;
		ret &= l.alliance == r.alliance;
		return ret;
	}
};

struct Palette {
	Palette()
	{
		const int ratio = 20;
		unsigned char r = 255;
		unsigned char g = 0;
		unsigned char b = 0;

		// Reds
		while (true) {
			colors.emplace_back(Pixel(r, g, b));

			if (g > 200)
				break;

			g += ratio;
			b += ratio;
		}

		// Greens
		r = 0;
		g = 255;
		b = 0;
		while (true) {
			colors.emplace_back(Pixel(r, g, b));

			if (r > 200)
				break;

			r += ratio;
			b += ratio;
		}

		// Blues
		r = 0;
		g = 0;
		b = 255;
		while (true) {
			colors.emplace_back(Pixel(r, g, b));

			if (r > 200)
				break;

			r += ratio;
			g += ratio;
		}

		// Yellow
		r = 255;
		g = 255;
		b = 0;
		while (true) {
			colors.emplace_back(Pixel(r, g, b));

			if (b > 200)
				break;

			b += ratio;
		}

		// Purple
		r = 255;
		g = 0;
		b = 255;
		while (true) {
			colors.emplace_back(Pixel(r, g, b));

			if (g > 200)
				break;

			g += ratio;
		}

		// Auqa
		r = 0;
		g = 255;
		b = 255;
		while (true) {
			colors.emplace_back(Pixel(r, g, b));

			if (r > 200)
				break;

			r += ratio;
		}
	}

	std::vector<Pixel> colors;
};

double euclid_dist(double x1, double x2, double y1, double y2)
{
	return pow(x2 - x1, 2) + pow(y2 - y1, 2);
}

double chebyshev_dist(double x1, double x2, double y1, double y2)
{
	return std::max(std::abs(x1 - x2), std::abs(y1 - y2));
}

double manhattan_dist(double x1, double x2, double y1, double y2)
{
	return std::abs(x1 - x2) + std::abs(y1 - y2);
}

Pixel get_next_color()
{
	static Palette p;
	static int i = 0;

	if (i >= p.colors.size()) {
		i = 0;
	}
	++i;
	return p.colors[i - 1];
}


int main(int argc, char** argv) {
	std::cout << "VoronoiEve" << std::endl
			<< "  An Eve Online basic map generator." << std::endl
			<< "  Philippe Groarke <philippe.groarke@gmail.com>" << std::endl;

	auto systems = std::make_shared<std::vector<SolarSystem>>();
	EveCrest eve_crest;
	while (!eve_crest.is_ready()) {
		std::this_thread::sleep_for(1s);
	}
	eve_crest.get_solar_systems(systems);
	while (!eve_crest.is_ready()) {
		std::this_thread::sleep_for(1s);
	}
	// Remove worm-holes. TEMPORARY?
	systems->erase(std::remove_if(systems->begin(), systems->end(),
		[](const SolarSystem& s) {
			return s.id > 31000000;
		}), systems->end());


	std::cout << std::endl << "Begin image generation." << std::endl;

	/* Find max x. */
	auto it = std::max_element(systems->begin(), systems->end(),
			[](const SolarSystem& a, const SolarSystem& b) {
		return a.x < b.x;
	});
	double max_x = (*it).x;

	/* Find min x. */
	it = std::min_element(systems->begin(), systems->end(),
			[](const SolarSystem& a, const SolarSystem& b) {
		return a.x < b.x;
	});
	double min_x = (*it).x;

	/* Find max z. */
	it = std::max_element(systems->begin(), systems->end(),
			[](const SolarSystem& a, const SolarSystem& b) {
		return a.z < b.z;
	});
	double min_z = -1 * (*it).z;

	/* Find min z. */
	it = std::min_element(systems->begin(), systems->end(),
			[](const SolarSystem& a, const SolarSystem& b) {
		return a.z < b.z;
	});
	double max_z = -1 * (*it).z;

	double height_diff = max_z / max_x;
	unsigned int height = width * height_diff;


	std::ofstream image_file("eve_map.ppm",
			std::ios::out | std::ios::binary);

	PPMImage image(width, height);
	image.fill(Pixel(0, 0, 0));

	std::vector<VoronoiCell> voronoi_cells;

	/* Initialize all origins. */
	for (const auto& x : *systems) {
		Point point;
		point.x = ((x.x - min_x) * (double)width) / (max_x - min_x);
		point.y = (((-1 * x.z) - min_z) * (double)(height - 1))
				/ (max_z - min_z);

		VoronoiCell cell;
		cell.origin = point;
		cell.alliance = x.alliance;
		cell.security = x.security_status;
		voronoi_cells.emplace_back(cell);
	}

	/* Draw all stars, since some wont be rendered later. */
	for (const auto& x : voronoi_cells) {
		image.draw_pixel(Pixel(255, 255, 255), x.origin.x, x.origin.y);
	}

	/* Generate all x,y pairs that we will test. */
	std::vector<std::pair<unsigned int, unsigned int>> x_y_pairs;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			auto p = std::pair<unsigned int, unsigned int>(x, y);
			x_y_pairs.emplace_back(p);
		}
	}
	std::sort(x_y_pairs.begin(), x_y_pairs.end());

	/* Also copy the testing cells, since they will be removed. */
	std::vector<VoronoiCell> untested_cells(voronoi_cells);
	std::sort(untested_cells.begin(), untested_cells.end());

	/* OUCH. Better way to do this? */
	for (auto& cell_k : voronoi_cells) {
		cell_k.color = get_next_color();
		std::vector<std::pair<unsigned int, unsigned int>> used_x_y;

		for (const auto& x_y : x_y_pairs) {
			if (x_y.first == cell_k.origin.x && x_y.second == cell_k.origin.y)
				continue;

			bool shortest = true;
			double dist_k = euclid_dist(cell_k.origin.x, x_y.first,
					cell_k.origin.y, x_y.second);

			double dist_k_cheby = chebyshev_dist(cell_k.origin.x, x_y.first,
					cell_k.origin.y, x_y.second);

			double dist_k_manhattan = manhattan_dist(cell_k.origin.x, x_y.first,
					cell_k.origin.y, x_y.second);

			for (const auto& cell_j : untested_cells) {
				if (x_y.first == cell_j.origin.x && x_y.second == cell_j.origin.y) {
					continue;
				}
				double dist_j = euclid_dist(cell_j.origin.x, x_y.first,
						cell_j.origin.y, x_y.second);

				// Voronoi.
				if (dist_k > dist_j) {
					shortest = false;
					break;
				}

				// Limit size to x% of map width.
//				if (dist_k > pow(width * 0.05, 2)) {
//					shortest = false;
//					break;
//				}

				// Chebyshev needs to be lower to merge.
				if (dist_k_cheby > width * 0.1) {
					shortest = false;
					break;
				}

				if (dist_k_manhattan > width * 0.14) {
					shortest = false;
					break;
				}
			}
			if (!shortest)
				continue;

			Point p;
			p.x = x_y.first;
			p.y = x_y.second;
			cell_k.points.emplace_back(p);
			used_x_y.push_back(x_y);
		}

		auto ib = used_x_y.begin();
		x_y_pairs.erase(std::remove_if(x_y_pairs.begin(), x_y_pairs.end(),
				[&ib, &used_x_y](const auto& x) {
			while (ib != used_x_y.end() && *ib < x)
				++ib;
			return (ib != used_x_y.end() && *ib == x);
		}), x_y_pairs.end());

		untested_cells.erase(std::remove(untested_cells.begin(),
				untested_cells.end(), cell_k), untested_cells.end());
		std::cout << "1 cell done, pixels size : " << x_y_pairs.size()
				<< ". Cells left : " << untested_cells.size() << std::endl;
	}

	std::sort(voronoi_cells.begin(), voronoi_cells.end());

	voronoi_cells.erase(std::remove_if(voronoi_cells.begin(), voronoi_cells.end(),
			[](const auto& x) {
		return x.security > 0 || x.alliance.empty();
	}), voronoi_cells.end());

	for (int i = 0; i < voronoi_cells.size(); ++i) {
		if (i + 1 >= voronoi_cells.size())
			break;

		VoronoiCell& cell = voronoi_cells[i];
		while (voronoi_cells[i + 1].alliance == cell.alliance) {
			VoronoiCell& cell2 = voronoi_cells[i + 1];
			cell.other_stars.push_back(cell2.origin);
			cell.points.insert(cell.points.end(), cell2.points.begin(),
					cell2.points.end());

			// Bad, bad me :(
			voronoi_cells.erase(std::remove(voronoi_cells.begin(),
					voronoi_cells.end(), cell2), voronoi_cells.end());

			if (i + 1 >= voronoi_cells.size())
				break;
		}
	}

	// TEMPORARY. Contested systems are deep red.
//	auto v_it = std::find_if(voronoi_cells.begin(), voronoi_cells.end(),
//			[](const auto& x) {
//		return x.alliance.empty();
//	});
//	(*v_it).color = Pixel(146, 20, 12);

	/* Draw the cells. */
	for (const auto& x : voronoi_cells) {
		image.draw_pixel(Pixel(255, 255, 255), x.origin.x, x.origin.y);
		std::cout << x.alliance << std::endl;

		// Stars
		for (const auto& y : x.other_stars) {
			image.draw_pixel(Pixel(255, 255, 255), y.x, y.y);
		}

		// Cells
		for (const auto& y : x.points) {
			image.draw_pixel(x.color, y.x, y.y);
		}
	}

	image_file << image;
	image_file.close();

	return 0;
}
