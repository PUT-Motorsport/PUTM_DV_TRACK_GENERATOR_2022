#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class IScreen
{
	public:
		IScreen(sf::RenderTarget* target, IScreen&& container);
		IScreen(sf::RenderTarget* target, IScreen*& container);
};