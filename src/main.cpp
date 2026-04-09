#include "SFML/Graphics.hpp"

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <print> 
#include <map>

namespace types
{
using loc = sf::Vector2f;
using pos = sf::Vector2f;
}

struct Pos
{
    float x{}, y{};

    auto operator<=>(const Pos&) const = default;

};

class Tile
{
    public:

    Tile()
        : tile{size}
    {   
        setColor(false);

        tile.setOutlineThickness(-1.f);
        tile.setOutlineColor(sf::Color{15, 15, 15});
    }

    void setColor(bool color) noexcept {tile.setFillColor(color ? sf::Color::White : sf::Color{85, 96, 102});}
    void setPos(const types::pos& position) noexcept { tile.setPosition(position); }

    bool isAlive() const noexcept { return tile.getFillColor() == sf::Color::White ? true : false; }

    sf::Vector2f getSize() const {return tile.getSize();}

    const sf::RectangleShape& getTileMesh() const noexcept { return tile; }


    static constexpr sf::Vector2f size{20, 20};

    private:

    sf::RectangleShape tile{};
};

class Chunk
    : public sf::Drawable
{
    public:

    Chunk(const types::pos& loc_)
        : loc{loc_}
    {   
        tiles.resize(g_sizeMax);
        genChunk();
    }


    void update()
    {
        std::vector<std::pair<types::pos, bool>> cells{};

        for (size_t y{}; y < g_size; y++)
        for (size_t x{}; x < g_size; x++)
        {
            types::pos i{(float)x, (float)y};

            std::uint8_t alive_neighbours{};

            auto& tile = tiles[i.x + i.y * g_size];

            if (contains({i.x + 1, i.y}))
                if (tiles[(i.x + 1) + i.y * g_size].isAlive())
                    alive_neighbours++;

            if (contains({i.x - 1, i.y}))
                if (tiles[(i.x - 1) + i.y * g_size].isAlive())
                    alive_neighbours++;

            if (contains({i.x, i.y + 1}))
                if (tiles[i.x + (i.y + 1) * g_size].isAlive())
                    alive_neighbours++;

            if (contains({i.x, i.y - 1}))
                if (tiles[i.x + (i.y - 1) * g_size].isAlive())
                    alive_neighbours++;


            if (contains({i.x + 1, i.y + 1}))
                if (tiles[(i.x + 1) + (i.y + 1) * g_size].isAlive())
                    alive_neighbours++;

            if (contains({i.x + 1, i.y - 1}))
                if (tiles[(i.x + 1) + (i.y - 1) * g_size].isAlive())
                    alive_neighbours++;

            if (contains({i.x - 1, i.y + 1}))
                if (tiles[(i.x - 1) + (i.y + 1) * g_size].isAlive())
                    alive_neighbours++;

            if (contains({i.x - 1, i.y - 1}))
                if (tiles[(i.x - 1) + (i.y - 1) * g_size].isAlive())
                    alive_neighbours++;


            if (alive_neighbours < 2 || alive_neighbours > 3)
                cells.push_back(std::pair<types::pos, bool>{i, false});

            if (alive_neighbours == 3)
                cells.push_back(std::pair<types::pos, bool>{i, true});
        }

        for(const auto& i : cells)
            tiles[i.first.x + i.first.y * g_size].setColor(i.second);

        genCount++;
    }

    void clear() 
    {
        for (auto& i : tiles)
            i.setColor(false);
    }

    bool contains(const types::pos& index)
    {
        if (index.x >= g_size || index.x < 0 ||
            index.y >= g_size || index.y < 0)
            return false;

        return true;
    }


    void genChunk()
    {
        for (size_t y{}; y < g_size; y++)
        for (size_t x{}; x < g_size; x++)
        {
            auto& tile = tiles[x + y * g_size];
            
            tile.setPos(loc + types::pos{(float)x * tile.getSize().x, (float)y * tile.getSize().y});
        }
    }

    void changeTileAt(const types::pos& raw_pos, bool color)
    {
        types::pos pos{raw_pos};
        pos.x /= Tile::size.x;
        pos.y /= Tile::size.y;
        pos = { std::floor(pos.x), std::floor(pos.y)};

        tiles[pos.x + pos.y * g_size].setColor(color);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
       for (const auto& i : tiles)
            target.draw(i.getTileMesh(), states);
    }   

    public:


    std::size_t g_size{150};
    std::size_t g_sizeMax{g_size * g_size};

    std::uint64_t genCount{};

    private:

    std::vector<Tile> tiles{};
    
    types::loc loc{};

    

};

/*
class Grid
    : public sf::Drawable
{
    public:


    void update(const types::pos& viewcenter)
    {
        static types::pos posTemp{};
        types::pos newPos{ viewcenter / (float)Chunk::g_size / (float)Tile::size.y};

        newPos = {std::floor(newPos.x), std::floor(newPos.y)};


        for (int y{(int)newPos.y - 3}; y < newPos.y + 3; y++)
        for (int x{(int)newPos.x - 3}; x < newPos.x + 3; x++)
        {
            chunks.emplace(std::pair<Pos, Chunk>{{(float)x, (float)y}, Chunk{{(float)x * Chunk::g_size * Tile::size.x, (float)y * Chunk::g_size * Tile::size.y}}});
        }



        std::vector<Pos> tobediscarded{};
        for (const auto& i : chunks)
            if (
                i.first.x < newPos.x - 3 || i.first.x > newPos.x + 3 ||
                i.first.y < newPos.y - 3 || i.first.y > newPos.y + 3
            )
            tobediscarded.push_back(i.first);


        for (const auto& i : tobediscarded)
            chunks.erase(i);
    }

    void changeTileAt(const types::pos& mousePos, bool color)
    {
        types::pos newPos{ mousePos / (float)Chunk::g_size / (float)Tile::size.y};
        newPos = {std::floor(newPos.x), std::floor(newPos.y)};
        Pos pos{newPos.x, newPos.y};

        if (chunks.contains(pos))
            chunks.at(pos).changeTileAt(mousePos, color);
    }


    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
       for (const auto& i : chunks)
            target.draw(i.second, states);
    }   


    private:

    std::map<Pos, Chunk> chunks{};

};*/


int main(void)
{
    sf::RenderWindow window{sf::VideoMode{{500, 500}}, "test"};

    Chunk chunk{{0,0}};


    sf::View view{{0, 0}, {500, 500}};

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
		{
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                using S = sf::Keyboard::Scancode;
                auto key = [&](sf::Keyboard::Scancode x)
                {
                    return keyPressed->scancode == x;
                };

                if(key(S::Escape))
                    window.close();

                if(key(S::F1))
                    chunk.update();

                if (key(S::F2))
                    chunk.clear();
            }

            if (const auto* butPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                using M = sf::Mouse::Button;
                auto but = [&](M x)
                {
                    return butPressed->button == x;
                };


                auto mousePos = (types::pos)window.mapPixelToCoords(sf::Mouse::getPosition(window), view);

                if (but(M::Left))
                    chunk.changeTileAt(mousePos, true);

                if (but(M::Right))
                    chunk.changeTileAt(mousePos, false);
            }

            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (wheel->delta > 0)
                    view.zoom(0.5f);
                else
                    view.zoom(1.5f);
            }


            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::Resized>())
                view.setSize((sf::Vector2f)window.getSize());

        }

        { 
            using S = sf::Keyboard::Scancode;

            if (sf::Keyboard::isKeyPressed(S::Up))
                view.move({0, -5});

            if (sf::Keyboard::isKeyPressed(S::Down))
                view.move({0, 5});

            if (sf::Keyboard::isKeyPressed(S::Right))
                view.move({5, 0});

            if (sf::Keyboard::isKeyPressed(S::Left))
                view.move({-5, 0});
        }

        window.setView(view);

        sf::Color backgroundColor{47, 56, 61};
        window.clear(backgroundColor);

        window.draw(chunk);

        window.display();
    }
}