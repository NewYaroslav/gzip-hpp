## GzipHpp C++ Library

Lightweight C++ library for gzip compression and decompression.

## Usage

```cpp
// Include the specific gzip headers your code needs, for example...
#include <gzip/compress.hpp>
#include <gzip/config.hpp>
#include <gzip/decompress.hpp>
#include <gzip/utils.hpp>
#include <gzip/version.hpp>

// Example input data
std::string data = "hello, gzip!";
const char* pointer = data.data();
std::size_t size = data.size();

// Check if the data is compressed (works with raw pointers and size)
bool isCompressed = gzip::is_compressed(pointer, size); // false

// Or check using std::string
bool isCompressed = gzip::is_compressed(data); // false

// Or check using std::vector<uint8_t>
std::vector<uint8_t> binary_data = {0x1F, 0x8B, 0x08};
bool isCompressedVector = gzip::is_compressed(binary_data);

// Compress data using raw pointer and size
std::string compressed_data = gzip::compress(pointer, size);

// Or compress using std::string
std::string compressed_data = gzip::compress(data);

// Decompress data using raw pointer and size
std::string decompressed_data = gzip::decompress(compressed_data.data(), compressed_data.size());

// Or decompress using std::string
std::string decompressed_data = gzip::decompress(compressed_data);

// Verify decompression
assert(data == decompressed_data);
```

#### Compress

```cpp
// Optionally include compression level
std::size_t size; // No default value, but what happens when not passed??
int level = Z_DEFAULT_COMPRESSION; // Z_DEFAULT_COMPRESSION is the default if no arg is passed

std::string compressed_data = gzip::compress(tile->data(), size, level);
```
#### Decompress

```cpp
#include <gzip/decompress.hpp>

std::string compressed_data = gzip::compress("hello, gzip!");

// Decompress raw data
std::string decompressed_data = gzip::decompress(compressed_data.data(), compressed_data.size());

// Decompress std::string
std::string decompressed_data_string = gzip::decompress(compressed_data);

// Decompress std::vector<uint8_t>
std::vector<uint8_t> compressed_vector = gzip::compress(std::vector<uint8_t>{'h', 'e', 'l', 'l', 'o'});
std::vector<uint8_t> decompressed_vector = gzip::decompress(compressed_vector);
```

## Test

```shell
# build test binaries
make

# run tests
make test
```

You can make Release test binaries as well

```shell
BUILDTYPE=Release make
BUILDTYPE=Release make test
```

## Versioning

This library is semantically versioned using the /include/gzip/version.cpp file. This defines a number of macros that can be used to check the current major, minor, or patch versions, as well as the full version string.

Here's how you can check for a particular version to use specific API methods

```cpp
#if GZIP_VERSION_MAJOR > 2
// use version 2 api
#else
// use older verion apis
#endif
```

Here's how to check the version string

```cpp
std::cout << "version: " << GZIP_VERSION_STRING << "/n";
// => version: 0.2.0
```

And lastly, mathematically checking for a specific version:

```cpp
#if GZIP_VERSION_CODE > 20001
// use feature provided in v2.0.1
#endif
```
