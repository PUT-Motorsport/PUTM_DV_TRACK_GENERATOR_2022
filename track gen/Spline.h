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
	size_t parent;
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

		void erase(std::vector < sf::Vector2f >::iterator where);
		void push_back(sf::Vector2f& val);
		void erase_last();

		bool segmentsCollide(size_t index1, size_t index2);

		int mouseEnteredPivotPoint(sf::Vector2f mouse_pos);
		int mouseEnteredPivotPoint(sf::Vector2f mouse_pos, bool highlight);

		float getGradient(size_t index) const;
		float getGradient(size_t index, float t) const;
		float getInflexion(size_t index, float t) const;
		float getLenght();
		float solveSegmentsClosestPoint(size_t index, sf::Vector2f point);

		size_t getPivotPointsCount();
		size_t getPointsCount();
		size_t getT() const;
		size_t size() const;

		sf::Vector2f getPivotPoint(size_t index) const;
		sf::Vector2f getPoint(size_t index) const;
		sf::Vector2f getPoint(size_t index, float t) const;
		sf::Vector2f getGradientVector(size_t index) const;
		sf::Vector2f getGradientVector(size_t index, float t) const;
		sf::Vector2f getInflexionVector(size_t index, float t) const;
		sf::Vector2f getCenter();

		sf::Vector2f& operator [] (size_t index);
		sf::Vector2f& front();
		sf::Vector2f& back();

		std::pair < float, float > solveGradientForIndex(size_t index);

		sf::FloatRect getSegmentBoundingBox(size_t index);

		std::vector < sf::Vector2f >::iterator begin();
		std::vector < sf::Vector2f >::iterator end();

		std::vector < sf::Vector2f > getPivotPoints();
		std::vector < sf::Vector2f > getSegmentCollisionBox(size_t index);

		std::vector < SplinePoint > getPointRepresenation();

		std::array < sf::Vector2f, 4 > getSegment(size_t t) const;

	//private:	

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
		float width;
		float bd_box_offset;

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
};