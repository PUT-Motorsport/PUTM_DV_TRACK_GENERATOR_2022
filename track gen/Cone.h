#pragma once

#include <SFML/System.hpp>

enum class Type
{
	Right,
	Left
};

struct Cone
{
	//explicit Cone(Type type) : type(type) { }

	const Type type;
	
	sf::Vector2f pos;
};

struct ConePair
{
	Cone right;// (Type::Right);
	Cone left;// (Type::Left);
};