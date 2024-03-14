#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>

// Define htonll() and ntohll() if not available
#if !defined(htobe64) && !defined(be64toh)
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        #define htonll(x) __builtin_bswap64(x)
        #define ntohll(x) __builtin_bswap64(x)
    #else
        #define htonll(x) (x)
        #define ntohll(x) (x)
    #endif
#endif

// Function to serialize a uint8_t into a byte array
size_t serialize_uint8(uint8_t *buffer, uint8_t value);

// Function to deserialize a uint8_t from a byte array
size_t deserialize_uint8(const uint8_t *buffer, uint8_t *value);

// Function to serialize a uint16_t into a byte array (big-endian)
size_t serialize_uint16(uint8_t *buffer, uint16_t value);

// Function to deserialize a uint16_t from a byte array (host byte order)
size_t deserialize_uint16(const uint8_t *buffer, uint16_t *value);

// Function to serialize a uint32_t into a byte array (big-endian)
size_t serialize_uint32(uint8_t *buffer, uint32_t value);

// Function to deserialize a uint32_t from a byte array (host byte order)
size_t deserialize_uint32(const uint8_t *buffer, uint32_t *value);

// Function to serialize a uint64_t into a byte array (big-endian)
size_t serialize_uint64(uint8_t *buffer, uint64_t value);

// Function to deserialize a uint64_t from a byte array (host byte order)
size_t deserialize_uint64(const uint8_t *buffer, uint64_t *value);

// Function to serialize an int8_t into a byte array
size_t serialize_int8(uint8_t *buffer, int8_t value);

// Function to deserialize an int8_t from a byte array
size_t deserialize_int8(const uint8_t *buffer, int8_t *value);

// Function to serialize an int16_t into a byte array (big-endian)
size_t serialize_int16(uint8_t *buffer, int16_t value);

// Function to deserialize an int16_t from a byte array (host byte order)
size_t deserialize_int16(const uint8_t *buffer, int16_t *value);

// Function to serialize an int32_t into a byte array (big-endian)
size_t serialize_int32(uint8_t *buffer, int32_t value);

// Function to deserialize an int32_t from a byte array (host byte order)
size_t deserialize_int32(const uint8_t *buffer, int32_t *value);

// Function to serialize an int64_t into a byte array (big-endian)
size_t serialize_int64(uint8_t *buffer, int64_t value);

// Function to deserialize an int64_t from a byte array (host byte order)
size_t deserialize_int64(const uint8_t *buffer, int64_t *value);

#endif // SERIALIZATION_H