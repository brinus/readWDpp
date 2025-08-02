#pragma once

#include <cstdint>
#include <string>

namespace readWDpp {

using event_index = uint64_t;
using time_vector = std::vector<float>;
using ampl_vector = std::vector<uint16_t>;

enum class boardType_t {
    DRS,
    WDB,
    LAB,
    Unknown
};

struct board {
    uint16_t index;
    uint16_t serial;
    uint32_t channel_count;
    std::vector<time_vector> time_bins;
};


} // namespace readWDpp
