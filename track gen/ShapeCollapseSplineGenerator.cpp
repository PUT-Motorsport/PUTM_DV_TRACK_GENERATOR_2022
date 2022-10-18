#include "ShapeCollapseSplineGenerator.h"
#include "Functions.inl"
#include "Config.h"

#include <math.h>
#include <string>

using sc = ShapeCollapseSplineGenerator;

sc::ShapeCollapseSplineGenerator(std::function<sf::Vector2f(sf::Vector2f)> shape_function) : shape_function(shape_function) 
{
	pivot_point_count_max = atoi(Config::get("SplinePivotPointCountMax").c_str());
	pivot_point_count_min = atoi(Config::get("SplinePivotPointCountMin").c_str());
	pivot_point_collapse_chanse = atof(Config::get("PivotPointCollapseChance").c_str());
}

void sc::generateFullTrack()
{
	generateShape();
	collapseShape();
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
		bool collapse = chance < collapse_chance;

		//collapse
		if (collapse)
		{
			float len = distance(shape_center, pivot);
			float rand_len = RandomGenerator::random<float>(len / 3.f, len * 2.f / 3.f);
			pivot += direction(shape_center - pivot) * rand_len;

			collapse_chance *= 0.5f;
		}
		else
		{
			collapse_chance = pivot_point_collapse_chanse;
		}
	}
}

void sc::firstShapeFilter()
{
	//float max_distance = 0.f;
	////float coef_1 = 1.f;
	////float coef_2 = 1.f;

	//sf::Vector2f shape_center;

	////pivot, lenght, gradient, cost
	//std::vector < Quad < sf::Vector2f&, float, float, float > > costs;
	//costs.resize(track.getPivotPointsCount());

	//for (auto const pivot : track)
	//{
	//	shape_center += pivot;
	//}
	//shape_center /= float(track.getPivotPointsCount());
	//for (int i = 0; i < track.getPivotPointsCount(); i++)
	//{
	//	//max_distance += distance(pivot, shape_center);
	//	auto dist = distance(track[i], shape_center);
	//	if (dist > max_distance) max_distance = dist;

	//	//costs[i] = Quad<sf::Vector2f&, float, float, float>(track[i], dist, track.getGradient(i), 0.f );
	//	costs[i] = { (sf::Vector2f&)(track[i]), dist, track.getGradient(i), 0.f };
	//	//{
	//	//	.item1 = (sf::Vector2f&)(track[i]),
	//	//	.item2 = dist,
	//	//	.item3 = track.getGradient(i),
	//	//	.item4 = 0.f
	//	//};
	//}
	//for (int i = 0; i < costs.size(); i++)
	//{
	//	int id = i - 1;
	//	int ii = i + 1;

	//	if (id == -1) id = id == costs.size() - 1;
	//	if (ii == costs.size()) ii = 0;
	//	auto& costf = costs[ii].item4;
	//	auto& costb = costs[id].item4;
	//	auto& [_, len, grad, cost] = costs[i];
	//	cost = len / max_distance * ((costf + cost + costb) / 3.f) ;
	//}


}