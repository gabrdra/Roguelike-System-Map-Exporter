#include "MapValidation.h"
#include <iostream>
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
                std::vector<std::shared_ptr<Connection>>& origin_room_conns = passage_pair.second;
                std::vector<std::shared_ptr<Connection>> duplicated_connections;
                for (std::shared_ptr<Connection>& origin_conn : origin_room_conns) {
                    duplicated_connections.push_back(std::make_shared<Connection>(
                        level->rooms[origin_conn->room->name], origin_conn->connected_passage
                    ));
                    level->rooms[origin_conn->room->name]->passages[origin_conn->connected_passage].push_back(
                        std::make_shared<Connection>(new_room, passage_pair.first)
                    );
                }
                new_room->passages[passage_pair.first] = duplicated_connections;
            }
        }
        origin_room->max_passes = 1;
    }
}