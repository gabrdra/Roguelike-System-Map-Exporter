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


    LevelData();
    LevelData(std::unordered_map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room);
    LevelData(std::unordered_map<std::string, std::shared_ptr<Room>> rooms, std::shared_ptr<Room> starter_room, std::list<std::list<int>> possibilities, std::list<std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>> connection_pairs);
};
#endif