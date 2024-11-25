#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
#include <map>
#include <memory>

class Connection;

class Room {
public:
    std::string name;
    std::string scene_uid;
    bool required;
    int max_passes;
    std::map<std::string, std::vector<std::shared_ptr<Connection>>> passages;

    Room(const std::string& _name = "", const std::string& _scene_uid = "", bool _required = false, int _max_passes = 1);
    Room(const std::string& _name, const std::string& _scene_uid, bool _required, int _max_passes, std::map<std::string, std::vector<std::shared_ptr<Connection>>> _passages);

};

#endif