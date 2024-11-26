#include "FileHandler.h"
#include <iostream>
#include <string>
#include <fstream>
#include "include\single_include\nlohmann\json.hpp"
#include "MapData.h"
#include "Room.h"
#include "Connection.h"
#include "LevelData.h"
using json = nlohmann::json;

MapData FileHandler::load_map_data(const std::string& path) {
    std::ifstream file;
    file.open(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading" << std::endl;
        return MapData();
    }

    json data_dict;
    try {
        file >> data_dict;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return MapData();
    }
    file.close();

    if (!data_dict.contains("file_type") || data_dict["file_type"] != "save_data" ||
        !data_dict.contains("passages_holder_name")) {
        std::cerr << "Error with plugin save file structure" << std::endl;
        return MapData();
    }

    MapData local_map_data;
    
    for (const auto& level_dict : data_dict["levels"]) {
        LevelData level;
        
        // First pass: Create all rooms
        for (const auto& room_dict : level_dict["rooms"]) {
            auto room = std::make_shared<Room>(
                room_dict["name"].get<std::string>(),
                room_dict["scene_uid"].get<std::string>(),
                room_dict["required"].get<bool>(),
                room_dict["max_passes"].get<int>()
            );
            level.rooms[room->name] = room;
        }

        // Second pass: Set up passages and connections
        for (const auto& room_dict : level_dict["rooms"]) {
            auto& room = level.rooms[room_dict["name"].get<std::string>()];
            for (const auto& passage_dict : room_dict["passages"]) {
                std::vector<std::shared_ptr<Connection>> connections;
                for (const auto& conn_dict : passage_dict["connections"]) {
                    std::shared_ptr<Connection> conn = std::make_shared<Connection>(
                        level.rooms[conn_dict["name"].get<std::string>()],
                        conn_dict["connected_passage"].get<std::string>()
                    );
                    connections.push_back(conn);
                }
                room->passages[passage_dict["name"].get<std::string>()] = connections;
            }
        }

        if (level_dict.contains("starter_room_name") && !level_dict["starter_room_name"].is_null()) {
            level.starter_room = level.rooms[level_dict["starter_room_name"].get<std::string>()];
        }
        else{
            std::cerr << "No starter room found for level " << level_dict["name"].get<std::string>() << std::endl;
            return MapData();
        }
        
        local_map_data.levels[level_dict["name"].get<std::string>()] = level;
    }

    local_map_data.passages_holder_name = data_dict["passages_holder_name"].get<std::string>();
    return local_map_data;
}