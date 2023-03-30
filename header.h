#pragma once
#include <SFML/Graphics.hpp>
#include <immintrin.h>

union __8f_t {
	__m256 data;
	float fdata[8];
};



struct FPS {
	sf::Clock clock = {};

	int frame = 0;

	char* fps = (char* ) calloc(FPS_BUFFER_SIZE, sizeof(char));
};

int buildMandelbrot(sf::Image* image, int* k1, int* k2);

void updateFPS(FPS* fps);

void getFPS(FPS* fps);

void dctorFPS(FPS* fps);