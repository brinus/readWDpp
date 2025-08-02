#include "file_reader.hpp"
#include "file_reader_impl.hpp"

namespace readWDpp
{

file_reader::file_reader(const std::string &filename)
    : _pimpl(std::make_unique<impl>(filename))
{}

const std::string& file_reader::get_filename() const {
    return _pimpl->get_filename();
}

file_reader::~file_reader() = default;

}