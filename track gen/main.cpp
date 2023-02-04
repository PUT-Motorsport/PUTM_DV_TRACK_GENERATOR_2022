#define _USE_MATH_DEFINES

#include <iostream>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

#include "Config.h"
#include "Spline.h"
#include "Cone.h"
#include "SplineGenerators.h"
#include "Functions.inl"
#include "XMLConeFileWriter.h"
#include "CSVConeFileWriter.h"
#include "CSVTrackFileWriter.h"
#include "UniformConeGenerator.h"

void mouseMovedAndRightClicked(sf::Event& const event, sf::RenderWindow& const window, sf::Vector2f prev_mouse_pos, sf::Vector2f mouse_pos);
void mouseMovedAndLeftClicked(sf::Event& const event, sf::RenderWindow& const window, sf::Vector2f prev_mouse_pos, sf::Vector2f mouse_pos, Spline& spline);
void mouseWheelScrolled(sf::Event& const event, sf::RenderWindow& const window);

void keyPressedManageSplineOptions(sf::Event& const event, Spline& spline);

sf::Vector2f genCircle(sf::Vector2f vec);
sf::Vector2f genRectangle(sf::Vector2f vec);

bool measure = false;

float total_zoom = 1.f;

const float circleR = 50.f;

sf::Vector2f meas_pt1 = { 0.f, 0.f };
sf::Vector2f meas_pt2 = { 0.f, 0.f };
sf::Vector2f angle_pts[3] = { { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f } };

ISplineGenerator* gen = new PathFindingSplineGenerator();//ShapeCollapseSplineGenerator(genCircle);//genRectangle genCircle

IConeGenerator* cone_gen = nullptr;// = new UniformConeGenerator;

sf::RenderWindow* w;

int main()
{
    int draw_for_cones = -1;

    Config::initConfig();

    sf::RenderWindow window(sf::VideoMode(1280, 960), "Track Cones Generator");
    window.setFramerateLimit(60);
    w = &window;

    std::vector < sf::Vector2f > spline_points;
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

                if (cone_gen != nullptr)
                {
                    auto view_pos = window.mapPixelToCoords(sf::Vector2i(mouse_pos));
                    auto cones = cone_gen->getCones();

                    for (int i = 0; i < cones.size(); i++)
                    {
                        if (distance(view_pos, cones[i].pos) < 1.f && cones[i].type == Type::Ctrl1)
                        {
                            draw_for_cones = i;
                            break;
                        }
                        else draw_for_cones = -1;
                    }
                }
                if (measure)
                {
                    bool found_cone = false;
                    if (cone_gen != nullptr)
                    {
                        auto cones = cone_gen->getCones();
                        for (auto cone : cones)
                        {
                            if (distance(cone.pos, w->mapPixelToCoords(sf::Mouse::getPosition(*w))) < 1.f)
                            {
                                meas_pt2 = cone.pos;
                                found_cone = true;
                                break;
                            }
                        }
                    }
                }

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
                else if(sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                {
                    auto buff_pivot = spline.getPivotPoints();
                    size_t index1 = 0;
                    size_t index2 = 0;
                    auto dist1 = distance(mouse_pos, buff_pivot[0]);
                    auto dist2 = distance(mouse_pos, buff_pivot[0]);

                    for (size_t i = 1; i < buff_pivot.size(); i++)
                    {
                        auto buff_vec = buff_pivot[i];
                        auto buff_dist = distance(mouse_pos, buff_vec);
                        if (buff_dist < dist1)
                        {
                            index1 = i;
                            dist1 = buff_dist;
                        }
                    }
                    for (size_t i = 1; i < buff_pivot.size(); i++)
                    {
                        auto buff_vec = buff_pivot[i];
                        auto buff_dist = distance(mouse_pos, buff_vec);
                        if (buff_dist < dist2 && i != index1)
                        {
                            index2 = i;
                            dist2 = buff_dist;
                        }
                    }
                    if (index1 < index2) spline.insertPivotPoint(index1 + 1, mouse_pos);
                    else spline.insertPivotPoint(index2 + 1, mouse_pos);
                }
            }
            keyPressedManageSplineOptions(event, spline);
        }

        window.clear();
        window.draw(spline);

        if (cone_gen != nullptr)
        {
            auto cones = cone_gen->getCones();

            for (auto cone : cones)
            {
                sf::CircleShape cir;
                cir.setRadius(0.25f);
                cir.setOrigin(0.25f, 0.25f);
                if (cone.type == Type::Right) cir.setFillColor(sf::Color::Color(0xff, 0xa5, 0x00));
                else if(cone.type == Type::Ctrl1) cir.setFillColor(sf::Color::Color(220, 20, 60));
                //else if(cone.type == Type::Ctrl2) cir.setFillColor(sf::Color::Color(219, 112, 147));
                else cir.setFillColor(sf::Color::Color(0x89, 0xcf, 0xf0));
                cir.setPosition(cone.pos);
                window.draw(cir);
            }
        }
        if (draw_for_cones != -1)
        {
            auto cones = cone_gen->getCones();
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(cones[draw_for_cones].pos)),
                sf::Vertex(sf::Vector2f(cones[draw_for_cones + 1].pos))
            };

            window.draw(line, 2, sf::Lines);

            line[0] = sf::Vertex(sf::Vector2f(cones[draw_for_cones].pos));
            line[1] = sf::Vertex(sf::Vector2f(cones[draw_for_cones + 2].pos));

            window.draw(line, 2, sf::Lines);
        }
        if (true)//(measure)
        {
            sf::Vertex line[] =
            {
                sf::Vertex(meas_pt1),
                sf::Vertex(meas_pt2)
            };
            window.draw(line, 2, sf::Lines);
            system("cls");
            std::cout << distance(meas_pt1, meas_pt2) << "\n";
            sf::Vector2f veca = angle_pts[1] - angle_pts[0];
            sf::Vector2f vecb = angle_pts[1] - angle_pts[2];
            float a = toDeg(angle2(veca, vecb));

            std::cout << a;
        }

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

    auto key = sf::Keyboard::F1;

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

    key = sf::Keyboard::F7;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F7) && !pressed[sf::Keyboard::F7])
    {
        pressed[key] = true;

        if (cone_gen != nullptr)
        {
            CSVConeFileWriter file_writer;

            file_writer.open(Config::get("ConeCSVFileDestination"));
            file_writer.writeMultiple(cone_gen->getCones());
            file_writer.close();
        }
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F7) && pressed[sf::Keyboard::F7]) pressed[sf::Keyboard::F7] = false;

    key = sf::Keyboard::F8;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F8) && !pressed[sf::Keyboard::F8])
    {
        pressed[key] = true;
        CSVTrackFileWriter file_writer;

        file_writer.open(Config::get("TrackCSVFileDestination"));
        file_writer.write(spline);
        file_writer.close();
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F8) && pressed[sf::Keyboard::F8]) pressed[sf::Keyboard::F8] = false;

    key = sf::Keyboard::F9;
    if (sf::Keyboard::isKeyPressed(key) && !pressed[key])
    {
        pressed[key] = true;
        if (cone_gen != nullptr) delete cone_gen;

        cone_gen = new UniformConeGenerator(spline);
        cone_gen->generateAllCones();
    }
    else if (!sf::Keyboard::isKeyPressed(key) && pressed[key]) pressed[key] = false;

    key = sf::Keyboard::F10;
    if (sf::Keyboard::isKeyPressed(key) && !pressed[key])
    {
        pressed[key] = true;
        gen->stepTrackGeneration();
        spline = gen->getTrack();
    }
    else if (!sf::Keyboard::isKeyPressed(key) && pressed[key]) pressed[key] = false;

    key = sf::Keyboard::M;
    if (sf::Keyboard::isKeyPressed(key) && !pressed[key])
    {
        pressed[key] = true;

        measure = !measure;
        if (measure)
        {
            bool is_any_pt_in_range = false;

            if (cone_gen != nullptr)
            {
                auto cones = cone_gen->getCones();
                for (auto cone : cones)
                {
                    if (distance(cone.pos, w->mapPixelToCoords(sf::Mouse::getPosition(*w))) < 1.f)
                    {
                        meas_pt1 = cone.pos;
                        is_any_pt_in_range = true;
                        break;
                    }
                }
            }
            if (!is_any_pt_in_range)
            {
                meas_pt1 = w->mapPixelToCoords(sf::Mouse::getPosition(*w));
            }
        }
        else
        {
            meas_pt1 = { 0.f, 0.f };
            meas_pt2 = { 0.f, 0.f };
        }
    }
    else if (!sf::Keyboard::isKeyPressed(key) && pressed[key]) pressed[key] = false;

    key = sf::Keyboard::A;
    if (sf::Keyboard::isKeyPressed(key) && !pressed[key])
    {
        pressed[key] = true;

        auto pts = spline.getPointRepresenation();
        auto cords = w->mapPixelToCoords(sf::Mouse::getPosition(*w));
        for (auto pt : pts)
        {
            if (distance(pt.position, cords) < 5.f)
            {
                angle_pts[0] = pt.position;
            }
        }
    }
    else if (!sf::Keyboard::isKeyPressed(key) && pressed[key]) pressed[key] = false;
    key = sf::Keyboard::S;
    if (sf::Keyboard::isKeyPressed(key) && !pressed[key])
    {
        pressed[key] = true;

        auto pts = spline.getPointRepresenation();
        auto cords = w->mapPixelToCoords(sf::Mouse::getPosition(*w));
        for (auto pt : pts)
        {
            if (distance(pt.position, cords) < 5.f)
            {
                angle_pts[1] = pt.position;
            }
        }
    }
    else if (!sf::Keyboard::isKeyPressed(key) && pressed[key]) pressed[key] = false;
    key = sf::Keyboard::D;
    if (sf::Keyboard::isKeyPressed(key) && !pressed[key])
    {
        pressed[key] = true;

        auto pts = spline.getPointRepresenation();
        auto cords = w->mapPixelToCoords(sf::Mouse::getPosition(*w));
        for (auto pt : pts)
        {
            if (distance(pt.position, cords) < 5.f)
            {
                angle_pts[2] = pt.position;
            }
        }
    }
    else if (!sf::Keyboard::isKeyPressed(key) && pressed[key]) pressed[key] = false;
}

sf::Vector2f genCircle(sf::Vector2f vec)
{
    return vec * circleR;
}

sf::Vector2f genRectangle(sf::Vector2f vec)
{
    const float x_size = 100.f;
    const float y_size = 50.f;

    auto len = lenght(sf::Vector2f( x_size, y_size ));
    float _x = x_size * 0.5f / len;
    float _y = y_size * 0.5f / len;

    auto x = std::clamp(vec.x, -_x, _x) * len * 2;
    auto y = std::clamp(vec.y, -_y, _y) * len * 2;
    return { x, y };
}