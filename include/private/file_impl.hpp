#pragma once

#include <fstream>

#include "file.hpp"
#include "common.hpp"

namespace readWDpp {
    class file::impl {
        public:
            explicit impl(const std::string& filename);
            ~impl();

        private:

            enum class header_type {
                FILE,
                TIME,
                EVENT,
                BOARD,
                CHANNEL,
                UNDEFINED
            };

            bool initialize();
            bool _read_header();
            bool _read_board();
            bool _read_channel();

            std::ifstream       _file;
            daq_type            _daq_type;
            std::vector<board>  _boards;

            uint16_t    _cur_bd_sn;
            uint32_t    _cur_ch_idx;
            header_type _prev_header;


            bool _is_init;
        
    };
}