#include "serialize.h"

size_t serialize_uint8(uint8_t *buffer, uint8_t value) {
    *buffer = value;
    return sizeof(uint8_t);
}

size_t deserialize_uint8(const uint8_t *buffer, uint8_t *value) {
    *value = *buffer;
    return sizeof(uint8_t);
}

size_t serialize_uint16(uint8_t *buffer, uint16_t value) {
    value = htons(value);
    memcpy(buffer, &value, sizeof(uint16_t));
    return sizeof(uint16_t);
}

size_t deserialize_uint16(const uint8_t *buffer, uint16_t *value) {
    memcpy(value, buffer, sizeof(uint16_t));
    *value = ntohs(*value);
    return sizeof(uint16_t);
}

size_t serialize_uint32(uint8_t *buffer, uint32_t value) {
    value = htonl(value);
    memcpy(buffer, &value, sizeof(uint32_t));
    return sizeof(uint32_t);
}

size_t deserialize_uint32(const uint8_t *buffer, uint32_t *value) {
    memcpy(value, buffer, sizeof(uint32_t));
    *value = ntohl(*value);
    return sizeof(uint32_t);
}

size_t serialize_uint64(uint8_t *buffer, uint64_t value) {
    value = htonll(value);
    memcpy(buffer, &value, sizeof(uint64_t));
    return sizeof(uint64_t);
}

size_t deserialize_uint64(const uint8_t *buffer, uint64_t *value) {
    memcpy(value, buffer, sizeof(uint64_t));
    *value = ntohll(*value);
    return sizeof(uint64_t);
}

size_t serialize_int8(uint8_t *buffer, int8_t value) {
    *buffer = (uint8_t)value;
    return sizeof(int8_t);
}

size_t deserialize_int8(const uint8_t *buffer, int8_t *value) {
    *value = (int8_t)(*buffer);
    return sizeof(int8_t);
}

size_t serialize_int16(uint8_t *buffer, int16_t value) {
    value = htons(value);
    memcpy(buffer, &value, sizeof(int16_t));
    return sizeof(int16_t);
}

size_t deserialize_int16(const uint8_t *buffer, int16_t *value) {
    memcpy(value, buffer, sizeof(int16_t));
    *value = ntohs(*value);
    return sizeof(int16_t);
}

size_t serialize_int32(uint8_t *buffer, int32_t value) {
    value = htonl(value);
    memcpy(buffer, &value, sizeof(int32_t));
    return sizeof(int32_t);
}

size_t deserialize_int32(const uint8_t *buffer, int32_t *value) {
    memcpy(value, buffer, sizeof(int32_t));
    *value = ntohl(*value);
    return sizeof(int32_t);
}

size_t serialize_int64(uint8_t *buffer, int64_t value) {
    value = htonll(value);
    memcpy(buffer, &value, sizeof(int64_t));
    return sizeof(int64_t);
}

size_t deserialize_int64(const uint8_t *buffer, int64_t *value) {
    memcpy(value, buffer, sizeof(int64_t));
    *value = ntohll(*value);
    return sizeof(int64_t);
}