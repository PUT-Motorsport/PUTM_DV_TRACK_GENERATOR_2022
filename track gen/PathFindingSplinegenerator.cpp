#include "PathFindingSplineGenerator.h"
#include "RandomGenerator.h"
#include "Functions.inl"

#include <thread>
#include <functional>
#include <mutex>

using pf = PathFindingSplineGenerator;


bool pf::isSafeField(size_t ind, float min_ang, float max_angle, std::vector < PathStruct >& build_path)
{
	int safe = 0;
	int max_possible_field = angle_shift * 2.f / angle_step;
	for (float t = min_ang; t <= max_angle; t += angle_step)
	{
		auto dir = direction(t);
		auto shift = dir * (max_step_dist + safe_dist);
		auto the_point = build_path[ind].p + shift;	
		if (doIntersect(bounding_points[0], bounding_points[1], build_path[ind].p, the_point) ||
			doIntersect(bounding_points[1], bounding_points[2], build_path[ind].p, the_point) ||
			doIntersect(bounding_points[2], bounding_points[3], build_path[ind].p, the_point) ||
			doIntersect(bounding_points[3], bounding_points[0], build_path[ind].p, the_point))
		{
			continue;
		}
	
		bool con = false;
		for (size_t i = 1; i < build_path.size() - 2; i++)
		{
			if (doIntersect(build_path[i].p, build_path[i - 1].p, build_path[ind].p, the_point))
			{
				con = true;
				break;
			}
		}
		if (con) continue;
		safe++;
	}
	return float(safe) / float(max_possible_field) >= safe_field_ratio;
}

pf::PathFindingSplineGenerator() : ISplineGenerator()
{
	bounding_box = { -100.f, -100.f, 200.f, 200.f };
	auto [x, y, w, h] = bounding_box;
	bounding_points = { sf::Vector2f(x, y), sf::Vector2f(x, y + h), sf::Vector2f(x + w, y + h), sf::Vector2f(x + h, y) };
	start_box = { 50.f, 50.f, 50.f, 50.f };
	max_step_dist = 15.f;
	min_step_dist = 8.f;
	safe_dist = 5.f;
	angle_shift = toRad(60);
	angle_step = 0.01f;
	timeout_max = 1000;
	safe_field_ratio = 0.5;
	max_parallel_threads = 10;
	min_ratio_of_threads_completed = 0.6;
	timeout_s = 15;
	finished = false;
}

bool pf::check(std::vector < PathStruct >& build_path)
{
	bool ang = abs(angle2(build_path.back().p - build_path.front().p, build_path.back().p - build_path[build_path.size() - 2].p));
	ang = 180 - ang;
	if (distance(build_path.front().p, build_path.back().p) <= max_step_dist && ang <= angle_shift)
	{
		for (size_t i = 2; i < build_path.size() - 2; i++)
		{
			if (doIntersect(build_path[i].p, build_path[i - 1].p, build_path.front().p, build_path.back().p))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

void pf::proceed(std::vector < PathStruct >& build_path)
{
	size_t ind = build_path.size() - 1;
	size_t ind_buff = ind;
	auto ang_vec = build_path[ind].p - build_path[ind - 1].p;
	float ang = atan2f(ang_vec.y, ang_vec.x);
	float max_ang = ang + angle_shift;
	float min_ang = ang - angle_shift;
	bool rnd_con = true;
	sf::Vector2f the_point;
	sf::Vector2f dir;
	float rnd_dist;
	float rnd_ang;
	int timeout = 0;
	while (rnd_con)
	{
		if (timeout >= timeout_max)
		{
			do
			{
				build_path.erase(build_path.end() - 1);
				ind--;

				ang_vec = build_path[ind].p - build_path[ind - 1].p;
				ang = atan2f(ang_vec.y, ang_vec.x);
				max_ang = ang + angle_shift;
				min_ang = ang - angle_shift;
			} while (!isSafeField(ind, min_ang, max_ang, build_path) || build_path[ind].safe);
			build_path[ind].safe = true;
		}
		rnd_con = false;
		rnd_dist = RandomGenerator::random(min_step_dist, max_step_dist);
		rnd_ang = RandomGenerator::random(min_ang, max_ang);
		//rnd_ang += toRad(180);
		//if (rnd_ang >= toRad(360.f)) rnd_ang -= toRad(360.f);
		dir = direction(rnd_ang);
		auto shift = dir * ( rnd_dist + safe_dist );
		the_point = build_path[ind].p + shift;

		//check
		if (!isSafeField(ind, min_ang, max_ang, build_path))
		{
			rnd_con = true;
			timeout++;
			continue;
		}

		if (doIntersect(bounding_points[0], bounding_points[1], build_path[ind].p, the_point) ||
			doIntersect(bounding_points[1], bounding_points[2], build_path[ind].p, the_point) ||
			doIntersect(bounding_points[2], bounding_points[3], build_path[ind].p, the_point) ||
			doIntersect(bounding_points[3], bounding_points[0], build_path[ind].p, the_point))
		{
			rnd_con = true;
			timeout++;
			continue;
		}

		for (size_t i = 1; i < build_path.size() - 1; i++)
		{
			if (doIntersect(build_path[i].p, build_path[i - 1].p, build_path[ind].p, the_point))
			{
				rnd_con = true;
				timeout++;
				break;
			}
		}

	}
	the_point = build_path[ind].p + dir * rnd_dist;
	build_path.push_back({ the_point, false });
}


void pf::generate(std::vector < PathStruct >& build_path, bool& exitThread, int& thread_finished)
{
	build_path.clear();

	float tmpx = RandomGenerator::random(0.f, start_box.width) + start_box.left + bounding_box.left;
	float tmpy = RandomGenerator::random(0.f, start_box.height) + start_box.top + bounding_box.top;

	float ox = RandomGenerator::random(min_step_dist, max_step_dist);
	float oy = RandomGenerator::random(min_step_dist, max_step_dist);

	build_path.push_back({ { tmpx, tmpy }, false });
	build_path.push_back({ { tmpx + ox, tmpy + oy }, false });
	for (int i = 0; i < 10; i++)
	{
		proceed(build_path);
	}
	while (!check(build_path))
	{
		if (exitThread) return;
		proceed(build_path);
	}

	thread_finished = 1;
	//track = Spline(build, true);
}

void pf::generateFullTrack()
{
	std::vector < std::vector < PathStruct > > build_paths;
	//std::vector < bool > thread_exit_state;
	bool thread_exit_state = false;
	std::vector < int > thread_state;
	std::vector < std::thread > threads;

	build_paths.resize(max_parallel_threads);
	//thread_exit_state.resize(max_parallel_threads, false);
	thread_state.resize(max_parallel_threads, 0);
	threads.resize(max_parallel_threads);
	for (size_t i = 0; i < max_parallel_threads; i++)
	{
		threads[i] = std::thread(&pf::generate, this, std::ref(build_paths[i]), std::ref(thread_exit_state), std::ref(thread_state[i]));
	}

	start = std::chrono::steady_clock::now();

	int c = 0;
	while (!thread_exit_state)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::chrono::steady_clock::duration elapsed = std::chrono::steady_clock::now() - start;
		auto e = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
		if (e >= timeout_s)
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

	int rng = RandomGenerator::random(0, c - 1);
	while (!thread_state[rng]) rng++;

	std::vector < sf::Vector2f > build;
	for (auto [p, _] : build_paths[rng]) build.push_back(p); 

	track = Spline(build, true);

	finished = true;
}

void pf::stepTrackGeneration()
{
	if (finished) return;
	//proceed();
	//check();

	std::vector < sf::Vector2f > build;
	for (auto [p, _] : build_path)
	{
		build.push_back(p);
	}

	track = Spline(build, true);
}