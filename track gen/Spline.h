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

struct SplinePoint
{
	sf::Vector2f position;
	sf::Vector2f gradient_vector;
	float gradient;
};

class Spline : public sf::Drawable
{
	public:
		Spline();
		//Spline(int spline_resolution);
		Spline(std::vector < sf::Vector2f > pivot_points);
		Spline(std::vector < sf::Vector2f > pivot_points, bool looped);
		Spline(std::vector < sf::Vector2f > pivot_points, bool looped, bool segment);
		//Spline(std::vector < sf::Vector2f > pivot_points, int spline_resolution);

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
		void insertPivotPoint(size_t at, sf::Vector2f new_pos);
		void forceHighlight(size_t index);

		void optimize();
		void segment() const;

		int mouseEnteredPivotPoint(sf::Vector2f mouse_pos);
		int mouseEnteredPivotPoint(sf::Vector2f mouse_pos, bool highlight);

		float getGradient(size_t index) const;
		float getGradient(size_t index, float t) const;
		float getLenght();

		size_t getPivotPointsCount();
		size_t getPointsCount();
		size_t getT() const;

		sf::Vector2f getPivotPoint(size_t index) const;
		sf::Vector2f getPoint(size_t index) const;
		sf::Vector2f getPoint(size_t index, float t) const;
		sf::Vector2f getGradientVector(size_t index) const;
		sf::Vector2f getGradientVector(size_t index, float t) const;
		sf::Vector2f getCenter();

		sf::Vector2f& operator [] (size_t index);

		std::vector < sf::Vector2f >::iterator begin();
		std::vector < sf::Vector2f >::iterator end();

		std::vector < sf::Vector2f > getPivotPoints();

		std::vector < SplinePoint > getPointRepresenation();

	private:	

		//##############################################################################################
		//std::function<void()> segmentingAlgorithm;
		//std::function<float()> aproximaionAlgorithm;
		void(Spline::* segmentingAlgorithmPtr)() const;
		float(Spline::* aproximaionAlgorithmPtr)() const;

		bool looped = false;

		mutable bool changed = true;

		uint32_t resolution;

		int highlight_pivot_point = -1;

		float max_gradient;
		float optimization_error;
		float width = 3.f;

		std::vector < sf::Vector2f > pivot_points;

		mutable std::vector < SplinePoint > point_representation;

		mutable sf::VertexArray spline;

		//##############################################################################################

		void init();
		void treeSegmenting() const;
		void lookAheadSegmenting() const;
		void constantSegementing() const;

		void segmentingAlgorithm() const { (this->*segmentingAlgorithmPtr)(); }

		float vectorLenghtSum() const;

		float aproximaionAlgorithm() const { return (this->*aproximaionAlgorithmPtr)(); }

		std::array < sf::Vector2f, 4 > getSegment(size_t t) const;
};