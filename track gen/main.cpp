#define _USE_MATH_DEFINES

#include <iostream>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

#include "Config.h"
#include "Spline.h"
#include "SplineGenerators.h"
#include "Functions.inl"

void mouseMovedAndRightClicked(sf::Event& const event, sf::RenderWindow& const window, sf::Vector2f prev_mouse_pos, sf::Vector2f mouse_pos);
void mouseMovedAndLeftClicked(sf::Event& const event, sf::RenderWindow& const window, sf::Vector2f prev_mouse_pos, sf::Vector2f mouse_pos, Spline& spline);
void mouseWheelScrolled(sf::Event& const event, sf::RenderWindow& const window);

void keyPressedManageSplineOptions(sf::Event& const event, Spline& spline);

float total_zoom = 1.f;

sf::Vector2f genCircle(sf::Vector2f vec);
sf::Vector2f genRectangle(sf::Vector2f vec);

const float circleR = 300.f;

ISplineGenerator* gen = new ShapeCollapseSplineGenerator(genRectangle);//genRectangle genCircle

int main()
{
    Config::initConfig();

    sf::RenderWindow window(sf::VideoMode(1280, 960), "Track Cones Generator");
    window.setFramerateLimit(60);

    std::vector < sf::Vector2f > spline_points;

    //int how_many = 15;
    //float r = 300.f;
    //sf::Vector2f start_vec = { 1280 / 2, 960 / 2 };
    //float angle = 1 / float(how_many) * M_PI * 2.f;
    //for (int i = 0; i < how_many; i++)
    //{
    //    spline_points.push_back({ sin(angle * i) * r + start_vec.x, cos(angle * i) * r + start_vec.y });
    //}

    gen->generateFullTrack();

    Spline spline = gen->getTrack();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseMoved)
            {
                static sf::Vector2f prev_mouse_pos = { 0, 0 };
                static sf::Vector2f mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window));
                mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window));

                mouseMovedAndRightClicked(event, window, prev_mouse_pos, mouse_pos);
                mouseMovedAndLeftClicked(event, window, prev_mouse_pos, mouse_pos, spline);

                prev_mouse_pos = mouse_pos;
            }
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                mouseWheelScrolled(event, window);
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                auto mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                auto index = spline.mouseEnteredPivotPoint(mouse_pos);
                if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) && index != -1)
                {
                    spline.removePivotPoint(index);
                }
            }
            keyPressedManageSplineOptions(event, spline);
        }

        window.clear();
        window.draw(spline);
        window.display();
    }

    return 0;
}

void mouseMovedAndRightClicked(sf::Event& const event, sf::RenderWindow& const window, sf::Vector2f prev_mouse_pos, sf::Vector2f mouse_pos)
{

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        auto move = prev_mouse_pos - mouse_pos;
        auto view = window.getView();
        move *= total_zoom;
        view.move(move);
        window.setView(view);
    }
}

void mouseMovedAndLeftClicked(sf::Event& const event, sf::RenderWindow& const window, sf::Vector2f prev_mouse_pos, sf::Vector2f mouse_pos, Spline& spline)
{
    static int bind_point = -1;

    auto view_pos = window.mapPixelToCoords(sf::Vector2i(mouse_pos));
    int point = spline.mouseEnteredPivotPoint(view_pos, true);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && bind_point == -1) bind_point = point;
    else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) bind_point = -1;
    if (bind_point > -1)
    {
        spline.setPivotPointPosition(bind_point, view_pos);
        spline.forceHighlight(bind_point);
    }
}

void mouseWheelScrolled(sf::Event& const event, sf::RenderWindow& const window)
{
    static float zoom = 1.f;
    auto view = window.getView();
    auto delta = std::clamp(float(event.mouseWheelScroll.delta), -9.999999f, 9.999999f);
    zoom = delta * 0.1 + 1.f;
    total_zoom *= zoom;
    view.zoom(zoom);
    window.setView(view);
}

void keyPressedManageSplineOptions(sf::Event& const event, Spline& spline)
{
    static bool pressed[sf::Keyboard::KeyCount] = { false };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1) && !pressed[sf::Keyboard::F1])
    {
        spline.draw_outline_points = !spline.draw_outline_points;
        pressed[sf::Keyboard::F1] = true;
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F1) && pressed[sf::Keyboard::F1]) pressed[sf::Keyboard::F1] = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2) && !pressed[sf::Keyboard::F2])
    {
        spline.draw_pivot_points = !spline.draw_pivot_points;
        pressed[sf::Keyboard::F2] = true;
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F2) && pressed[sf::Keyboard::F2]) pressed[sf::Keyboard::F2] = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3) && !pressed[sf::Keyboard::F3])
    {
        spline.draw_segmentation_points = !spline.draw_segmentation_points;
        pressed[sf::Keyboard::F3] = true;
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F3) && pressed[sf::Keyboard::F3]) pressed[sf::Keyboard::F3] = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4) && !pressed[sf::Keyboard::F4])
    {
        spline.highlight_pivot_points = !spline.highlight_pivot_points;
        pressed[sf::Keyboard::F4] = true;
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F4) && pressed[sf::Keyboard::F4]) pressed[sf::Keyboard::F4] = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5) && !pressed[sf::Keyboard::F5])
    {
        gen->generateFullTrack();
        spline = gen->getTrack();
        pressed[sf::Keyboard::F5] = true;
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F5) && pressed[sf::Keyboard::F5]) pressed[sf::Keyboard::F5] = false;
}

sf::Vector2f genCircle(sf::Vector2f vec)
{
    return vec * circleR;
}

sf::Vector2f genRectangle(sf::Vector2f vec)
{
    const float x_size = 200.f;
    const float y_size = 100.f;

    auto len = lenght(sf::Vector2f( x_size, y_size ));
    float _x = x_size * 0.5f / len;
    float _y = y_size * 0.5f / len;
    //float r = 1.f;

    auto x = std::clamp(vec.x, -_x, _x) * len * 2;
    auto y = std::clamp(vec.y, -_y, _y) * len * 2;
    return { x, y };
}