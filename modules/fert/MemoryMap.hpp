#pragma once

#include <cstddef>
#include <string>
#include <sys/stat.h>

namespace geco
{
    class CMemoryMap;
}

/// @brief A mapped memory region.
class geco::CMemoryMap
{
public:
    /// @brief Create a memory_map of a regular file.
    /// @param path Path to a file.
    /// @throw std::system_error if the memory_map cannot be created.
    explicit CMemoryMap(const char *path);

    /// @overload
    explicit CMemoryMap(const std::string &path) : CMemoryMap{path.c_str()} {}

    /// @brief Read-only access to the beginning of the mapped region.
    const char *data() const { return buffer; }

    /// @brief Read/write access to the beginning of the mapped region.
    char *data() { return buffer; }

    /// @brief Size in bytes of the mapped region.
    std::size_t size() const { return m_size; }

    /// @brief Destroy a memory_map.
    ~CMemoryMap();

private:
    char *buffer{nullptr};
    std::size_t m_size{0};
};