#include "opt_vector.h"

template <>
opt_vector<uint32_t>::opt_vector() : len(0), small_data(static_cast<uint32_t>(0)) {}

template <>
opt_vector<uint32_t>::opt_vector(size_t new_len) : len(new_len), small_data(0) {
    if (len > 1) {
        big_data = std::make_shared<std::vector<uint32_t>>(new_len);
    }
}

template <>
opt_vector<uint32_t>::opt_vector(size_t new_len, uint32_t new_val) : len(new_len) {
    if (len > 1) {
        big_data = std::make_shared<std::vector<uint32_t>>(new_len, new_val);
    } else if (len == 1) {
        small_data = new_val;
    }
}

template <>
opt_vector<uint32_t>::~opt_vector() {
    if (!is_small()) {
        big_data.reset();
    }
}

template <>
size_t opt_vector<uint32_t>::size() const {
    return len;
}

template <>
uint32_t opt_vector<uint32_t>::operator[](size_t pos) const {
    assert (pos < len);

    if (is_small()) {
        return small_data;
    }
    return big_data->operator[](pos);
}

template <>
uint32_t &opt_vector<uint32_t>::operator[](size_t pos) {
    assert (pos < len);

    if (is_small()) {
        return small_data;
    }
    make_unique();
    return big_data->operator[](pos);
}

template <>
uint32_t opt_vector<uint32_t>::back() const {
    assert (len > 0);

    if (is_small()) {
        return small_data;
    }
    return big_data->back();
}

template <>
void opt_vector<uint32_t>::resize(size_t new_len) {
    if (is_small()) {
        if (new_len > 1) {
            big_data = std::make_shared<std::vector<uint32_t>>(new_len);
            big_data->at(0) = small_data;
        }
    } else {
        if (new_len > 1) {
            make_unique();
            big_data->resize(new_len);
        } else {
            if (new_len == 1) {
                small_data = *big_data->begin();
            }
            big_data.reset();
        }
    }

    len = new_len;
}

template <>
void opt_vector<uint32_t>::push_back(uint32_t new_val) {
    switch (len++) {
        case 0:
            small_data = new_val;
            break;
        case 1:
            big_data = std::make_shared<std::vector<uint32_t>>(2);
            big_data->at(0) = small_data;
            big_data->at(1) = new_val;
            break;
        default:
            make_unique();
            big_data->push_back(new_val);
            break;
    }
}

template <>
void opt_vector<uint32_t>::pop_back() {
    assert (len > 1);

    switch (len--) {
        case 1:
            small_data = 0;
            break;
        case 2:
            small_data = *big_data->begin();
            big_data.reset();
            break;
        default:
            make_unique();
            big_data->pop_back();
            break;
    }
}

template <>
bool opt_vector<uint32_t>::operator==(opt_vector const& other) const {
    if (this->len != other.len) {
        return false;
    }
    if (this->is_small()) {
        return this->small_data == other.small_data;
    }
    return *this->big_data == *other.big_data;
}