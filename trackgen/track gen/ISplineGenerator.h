#pragma once

#include <vector>

#include "Cone.h"
#include "Spline.h"

enum class GeneratorType
{
	PathFidingGenerator,
	ShapeCollapseGenerator
};

class ISplineGenerator
{
	public:

		virtual void generateFullTrack() = 0;
		virtual void stepTrackGeneration() = 0;

		bool finishedTrackGeneration() { return this->finished_track_generation; };

		Spline getTrack() { return this->track; };

		virtual ~ISplineGenerator() = default;

		const GeneratorType type;

	protected:
		ISplineGenerator(GeneratorType type);

		bool finished_track_generation = false;

		int step = 0;

		float max_gradient = 1.f;

		Spline track;

		void optimizeSpline();
};