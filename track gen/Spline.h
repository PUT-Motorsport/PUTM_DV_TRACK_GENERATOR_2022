#pragma once

#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

#include "Config.h"

enum class AproximationAlgorithm
{
	UnDefined,
	LineSplit,
	QubedVectorLenght
};

enum class SegmentingAlgorithm
{
	UnDefined,
	LookAhead,
	Tree,
	Constant
};

class Spline : public sf::Drawable//, public sf::Tra
{
	public:
		Spline();
		Spline(int spline_resolution);
		Spline(std::vector < sf::Vector2f > pivot_points);
		Spline(std::vector < sf::Vector2f > pivot_points, bool looped);
		Spline(std::vector < sf::Vector2f > pivot_points, int spline_resolution);

		bool draw_segmentation_points = false;
		bool draw_outline_points = false;
		bool draw_pivot_points = true;
		bool highlight_pivot_points = true;

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void setWidth(float width);

		void setAproximationAlgorithm(AproximationAlgorithm algorithm);
		void setSegmentingAlgorithm(SegmentingAlgorithm algorithm);

		void setPivotPointPosition(size_t index, sf::Vector2f position);
		void movePivotPoint(size_t index, sf::Vector2f move);
		void removePivotPoint(size_t index);
		void removePivotPoint(size_t first, size_t last);
		void forceHighlight(size_t index);

		void optimize();

		int mouseEnteredPivotPoint(sf::Vector2f mouse_pos);
		int mouseEnteredPivotPoint(sf::Vector2f mouse_pos, bool highlight);

		float getGradient(size_t index);

		size_t getPivotPointsCount();

		sf::Vector2f getPivotPoint(size_t index);

		sf::Vector2f& operator [] (size_t index);

		std::vector < sf::Vector2f >::iterator begin();
		//std::vector < sf::Vector2f >::iterator back();
		std::vector < sf::Vector2f >::iterator end();

		//float lenght();

	private:	
		struct SplinePoint
		{
			sf::Vector2f position;
			sf::Vector2f gradient_vector;
			float gradient;
		};

		//##############################################################################################
		//Spline::* 
		void(Spline::*segmenting_algorithm)(std::array < sf::Vector2f, 4 >) const;
		float(Spline::*aproximaion_algorithm)(std::array < sf::Vector2f, 4 >) const;

		bool looped = false;

		mutable bool changed = true;

		uint32_t resolution;

		int highlight_pivot_point = -1;

		float max_gradient;
		float optimization_error;
		float width = 10.f;

		std::vector < sf::Vector2f > pivot_points;

		mutable std::vector < SplinePoint > spline_point_representation;

		mutable sf::VertexArray spline;

		//##############################################################################################

		void init();
		void treeSegmenting(std::array < sf::Vector2f, 4 > segment) const;
		void lookAheadSegmenting(std::array < sf::Vector2f, 4 > segment) const;
		void constantSegementing(std::array < sf::Vector2f, 4 > segment) const;

		template < typename T >
		T toEnum(std::string name);
		template <>
		AproximationAlgorithm toEnum(std::string name);
		template <>
		SegmentingAlgorithm toEnum(std::string name);

		int aproximateSegmentation(std::array < sf::Vector2f, 4 > segment) const;

		float lineSplitLenght(std::array < sf::Vector2f, 4 > segment) const;
		float vectorLenghtQubedLenght(std::array < sf::Vector2f, 4 > segment) const;

		sf::Vector2f getSplinePoint(std::array < sf::Vector2f, 4 > segment, float t) const;
		sf::Vector2f getSplineGradient(std::array < sf::Vector2f, 4 > segment, float t) const;
};