#pragma once

#include "ISplineGenerator.h"

class PathFindingSplineGenerator : public ISplineGenerator
{
	public:		
		void generateFullTrack() override;
		void stepTrackGeneration() override;
};