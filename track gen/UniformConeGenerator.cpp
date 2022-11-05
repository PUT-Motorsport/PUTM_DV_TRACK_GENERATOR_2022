#include "UniformConeGenerator.h"

#include "Functions.inl"

#include <array>

using ucg = UniformConeGenerator;

ucg::UniformConeGenerator(Spline spline) : IConeGenerator(spline)
{
	genration_algorithm = &UniformConeGenerator::fillAndOffsetAlgorithm;//&UniformConeGenerator::trianglesAlgorithm;
}

void ucg::generateAllCones()
{
	generate();
}

void ucg::trianglesAlgorithm()
{
	std::vector < Cone > buff_right;
	std::vector < Cone > buff_left;
	std::vector < sf::Vector2f > buff_pivot = spline.getPivotPoints();

	//for (size_t i = 0; i < buff_pivot.size(); i++)
	//{
	//	auto grad = spline.getGradient(i, 0.f);
	//	auto& pivot = buff_pivot[i];
	//	buff_right.push_back({ Type::Right, pivot });
	//	buff_left.push_back({ Type::Left, pivot });
	//}

	//################################################################################################################################

	std::vector < Cone > found_right;

	//auto center = spline.getCenter();

	auto typ = Type::Left;

	for (size_t i = 0; i < buff_pivot.size(); i++)
	{
		if (typ == Type::Left) typ = Type::Right;
		else if (typ == Type::Right) typ = Type::Left;

		std::array < int, 4 > ii = { i,  i + 1, i + 2, i + 3 };
		for (auto& j : ii) if (j >= buff_pivot.size()) j -= buff_pivot.size();

		std::vector < sf::Vector2f > points = { buff_pivot[ii[0]], buff_pivot[ii[1]], buff_pivot[ii[2]], buff_pivot[ii[3]] };

		Spline segment(points, false, true);

		float len = segment.getLenght();

		/*
		* the find the lengh of side of a track by scaling segments scale approach
		*/
		sf::Vector2f ab = segment.getPivotPoint(2) - segment.getPivotPoint(1);

		auto g1 = segment.getGradientVector(0, 0.f);
		g1 = { -g1.y, g1.x };
		auto g2 = segment.getGradientVector(0, 1.f);
		g2 = { -g2.y, g2.x };

		auto ang = angle(g1, g2);
		if (ang >= toRad(180.f)) ang -= toRad(180.f);

		if (ang > toRad(1.f) && angle(g1, g2) < toRad(179.f))
		{
			auto ang1 = -angle(ab, g1);
			auto ang2 = -angle(-ab, g2);

			auto point = findThirdPoint(segment.getPivotPoint(1), ang1, segment.getPivotPoint(2), ang2);
			float sign = -sgnf(dot(segment.getPivotPoint(1), segment.getPivotPoint(2), point));

			found_right.push_back({ Type::Ctrl1, point });
			found_right.push_back({ Type::Ctrl2, segment.getPivotPoint(1) });
			found_right.push_back({ Type::Ctrl2, segment.getPivotPoint(2) });

			long double buff_len = 0.0;

			for (size_t i = 1; i < segment.getPointsCount(); i++)
			{
				sf::Vector2<long double> a = sf::Vector2<long double>(segment.getPoint(i - 1));//segment[i - 1];
				sf::Vector2<long double> b = sf::Vector2<long double>(segment.getPoint(i));

				auto pa = a - sf::Vector2<long double>(point);
				auto pb = b - sf::Vector2<long double>(point);
				auto pa_len = lenght(pa);
				auto pb_len = lenght(pb);
				auto apb_ang = angle(pa, pb);
				pa_len += sign * track_width;
				pb_len += sign * track_width;
				long double a1b1 = sqrtl(sq(pa_len) + sq(pb_len) - 2 * pa_len * pb_len * cosl(apb_ang));
				buff_len += a1b1;
			}

			len = float(buff_len);
		}

		auto coef = (max_cone_distance - fmodf(len, max_cone_distance)) / (floor(len / max_cone_distance) + 1.f);
		auto opt_dist = max_cone_distance - coef;
		size_t cone_count = size_t(roundf(len / opt_dist)) - 1;

		auto g = segment.getGradient(0, 0.f);
		auto pt = segment.getPoint(0, 0.f);
		found_right.push_back({ Type::Ctrl1, { pt.x + sin(g) * -track_width, pt.y + cos(g) * -track_width } });

		float t = 0.f;
		bool oversteped = false;
		for (size_t c = 0; c < cone_count; c++)
		{
			if (t >= 1.f) oversteped = true;
			float step = 0.001f;
			float step_step = step;

			g = segment.getGradient(0, t + step);
			pt = segment.getPoint(0, t + step);
			sf::Vector2f vec = { pt.x + sin(g) * -track_width, pt.y + cos(g) * -track_width };

			while (distance(found_right.back().pos, vec) < opt_dist)
			{
				step += step_step;
				g = segment.getGradient(0, t + step);
				pt = segment.getPoint(0, t + step);
				vec = { pt.x + sin(g) * -track_width, pt.y + cos(g) * -track_width };
			}

			found_right.push_back({ typ, vec });

			t += step;
		}

		// let's say 0.5 m diff is ok 
		bool dist_violation = distance(found_right.back().pos, segment.getPoint(0, 1.f)) < opt_dist - 0.5f;

		if (dist_violation || oversteped)
		{

		}
	}

	buff_right.insert(buff_right.begin(), found_right.begin(), found_right.end());

	track.clear();
	track = buff_right;

	//TODO
}

void ucg::fillAndOffsetAlgorithm()
{
	std::vector < sf::Vector2f > buff_pivot = spline.getPivotPoints();

	std::vector < Cone > found_right;
	std::vector < Cone > found_left;

	faoSubAlgorithm1(Type::Right, -1.f, found_right, buff_pivot);
	faoSubAlgorithm1(Type::Left, 1.f, found_left, buff_pivot);

	track.clear();
	track = found_right;
	track.insert(track.end() - 1, found_left.begin(), found_left.end());

	//TODO
}

void ucg::faoSubAlgorithm1(Type typ, float sign, std::vector < Cone >& container, std::vector < sf::Vector2f >& const buff_pivot)
{
	for (size_t i = 0; i < buff_pivot.size(); i++)
	{
		//if (typ == Type::Left) typ = Type::Right;
		//else if (typ == Type::Right) typ = Type::Left;

		std::array < int, 4 > ii = { i,  i + 1, i + 2, i + 3 };
		for (auto& j : ii) if (j >= buff_pivot.size()) j -= buff_pivot.size();

		std::vector < sf::Vector2f > points = { buff_pivot[ii[0]], buff_pivot[ii[1]], buff_pivot[ii[2]], buff_pivot[ii[3]] };

		Spline segment(points, false, true);

		float step = 0.001f;
		float t = step;
		float opt_dist = max_cone_distance;

		size_t cone_count = 0;

		auto g = segment.getGradient(0, 0.f);
		auto pt = segment.getPoint(0, 0.f);
		sf::Vector2f prev_cone;
		sf::Vector2f cone = { pt.x + sin(g) * sign * track_width, pt.y + cos(g) * sign * track_width };

		while (t < 1.f)
		{
			cone_count++;
			auto g = segment.getGradient(0, t);
			auto pt = segment.getPoint(0, t);
			prev_cone = cone;
			cone = { pt.x + sin(g) * sign * track_width, pt.y + cos(g) * sign * track_width };
			while (distance(cone, prev_cone) <= opt_dist && t < 1.f)
			{
				t += step;
				g = segment.getGradient(0, t);
				pt = segment.getPoint(0, t);
				cone = { pt.x + sin(g) * sign * track_width, pt.y + cos(g) * sign * track_width };
			}
		}
		g = segment.getGradient(0, 1.f);
		pt = segment.getPoint(0, 1.f);
		sf::Vector2f last_cone = { pt.x + sin(g) * sign * track_width, pt.y + cos(g) * sign * track_width };
		auto dist = distance(prev_cone, last_cone);
		auto offset = (5 - dist) / (cone_count);

		opt_dist -= offset;

		step = 0.001f;
		t = step;
		prev_cone = segment.getPoint(0, 0.f);

		g = segment.getGradient(0, 0.f);
		pt = segment.getPoint(0, 0.f);
		prev_cone = { pt.x + sin(g) * sign * track_width, pt.y + cos(g) * sign * track_width };
		container.push_back({ typ, prev_cone });

		for (size_t j = 0; j < cone_count - 1; j++)
		{
			auto g = segment.getGradient(0, t);
			auto pt = segment.getPoint(0, t);
			sf::Vector2f cone = { pt.x + sin(g) * sign * track_width, pt.y + cos(g) * sign * track_width };
			while (distance(cone, prev_cone) <= opt_dist && t < 1.f)
			{
				t += step;
				g = segment.getGradient(0, t);
				pt = segment.getPoint(0, t);
				cone = { pt.x + sin(g) * sign * track_width, pt.y + cos(g) * sign * track_width };
			}
			prev_cone = cone;
			container.push_back({ typ, cone });
		}
	}
}