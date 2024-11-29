#include "MapValidation.h"
#include <iostream>
#include <unordered_set>
MapData MapValidation::validate_map(const MapData map_data) {
    std::unordered_map<std::string, LevelData> levels;
    for (auto& level : map_data.levels) {
        if (level.second->starter_room == nullptr) {
            std::cerr << "The "<<level.first<<" has no starter room" << std::endl;
            return MapData();
        }
        multiply_rooms(level.second);
        break;
        // LevelData return_level_data = generate_level_possibilities(level.second);
        // if (return_level_data.possibilities.empty()) {
        //     std::cerr << "The "<<level.first<<" couldn't generate at least one viable instance" << std::endl;
        //     return MapData();
        // }
        // levels[level.first] = return_level_data;
    }
    //return MapData(levels, map_data.passages_holder_name);
    return map_data;
}
void MapValidation::multiply_rooms(std::shared_ptr<LevelData> level) {
    for (auto& origin_room_pair : level->rooms) {
        std::shared_ptr<Room>& origin_room = origin_room_pair.second;
        if (origin_room->max_passes == 1) {
            continue;
        }
        for (int i = 0; i < origin_room->max_passes - 1; ++i) {
            std::shared_ptr<Room> new_room = std::make_shared<Room>(
                origin_room->name + "_" + std::to_string(i + 2),
                origin_room->scene_uid,
                origin_room->required,
                1
            );
            level->rooms[new_room->name] = new_room;
            for (auto& passage_pair : origin_room->passages) {
                std::vector<std::shared_ptr<Connection>>& origin_room_conns = std::get<std::vector<std::shared_ptr<Connection>>>(passage_pair.second);
                std::vector<std::shared_ptr<Connection>> duplicated_connections;
                for (std::shared_ptr<Connection>& origin_conn : origin_room_conns) {
                    duplicated_connections.push_back(std::make_shared<Connection>(
                        level->rooms[origin_conn->room->name], origin_conn->connected_passage
                    ));
                    std::get<std::vector<std::shared_ptr<Connection>>>(level->rooms[origin_conn->room->name]->passages[origin_conn->connected_passage]).push_back(
                        std::make_shared<Connection>(new_room, passage_pair.first)
                    );
                }
                new_room->passages[passage_pair.first] = duplicated_connections;
            }
        }
        origin_room->max_passes = 1;
    }
}
// LevelData MapValidation::generate_level_possibilities(const std::shared_ptr<LevelData> input_level) {
// }

std::stack<Connection> get_unused_connections_from_room(std::shared_ptr<Room> room) {
    std::vector<std::string> passages_names;
    for (const auto& passage_pair : room->passages) {
        if (std::get<std::shared_ptr<Connection>>(passage_pair.second) == nullptr) {
            passages_names.push_back(passage_pair.first);
        }
    }
    std::stack<Connection> conns;
    for (const auto& passage_name : passages_names) {
        conns.push(Connection(room, passage_name));
    }
    return conns;
}

std::shared_ptr<Room> create_used_room(std::shared_ptr<Room> room){
    std::shared_ptr<Room> used_room = std::make_shared<Room>(
        room->name,
        room->scene_uid,
        get_passages_for_room(room)
    );
}
 std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>>> get_passages_for_room(std::shared_ptr<Room> room, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>> default_value = nullptr){
    std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>, std::vector<std::shared_ptr<Connection>>>> passages;
    for (const auto& passage_pair : room->passages) {
        passages[passage_pair.first] = default_value;
    }
    return passages;
}
void remove_rooms_from_used_rooms(std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms, std::shared_ptr<BacktrackData> initial_room_to_erase) {
    std::stack<std::shared_ptr<BacktrackData>> rooms_to_erase_stack;
    std::unordered_set<std::string> erased_rooms;
    rooms_to_erase_stack.push(initial_room_to_erase);

    while (!rooms_to_erase_stack.empty()) {
        std::shared_ptr<BacktrackData> room_to_erase = rooms_to_erase_stack.top();
        rooms_to_erase_stack.pop();

        if (erased_rooms.find(room_to_erase->room->name) != erased_rooms.end()) {
            continue;
        }
        erased_rooms.insert(room_to_erase->room->name);

        for (const auto& passage_pair : room_to_erase->room->passages) {
            auto connection = std::get<std::shared_ptr<Connection>>(passage_pair.second);
            if (connection == nullptr) {
                continue;
            }
            used_rooms[connection->room->name]->room->passages[connection->connected_passage] = nullptr;
            if (used_rooms[connection->room->name]->parent_room_name != room_to_erase->room->name) {
                continue;
            }
            rooms_to_erase_stack.push(used_rooms[connection->room->name]);
        }
        used_rooms.erase(room_to_erase->room->name);
    }
}
std::stack<Connection> recreate_unused_connections(std::shared_ptr<BacktrackData> starter_room, std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms) {
    std::stack<Connection> unused_connections = std::stack<Connection>();
    if (used_rooms.empty()) {
        return unused_connections;
    }

    std::stack<Connection> stack;
    std::unordered_set<std::string> visited;

    for (const auto& passage_pair : starter_room->room->passages) {
        auto connection = std::get<std::shared_ptr<Connection>>(passage_pair.second);
        if (connection == nullptr) {
            unused_connections.push(Connection(starter_room->room, passage_pair.first));
        } else {
            stack.push(*connection);
        }
    }
    visited.insert(starter_room->room->name);

    while (!stack.empty()) {
        Connection curr_conn = stack.top();
        stack.pop();

        if (visited.find(curr_conn.room->name) != visited.end()) {
            continue;
        }
        visited.insert(curr_conn.room->name);

        std::shared_ptr<BacktrackData> curr_room = used_rooms[curr_conn.room->name];
        for (const auto& passage_pair : curr_room->room->passages) {
            auto connection = std::get<std::shared_ptr<Connection>>(passage_pair.second);
            if (connection == nullptr) {
                unused_connections.push(Connection(curr_room->room, passage_pair.first));
            } else {
                stack.push(*connection);
            }
        }
    }
    return unused_connections;
}
// bool check_level_validity(std::shared_ptr<BacktrackData> starter_room, std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms, const std::unordered_map<std::string, std::shared_ptr<Room>>& input_rooms, int required_rooms_size){

// }