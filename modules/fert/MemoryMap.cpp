#include <iostream>
#include <cstddef>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "MemoryMap.hpp"

geco::CMemoryMap::CMemoryMap(const char *path)
{
    auto fp = open(path, O_RDONLY);
    if (fp == -1)
    {
        throw std::system_error(errno, std::system_category(), path);
    }

    struct stat sb;
    if (fstat(fp, &sb) == -1)
    {
        close(fp);
        throw std::system_error(errno, std::system_category(), path);
    }

    m_size = sb.st_size;
    buffer = static_cast<char *>(
        mmap(nullptr, m_size, PROT_READ, MAP_SHARED, fp, 0));

    close(fp);
    if (buffer == MAP_FAILED)
    {
        throw std::system_error(errno, std::system_category(), path);
    }
}

geco::CMemoryMap::~CMemoryMap()
{
    if (buffer != nullptr)
    {
        munmap(data(), size());
        buffer = nullptr;
        m_size = 0;
    }
}