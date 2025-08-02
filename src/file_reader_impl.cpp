#include "file_reader.hpp"
#include "file_reader_impl.hpp"

#include <iostream>

namespace readWDpp
{

file_reader::impl::impl(const std::string& filename)
    : _filename(filename)
    , _is_open(false)
    , _current_event_index(0) 
    , _event_offsets()
    , _time_header_pos(0)
    , _board_type(boardType_t::Unknown)
    , _boards(){

    try {
        _file.open(_filename, std::ios::binary);
        if (_file.is_open()) {
            _is_open = true;
            initialize();
        } else {
            throw std::runtime_error("Failed to open file: " + _filename);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Error initializing file_reader: " + std::string(e.what()));
    }

}

file_reader::impl::~impl() = default;

void file_reader::impl::initialize() {

    print_opening_file_message();

    char header[4];
    uint version;
    _file.read(header, 4);
    if (_file.gcount() != 4) {
        throw std::runtime_error("File too short or error reading initial header");
    }

    if (header[0] == 'D' && header[1] == 'R' && header[2] == 'S') {
        version = static_cast<unsigned char>(header[3]) - '0';
        if (version == 8) {
            _board_type = boardType_t::WDB;
            _file.read(header, 4);
        } else if (version < 8 && version > 0) {
            _board_type = boardType_t::DRS;
            _file.read(header, 4);
        } else {
            throw std::runtime_error("Invalid board type in file header");
        }
    }

    if (memcmp(header, "TIME", 4) == 0) {
        if (_board_type == boardType_t::Unknown) {
            _board_type = boardType_t::LAB;
        }
    } else {
        throw std::runtime_error("Error reading TIME header");
    }

    switch (_board_type) {
        case boardType_t::DRS:
            std::cout << " File type: DRS" << version << std::endl;
            break;
        case boardType_t::WDB:
            std::cout << " File type: WDB" << std::endl;
            break;
        case boardType_t::LAB:
            std::cout << " File type: DRS (LAB)" << std::endl;
            break;
        default:
            throw std::runtime_error("Unknown board type in file header");
    }

    _time_header_pos = _file.tellg();

    uint16_t board_index = 0;

    _boards.clear();
    while (true) {
        char board_header[2];
        _file.read(board_header, 2);
        if (_file.gcount() != 2 || board_header[0] != 'B' || board_header[1] != '#') {
            _file.seekg(-2, std::ios::cur);
            break;
        }
        uint16_t board_number = 0;
        _file.read(reinterpret_cast<char*>(&board_number), sizeof(board_number));
        readWDpp::board board{};
        board.index = static_cast<uint16_t>(_boards.size());
        board.serial = board_number;
        board.channel_count = 0;
        board.time_bins.clear();
        while (true) {
            char ch_header[4];
            _file.read(ch_header, 4);
            if (_file.gcount() != 4 || ch_header[0] != 'C') {
                _file.seekg(-4, std::ios::cur);
                break;
            }
            readWDpp::time_vector time_bins(1024);
            _file.read(reinterpret_cast<char*>(time_bins.data()), 1024 * sizeof(float));
            if (_file.gcount() != 1024 * sizeof(float)) {
                throw std::runtime_error("Error reading time bin width vector");
            }
            board.time_bins.push_back(std::move(time_bins));
        }
        board.channel_count = static_cast<uint32_t>(board.time_bins.size());
        _boards[board.index] = std::move(board);
    }
    std::cout << " Board list:" << std::endl;
    for (const auto& [key, board] : _boards) {
        std::cout << "\tB#" << board.serial << " (" << board.channel_count << " channels)" << std::endl;
    }
}

void file_reader::impl::print_opening_file_message() const {
    std::cout << " --- ReadWDpp Library --------------------------" << std::endl;
    std::cout << " Opening file: " << _filename << std::endl;
}

} // namespace readWDpp