#pragma once

#include <string>

namespace readWDpp
{
    class file
    {
    public:
        explicit file(const std::string &filename);
        ~file();

    private:
        class impl;
        std::unique_ptr<impl> _pimpl;
    };
}