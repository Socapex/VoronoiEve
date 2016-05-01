#pragma once

#include <iostream>
#include <string>
#include <vector>

struct Pixel {
	Pixel(int red, int green, int blue)
	{
		rgba = 0xff << 24 | blue << 16 | green << 8 | red;
	}

	friend std::ostream& operator <<(std::ostream& os,
			const Pixel& pixel)
	{
		const unsigned char red = pixel.rgba;
		const unsigned char green = (pixel.rgba >> 8) & 0xff;
		const unsigned char blue = (pixel.rgba >> 16) & 0xff;
		os << red << green << blue;
		return os;
	}

	unsigned int rgba = 0;
};

struct PPMImage {
	PPMImage(int w, int h)
	{
		width = w;
		height = h;

		header.resize(512);
		header = "P6\n";
		header += std::to_string(width) + " ";
		header += std::to_string(height) + "\n";
		header += std::to_string(max_color_value) + "\n";
	}

	void fill(Pixel pix)
	{
		for (int y = 0; y < height; ++y) {
			std::vector<Pixel> line;
			line.reserve(width);

			for (int x = 0; x < width; ++x) {
				line.emplace_back(Pixel(pix));
			}
			pixmap.emplace_back(line);
		}
	}

	friend std::ostream& operator <<(std::ostream& os, const PPMImage& image)
	{
		os << image.header;
		for (const auto& y : image.pixmap) {
			for (const auto& x : y) {
				os << x;
			}
		}
		return os;
	}

	const int max_color_value = 255;
	int width, height;

	std::string header;
	std::vector<std::vector<Pixel>> pixmap;
};
