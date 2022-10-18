#pragma once

#include "ISplineGenerator.h"
#include "RandomGenerator.h"

#include <SFML/System.hpp>
#include <functional>

class ShapeCollapseSplineGenerator : public ISplineGenerator
{
	public:
		ShapeCollapseSplineGenerator(std::function<sf::Vector2f(sf::Vector2f)> shape_function); 

		float angle_step = 18.f;

		void generateFullTrack() override;
		void stepTrackGeneration() override; 

	private:
		std::function<sf::Vector2f(sf::Vector2f)> shape_function = nullptr;

		uint32_t pivot_point_count_max;
		uint32_t pivot_point_count_min;

		float pivot_point_collapse_chanse;

		void generateShape();
		void collapseShape();
		void firstShapeFilter();
		void smoothShape();
};