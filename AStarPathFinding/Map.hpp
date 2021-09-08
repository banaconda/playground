#pragma once

#include <iostream>

#include "Vector2D.h"
#include "PriorityQueue.hpp"
#include "UnorderedMap.hpp"

enum class TileType {
    GRASS,
    WALL,
    DIRT,
    WATER,
    START,
    END,
    PATH,
};

class Tile
{
  public:
    Vector2D pos;
    TileType type;

    Tile() {}
    Tile(Vector2D pos, TileType type)
    {
        this->pos = pos;
        this->type = type;
    }

    friend std::ostream &
    operator<<(std::ostream &stream, const Tile &t)
    {
        switch (t.type) {
        case TileType::GRASS:
            stream << "   ";
            break;
        case TileType::WALL:
            stream << " # ";
            break;
        case TileType::DIRT:
            stream << " - ";
            break;
        case TileType::WATER:
            stream << " + ";
            break;
        case TileType::START:
            stream << " @ ";
            break;
        case TileType::END:
            stream << " $ ";
            break;
        case TileType::PATH:
            stream << " * ";
            break;
        default:
            stream << "   ";
            break;
        }
        return stream;
    }
};

class Map
{
    Tile **tileMap;
    size_t rowSize;
    size_t colSize;

  public:
    Map(size_t rowSize, size_t colSize)
    {
        this->rowSize = rowSize;
        this->colSize = colSize;
        tileMap = new Tile *[rowSize];
        for (int y = 0; y < rowSize; y++) {
            tileMap[y] = new Tile[colSize];
            for (int x = 0; x < colSize; x++) {
                tileMap[y][x] = Tile(Vector2D(x, y), TileType::GRASS);
            }
        }
    }

    void
    setTileType(size_t x, size_t y, TileType type)
    {
        tileMap[y][x].type = type;
    }

    Tile &
    getTile(size_t x, size_t y)
    {
        return tileMap[y][x];
    }

    Tile &
    getTile(Vector2D &vec)
    {
        return getTile(vec.x, vec.y);
    }

    std::list<Vector2D>
    getNeighbors(Vector2D vec)
    {
        int dir[4][2] = {
            {-1, 0}, // left
            {1, 0},  // right
            {0, -1}, // up
            {0, 1},  // down
        };
        std::list<Vector2D> neighbors;
        for (int i = 0; i < 4; i++) {
            Vector2D v(vec.x + dir[i][0], vec.y + dir[i][1]);
            if (v.x < 0 || v.y < 0 || v.x >= colSize || v.y >= rowSize) {
                continue;
            }
            neighbors.push_back(v);
        }

        return neighbors;
    }

    float
    getManhattanDistance(Vector2D &start, Vector2D &end)
    {
        return std::abs(start.x - end.x) + std::abs(start.y - end.y);
    }

    std::list<Vector2D>
    getPath(Vector2D start, Vector2D end)
    {
        PriorityQueue<Vector2D> frontier;
        frontier.enqueue(start, 0);
        UnorderedMap<Vector2D, Vector2D> cameFrom;
        UnorderedMap<Vector2D, float> costSoFar;
        cameFrom.insert(start, start);
        costSoFar.insert(start, 0);

        Vector2D *currentPtr = nullptr;
        while ((currentPtr = frontier.dequeue()) != nullptr) {
            Vector2D current = *currentPtr;
            if (current == end) {
                break;
            }
            for (auto &next : getNeighbors(current)) {
                Tile tile = getTile(next);
                // if (tile.type == TileType::WALL || cameFrom.getValue(next) != nullptr) {
                if (tile.type == TileType::WALL) {
                    continue;
                }

                TileType tileType = getTile(next).type;
                float movement = 1;
                if (tileType == TileType::DIRT) {
                    movement = 5;
                }
                float newCost = *costSoFar.getValue(current) + movement;
                if (costSoFar.getValue(next) == nullptr || newCost < *costSoFar.getValue(next)) {
                    costSoFar.insert(next, newCost);
                    float priority = newCost + getManhattanDistance(next, end);
                    frontier.enqueue(next, priority);
                    cameFrom.insert(next, current);
                    std::cout << "cur: " << current << "next: " << next << std::endl;
                }
            }
        }

        std::list<Vector2D> path;
        Vector2D current = end;
        while (current != start) {
            path.push_back(current);
            current = *cameFrom.getValue(current);
            setTileType(current.x, current.y, TileType::PATH);
        }

        return path;
    }

    friend std::ostream &
    operator<<(std::ostream &stream, const Map &m)
    {
        for (int y = 0; y < m.rowSize; y++) {
            for (int x = 0; x < m.colSize; x++) {
                stream << m.tileMap[y][x];
            }
            stream << std::endl << std::endl;
        }
        return stream;
    }
};