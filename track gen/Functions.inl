#pragma once

#define _USE_MATH_DEFINES
#define sq(x) (x)*(x)

#include <SFML/System.hpp>

#include <math.h>

template < class T1, class T2, class T3 >
struct Tri
{
	T1 item1;
	T2 item2;
	T3 item3;
};

template < class T1, class T2, class T3, class T4 >
struct Quad
{
	//Quad(Quad&& q) : item1(q.item1), item2(q.item2), item3(q.item3), item4(q.item4) { };
	//Quad(T1 i1, T2 i2, T3 i3, T4 i4) : item1(i1), item2(i2), item3(i3), item4(i4) { };

	T1 item1;
	T2 item2;
	T3 item3;
	T4 item4;
};

template <>
struct Quad < sf::Vector2f&, float, float, float >;

	template < class T >
	inline float lenght(sf::Vector2<T> vec)
	{
		return sqrtf(float(sq((vec.x)) + sq(vec.y)));
	}

	template < class T >
	inline float distance(sf::Vector2<T> vec1, sf::Vector2<T> vec2)
	{
		auto vec = vec1 - vec2;
		return sqrtf(float(sq(vec.x) + sq(vec.y)));
	}

	template < class T >
	inline float dot(sf::Vector2<T> vec1, sf::Vector2<T> vec2)
	{
		return float(vec1.x * vec2.x + vec1.y * vec2.y);
	}

	template < class T >
	inline float angle(sf::Vector2<T> vec1, sf::Vector2<T> vec2)
	{
		auto x = std::clamp(float(dot(vec1, vec2)) / (lenght(vec1) * lenght(vec2)), -1.f, 1.f);
		return acosf(x);
	}

	inline float toRad(float angle)
	{
		return angle * 0.01745329251994329576923690768489f;// / 180.f * M_PI;
	}

	template < class T >
	sf::Vector2<T> castOnVec(sf::Vector2<T> point1, sf::Vector2<T> point2, sf::Vector2<T> point)
	{
		auto p1 = sf::Vector2f(point1);
		auto p2 = sf::Vector2f(point2);
		auto p = sf::Vector2f(point);

		float y1 = p1.y + (p2.y - p1.y) * ((p.x - p1.x) / (p2.x - p1.x));
		float x1 = p.x;

		float y2 = p.y;
		float x2 = p1.x + (p2.x - p1.x) * ((p.y - p.y) / (p2.y - p1.y));

		T x = T((x1 + x2) / 2.f);
		T y = T((y1 + y2) / 2.f);

		x = point1.x < point2.x ? std::clamp(x, point1.x, point2.x) : std::clamp(x, point2.x, point1.x);
		y = point1.y < point2.y ? std::clamp(y, point1.y, point2.y) : std::clamp(y, point2.y, point1.y);

		return { x, y };
	}

	template < class T >
	inline float slope(sf::Vector2<T> a, sf::Vector2<T> b)
	{
		if (b.x - a.x == 0) return 1.f;
		return float(b.y - a.y) / float(b.x - a.x);
	}

	template < class T >
	bool colinear(sf::Vector2<T> a, sf::Vector2<T> b, sf::Vector2<T> c)
	{
		auto slope_ab = slope(a, b);
		auto slope_bc = slope(b, c);
		return slope_ab == slope_bc;
	}

	template < class T >
	bool colinear(sf::Vector2<T> a, sf::Vector2<T> b, sf::Vector2<T> c, float error)
	{
		auto slope_ab = slope(a, b);
		auto slope_bc = slope(b, c);
		return abs(slope_ab - slope_bc) <= error;
	}

	template < class T >
	sf::Vector2f direction(sf::Vector2<T> vec)
	{
		return vec / lenght(vec);
	}

	template < class T >
	bool overlap(sf::Vector2<T> a, sf::Vector2<T> b, float error)
	{
		return distance(a, b) <= error;
	}
//}