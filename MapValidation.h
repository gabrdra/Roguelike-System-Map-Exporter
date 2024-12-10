#ifndef MAPVALIDATION_H
#define MAPVALIDATION_H

#include "MapData.h"
#include "Connection.h"
#include <stack>
#include "BacktrackData.h"

class MapValidation
{
public:
    MapData validate_map(const MapData map_data, int max_instances);
private:
    void multiply_rooms(std::shared_ptr<LevelData> level);
    std::shared_ptr<LevelData> generate_level_possibilities(const std::shared_ptr<LevelData> input_level, int max_instances);
    std::stack<std::shared_ptr<Connection>> get_unused_connections_from_room(std::shared_ptr<Room> room);
    std::shared_ptr<Room> create_used_room(std::shared_ptr<Room> room);
    std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>>> get_passages_for_room(std::shared_ptr<Room> room, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>> default_value = nullptr);
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Connection>>> get_passages_attempts_for_backtrack_data(std::shared_ptr<Room> room);
    void remove_rooms_from_used_rooms(std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms, std::shared_ptr<BacktrackData> initial_room_to_erase);
    std::stack<std::shared_ptr<Connection>> recreate_unused_connections(std::shared_ptr<BacktrackData> starter_room, std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms);
    bool check_level_validity(std::shared_ptr<BacktrackData> starter_room, std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms, const std::unordered_map<std::string, std::shared_ptr<Room>>& input_rooms, long long unsigned int required_rooms_size);
};


#endif