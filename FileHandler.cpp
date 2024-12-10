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
        std::shared_ptr<LevelData> level = std::make_shared<LevelData>();
        
        // First pass: Create all rooms
        for (const auto& room_dict : level_dict["rooms"]) {
            auto room = std::make_shared<Room>(
                room_dict["name"].get<std::string>(),
                room_dict["scene_uid"].get<std::string>(),
                room_dict["required"].get<bool>(),
                room_dict["max_passes"].get<int>()
            );
            level->rooms[room->name] = room;
        }

        // Second pass: Set up passages and connections
        for (const auto& room_dict : level_dict["rooms"]) {
            auto& room = level->rooms[room_dict["name"].get<std::string>()];
            for (const auto& passage_dict : room_dict["passages"]) {
                std::vector<std::shared_ptr<Connection>> connections;
                for (const auto& conn_dict : passage_dict["connections"]) {
                    std::shared_ptr<Connection> conn = std::make_shared<Connection>(
                        level->rooms[conn_dict["name"].get<std::string>()],
                        conn_dict["connected_passage"].get<std::string>()
                    );
                    connections.push_back(conn);
                }
                room->passages[passage_dict["name"].get<std::string>()] = connections;
            }
        }

        if (level_dict.contains("starter_room_name") && !level_dict["starter_room_name"].is_null()) {
            level->starter_room = level->rooms[level_dict["starter_room_name"].get<std::string>()];
        }
        else{
            std::cerr << "No starter room found for level " << level_dict["name"].get<std::string>() << std::endl;
            return MapData();
        }
        level->max_rooms = level_dict["max_rooms"].get<int>();
        level->min_rooms = level_dict["min_rooms"].get<int>();
        local_map_data.levels[level_dict["name"].get<std::string>()] = level;
    }

    local_map_data.passages_holder_name = data_dict["passages_holder_name"].get<std::string>();
    return local_map_data;
}

bool FileHandler::save_validated_map_data(const std::string& file_path, const MapData& map_data) {
    std::ofstream save(file_path);
    if (!save.is_open()) {
        std::cerr << "Failed to open file for writing the export data" << std::endl;
        return false;
    }

    json save_dict;
    save_dict["file_type"] = "exported_map_data";
    save_dict["passages_holder_name"] = map_data.passages_holder_name;
    save_dict["levels"] = json::array();

    for (const auto& [level_name, level] : map_data.levels) {
        json level_dict;
        level_dict["name"] = level_name;
        level_dict["starter_room_name"] = level->starter_room->name;
        level_dict["possibilities"] = level->possibilities;
        level_dict["connection_pairs"] = json::array();
        level_dict["max_rooms"] = level->max_rooms;
        level_dict["min_rooms"] = level->min_rooms;
        for (const auto& pair : level->connection_pairs) {
            level_dict["connection_pairs"].push_back(pair.first->to_string() +" - " + pair.second->to_string());
        }

        level_dict["rooms"] = json::array();
        for (const auto& [room_name, room] : level->rooms) {
            json room_dict;
            room_dict["name"] = room_name;
            room_dict["scene_uid"] = room->scene_uid;
            room_dict["required"] = room->required;
            room_dict["max_passes"] = room->max_passes;
            room_dict["passages"] = json::array();

            for (const auto& [passage_name, passage_variant] : room->passages) {
                json passage_dict;
                passage_dict["name"] = passage_name;
                passage_dict["connections"] = json::array();

                if (std::holds_alternative<std::vector<std::shared_ptr<Connection>>>(passage_variant)) {
                    for (const auto& connection : std::get<std::vector<std::shared_ptr<Connection>>>(passage_variant)) {
                        json connection_dict;
                        connection_dict["name"] = connection->room->name;
                        connection_dict["connected_passage"] = connection->connected_passage;
                        passage_dict["connections"].push_back(connection_dict);
                    }
                }
                room_dict["passages"].push_back(passage_dict);
            }
            level_dict["rooms"].push_back(room_dict);
        }
        save_dict["levels"].push_back(level_dict);
    }

    save << save_dict.dump();
    save.close();
    return true;
}
