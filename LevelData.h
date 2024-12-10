#ifndef LEVELDATA_H
#define LEVELDATA_H

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include "Room.h"
#include "Connection.h"


class LevelData {
public:
    std::unordered_map<std::string, std::shared_ptr<Room>> rooms;
    std::shared_ptr<Room> starter_room;
    std::list<std::list<int>> possibilities;
    std::list<std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>> connection_pairs;
    int max_rooms = 10000;
    int min_rooms = 1;


    LevelData();
    LevelData(std::unordered_map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room, int max_rooms, int min_rooms);
    LevelData(std::unordered_map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room, std::list<std::list<int>> possibilities, std::list<std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>> connection_pairs, int max_rooms, int min_rooms);
};
#endif