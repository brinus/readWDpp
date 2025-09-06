#include <iostream>

#include "file_impl.hpp"

namespace readWDpp
{
    file::impl::impl(const std::string &filename)
        : _file(filename, std::ios::binary), _daq_type(daq_type::Undefined), _boards(), _cur_bd_sn(0), _cur_ch_idx(0), _prev_header(header_type::UNDEFINED)
    {
        _is_init = initialize();
    }

    file::impl::~impl() = default;

    bool file::impl::initialize()
    {

        _read_header();
        /*
        while(_read_board()) {
            while(_read_channel()) {
                _read_times_array()
            }
        }
        */

        return true;
    }

    bool file::impl::_read_header()
    {
        char header[4];
        _file.read(header, 4);
        unsigned int version = header[3] - '0';

        std::cout << "--- ReadWDpp Library ---------------------" << std::endl;

        if (header[0] == 'D' && header[1] == 'R' && header[2] == 'S')
        {
            if (version < 8)
                _daq_type = daq_type::DRS;
            else if (version == 8)
                _daq_type = daq_type::WDB;
            else
                throw std::runtime_error("Error reading FILE header!");
            _file.read(header, 4);
        }

        if (strcmp(header, "TIME") == 0)
        {
            switch (_daq_type)
            {
            case daq_type::DRS:
            case daq_type::WDB:
                return true;
            case daq_type::Undefined:
                _daq_type = daq_type::LAB;
                return true;
            default:
                throw std::runtime_error("Error reading TIME header!");
            }
        }

        std::string label;
        switch (_daq_type)
        {
        case daq_type::DRS:
            label = "DRS";
            break;
        case daq_type::WDB:
            label = "WDB";
            break;
        case daq_type::LAB:
            label = "LAB";
            version = 0;
            break;
        }

        std::cout << " DAQ Type: " << label << " (DRS" << version << ")" << std::endl;

        return false;
    }
}