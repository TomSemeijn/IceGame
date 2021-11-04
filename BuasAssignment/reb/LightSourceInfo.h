#ifndef REBOOTLIGHTSOURCEINFO_H
#define REBOOTLIGHTSOURCEINFO_H

#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Vector2.h"
#include "Color.h"

namespace reb {

	//keeps general info of multiple lightsources of one type
	struct LightSourceInfo {
		std::vector<sf::Glsl::Vec3> positions;
		std::vector<sf::Glsl::Vec3> colors;
		std::vector<float> intensities;
	};

}

#endif