#ifndef READWDPP_FILE_H
#define READWDPP_FILE_H

#include <string>
#include <memory>

namespace readWDpp {

class file final {
public:
    explicit file(const std::string& fname);
    ~file();

private:
    void initialize();

    class pimpl;

    using _Ptr_Impl = std::unique_ptr<pimpl>;

    _Ptr_Impl   _pimpl;
    std::string _fname;
};

} // namespace readWDpp

#endif // READWDPP_FILE_H