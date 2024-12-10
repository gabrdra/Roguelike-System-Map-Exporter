#include "MapValidation.h"
#include <iostream>
#include <unordered_set>
#include <list>
#include <string>
MapData MapValidation::validate_map(const MapData map_data, int max_instances) {
    std::unordered_map<std::string, std::shared_ptr<LevelData>> levels;
    for (auto& level : map_data.levels) {
        if (level.second->starter_room == nullptr) {
            std::cerr << "The "<<level.first<<" has no starter room" << std::endl;
            return MapData();
        }
        multiply_rooms(level.second);
        // break;
        std::shared_ptr<LevelData> return_level_data = generate_level_possibilities(level.second, max_instances);
        if (return_level_data == nullptr) {
            std::cerr << "The "<<level.first<<" couldn't generate at least one viable instance" << std::endl;
            return MapData();
        }
        levels[level.first] = return_level_data;
    }
    return MapData(levels, map_data.passages_holder_name);
    // return map_data;
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

// void print_used_rooms_and_conns(std::unordered_map<std::string, std::shared_ptr<BacktrackData>> used_rooms, std::vector<std::shared_ptr<Connection>> connections_order){
//     std::cout<<"Used rooms: "<<std::endl;
//     for (const auto& [room_name, room] : used_rooms) {
//         std::cout << "Room: " << room_name << std::endl;
//         for (const auto& [passage_name, conn] : room->room->passages) {
//             std::cout << "Passage: " << passage_name << "    " << ((std::get<std::shared_ptr<Connection>>(conn)==nullptr)?"null":std::get<std::shared_ptr<Connection>>(conn)->to_string()) << std::endl;
//         }
//     }
//     std::cout<<"Connections order: "<<std::endl;
//     for (const auto& conn : connections_order) {
//         std::cout << "    " << conn->to_string() << std::endl;
//     }
// }

struct pair_hash {
    std::size_t operator() (const std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>& pair) const {
        return std::hash<std::string>()(pair.first->room->name + pair.first->connected_passage) ^ 
               std::hash<std::string>()(pair.second->room->name + pair.second->connected_passage);
    }
};
struct pair_equal {
    bool operator() (const std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>& lhs,
                     const std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>& rhs) const {
        return lhs.first->room->name == rhs.first->room->name &&
               lhs.first->connected_passage == rhs.first->connected_passage &&
               lhs.second->room->name == rhs.second->room->name &&
               lhs.second->connected_passage == rhs.second->connected_passage;
    }
};
std::shared_ptr<LevelData> MapValidation::generate_level_possibilities(const std::shared_ptr<LevelData> input_level, int max_instances) {
    auto input_rooms = input_level->rooms;
    std::unordered_map<std::string, std::shared_ptr<BacktrackData>> used_rooms;
    std::vector<std::string> required_rooms_names;
    std::vector<std::shared_ptr<Connection>> connections_order;
    std::list<std::list<int>> possibilities;
    std::list<std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>> connection_pairs;
    std::unordered_map<std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>>, int, pair_hash, pair_equal> connection_pairs_indexes;

    for (const auto& room_pair : input_rooms) {
        if (room_pair.second->required) {
            required_rooms_names.push_back(room_pair.first);
        }
    }

    used_rooms[input_level->starter_room->name] = std::make_shared<BacktrackData>("", create_used_room(input_level->starter_room));
    auto starter_room = used_rooms[input_level->starter_room->name];
    starter_room->passages_attempts = get_passages_attempts_for_backtrack_data(starter_room->room);
    
    bool level_is_valid = false;
    auto unused_connections = get_unused_connections_from_room(starter_room->room);

    while (true) {
        level_is_valid = false;
        while (!unused_connections.empty()) {
            auto outgoing_connection = unused_connections.top();
            unused_connections.pop();
            //std::cout<<"Outgoing connection: "<<outgoing_connection->to_string()<<std::endl;
            connections_order.push_back(outgoing_connection);

            if (std::get<std::shared_ptr<Connection>>(outgoing_connection->room->passages[outgoing_connection->connected_passage]) != nullptr) {
                continue;
            }

            auto possible_connections = std::get<std::vector<std::shared_ptr<Connection>>>(input_rooms[outgoing_connection->room->name]->passages[outgoing_connection->connected_passage]);
            std::shared_ptr<Connection> incomming_connection = nullptr;
            for (const auto& conn : possible_connections) {
                bool valid_room = true;
                if (used_rooms.find(conn->room->name) != used_rooms.end()) {
                    valid_room = std::get<std::shared_ptr<Connection>>(used_rooms[conn->room->name]->room->passages[conn->connected_passage]) == nullptr;
                }
                valid_room = !used_rooms[outgoing_connection->room->name]->passages_attempts[outgoing_connection->connected_passage].count(conn) && valid_room;
                if (valid_room) {
                    incomming_connection = conn;
                    break;
                }
            }

            if (incomming_connection == nullptr) {
                remove_rooms_from_used_rooms(used_rooms, used_rooms[outgoing_connection->room->name]);
                unused_connections = recreate_unused_connections(starter_room, used_rooms);
                std::vector<std::shared_ptr<Connection>> new_conn_order;
                for (const auto& conn : connections_order) {
                    if(used_rooms.find(conn->room->name) != used_rooms.end()){
                        if (std::get<std::shared_ptr<Connection>>(used_rooms[conn->room->name]->room->passages[conn->connected_passage]) != nullptr) {
                            new_conn_order.push_back(conn);
                        }
                    }
                }
                connections_order = new_conn_order;
                continue;
            }

            //auto incomming_connection = possible_connections.back();
            possible_connections.pop_back();
            used_rooms[outgoing_connection->room->name]->passages_attempts[outgoing_connection->connected_passage].insert(incomming_connection);

            if (used_rooms.find(incomming_connection->room->name) == used_rooms.end()) {
                auto new_used_room = create_used_room(incomming_connection->room);
                used_rooms[new_used_room->name] = std::make_shared<BacktrackData>(outgoing_connection->room->name, new_used_room);
                used_rooms[new_used_room->name]->passages_attempts = get_passages_attempts_for_backtrack_data(new_used_room);
                new_used_room->passages[incomming_connection->connected_passage] = outgoing_connection;
                outgoing_connection->room->passages[outgoing_connection->connected_passage] = std::make_shared<Connection>(new_used_room, incomming_connection->connected_passage);
                auto new_unused_connections = get_unused_connections_from_room(new_used_room);
                while (!new_unused_connections.empty()) {
                    unused_connections.push(new_unused_connections.top());
                    new_unused_connections.pop();
                }
            } else {
                used_rooms[incomming_connection->room->name]->room->passages[incomming_connection->connected_passage] = outgoing_connection;
                outgoing_connection->room->passages[outgoing_connection->connected_passage] = incomming_connection;
            }

            level_is_valid = check_level_validity(starter_room, used_rooms, input_rooms, required_rooms_names.size());
            if (!level_is_valid) {
                auto latest_connection = connections_order.back();
                connections_order.pop_back();
                unused_connections.push(latest_connection);

                auto outgoing_connection_room = used_rooms[latest_connection->room->name];
                auto incomming_connection = std::get<std::shared_ptr<Connection>>(outgoing_connection_room->room->passages[latest_connection->connected_passage]);
                auto incomming_connection_room = used_rooms[incomming_connection->room->name];
                outgoing_connection_room->room->passages[latest_connection->connected_passage] = nullptr;

                bool had_more_connections = false;
                for (const auto& passage_name : incomming_connection_room->room->passages) {
                    if (passage_name.first == incomming_connection->connected_passage) {
                        continue;
                    }
                    if (std::get<std::shared_ptr<Connection>>(incomming_connection_room->room->passages[passage_name.first]) != nullptr) {
                        had_more_connections = true;
                        break;
                    }
                }

                if (!had_more_connections) { //use a double linked list instead of a stack as to not need to duplicate it
                    // unused_connections = std::stack<std::shared_ptr<Connection>>(std::stack<std::shared_ptr<Connection>>());
                    // std::stack<std::shared_ptr<Connection>> temp_stack;
                    // while (!unused_connections.empty()) {
                    //     auto conn = unused_connections.top();
                    //     unused_connections.pop();
                    //     if (conn->room->name != incomming_connection_room->room->name) {
                    //         temp_stack.push(conn);
                    //     }
                    // }
                    // unused_connections = temp_stack;
                    used_rooms.erase(incomming_connection_room->room->name);
                    unused_connections = recreate_unused_connections(starter_room, used_rooms);
                } else {
                    incomming_connection_room->room->passages[incomming_connection->connected_passage] = nullptr;
                    unused_connections = recreate_unused_connections(starter_room, used_rooms);
                }
            }
        }

        if (connections_order.empty() || !level_is_valid) {
            break;
        }
        //print_used_rooms_and_conns(used_rooms, connections_order);
        if (used_rooms.size()<=input_level->max_rooms && used_rooms.size()>=input_level->min_rooms) {
            std::list<int> possibility;
            for (const auto& conn : connections_order) {
                auto other_side_conn = std::get<std::shared_ptr<Connection>>(used_rooms[conn->room->name]->room->passages[conn->connected_passage]);
                std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>> conn_pair = std::make_pair(conn, other_side_conn);
                int index = -1;
                if (connection_pairs_indexes.find(conn_pair) != connection_pairs_indexes.end()) {
                    index = connection_pairs_indexes[conn_pair];
                }
                if (index == -1) {
                    connection_pairs_indexes[conn_pair] = connection_pairs.size();
                    std::pair<std::shared_ptr<Connection>, std::shared_ptr<Connection>> inverse_conn_pair = std::make_pair(other_side_conn, conn);
                    connection_pairs_indexes[inverse_conn_pair] = connection_pairs.size();
                    index = connection_pairs.size();
                    connection_pairs_indexes[conn_pair] = index;
                    connection_pairs.push_back(conn_pair);
                }
                possibility.push_back(index);
            }
            possibilities.push_back(possibility);
            std::cout<< "instances created: "<<possibilities.size()<<std::endl;
            if (possibilities.size() >= max_instances) {
                break;
            }
        }
        auto incomming_connection = std::get<std::shared_ptr<Connection>>(used_rooms[connections_order.back()->room->name]->room->passages[connections_order.back()->connected_passage]);
        auto latest_connection = connections_order.back();
        connections_order.pop_back();
        unused_connections.push(latest_connection);

        auto outgoing_connection_room = used_rooms[latest_connection->room->name];
        auto incomming_connection_room = used_rooms[incomming_connection->room->name];
        outgoing_connection_room->room->passages[latest_connection->connected_passage] = nullptr;

        bool had_more_connections = false;
        for (const auto& passage_name : incomming_connection_room->room->passages) {
            if (passage_name.first == incomming_connection->connected_passage) {
                continue;
            }
            if (std::get<std::shared_ptr<Connection>>(incomming_connection_room->room->passages[passage_name.first]) != nullptr) {
                had_more_connections = true;
                break;
            }
        }

        if (!had_more_connections) { //use a double linked list instead of a stack as to not need to duplicate it
            // unused_connections = std::stack<std::shared_ptr<Connection>>(std::stack<std::shared_ptr<Connection>>());
            // std::stack<std::shared_ptr<Connection>> temp_stack;
            // while (!unused_connections.empty()) {
            //     auto conn = unused_connections.top();
            //     unused_connections.pop();
            //     if (conn->room->name != incomming_connection_room->room->name) {
            //         temp_stack.push(conn);
            //     }
            // }
            // unused_connections = temp_stack;
            used_rooms.erase(incomming_connection_room->room->name);
            unused_connections = recreate_unused_connections(starter_room, used_rooms);
        } else {
            incomming_connection_room->room->passages[incomming_connection->connected_passage] = nullptr;
            unused_connections = recreate_unused_connections(starter_room, used_rooms);
        }
    }

    if (possibilities.empty()) {
        return nullptr;
    }
    std::shared_ptr<LevelData> validated_level = std::make_shared<LevelData>(input_level->rooms,input_level->starter_room, possibilities, connection_pairs, input_level->max_rooms, input_level->min_rooms);
    return validated_level;
}

std::stack<std::shared_ptr<Connection>> MapValidation::get_unused_connections_from_room(std::shared_ptr<Room> room) {
    std::vector<std::string> passages_names;
    for (const auto& passage_pair : room->passages) {
        if (std::get<std::shared_ptr<Connection>>(passage_pair.second) == nullptr) {
            passages_names.push_back(passage_pair.first);
        }
    }
    std::stack<std::shared_ptr<Connection>> conns;
    for (const auto& passage_name : passages_names) {
        conns.push(std::make_shared<Connection>(room, passage_name));
    }
    return conns;
}

std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>>> MapValidation::get_passages_for_room(std::shared_ptr<Room> room, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>> default_value){
    std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>, std::vector<std::shared_ptr<Connection>>>> passages;
    for (const auto& passage_pair : room->passages) {
        passages[passage_pair.first] = default_value;
    }
    return passages;
}

std::shared_ptr<Room> MapValidation::create_used_room(std::shared_ptr<Room> room){
    return std::make_shared<Room>(
        room->name,
        room->scene_uid,
        get_passages_for_room(room)
    );
}

std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Connection>>> MapValidation::get_passages_attempts_for_backtrack_data(std::shared_ptr<Room> room){
    std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Connection>>> passages_attempts;
    for (const auto& passage_pair : room->passages) {
        passages_attempts[passage_pair.first] = std::unordered_set<std::shared_ptr<Connection>>();
    }
    return passages_attempts;
}
void MapValidation::remove_rooms_from_used_rooms(std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms, std::shared_ptr<BacktrackData> initial_room_to_erase) {
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
std::stack<std::shared_ptr<Connection>> MapValidation::recreate_unused_connections(std::shared_ptr<BacktrackData> starter_room, std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms) {
    std::stack<std::shared_ptr<Connection>> unused_connections = std::stack<std::shared_ptr<Connection>>();
    if (used_rooms.empty()) {
        return unused_connections;
    }

    std::stack<std::shared_ptr<Connection>> stack;
    std::unordered_set<std::string> visited;

    for (const auto& passage_pair : starter_room->room->passages) {
        auto connection = std::get<std::shared_ptr<Connection>>(passage_pair.second);
        if (connection == nullptr) {
            unused_connections.push(std::make_shared<Connection>(starter_room->room, passage_pair.first));
        } else {
            stack.push(std::make_shared<Connection>(*connection));
        }
    }
    visited.insert(starter_room->room->name);

    while (!stack.empty()) {
        std::shared_ptr<Connection> curr_conn = stack.top();
        stack.pop();

        if (visited.find(curr_conn->room->name) != visited.end()) {
            continue;
        }
        visited.insert(curr_conn->room->name);

        std::shared_ptr<BacktrackData> curr_room = used_rooms[curr_conn->room->name];
        for (const auto& passage_pair : curr_room->room->passages) {
            auto connection = std::get<std::shared_ptr<Connection>>(passage_pair.second);
            if (connection == nullptr) {
                unused_connections.push(std::make_shared<Connection>(curr_room->room, passage_pair.first));
            } else {
                stack.push(std::make_shared<Connection>(*connection));
            }
        }
    }
    return unused_connections;
}
bool MapValidation::check_level_validity(std::shared_ptr<BacktrackData> starter_room, std::unordered_map<std::string, std::shared_ptr<BacktrackData>>& used_rooms, const std::unordered_map<std::string, std::shared_ptr<Room>>& input_rooms, long long unsigned int required_rooms_size) {
    std::stack<std::shared_ptr<Connection>> stack;
    std::unordered_set<std::string> rooms_visited;
    std::unordered_set<std::string> required_rooms_found;
    required_rooms_found.insert(starter_room->room->name);

    for (const auto& passage_pair : starter_room->room->passages) {
        auto connection = std::get<std::shared_ptr<Connection>>(passage_pair.second);
        if (connection != nullptr) {
            stack.push(connection);
        } else {
            auto& possible_connections = std::get<std::vector<std::shared_ptr<Connection>>>(input_rooms.at(starter_room->room->name)->passages.at(passage_pair.first));
            for (const auto& conn : possible_connections) {
                stack.push(conn);
            }
        }
    }
    rooms_visited.insert(starter_room->room->name);

    while (!stack.empty()) {
        std::shared_ptr<Connection> curr_conn = stack.top();
        stack.pop();

        if (rooms_visited.find(curr_conn->room->name) != rooms_visited.end()) {
            continue;
        }
        rooms_visited.insert(curr_conn->room->name);

        if (used_rooms.find(curr_conn->room->name) != used_rooms.end()) {
            std::shared_ptr<BacktrackData> curr_room_backtrack = used_rooms[curr_conn->room->name];
            for (const auto& passage_pair : curr_room_backtrack->room->passages) {
                auto curr_passage = std::get<std::shared_ptr<Connection>>(passage_pair.second);
                if (curr_passage != nullptr) {
                    stack.push(curr_passage);
                } else {
                    auto& possible_connections = std::get<std::vector<std::shared_ptr<Connection>>>(input_rooms.at(curr_room_backtrack->room->name)->passages.at(passage_pair.first));
                    std::vector<std::shared_ptr<Connection>> valid_connections;
                    for (const auto& conn : possible_connections) {
                        bool valid_room = true;
                        if (used_rooms.find(conn->room->name) != used_rooms.end()) {
                            valid_room = (std::get<std::shared_ptr<Connection>>(used_rooms[conn->room->name]->room->passages[conn->connected_passage]) == nullptr);
                        }
                        valid_room = !curr_room_backtrack->passages_attempts[passage_pair.first].count(conn) && valid_room;
                        if (valid_room) {
                            valid_connections.push_back(conn);
                        }
                    }
                    if (valid_connections.empty()) {
                        return false; // a room in the final level has 0 possible connections, meaning an inconsistent state
                    }
                    for (const auto& conn : valid_connections) {
                        stack.push(conn);
                    }
                }
            }
        } else {
            std::shared_ptr<Room> curr_room = input_rooms.at(curr_conn->room->name);
            for (const auto& passage_pair : curr_room->passages) {
                auto& possible_connections = std::get<std::vector<std::shared_ptr<Connection>>>(passage_pair.second);
                for (const auto& conn : possible_connections) {
                    stack.push(conn);
                }
            }
        }
        if (input_rooms.at(curr_conn->room->name)->required) {
            required_rooms_found.insert(curr_conn->room->name);
        }
    }
    return required_rooms_found.size() == required_rooms_size;
}

