#include <iostream>
#include <math.h>
#include <memory>

#include "PriorityQueue.hpp"
#include "UnorderedMap.hpp"
#include "Vector2D.h"

#include "Map.hpp"

using namespace std;

int
main(int argc, char **argv)
{
    PriorityQueue<Vector2D> frontier;

    Map m = Map(10, 10);

    for (int i = 0; i < 10; i++) {
        m.setTileType(i, 0, TileType::WALL);
        m.setTileType(0, i, TileType::WALL);
        m.setTileType(i, 9, TileType::WALL);
        m.setTileType(9, i, TileType::WALL);
    }

    for (int i = 3; i < 7; i++) {
        m.setTileType(i, 3, TileType::WALL);
        m.setTileType(6, i, TileType::WALL);
    }

    m.setTileType(4, 6, TileType::DIRT);
    m.setTileType(3, 5, TileType::DIRT);
    m.setTileType(2, 5, TileType::DIRT);
    m.setTileType(6, 7, TileType::DIRT);

    m.setTileType(3, 6, TileType::START);
    m.setTileType(7, 2, TileType::END);

    std::list<Vector2D> path = m.getPath(Vector2D(3, 6), Vector2D(7, 2));

    cout << m << endl;

    return 0;
}