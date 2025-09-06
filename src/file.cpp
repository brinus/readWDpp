#include "file.hpp"
#include "file_impl.hpp"

namespace readWDpp
{
    file::file(const std::string &filename)
        : _pimpl(std::make_unique<file::impl>(filename))
    {
    }

    file::~file() = default;
}