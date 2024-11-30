#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <memory>
#include "Room.h"

class Connection {
public:
    std::shared_ptr<Room> room;
    std::string connected_passage;

    Connection(std::shared_ptr<Room> _room, const std::string& _connected_passage);
    std::string to_string() const;
    bool equals(const Connection& other_connection) const;

    // Custom comparator
    bool operator<(const Connection& other) const {
        return room->name == other.room->name && connected_passage == other.connected_passage;
    }
};

#endif