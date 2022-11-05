#pragma once

#include <string>

#include "Cone.h"
#include "Spline.h"
#include "Config.h"

class IConeGenerator
{
	public:
		explicit IConeGenerator(Spline spline) : spline(spline) 
		{ 
			min_cone_distance = std::atof(Config::get("MinConeDistance").c_str());
			max_cone_distance = std::atof(Config::get("MaxConeDistance").c_str());
			track_width = std::atof(Config::get("TrackWidth").c_str()) * 0.5f;
		}

		virtual void generateAllCones() = 0;
		virtual void stepConeGeneration() = 0;

		bool finishedTrackGeneration() { return this->finished_cone_generation; };

		std::vector < Cone > getCones() { return this->track; };

		virtual ~IConeGenerator() = default;

	protected:
		float min_cone_distance;
		float max_cone_distance;
		float track_width;

		Spline spline;
		std::vector < Cone > track;
		bool finished_cone_generation = false;
};