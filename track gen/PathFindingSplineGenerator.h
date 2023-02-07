#pragma once

#include "ISplineGenerator.h"

#include <array>
#include <chrono>

class PathFindingSplineGenerator : public ISplineGenerator
{
	public:
		struct PathStruct
		{
			sf::Vector2f p;
			bool safe = false;
		};

		PathFindingSplineGenerator();
		void generateFullTrack() override;
		void stepTrackGeneration() override;

		void generate(std::vector < PathStruct >& build_path, bool& exitThread, int& thread_inished);

		//void proceed();
		void proceed(std::vector < PathStruct >& build_path);
		//void check();
		bool check(std::vector < PathStruct >& build_path);
		void filter(std::vector < PathStruct >& build_path);

		bool isSafeField(size_t ind, float min_ang, float max_angle, std::vector < PathStruct >& build_path);

		sf::FloatRect bounding_box;
		sf::FloatRect start_box;

		std::vector < PathStruct >  build_path;

		std::array < sf::Vector2f, 4 > bounding_points;

		std::chrono::steady_clock::time_point start;

		float max_step_dist;
		float min_step_dist;
		float safe_dist;
		float angle_shift;
		float angle_change_speed_ratio;
		float angle_step;
		float safe_field_ratio;
		float min_ratio_of_threads_completed;

		int timeout_max;
		int thread_timeout;
		int max_parallel_threads;

		bool finished;
};