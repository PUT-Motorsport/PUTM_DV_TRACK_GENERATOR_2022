#include "Spline.h"
#include "Functions.inl"

#define _USE_MATH_DEFINES

#include <math.h>
#include <stack>
#include <queue>
#include <array>
#include <list>
#include <string>

static const std::vector < sf::Vector2f > default_points = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

Spline::Spline() : resolution(1)//, pivot_points(default_points) 
{
	init();
}

Spline::Spline(int resolution) : resolution(resolution)//, pivot_points(default_points) 
{
	init();
} 

Spline::Spline(std::vector < sf::Vector2f > pivot_points) : resolution(1), pivot_points(pivot_points) 
{
	init();
}

Spline::Spline(std::vector < sf::Vector2f > pivot_points, bool looped) : resolution(1), looped(looped), pivot_points(pivot_points)
{
	init();
}

Spline::Spline(std::vector < sf::Vector2f > pivot_points, int resolution) : resolution(resolution), pivot_points(pivot_points) 
{
	init();
}

void Spline::setAproximationAlgorithm(AproximationAlgorithm algorithm)
{
	switch (algorithm)
	{
		case AproximationAlgorithm::LineSplit: aproximaion_algorithm = &Spline::lineSplitLenght; break;
		case AproximationAlgorithm::QubedVectorLenght: aproximaion_algorithm = &Spline::vectorLenghtQubedLenght; break;
		default: return;
	}
}

void Spline::setSegmentingAlgorithm(SegmentingAlgorithm algorithm)
{
	switch (algorithm)
	{
		case SegmentingAlgorithm::LookAhead: segmenting_algorithm = &Spline::lookAheadSegmenting; break;
		case SegmentingAlgorithm::Tree: segmenting_algorithm = &Spline::treeSegmenting; break;
		case SegmentingAlgorithm::Constant: segmenting_algorithm = &Spline::constantSegementing; break;
		default: return;
	}
}

void Spline::init()
{
	setAproximationAlgorithm(toEnum<AproximationAlgorithm>(Config::get("SplineLenghtApproximationAlgorithm")));
	setSegmentingAlgorithm(toEnum<SegmentingAlgorithm>(Config::get("SplineRenderSegmentationAlgorithm")));

	max_gradient = toRad(float(std::atof(Config::get("SplineMaxGradient").c_str())));
	optimization_error = float(std::atof(Config::get("SplineOptimizationError").c_str()));

	spline.setPrimitiveType(sf::TriangleStrip);

	//points_copy = points;
}

void Spline::setWidth(float width)
{
	this->width = width;
}

void Spline::constantSegementing(std::array < sf::Vector2f, 4 > segment) const
{
	for (float t = 0.f; t < 1.f; t += 0.001f)
	{
		auto g = getSplineGradient(segment, t);
		spline_point_representation.push_back({ getSplinePoint(segment, t), { g.x, g.y }, atan2f(-g.y, g.x) });
	}

	auto p = pivot_points.end() - 2;
	auto g = getSplineGradient({ *(p - 2), *(p - 1), *(p - 0), *(p + 1) }, 1.f);
	spline_point_representation.push_back({ pivot_points[pivot_points.size() - 2], { g.x, g.y }, atan2f(-g.y, g.x) });
}

void Spline::treeSegmenting(std::array < sf::Vector2f, 4 > segment) const
{

}

void Spline::movePivotPoint(size_t index, sf::Vector2f move)
{
	pivot_points[index] += move;
	changed = true;
}

void Spline::setPivotPointPosition(size_t index, sf::Vector2f position)
{
	pivot_points[index] = position;
	changed = true;
}

void Spline::removePivotPoint(size_t index)
{
	pivot_points.erase(pivot_points.begin() + index);
	changed = true;
	highlight_pivot_point = -1;
}

void Spline::removePivotPoint(size_t first, size_t last)
{
	pivot_points.erase(pivot_points.begin() + first, pivot_points.begin() + last);
	changed = true;
	highlight_pivot_point = -1;
}

void Spline::lookAheadSegmenting(std::array < sf::Vector2f, 4 > segment) const
{
	float t = 0.f;
	
	if (spline_point_representation.size() == 0)
	{
		auto buff = getSplineGradient(segment, t);
		spline_point_representation.push_back({ getSplinePoint(segment, 0.f), {buff.x, buff.y}, atan2f(-buff.y, buff.x)});
	}

	while (t < 1.0f)
	{
		auto step = 0.f;
		auto step_step = 0.05f;

		auto grad_s_vec = getSplineGradient(segment, t + step);
		auto grad_t_vec = spline_point_representation.back().gradient_vector;

		float grad = angle(grad_t_vec, grad_s_vec);
		while (grad < max_gradient)
		{
			step += step_step;
			grad = angle(getSplineGradient(segment, t + step), spline_point_representation.back().gradient_vector);
			step_step *= 2.f;
		}
		step_step *= 0.5;
		while (step_step > 0.001f)
		{
			if (grad > max_gradient) step -= step_step;
			else step += step_step;
			grad = angle(getSplineGradient(segment, t + step), spline_point_representation.back().gradient_vector);
			step_step *= 0.5;
		}
		if (t + step < 1.f) t += step;
		else t = 1.f;

		auto g = getSplineGradient(segment, t);
		spline_point_representation.push_back({ getSplinePoint(segment, t), { g.x, g.y }, atan2f(-g.y, g.x) });
	}
}

int Spline::mouseEnteredPivotPoint(sf::Vector2f mouse_pos)
{	
	for (size_t i = 0; i < pivot_points.size(); i++)
	{
		auto len = distance(pivot_points[i], mouse_pos);
		if (len <= 5.f)
		{
			return i;
		}
	}
	return -1;
}

int Spline::mouseEnteredPivotPoint(sf::Vector2f mouse_pos, bool highlight)
{
	for (size_t i = 0; i < pivot_points.size(); i++)
	{
		auto len = distance(pivot_points[i], mouse_pos);
		if (len <= 5.f)
		{
			highlight_pivot_point = i;
			return highlight_pivot_point;
		}
	}
	highlight_pivot_point = -1;
	return highlight_pivot_point;
}

//basicOptimize
void Spline::optimize()
{
	const float error = 0.001f;

	size_t j = 0;
	//erase overlapping
	while (j < pivot_points.size())
	{
		bool match = false;

		size_t i = 1;
		if (j + i < pivot_points.size() && overlap(pivot_points[j], pivot_points[j + i], 0.001)) match = true; //pivot_points[j] == pivot_points[j + i]
		while (j + i < pivot_points.size() && overlap(pivot_points[j], pivot_points[j + i], 0.001)) i++; //pivot_points[j] == pivot_points[j + i]
		if (match) pivot_points.erase(pivot_points.begin() + j, pivot_points.begin() + j + i - 1);
		j++;
	}
	if (overlap(pivot_points.front(), pivot_points.back(), 0.001)) pivot_points.erase(pivot_points.end() - 1);

	//erase colinear: leave it for now
	//j = 0;
	//while  (j < pivot_points.size())
	//{
	//	bool match = false;

	//	size_t i = 2;
	//	if (j + i < pivot_points.size() && colinear(pivot_points[j], pivot_points[j + 1], pivot_points[j + i], error)) match = true;
	//	while (j + i < pivot_points.size() && colinear(pivot_points[j], pivot_points[j + 1], pivot_points[j + i], error)) i++;
	//	// 4 because of how the spline is imlemented
	//	if (match && i > 6) pivot_points.erase(pivot_points.begin() + j + 3, pivot_points.begin() + j + i - 4);
	//	j++;
	//}
	//TODO: beg / end check for colinear

	//erase points that won't change much overall shape of a segment
	//make some kind of a dynamic cost function?

	//auto processing = true;

	//while (processing)
	//{
	//	processing = false;
	//	for(int i = )
	//}

	changed = true;
}

//line fit optimazation

void Spline::forceHighlight(size_t index)
{
	highlight_pivot_point = index;
}

void Spline::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	auto width = this->width * 0.5f;

	if (changed)
	{
		spline_point_representation.clear();

		if (looped)
		{
			for (int i = 0; i < pivot_points.size(); i++)
			{
				std::array < int, 3 > ii = { i + 1, i + 2, i + 3 };
				for(auto& j : ii) if (j >= pivot_points.size()) j -= pivot_points.size();

				(this->*segmenting_algorithm)({ pivot_points[i], pivot_points[ii[0]], pivot_points[ii[1]], pivot_points[ii[2]] });
			}
		}
		else
		{
			for (int i = 0; i + 3 < pivot_points.size(); i++)
			{
				std::array < int, 3 > ii = { i + 1, i + 2, i + 3 };

				(this->*segmenting_algorithm)({ pivot_points[i], pivot_points[ii[0]], pivot_points[ii[1]], pivot_points[ii[2]] });
			}
		}

		spline.clear();
		spline.resize(spline_point_representation.size() * 2);

		for (int j = 0; j < spline_point_representation.size() * 2; j += 2)
		{
			auto [vec, _, grad] = spline_point_representation[j / 2];

			spline[j].position = { vec.x + sin(grad) * -width, vec.y + cos(grad) * -width };
			spline[j + 1].position = { vec.x + sin(grad) * width, vec.y + cos(grad) * width };
		}

		changed = false;
	}

	target.draw(spline);
	if (draw_segmentation_points)
	{
		sf::CircleShape cir;
		cir.setFillColor(sf::Color::Blue);
		cir.setRadius(2.f);
		cir.setOrigin(2.f, 2.f);
		for (auto vex : spline_point_representation)// int i = 0; i < spline_point_representation.size(); i++)
		{
			cir.setPosition(vex.position);
			target.draw(cir);
		}
	}
	if(draw_outline_points)
	{
		sf::CircleShape cir;
		cir.setFillColor(sf::Color::Cyan);
		cir.setRadius(2.f);
		cir.setOrigin(2.f, 2.f);
		for (auto [vec, _, grad] : spline_point_representation)
		{
			cir.setPosition({ vec.x + sin(grad) * -width, vec.y + cos(grad) * -width });
			target.draw(cir);
		}
		cir.setFillColor(sf::Color::Yellow);
		for (auto [vec, _, grad] : spline_point_representation)
		{
			cir.setPosition({ vec.x + sin(grad) * width, vec.y + cos(grad) * width });
			target.draw(cir);
		}
	}
	if (draw_pivot_points)
	{
		sf::CircleShape cir;
		cir.setFillColor(sf::Color::Red);
		cir.setRadius(2.f);
		cir.setOrigin(2.f, 2.f);
		for (auto pt : pivot_points)
		{
			cir.setPosition(pt);
			target.draw(cir);
		}
	}
	if (highlight_pivot_point != -1 && highlight_pivot_points)
	{
		sf::CircleShape cir;
		cir.setFillColor(sf::Color::Red);
		cir.setRadius(5.f);
		cir.setOrigin(5.f, 5.f);
		cir.setPosition(pivot_points[highlight_pivot_point]);
		target.draw(cir);
	}
}

template <>
AproximationAlgorithm Spline::toEnum(std::string name)
{
	if (name == "LineSplit") return AproximationAlgorithm::LineSplit;
	if (name == "VectorLenghtQubed") return AproximationAlgorithm::QubedVectorLenght;
	return AproximationAlgorithm::UnDefined;
}

template <>
SegmentingAlgorithm Spline::toEnum(std::string name)
{
	if (name == "LookAhead") return SegmentingAlgorithm::LookAhead;
	if (name == "Tree") return SegmentingAlgorithm::Tree;
	if (name == "Constant") return SegmentingAlgorithm::Constant;
	return SegmentingAlgorithm::UnDefined;
}

int Spline::aproximateSegmentation(std::array < sf::Vector2f, 4 > segment) const
{
	return 0;
}

//float Spline::splineIntegral()
//{
//	return 0.f;
//}

size_t Spline::getPivotPointsCount()
{
	return pivot_points.size();
}

sf::Vector2f& Spline::operator [] (size_t index)
{
	return pivot_points[index];
}

std::vector < sf::Vector2f >::iterator Spline::begin()
{
	return pivot_points.begin();
}

std::vector < sf::Vector2f >::iterator Spline::end()
{
	return pivot_points.end();
}

float Spline::lineSplitLenght(std::array < sf::Vector2f, 4 > segment) const
{
	//auto& [a, b, c, d] = segment;
	//
	//float start = 0.f;
	//float end = 1.f;

	//std::stack < std::pair < float, float > > stack;

	//std::list < float > 

	return 0.f;
}

float Spline::vectorLenghtQubedLenght(std::array < sf::Vector2f, 4 > segment) const
{
	auto& [a, b, c, d] = segment;

	sf::Vector2f buff = { std::abs(b.x - c.x), std::abs(b.y - c.y) };

	return buff.x * buff.x + buff.y * buff.y;
}

float Spline::getGradient(size_t index)
{
	if (looped)
	{
		std::array < int, 3 > ii = { index + 1, index + 2, index + 3 };
		for (auto& j : ii) if (j >= pivot_points.size()) j -= pivot_points.size();
		auto vec = getSplineGradient({ pivot_points[index], pivot_points[ii[0]], pivot_points[ii[1]], pivot_points[ii[2]] }, 0.f);
		return atan2f(-vec.y, vec.x);
	}

	//TODO: implement for not looped;
	return 0.f;
}

sf::Vector2f Spline::getSplinePoint(std::array < sf::Vector2f, 4 > segment, float t) const
{
	auto& [a, b, c, d] = segment;

	float t2 = t * t;
	float t3 = t2 * t;

	float q1 = -t3 + 2.0f * t2 - t;
	float q2 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	float q3 = -3.0f * t3 + 4.0f * t2 + t;
	float q4 = t3 - t2;

	float tx = 0.5f * (a.x * q1 + b.x * q2 + c.x * q3 + d.x * q4);
	float ty = 0.5f * (a.y * q1 + b.y * q2 + c.y * q3 + d.y * q4);

	return { tx, ty };
}

sf::Vector2f Spline::getSplineGradient(std::array < sf::Vector2f, 4 > segment, float t) const
{
	auto& [a, b, c, d] = segment;

	float t2 = t * t;

	float q1 = -3.f * t2 + 4.f * t - 1;
	float q2 = 9.f * t2 - 10.f * t;
	float q3 = -9.f * t2 + 8.f * t + 1.f;
	float q4 = 3.f * t2 - 2.f * t;

	float tx = 0.5f * (a.x * q1 + b.x * q2 + c.x * q3 + d.x * q4);
	float ty = 0.5f * (a.y * q1 + b.y * q2 + c.y * q3 + d.y * q4);

	return { tx, ty };
}
