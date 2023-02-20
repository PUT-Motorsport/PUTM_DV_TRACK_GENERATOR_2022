#pragma once

#include "IConeGenerator.h"

class UniformConeGenerator : public IConeGenerator
{	
	public:
		UniformConeGenerator(Spline spline);
		
		void generateAllCones() override;
		void stepConeGeneration() override { };
	private:
		void(UniformConeGenerator::* genration_algorithm)();

		void generate() { (this->*genration_algorithm)(); }
		
		void trianglesAlgorithm();
		void fillAndOffsetAlgorithm();
		void faoSubAlgorithm1(Type typ, float sign, std::vector < Cone >& container, std::vector < sf::Vector2f >& const buff_pivot);
};