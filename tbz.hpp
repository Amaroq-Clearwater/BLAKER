#ifndef TBZ_HPP_
#define TBZ_HPP_

#include <vector>
#include "mtar_wrap.hpp"
#include "comp_decomp_bzlib.hpp"

struct FILE_ITEM
{
    std::wstring filename;
    std::string binary;
};
typedef std::vector<FILE_ITEM> BINS;

int tbz2tar(const void *tbz, size_t tbz_size, std::string& tar);
int tar2tbz(const void *tar, size_t tar_size, std::string& tbz);

int tar2bins(const void *tar, size_t tar_size, BINS& bins);
int bins2tar(const BINS& bins, std::string& tar);

#endif  // ndef TBZ_HPP_
