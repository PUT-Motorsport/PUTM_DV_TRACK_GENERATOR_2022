#include "PathFindingSplineGenerator.h"
#include "RandomGenerator.h"
#include "Functions.inl"

#include <thread>
#include <functional>
#include <mutex>
#include <exception>
#include <map>

using pf = PathFindingSplineGenerator;

bool pf::isSafeField(Spline& build_path, sf::Vector2f & const pt1)
{
	auto rr = sq(build_path.width * 0.5f + safe_dist);
	auto const pt2 = build_path.back();
	for (auto p1 = bounding_points.begin(); p1 != bounding_points.end(); p1++)
	{
		auto p2 = p1 + 1;
		if (p2 == bounding_points.end()) p2 = bounding_points.begin();
		if (doIntersect(pt1, pt2, *p1, *p2)) return false;
	}
	for (auto o : obstacles)
	{
		if (o.contains(pt1)) return false;
		auto o_points = rectToPoints(o);
		for (auto p1 = o_points.begin(); p1 != o_points.end(); p1++)
		{
			auto p2 = p1 + 1;
			if (p2 == o_points.end()) p2 = o_points.begin();
			if (doIntersect(pt1, pt2, *p1, *p2)) return false;
		}
	}

	if (build_path.size() < 4) return true;

	for (auto p1 = build_path.begin() + 1; p1 != build_path.end() - 1; p1++)
	{
		auto p2 = p1 + 1;
		if (p2 == build_path.end() - 1) p2 = build_path.begin() + 1;
		if (doIntersect(pt1, pt2, *p1, *p2)) return false;
	}
	build_path.segment();
	for (size_t i = 0; i < build_path.size() - 2; i++)
	{
		auto col_box = build_path.getSegmentCollisionBox(i);
		for (auto p1 = col_box.begin(); p1 != col_box.end(); p1++)
		{
			auto p2 = p1 + 1;
			if (p2 == col_box.end()) p2 = col_box.begin();
			auto dist = distSq(pt1, castOnVec(*p1, *p2, pt1));
			if (dist <= rr) return false;
		}
	}
	return true;
}


pf::PathFindingSplineGenerator() : ISplineGenerator(GeneratorType::PathFidingGenerator)
{
	//bounding_box = { -100.f, -100.f, 200.f, 200.f };
	//bounding_box = { -25.f, -25.f, 50.f, 50.f };
	bounding_box = { 0.f, 0.f, 50.f, 100.f };
	auto [x, y, w, h] = bounding_box;
	bounding_points = { sf::Vector2f(x, y), sf::Vector2f(x, y + h), sf::Vector2f(x + w, y + h), sf::Vector2f(x + w, y) };
	//start_box = { 50.f, 50.f, 50.f, 50.f };
	start_box = { 25.f, 25.f, 10.f, 10.f };
	//max_step_dist = 10.f;
	//min_step_dist = 6.f;
	max_step_dist = 15.f;
	min_step_dist = 6.f;
	safe_dist = 1.f;
	angle_shift = toRad(75);
	angle_change_speed_ratio = 100.f;
	angle_step = angle_shift * 2.f / angle_change_speed_ratio;
	timeout_max = 1000;//20;
	safe_field_ratio = 0.5;
	max_parallel_threads = 3;//4;
	min_ratio_of_threads_completed = 0.3;
	thread_timeout = 2;//15;
	finished = false;
	filter_coef = 0.75f;
	max_num_of_x = 2;
	allow_x = false;
	x_min_ang = toRad(70);
	x_max_ang = toRad(110);
	max_angle = toRad(90);
	do_filter = true;
	obstacles =
	{
		//{ 20.f, 0.f, 5.f, 25.f },
		//{ 0.f, 35.f, 20.f, 5.f },
		//{ 40.f, 25.f, 10.f, 5.f },
		//{ 5.f, 10.f, 5.f, 10.f },
	};
}

void pf::generate(Spline& build_path, bool& exitThread, int& thread_finished)
{
	try
	{
		int allowed_x = 0;

		auto find = [&]() -> sf::Vector2f
		{
			auto p1 = build_path.end() - 2;
			auto p2 = p1 + 1;
			auto dir = *p2 - *p1;
			auto angle = atanv2(dir);
			auto rng_r = RandomGenerator::random(min_step_dist, max_step_dist);
			auto rng_angle = RandomGenerator::random(angle - angle_shift, angle + angle_shift);
			auto found = polarToCartesian(rng_r, rng_angle);

			return build_path.back() + found;
		};
		auto check = [&]() -> int
		{
			if (build_path.size() < 5) return 0;
			if (distance(build_path.front(), build_path.back()) <= max_step_dist)
			{
				return 1;
			}
			return 0;
		};
		std::function < int(sf::Vector2f) > proceed;
		proceed = [&](sf::Vector2f found) -> int
		{
			if (exitThread) return 2;
			if (isSafeField(build_path, found))
			{
				build_path.push_back(found);
				if(check()) return 1;
				auto timeout = 0;
				auto result = 0;
				do
				{
					result = proceed(find());
					timeout++;
					if (timeout > timeout_max)
					{
						build_path.erase_last();
						return 0;
					}
				} while (result == 0);
				return result;
			}
			return 0;
		};
		auto filter = [&]() -> int
		{
			auto sub1 = [&]() -> bool
			{
				for (auto p1 = build_path.begin(); p1 != build_path.end(); p1++)
				{
					auto p2 = p1 + 1;
					if (p2 == build_path.end()) p2 = build_path.begin();
					if (distance(*p1, *p2) < min_step_dist)
					{
						build_path.erase(p1);
						return false;
					}
				}
				return true;
			};
			auto sub2 = [&]() -> bool
			{
				for (auto p1 = build_path.begin(); p1 != build_path.end(); p1++)
				{
					auto p2 = p1 + 1;
					if (p2 == build_path.end()) p2 = build_path.begin();
					auto p3 = p2 + 1;
					if (p3 == build_path.end()) p3 = build_path.begin();
					//auto dist12 = distance(*p1, *p2);
					//auto dist23 = distance(*p2, *p3);
					//auto dist13 = distance(*p1, *p3);
					auto vec12 = *p2 - *p1;
					auto vec23 = *p3 - *p2;
					auto angle = abs(angle2(vec12, vec23));
					if (angle > toRad(180)) angle = toRad(360) - angle;
					if (angle >= max_angle)//dist12 >= dist13 || dist23 >= dist13 || 
					{
						build_path.erase(p2);
						return false;
					}
				}
				return true;
			};

			while (true)
			{
				if (!sub1()) continue;
				if (!sub2()) continue;
				if (build_path.size() < 4) return 2;

				return 1;
			}
		};
		auto findSafe = [&](sf::Vector2f pt1) -> sf::Vector2f
		{
			while(true)
			{
				float ox = RandomGenerator::random(min_step_dist, max_step_dist);
				float oy = RandomGenerator::random(min_step_dist, max_step_dist);

				sf::Vector2 pt2 = { pt1.x + ox, pt1.y + oy };

				auto rr = sq(build_path.width * 0.5f + safe_dist);
				for (auto p1 = bounding_points.begin(); p1 != bounding_points.end(); p1++)
				{
					auto p2 = p1 + 1;
					if (p2 == bounding_points.end()) p2 = bounding_points.begin();
					if (doIntersect(pt1, pt2, *p1, *p2)) continue;
				}
				for (auto o : obstacles)
				{
					if (o.contains(pt1)) continue;
					auto o_points = rectToPoints(o);
					for (auto p1 = o_points.begin(); p1 != o_points.end(); p1++)
					{
						auto p2 = p1 + 1;
						if (p2 == o_points.end()) p2 = o_points.begin();
						if (doIntersect(pt1, pt2, *p1, *p2)) continue;
					}
				}
				return pt2;
			}
		};

		float tmpx = RandomGenerator::random(0.f, start_box.width) + start_box.left;
		float tmpy = RandomGenerator::random(0.f, start_box.height) + start_box.top;

		sf::Vector2f pt1 = { tmpx , tmpy };
		sf::Vector2f pt2 = findSafe(pt1);

		std::vector < sf::Vector2f > buff = { pt1, pt2 };
		build_path = Spline(buff, true);
		thread_finished = proceed(find());
		if (thread_finished == 1 && do_filter) thread_finished = filter();
	}
	catch (...) { thread_finished = 2; }
}

void pf::generateFullTrack()
{
	std::vector < Spline > build_paths;
	bool thread_exit_state;
	std::vector < int > thread_state;
	std::vector < std::thread > threads;
	bool ok = false;
	int c = 0;
	while (!ok)
	{
		thread_exit_state = false;
		ok = true;
		threads.clear();
		build_paths.clear();
		thread_state.clear();

		build_paths.resize(max_parallel_threads);
		threads.resize(max_parallel_threads);
		thread_state.resize(max_parallel_threads, 0);

		for (size_t i = 0; i < max_parallel_threads; i++)
		{
			threads[i] = std::thread(&pf::generate, this, std::ref(build_paths[i]), std::ref(thread_exit_state), std::ref(thread_state[i]));
		}

		start = std::chrono::steady_clock::now();

		while (!thread_exit_state)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			std::chrono::steady_clock::duration elapsed = std::chrono::steady_clock::now() - start;
			auto e = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
			if (e >= thread_timeout)
			{
				thread_exit_state = true;
				break;
			}
			c = 0;
			for (auto state : thread_state) if (state == 1) c++;
			float r = float(c) / max_parallel_threads;
			if (r >= min_ratio_of_threads_completed)
			{
				thread_exit_state = true;
				break;
			}
		}

		for (auto& thread : threads) thread.join();

		c = 0;
		for (auto state : thread_state) if (state == 1) c++;
		if (c == 0)
		{
			ok = false;
		}
	}

	int rng = RandomGenerator::random(0, c - 1);
	while (thread_state[rng] != 1) rng++;
	track = build_paths[rng];

	finished = true;
}

void pf::stepTrackGeneration()
{
	if (finished) return;
	//proceed();
	//check();

	track = Spline(build_path, true);
}