#include "tbz.hpp"
#include "TextToText.hpp"
#include <cassert>

int tbz2tar(const void *tbz, size_t tbz_size, std::string& tar)
{
    return bzlib_decomp(tar, tbz, tbz_size);
}

int tar2tbz(const void *tar, size_t tar_size, std::string& tbz)
{
    return bzlib_comp(tbz, tar, tar_size);
}

int tar2bins(const void *tar, size_t tar_size, BINS& bins)
{
    bins.clear();

    mtar_header_t h;
    mtar_wrap mtar;
    int error;

    error = mtar.open_memory((void *)tar, tar_size);
    if (error)
    {
        assert(0);
        return error;
    }

    FILE_ITEM file_item;
    for (;;)
    {
        error = mtar.read_header(&h);
        if (error)
        {
            if (error == MTAR_ENULLRECORD)
                error = 0;
            else
                assert(0);
            break;
        }

        file_item.filename = Utf8ToWide(h.name);
        file_item.binary.resize(h.size);

        error = mtar.read_data(&file_item.binary[0], h.size);
        if (error)
        {
            assert(0);
            break;
        }

        bins.push_back(file_item);

        mtar.next();
    }

    return error;
}

int bins2tar(const BINS& bins, std::string& tar)
{
    tar.clear();

    mtar_wrap mtar;
    int error = mtar.open_memory(NULL, 0);
    if (error)
    {
        assert(0);
        return error;
    }

    for (size_t i = 0; i < bins.size(); ++i)
    {
        const FILE_ITEM& file_item = bins[i];

        std::string filename = WideToUtf8(file_item.filename);

        error = mtar.write_file_header(filename.c_str(), file_item.binary.size());
        if (error)
        {
            assert(0);
            break;
        }

        error = mtar.write_data(file_item.binary.c_str(), file_item.binary.size());
        if (error)
        {
            assert(0);
            break;
        }
    }

    if (!error)
    {
        error = mtar.finalize();
        if (error)
        {
            assert(0);
        }
        else
        {
            tar.resize(mtar.memory_size());
            memcpy(&tar[0], mtar.memory(), mtar.memory_size());
        }
    }

    return error;
}
