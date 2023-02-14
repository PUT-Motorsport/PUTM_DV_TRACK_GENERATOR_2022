#include "Spline.h"

#include "Functions.inl"
#include "EnumWrapper.inl"

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

//Spline::Spline(int resolution) : resolution(resolution)//, pivot_points(default_points) 
//{
//	init();
//} 

Spline::Spline(std::vector < sf::Vector2f > pivot_points) : resolution(1), pivot_points(pivot_points) 
{
	init();
}

Spline::Spline(std::vector < sf::Vector2f > pivot_points, bool looped) : resolution(1), looped(looped), pivot_points(pivot_points)
{
	init();
}

Spline::Spline(std::vector < sf::Vector2f > pivot_points, bool looped, bool segment) : resolution(1), looped(looped), pivot_points(pivot_points)
{
	init();
	if (segment)
	{
		this->segment();
	}
}

//Spline::Spline(std::vector < sf::Vector2f > pivot_points, int resolution) : resolution(resolution), pivot_points(pivot_points) 
//{
//	init();
//}

void Spline::setAproximationAlgorithm(AproximationAlgorithm algorithm)
{
	switch (algorithm)
	{
		case AproximationAlgorithm::LineSplit: aproximaionAlgorithmPtr = &Spline::vectorLenghtSum; break;
		//case AproximationAlgorithm::QubedVectorLenght: aproximaion_algorithm = &Spline::vectorLenghtQubedLenght; break;
		default: return;
	}
}

void Spline::setSegmentingAlgorithm(SegmentingAlgorithm algorithm)
{
	switch (algorithm)
	{
		case SegmentingAlgorithm::LookAhead: segmentingAlgorithmPtr = &Spline::lookAheadSegmenting; break;
		//case SegmentingAlgorithm::Tree: segmenting_algorithm = &Spline::treeSegmenting; break;
		//case SegmentingAlgorithm::Constant: segmenting_algorithm = &Spline::constantSegementing; break;
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

	bd_box_offset = 1.1;
	width = 3.f;
}

void Spline::setWidth(float width)
{
	this->width = width;
}

void Spline::constantSegementing() const
{

}

void Spline::treeSegmenting() const
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

void Spline::lookAheadSegmenting() const
{
	float t_size = getT();
	for (size_t t = 0; t < t_size; t++)
	{
		float lt = 0.f;

		auto grad_vec = getGradientVector(t, 0.f);
		point_representation.push_back({ getPoint(t, 0.f), grad_vec, atanv2(grad_vec), t});

		while (lt < 1.f)
		{
			auto step = 0.f;
			auto step_step = 0.05f;

			auto grad_s_vec = getGradientVector(t, lt + step);
			auto grad_t_vec = point_representation.back().gradient_vector;

			float grad = angle(grad_t_vec, grad_s_vec);
			while (grad < max_gradient && lt + step < 1.f)
			{
				step += step_step;
				grad = angle(getGradientVector(t, lt + step), point_representation.back().gradient_vector);
				step_step *= 2.f;
			}
			step_step *= 0.5f;
			while (step_step > 0.001f)
			{
				if (grad > max_gradient) step -= step_step;
				else step += step_step;
				grad = angle(getGradientVector(t, lt + step), point_representation.back().gradient_vector);
				step_step *= 0.5f;
			}
			if (lt + step < 1.f) lt += step;
			else lt = 1.f;

			auto g = getGradientVector(t, lt);
			point_representation.push_back({ getPoint(t, lt), g, atanv2(g), t});
		}
	}
}

void Spline::insertPivotPoint(size_t at, sf::Vector2f new_pos)
{
	pivot_points.insert(pivot_points.begin() + at, new_pos);
	changed = true;
}

int Spline::mouseEnteredPivotPoint(sf::Vector2f mouse_pos)
{	
	for (size_t i = 0; i < pivot_points.size(); i++)
	{
		auto len = distance(pivot_points[i], mouse_pos);
		if (len <= this->width * 0.3 * 2.f)
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
		if (len <= this->width * 0.3 * 2.f)
		{
			highlight_pivot_point = i;
			return highlight_pivot_point;
		}
	}
	highlight_pivot_point = -1;
	return highlight_pivot_point;
}

size_t Spline::size() const
{
	return getT();
}

void Spline::erase(std::vector < sf::Vector2f >::iterator where)
{
	pivot_points.erase(where);
}

void Spline::erase_last()
{
	pivot_points.erase(pivot_points.end() - 1);
}

void Spline::push_back(sf::Vector2f& val)
{
	pivot_points.push_back(val);
}

void Spline::optimize()
{
	const float error = 0.001f;

	size_t j = 0;
	//erase overlapping
	while (j < pivot_points.size())
	{
		size_t i = 1;
		if (j + i < pivot_points.size() && overlap(pivot_points[j], pivot_points[j + i], 0.001))
		{
			while (j + i < pivot_points.size() && overlap(pivot_points[j], pivot_points[j + i], 0.001)) i++; //pivot_points[j] == pivot_points[j + i]
			pivot_points.erase(pivot_points.begin() + j, pivot_points.begin() + j + i - 1);
		}
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

void Spline::forceHighlight(size_t index)
{
	highlight_pivot_point = index;
}

void Spline::segment() const
{
	point_representation.clear();
	segmentingAlgorithm();
}

void Spline::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	auto width = this->width * 0.5f;

	if (changed)
	{
		point_representation.clear();

		segmentingAlgorithm();

		spline.clear();
		spline.resize(point_representation.size() * 2);

		for (int j = 0; j < point_representation.size() * 2; j += 2)
		{
			auto [vec, _, grad, __] = point_representation[j / 2];

			spline[j].position = { vec.x + sin(grad) * -width, vec.y + cos(grad) * -width };
			spline[j + 1].position = { vec.x + sin(grad) * width, vec.y + cos(grad) * width };
		}

		changed = false;
	}

	auto cir_w = this->width * 0.3;

	target.draw(spline);
	if (draw_segmentation_points)
	{
		sf::CircleShape cir;
		cir.setFillColor(sf::Color::Blue);
		cir.setRadius(cir_w);
		cir.setOrigin(cir_w, cir_w);
		for (auto vex : point_representation)// int i = 0; i < point_representation.size(); i++)
		{
			cir.setPosition(vex.position);
			target.draw(cir);
		}
	}
	if(draw_outline_points)
	{
		sf::CircleShape cir;
		cir.setFillColor(sf::Color::Cyan);
		cir.setRadius(cir_w);
		cir.setOrigin(cir_w, cir_w);
		for (auto [vec, _, grad, __] : point_representation)
		{
			cir.setPosition({ vec.x + sin(grad) * -width, vec.y + cos(grad) * -width });
			target.draw(cir);
		}
		cir.setFillColor(sf::Color::Yellow);
		for (auto [vec, _, grad, __] : point_representation)
		{
			cir.setPosition({ vec.x + sin(grad) * width, vec.y + cos(grad) * width });
			target.draw(cir);
		}
	}
	if (draw_pivot_points)
	{
		sf::CircleShape cir;
		cir.setFillColor(sf::Color::Red);
		cir.setRadius(cir_w);
		cir.setOrigin(cir_w, cir_w);
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
		cir.setRadius(cir_w * 2.f);
		cir.setOrigin(cir_w * 2.f, cir_w * 2.f);
		cir.setPosition(pivot_points[highlight_pivot_point]);
		target.draw(cir);
	}
}

//float Spline::splineIntegral()
//{
//	return 0.f;
//}

size_t Spline::getPivotPointsCount()
{
	return pivot_points.size();
}

size_t Spline::getPointsCount()
{
	return point_representation.size();
}

sf::Vector2f Spline::getPivotPoint(size_t index) const
{
	return pivot_points[index];
}

sf::Vector2f Spline::getPoint(size_t index) const
{
	return point_representation[index].position;
}

sf::Vector2f Spline::getPoint(size_t index, float t) const
{
	auto [a, b, c, d] = getSegment(index);

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

sf::Vector2f Spline::getGradientVector(size_t index) const
{
	return point_representation[index].gradient_vector;
}

sf::Vector2f Spline::getGradientVector(size_t index, float t) const
{
	auto [a, b, c, d] = getSegment(index);

	float t2 = t * t;

	float q1 = -3.f * t2 + 4.f * t - 1.f;
	float q2 = 9.f * t2 - 10.f * t;
	float q3 = -9.f * t2 + 8.f * t + 1.f;
	float q4 = 3.f * t2 - 2.f * t;

	float tx = 0.5f * (a.x * q1 + b.x * q2 + c.x * q3 + d.x * q4);
	float ty = 0.5f * (a.y * q1 + b.y * q2 + c.y * q3 + d.y * q4);

	return { tx, ty };
}

sf::Vector2f Spline::getInflexionVector(size_t index, float t) const
{
	auto [a, b, c, d] = getSegment(index);

	float q1 = -6.f * t + 4;
	float q2 = 18.f * t - 10;
	float q3 = -18.f * t + 8;
	float q4 = 6 * t - 2;

	float tx = 0.5f * (a.x * q1 + b.x * q2 + c.x * q3 + d.x * q4);
	float ty = 0.5f * (a.y * q1 + b.y * q2 + c.y * q3 + d.y * q4);

	return { tx, ty };
}

sf::Vector2f Spline::getCenter()
{
	sf::Vector2f center;
	for (auto& pt : point_representation) center += pt.position;
	center = center / float(point_representation.size());
	return center;
}

sf::Vector2f& Spline::operator [] (size_t index)
{
	return pivot_points[index];
}

sf::Vector2f& Spline::front()
{
	return pivot_points.front();
}

sf::Vector2f& Spline::back()
{
	return pivot_points.back();
}

std::pair < float, float > Spline::solveGradientForIndex(size_t index)
{
	float t1 = -1.f;
	float t2 = -1.f;
	auto [aa, bb, cc, dd] = getSegment(index);
	auto v = bb - cc;
	auto tgx = -v.y / v.x;
	float a = aa.y + aa.x * tgx;
	float b = bb.y + bb.x * tgx;
	float c = cc.y + cc.x * tgx;
	float d = dd.y + dd.x * tgx;
	float A = -3.f * a + 9.f  * b - 9.f * c + 3.f * d;
	float B = 4.f * a - 10.f * b + 8.f * c - 2.f * d;
	float C = c - a;
	float delta = sq(B) - 4.f * A * C;
	if (delta > 0)
	{
		float sq_delta = sqrt(delta);
		t1 = (-B + sq_delta) / (2 * A);
		t2 = (-B - sq_delta) / (2 * A);
	} 
	else if (delta == 0)
	{
		t1 = -B / (2 * A);
	}

	return { t1, t2 };
}

float Spline::solveSegmentsClosestPoint(size_t index, sf::Vector2f point)
{
	//auto [A, B, C, D] = getSegment(index);
	//auto [x, a] = A;
	//auto [y, b] = B;
	//auto [w, c] = C;
	//auto [z, d] = D;
	//auto [g, h] = point;

	return 0.f;
}

sf::FloatRect Spline::getSegmentBoundingBox(size_t index)
{
	sf::Vector2f left = { INFINITY, INFINITY };
	sf::Vector2f right = { -INFINITY, -INFINITY };
	std::vector points = getSegmentCollisionBox(index);
	for (auto p : points)
	{
		left.x = std::min(left.x, p.x);
		left.y = std::min(left.y, p.y);
		right.x = std::max(right.x, p.x);
		right.y = std::max(right.y, p.y);
	}
	//std::array < size_t, 2 > ii = { index + 1, index + 2 };
	//if (looped) for (auto& j : ii) if (j >= pivot_points.size()) j -= pivot_points.size();
	//for (auto& j : ii)
	//{
	//	for (int i = -1; i <= 1; i += 2)
	//	{
	//		float t = i == -1 ? 0 : 1.f;
	//		auto grad = getGradient(index, t);
	//		auto vec = sf::Vector2f(sin(grad), cos(grad));
	//		auto pt = pivot_points[j] + float(i) * bd_box_offset * vec * width; //direction(vec)
	//		left.x = std::min(left.x, pt.x);
	//		left.y = std::min(left.y, pt.y);
	//		right.x = std::max(right.x, pt.x);
	//		right.y = std::max(right.y, pt.y);
	//	}
	//}
	//auto [t1, t2] = solveGradientForIndex(index);
	//std::array < float, 2 > tt = { t1, t2 };
	//for (auto t : tt)
	//{
	//	if (t > 0.f && t < 1.f)
	//	for (int i = -1; i <= 1; i += 2)
	//	{
	//		auto v = getPoint(index, t) + float(i) * bd_box_offset * direction(getGradientVector(index, t)) * width;
	//		left.x = std::min(left.x, v.x);
	//		left.y = std::min(left.y, v.y);
	//		right.x = std::max(right.x, v.x);
	//		right.y = std::max(right.y, v.y);
	//	}
	//}

	auto w = right.x - left.x;
	auto h = right.y - left.y;

	return { left.x, left.y, w, h };
}

std::vector < sf::Vector2f >::iterator Spline::begin()
{
	return pivot_points.begin();
}

std::vector < sf::Vector2f >::iterator Spline::end()
{
	return pivot_points.end();
}

float Spline::vectorLenghtSum() const
{
	float lenght = 0.f;
	for (auto point = point_representation.begin(); point != point_representation.end() - 1; point++)
	{
		auto fpoint = point + 1;
		lenght += distance(point->position, fpoint->position);
	}
	if (looped) lenght += distance(point_representation.front().position, point_representation.back().position);
	return lenght;
}

float Spline::getGradient(size_t index) const
{
	return point_representation[index].gradient;
}

float Spline::getGradient(size_t index, float t) const
{
	auto vec = getGradientVector(index, t);
	return atan2f(-vec.y, vec.x);
}

float Spline::getInflexion(size_t index, float t) const
{
	auto vec = getInflexionVector(index, t);
	return atan2f(-vec.y, vec.x);
}

size_t Spline::getT() const
{
	if (looped) return pivot_points.size();
	return pivot_points.size() - 3.f;
}

float Spline::getLenght()
{
	return aproximaionAlgorithm();
}

std::vector < SplinePoint > Spline::getPointRepresenation()
{
	return point_representation;
}

std::vector < sf::Vector2f > Spline::getPivotPoints()
{
	return pivot_points;
}

bool Spline::segmentsCollide(size_t index1, size_t index2)
{
	std::vector < sf::Vector2f > col_box_1;
	std::vector < sf::Vector2f > col_box_2;
	sf::FloatRect bd_box_1;
	sf::FloatRect bd_box_2;

	auto subfun = [&]() -> bool
	{
		bd_box_1 = getSegmentBoundingBox(index1);
		bd_box_2 = getSegmentBoundingBox(index2);

		if (!bd_box_1.intersects(bd_box_2)) return false;

		col_box_1 = getSegmentCollisionBox(index1);
		col_box_2 = getSegmentCollisionBox(index2);

		for (auto a = col_box_1.begin(); a != col_box_1.end(); a++)
		{
			auto b = a + 1;
			if (b == col_box_1.end()) b = col_box_1.begin();
			for (auto x = col_box_2.begin(); x != col_box_2.end(); x++)
			{
				auto y = x + 1;
				if (y == col_box_2.end()) y = col_box_2.begin();
				if (doIntersect(*a, *b, *x, *y)) return true;
			}
		}

		return false;
	};

	bool result = subfun();
	return result;
}


std::vector < sf::Vector2f > Spline::getSegmentCollisionBox(size_t index)
{
	float lt = 0.f;
	size_t index_end = (index + 1 == pivot_points.size() ? 0 : index + 1);

	std::vector < SplinePoint > points;
	std::vector < sf::Vector2f > collision_box;
	std::copy_if(point_representation.begin(), point_representation.end(), std::back_inserter(points), [&](SplinePoint p) -> bool
		{
			return p.parent == index;
		});
	auto last = std::find_if(point_representation.begin(), point_representation.end(), [&](SplinePoint p) -> bool
		{
			return p.parent == index_end;
		});
	points.push_back(*last);
	collision_box.reserve(points.size() * 2);
	for (auto p : points)
	{
		sf::Vector2f v = { p.position.x - width * sin(p.gradient) * 0.5f, p.position.y - width * cos(p.gradient) * 0.5f };
		collision_box.push_back(v);
	}
	for (auto _p = points.rbegin(); _p != points.rend(); _p++)
	{
		auto p = *_p;
		sf::Vector2f v = { p.position.x + width * sin(p.gradient) * 0.5f, p.position.y + width * cos(p.gradient) * 0.5f };
		collision_box.push_back(v);
	}

	return collision_box;
}

std::array < sf::Vector2f, 4 > Spline::getSegment(size_t i) const
{
	std::array < size_t, 4 > ii = { i,  i + 1, i + 2, i + 3 };
	if (looped) for (auto& j : ii) if (j >= pivot_points.size()) j -= pivot_points.size();
	return { pivot_points[ii[0]], pivot_points[ii[1]], pivot_points[ii[2]], pivot_points[ii[3]] };
}