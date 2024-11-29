#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

class Connection;

class Room {
public:
    std::string name;
    std::string scene_uid;
    bool required;
    int max_passes;
    std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>>> passages;

    Room(const std::string& _name = "", const std::string& _scene_uid = "", bool _required = false, int _max_passes = 1);
    Room(const std::string& _name, const std::string& _scene_uid, bool _required, int _max_passes, std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>>> _passages);
    Room(const std::string& _name, const std::string& _scene_uid, std::unordered_map<std::string, std::variant<std::shared_ptr<Connection>,std::vector<std::shared_ptr<Connection>>>> passages = {});
};

#endif