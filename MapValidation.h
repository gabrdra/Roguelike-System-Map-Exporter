#ifndef MAPVALIDATION_H
#define MAPVALIDATION_H

#include "MapData.h"
#include "Connection.h"
#include <stack>
#include "BacktrackData.h"

class MapValidation
{
public:
    MapData validate_map(const MapData map_data);
private:
    void multiply_rooms(std::shared_ptr<LevelData> level);
    LevelData generate_level_possibilities(const std::shared_ptr<LevelData> input_level);
    std::stack<Connection> get_unused_connections_from_room(std::shared_ptr<Room> room);
    std::shared_ptr<Room> create_used_room(std::shared_ptr<Room> room);
    std::map<std::string, std::shared_ptr<Connection>> create_passages_for_used_room(std::shared_ptr<Room> room);
    std::map<std::string, std::vector<std::shared_ptr<Connection>>> create_passages_attempts_for_room(std::shared_ptr<Room> room);
    void remove_rooms_from_used_rooms(std::unordered_map<std::string, std::shared_ptr<Room>>& used_rooms, std::shared_ptr<Room> initial_room_to_erase);
    std::stack<Connection> recreate_unused_connections(std::shared_ptr<BacktrackData> starter_room, std::unordered_map<std::string, std::shared_ptr<Room>>& used_rooms);
    bool check_level_validity(std::shared_ptr<BacktrackData> starter_room, std::unordered_map<std::string, std::shared_ptr<Room>>& used_rooms, const std::unordered_map<std::string, std::shared_ptr<Room>>& input_rooms, int required_rooms_size);
};


#endif