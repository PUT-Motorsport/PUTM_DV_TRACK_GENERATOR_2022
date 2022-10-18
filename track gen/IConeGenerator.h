#pragma once

#include "Cone.h"
#include "Spline.h"

class IConeGenerator
{
	public:
		explicit IConeGenerator(Spline spline);

		virtual void generateAllCones() = 0;
		virtual void stepConeGeneration() = 0;

		bool finishedTrackGeneration() { return this->finished_cone_generation; };

		std::vector < ConePair > getCones() { return this->track; };

		virtual ~IConeGenerator() = default;

	protected:
		Spline spline;
		std::vector < ConePair > track;
		bool finished_cone_generation = false;
};