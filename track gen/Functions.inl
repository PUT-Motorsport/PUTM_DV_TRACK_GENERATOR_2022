#pragma once

#define _USE_MATH_DEFINES
#define sq(x) (x)*(x)

#include <SFML/System.hpp>

#include <math.h>

template < class T >
inline T lenght(sf::Vector2<T> vec)
{
	return sqrt(float(sq((vec.x)) + sq(vec.y)));
}

template <>
inline long double lenght(sf::Vector2<long double> vec)
{
	return sqrtl(sq((vec.x)) + sq(vec.y));
}

template < class T >
inline T distance(sf::Vector2<T> vec1, sf::Vector2<T> vec2)
{
	auto vec = vec1 - vec2;
	return sqrt(float(sq(vec.x) + sq(vec.y)));
}

template <>
inline long double distance(sf::Vector2<long double> vec1, sf::Vector2<long double> vec2)
{
	auto vec = vec1 - vec2;
	return sqrtl(sq(vec.x) + sq(vec.y));
}

template < class T >
inline T dot(sf::Vector2<T> vec1, sf::Vector2<T> vec2)
{
	return float(vec1.x * vec2.x + vec1.y * vec2.y);
}

template < class T >
inline T dot(sf::Vector2<T> a, sf::Vector2<T> b, sf::Vector2<T> p)
{
	return float((p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x));
}

template < class T >
inline T angle(sf::Vector2<T> vec1, sf::Vector2<T> vec2)
{
	auto x = std::clamp(float(dot(vec1, vec2)) / (lenght(vec1) * lenght(vec2)), -1.f, 1.f);
	return acosf(x);
}

template <>
inline long double angle(sf::Vector2<long double> vec1, sf::Vector2<long double> vec2)
{
	long double x = dot(vec1, vec2) / (lenght(vec1) * lenght(vec2));
	if (x > 1.l) x = 1.l;
	else if (x < -1.l) x = -1.l;
	return acosl(x);
}

inline constexpr float toRad(float angle)
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

inline sf::Vector2f findThirdPoint(sf::Vector2f pos1, float grad1, sf::Vector2f pos2, float grad2)
{
	const auto min_angle = 2.f;
	//if (std::abs(grad1 - grad2) > toRad(360.f - min_angle) || std::abs(grad1 - grad2) < toRad(min_angle)) return { { 0.f, 0.f }, false };

	auto x1 = pos1.x;//0.f;
	auto y1 = pos1.y;//0.f;
	auto x2 = pos2.x;//0.f;
	auto y2 = pos2.y;//0.f;
	auto alp1 = grad1;//2.f * M_PI / 3.f;
	auto alp2 = grad2;//M_PI / 6.f;
	auto u = x2 - x1;
	auto v = y2 - y1;
	auto a3 = sqrt(u * u + v * v);
	auto alp3 = M_PI - alp1 - alp2;
	auto a2 = a3 * sin(alp2) / sin(alp3);
	auto rhs1 = x1 * u + y1 * v + a2 * a3 * cos(alp1);
	auto rhs2 = y2 * u - x2 * v - a2 * a3 * sin(alp1);
	auto x3 = (1.f / (a3 * a3)) * (u * rhs1 - v * rhs2);
	auto y3 = (1.f / (a3 * a3)) * (v * rhs1 + u * rhs2);

	return { float(x3), float(y3) };
}

//template < class T >
inline int sgnf(float x)
{
	if (x > 0.f) return 1;
	if (x < 0.f) return -1;
	return 0;
}