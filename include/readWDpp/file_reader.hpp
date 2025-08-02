#pragma once

#include <string>
#include <memory>

namespace readWDpp {

    class file_reader {
    public:
        explicit file_reader(const std::string &filename);
        ~file_reader();

        file_reader(const file_reader &)            = delete;
        file_reader &operator=(const file_reader &) = delete;
        file_reader(file_reader &&)                 = delete;
        file_reader &operator=(file_reader &&)      = delete;

        const std::string& get_filename() const;

    private:
        class impl;
        std::unique_ptr<impl> _pimpl;

    };

} // namespace readWDpp