#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "file_reader.hpp"
#include "common.hpp"

namespace readWDpp {

using event_pos        = std::streampos;
using event_pos_vector = std::vector<event_pos>;

class file_reader::impl {
    public:
        explicit impl(const std::string& filename);
        ~impl();

        const std::string& get_filename()       const { return _filename; }
        bool is_open()                          const { return _is_open; }
        boardType_t get_board_type()            const { return _board_type; }
        event_index get_current_event_index()   const { return _current_event_index; }

    private:
        void initialize();
        void print_opening_file_message() const;

    private:
        std::ifstream       _file;
        std::string         _filename;
        bool                _is_open;
        event_index         _current_event_index;
        event_pos_vector    _event_offsets;
        event_pos           _time_header_pos;
        boardType_t         _board_type;

        std::map<uint16_t, board> _boards;
};

} // namespace readWDpp