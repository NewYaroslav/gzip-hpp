#ifndef GZIP_DECOMPRESS_HPP_INCLUDED
#define GZIP_DECOMPRESS_HPP_INCLUDED

/// \file decompress.hpp
/// \brief Utility for gzip decompression of binary data.
///
/// This file defines a class and helper functions for decompressing
/// gzip-compressed data. The functionality supports input formats such as
/// `std::string` and `std::vector<uint8_t>` and provides decompressed output
/// in the same container type.
///
/// The implementation relies on zlib and includes memory usage checks to
/// prevent excessive resource consumption during decompression.
///
/// Example usage:
/// \code
/// #include <gzip/decompress.hpp>
///
/// // Decompress data from a std::string
/// std::string compressed_data = ...; // Load compressed data
/// std::string decompressed_data = gzip::decompress(compressed_data);
///
/// // Decompress data from a std::vector<uint8_t>
/// std::vector<uint8_t> compressed_vector = ...; // Load compressed data
/// std::vector<uint8_t> decompressed_vector = gzip::decompress(compressed_vector);
/// \endcode

#include <gzip/config.hpp>

// zlib
#include <zlib.h>

// std
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace gzip {

    /// \class Decompressor
    /// \brief A class for decompressing gzip data with size limits.
    class Decompressor {
        std::size_t max_; ///< Maximum allowed decompressed size in bytes.

      public:

        /// \brief Constructor with an optional maximum size.
        /// \param max_bytes The maximum allowed size for decompression (default: 2GB).
        Decompressor(std::size_t max_bytes = 2000000000) : // by default refuse operation if compressed data is > 2GB
            max_(max_bytes) {
        }

        /// \brief Decompresses gzip-compressed data into the output container.
        /// \tparam OutputType The type of the container for decompressed data (e.g., std::string, std::vector<uint8_t>).
        /// \param output The container to store decompressed data.
        /// \param data Pointer to the compressed data.
        /// \param size Size of the compressed data in bytes.
        /// \throws std::runtime_error On decompression failure or if size limits are exceeded.
        template <typename OutputType>
        void decompress(OutputType& output,
                        const char* data,
                        std::size_t size) const
        {
            z_stream inflate_s;

            inflate_s.zalloc = Z_NULL;
            inflate_s.zfree = Z_NULL;
            inflate_s.opaque = Z_NULL;
            inflate_s.avail_in = 0;
            inflate_s.next_in = Z_NULL;

            // The windowBits parameter is the base two logarithm of the window size (the size of the history buffer).
            // It should be in the range 8..15 for this version of the library.
            // Larger values of this parameter result in better compression at the expense of memory usage.
            // This range of values also changes the decoding type:
            //  -8 to -15 for raw deflate
            //  8 to 15 for zlib
            // (8 to 15) + 16 for gzip
            // (8 to 15) + 32 to automatically detect gzip/zlib header
            constexpr int window_bits = 15 + 32; // auto with windowbits of 15

#           pragma GCC diagnostic push
#           pragma GCC diagnostic ignored "-Wold-style-cast"
            if (inflateInit2(&inflate_s, window_bits) != Z_OK) {
                throw std::runtime_error("inflate init failed");
            }
#           pragma GCC diagnostic pop
            inflate_s.next_in = reinterpret_cast<z_const Bytef*>(data);

#           ifdef DEBUG
            // Verify if (size * 2) fits into unsigned int (used by zlib for avail_in)
            std::uint64_t size_64 = size * 2;
            if (size_64 > std::numeric_limits<unsigned int>::max()) {
                inflateEnd(&inflate_s);
                throw std::runtime_error("size arg is too large to fit into unsigned int type x2");
            }
#           endif
            // Check if the expected memory usage exceeds the allowed maximum.
            if (size > max_ || (size * 2) > max_) {
                inflateEnd(&inflate_s);
                throw std::runtime_error("size may use more memory than intended when decompressing");
            }
            inflate_s.avail_in = static_cast<unsigned int>(size);
            std::size_t size_uncompressed = 0;
            try {
                do {
                    // Calculate the new size for the output container to accommodate additional data.
                    std::size_t resize_to = size_uncompressed + 2 * size;
                    if (resize_to > max_) {
                        throw std::runtime_error("size of output string will use more memory then intended when decompressing");
                    }
                    output.resize(resize_to);
                    inflate_s.avail_out = static_cast<unsigned int>(2 * size);
                     // Set pointer to the next output position.
                    inflate_s.next_out = reinterpret_cast<Bytef*>(&output[0] + size_uncompressed);
                    int ret = inflate(&inflate_s, Z_FINISH);
                    if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR) {
                        std::string error_msg = inflate_s.msg;
                        throw std::runtime_error(error_msg);
                    }
                    // Update the total decompressed size.
                    size_uncompressed += (2 * size - inflate_s.avail_out);
                } while (inflate_s.avail_out == 0);
                // Resize the output container to the actual decompressed size.
                output.resize(size_uncompressed);
            } catch (...) {
                inflateEnd(&inflate_s);
                throw; // Rethrow the original exception.
            }
            inflateEnd(&inflate_s);
        }
    };

    /// \brief Decompress gzip data into a std::string.
    /// \param data Pointer to the compressed data.
    /// \param size Size of the compressed data.
    /// \param max_bytes Maximum allowed decompressed size in bytes (default: 2GB).
    /// \return A std::string containing decompressed data.
    inline std::string decompress(
            const char* data, 
            std::size_t size, 
            std::size_t max_bytes = 2000000000) {
        Decompressor decomp(max_bytes);
        std::string output;
        decomp.decompress(output, data, size);
        return output;
    }

    /// \brief Decompress gzip data from a std::string.
    /// \param data Compressed data as a std::string.
    /// \param max_bytes Maximum allowed decompressed size in bytes (default: 2GB).
    /// \return A std::string containing decompressed data.
    inline std::string decompress(
            const std::string& data, 
            std::size_t max_bytes = 2000000000) {
        Decompressor decomp(max_bytes);
        std::string output;
        decomp.decompress(output, data.c_str(), data.size());
        return output;
    }

    /// \brief Decompress gzip data from a std::vector<uint8_t>.
    /// \param binary_data Compressed data as a std::vector<uint8_t>.
    /// \param max_bytes Maximum allowed decompressed size in bytes (default: 2GB).
    /// \return A std::vector<uint8_t> containing decompressed data.
    inline std::vector<uint8_t> decompress(
            const std::vector<uint8_t>& binary_data, 
            std::size_t max_bytes = 2000000000) {
        Decompressor decomp(max_bytes);
        std::vector<uint8_t> output;
        decomp.decompress(output, reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
        return output;
    }

} // namespace gzip
#endif
