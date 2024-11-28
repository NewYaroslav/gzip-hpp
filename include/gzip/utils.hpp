#ifndef GZIP_UTILIS_HPP_INCLUDED
#define GZIP_UTILIS_HPP_INCLUDED

/// \file utils.hpp
/// \brief Utility functions for detecting zlib or gzip compressed data.
///
/// This file provides helper functions for checking whether data is compressed
/// using zlib or gzip formats. These functions are lightweight and do not
/// depend on zlib's internal structures or functions.
///
/// Example usage:
/// \code
/// #include <gzip/utils.hpp>
///
/// const char gzip_data[] = {0x1F, 0x8B, 0x08};
/// const std::string plain_data = "Hello, world!";
///
/// if (gzip::is_compressed(gzip_data, sizeof(gzip_data))) {
///     std::cout << "The data is compressed!" << std::endl;
/// }
/// if (!gzip::is_compressed(plain_data)) {
///     std::cout << "The data is not compressed." << std::endl;
/// }
/// \endcode

#include <cstdlib>

namespace gzip {

	/// \brief Checks if the given data is compressed (zlib or gzip).
	///
	/// These live in gzip.hpp because it doesnt need to use deps.
	/// Otherwise, they would need to live in impl files if these methods used
	/// zlib structures or functions like inflate/deflate)
	///
    /// \param data Pointer to the data to check.
    /// \param size Size of the data in bytes.
    /// \return True if the data matches the zlib header, false otherwise.
	inline bool is_compressed(const char* data, std::size_t size) {
		return size > 2 &&
			   (
				   // zlib
				   (
					   static_cast<uint8_t>(data[0]) == 0x78 &&
					   (static_cast<uint8_t>(data[1]) == 0x9C ||
						static_cast<uint8_t>(data[1]) == 0x01 ||
						static_cast<uint8_t>(data[1]) == 0xDA ||
						static_cast<uint8_t>(data[1]) == 0x5E)) ||
				   // gzip
				   (static_cast<uint8_t>(data[0]) == 0x1F && static_cast<uint8_t>(data[1]) == 0x8B));
	}

	/// \brief Checks if the given std::string is compressed (zlib or gzip).
    /// \param data The input data as a std::string.
    /// \return True if the data is compressed, false otherwise.
	inline bool is_compressed(const std::string& data) {
        return is_compressed(data.c_str(), data.size());
	}

	/// \brief Checks if the given std::vector<uint8_t> is compressed (zlib or gzip).
    /// \param binary_data The input data as a std::vector<uint8_t>.
    /// \return True if the data is compressed, false otherwise.
	inline bool is_compressed(const std::vector<uint8_t>& binary_data) {
        return is_compressed(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
	}

} // namespace gzip

#endif
