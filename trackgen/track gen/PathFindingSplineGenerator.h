#pragma once

#include "ISplineGenerator.h"

#include <array>
#include <chrono>

class PathFindingSplineGenerator : public ISplineGenerator
{
	public:
		PathFindingSplineGenerator();
		void generateFullTrack() override;
		void stepTrackGeneration() override;

		void generate(Spline& build_path, bool& exitThread, int& thread_inished);

		//void proceed();
		//void proceed(std::vector < sf::Vector2f >& build_path);
		//void check();
		//bool check(std::vector < sf::Vector2f >& build_path);
		//void filter(std::vector < sf::Vector2f >& build_path);

		bool isSafeField(Spline& build_path, sf::Vector2f & const pt);

		sf::FloatRect bounding_box;
		sf::FloatRect start_box;

		std::vector < sf::FloatRect > obstacles;

		std::vector < sf::Vector2f >  build_path;

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
		float filter_coef;
		float x_min_ang;
		float x_max_ang;
		float max_angle;

		int timeout_max;
		int thread_timeout;
		int max_parallel_threads;
		int max_num_of_x;

		bool finished;
		bool allow_x;
		bool do_filter;
};