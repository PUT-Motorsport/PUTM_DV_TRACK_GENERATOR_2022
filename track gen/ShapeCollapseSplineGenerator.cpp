#include "ShapeCollapseSplineGenerator.h"
#include "Config.h"
#include "Functions.inl"
#include "DataModels.inl"

#include <math.h>
#include <string>

using sc = ShapeCollapseSplineGenerator;

sc::ShapeCollapseSplineGenerator(std::function<sf::Vector2f(sf::Vector2f)> shape_function) : shape_function(shape_function) 
{
	pivot_point_count_max = atoi(Config::get("SplinePivotPointCountMax").c_str());
	pivot_point_count_min = atoi(Config::get("SplinePivotPointCountMin").c_str());

	pivot_point_collapse_chanse = atof(Config::get("PivotPointCollapseChance").c_str());
	min_angle = toRad(atof(Config::get("TrackMinAngle").c_str())); 
	max_rms_flex = atof(Config::get("TrackMinAvgFlex").c_str());
}

void sc::generateFullTrack()
{
	generateShape();
	collapseShape();
	firstShapeFilter();
	//firstShapeFilter();
}

void sc::stepTrackGeneration()
{

}

void sc::generateShape()
{
	std::vector < sf::Vector2f > points;

	auto step_count = RandomGenerator::random<int>(pivot_point_count_min, pivot_point_count_max);

	angle_step = 360.f / step_count;

	for (float angle = 0.f; angle < 360.f; angle += angle_step)
	{
		auto rad = toRad(angle);
		points.push_back(shape_function({ sinf(rad), cosf(rad) }));
	}

	track = Spline(points, true);
	track.optimize();
}

void sc::collapseShape()
{
	sf::Vector2f shape_center;
	auto collapse_chance = pivot_point_collapse_chanse;

	for (auto const pivot : track)
	{
		shape_center += pivot;
	}

	shape_center /= float(track.getPivotPointsCount());

	for (auto& pivot : track)
	{
		//determine
		float chance = RandomGenerator::random<float>(0.f, 1.f);

		//collapse
		if (chance < collapse_chance)
		{
			float len = distance(shape_center, pivot);
			float rand_len = RandomGenerator::random<float>(-len * 2.f / 3.f, len * 2.f / 3.f);
			pivot += direction(shape_center - pivot) * rand_len;
		}
		else
		{
			collapse_chance = pivot_point_collapse_chanse;
		}
	}
}

void sc::firstShapeFilter()
{
	for (size_t k = 0; k < track.getPivotPointsCount(); k++)
	{
		size_t i[3] = { k, k + 1, k + 2 };
		if (i[1] == track.getPivotPointsCount())
		{
			i[1] = 0;
			i[2] = 1;
		}
		else if (i[2] == track.getPivotPointsCount())
		{
			i[2] = 0;
		}

		sf::Vector2f points[3] = { track.getPivotPoint(i[0]), track.getPivotPoint(i[1]), track.getPivotPoint(i[2]) };
		sf::Vector2f veca = points[1] - points[0];
		sf::Vector2f vecb = points[1] - points[2];
		float ang = abs(angle2(veca, vecb));

		if (ang < min_angle)
		{
			track.removePivotPoint(i[1]);
			k--;
		}
	}

	for (size_t k = 0; k < track.getPivotPointsCount(); k++)
	{
		float avg_inflexion = 0.f;
		for (float t = 0.f; t < 1.f; t += 0.05)
		{
			//avg_inflexion += sq(track.getInflexion(k, t));
			avg_inflexion += track.getInflexion(k, t);
		}
		avg_inflexion / (1.f / 0.05f);
		float rms_inflexion = avg_inflexion;//sqrtf(avg_inflexion);
		if (rms_inflexion < max_rms_flex)
		{
			track.removePivotPoint(k);
		}
	}

}

void sc::smoothShape()
{

}