#pragma once

#include <SFML/System.hpp>
#include <string>

enum class Type
{
	Right,
	Left,
	Ctrl1,
	Ctrl2,
};

struct Cone
{
	Cone();
	//explicit Cone(Type type) : type(type) { }
	Cone(Type type, sf::Vector2f pos) : type(type), pos(pos) { };

	Type type;
	
	sf::Vector2f pos;
};

struct ConePair
{
	Cone right;// (Type::Right);
	Cone left;// (Type::Left);
};