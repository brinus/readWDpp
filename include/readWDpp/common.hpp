#pragma once

#include <array>
#include <vector>

namespace readWDpp {

    inline constexpr std::size_t N_SAMPLES = 1024;

    enum class daq_type {
        DRS,
        WDB,
        LAB,
        Undefined
    };

    using ampl_type = uint16_t;
    using time_type = float;

    using ampl_vector = std::array<ampl_type, N_SAMPLES>;
    using time_vector = std::array<time_type, N_SAMPLES>;

    struct channel {
        std::size_t     channel_index;
        time_vector     times;
        ampl_vector     ampls;
    };

    struct board {
        std::size_t             board_index;
        uint16_t                board_serial;
        std::vector<channel>    channels;
    };

}