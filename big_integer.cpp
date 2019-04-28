#include "big_integer.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <stdio.h>
#include <assert.h>

const uint32_t LOG2_BASE = 32;

// Removes redundant digits in data
void refresh(big_integer &a) {
    while (a.data.size() > 1 && a.data.back() == 0) {
        a.data.pop_back();
    }

    if (a.data.size() == 1 && a.data[0] == 0) {
        a.negative = false;
    }
}

// Setting ZERO
big_integer::big_integer() : data(1), negative(false) {}

// big_integer a = (big_integer) b
big_integer::big_integer(big_integer const &other) : data(other.data), negative(other.negative) {
    refresh(*this);
}

uint32_t cast_to_unsigned(int32_t a) {
    auto b = (int64_t) a;
    return (uint32_t) (b < 0 ? -b : b);
}

// big_integer a = (int) b
big_integer::big_integer(int32_t a) : data(1), negative(a < 0) {
    data[0] = cast_to_unsigned(a);
    refresh(*this);
}

big_integer::big_integer(uint32_t a) : data(1), negative(false) {
    data[0] = a;
}

// big_integer a = (std::string) b, initialize negative is strictly needed
big_integer::big_integer(std::string const &str) : data(1), negative(false) {
    assert (!str.empty());
    for (size_t i = (str[0] == '-' ? 1 : 0); i < str.length(); ++i) {
        assert ('0' <= str[i] && str[i] <= '9');
        *this += str[i] - '0';
        *this *= 10;
    }
    *this /= 10;
    negative = (str[0] == '-');
    refresh(*this);
}

// big_integer destructor
big_integer::~big_integer() = default;

big_integer &big_integer::operator=(big_integer const &other) {
    data = other.data;
    negative = other.negative;
    refresh(*this);
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    return *this = *this + rhs;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this = *this - rhs;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    return *this = *this * rhs;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    return *this = *this / rhs;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this = *this % rhs;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return *this = *this & rhs;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return *this = *this | rhs;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return *this = *this ^ rhs;
}

big_integer &big_integer::operator<<=(int rhs) {
    return *this = *this << rhs;
}

big_integer &big_integer::operator>>=(int rhs) {
    return *this = *this >> rhs;
}

// Returns this
big_integer big_integer::operator+() const {
    return *this;
}

// Returns opposite
big_integer big_integer::operator-() const {
    big_integer res(*this);
    res.negative = !res.negative;
    refresh(res);
    return res;
}

// Returns inverted
big_integer big_integer::operator~() const {
    return -*this - 1;
}

// ++a
big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

// a++
big_integer big_integer::operator++(int) {
    big_integer res = *this;
    ++*this;
    return res;
}

// --a
big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

// a--
big_integer big_integer::operator--(int) {
    big_integer res = *this;
    --*this;
    return res;
}

big_integer operator+(big_integer a, big_integer const &b) {
    if (a.negative) {
        if (b.negative) {
            return -(-a + -b);
        } else {
            return b - -a;
        }
    } else if (b.negative) {
        return a - -b;
    }

    uint32_t carry = 0;
    size_t res_len = std::max(a.data.size(), b.data.size()) + 1;
    a.data.resize(res_len);
    for (size_t i = 0; i < res_len || carry > 0; ++i) {
        uint64_t sum = (uint64_t) carry + a.data[i] + b.get_digit(i, false);
        a.data[i] = (uint32_t) sum;
        carry = sum >> LOG2_BASE;
    }

    refresh(a);
    return a;
}

big_integer operator-(big_integer a, big_integer const &b) {
    if (a.negative) {
        if (b.negative) {
            return -(-a - -b);
        } else {
            return -(-a + b);
        }
    } else if (b.negative) {
        return a + b;
    }

    if (a < b) {
        return -(b - a);
    }

    uint32_t carry = 0;
    for (size_t i = 0; i < b.data.size() || carry > 0; ++i) {
        uint64_t difference = (uint64_t) a.data[i] - b.get_digit(i, false) - carry;
        a.data[i] = (uint32_t) difference;
        carry = difference >> (LOG2_BASE * 2 - 1);
    }

    refresh(a);
    return a;
}

big_integer operator*(big_integer a, big_integer const &b) {
    big_integer res;
    res.negative = a.negative ^ b.negative;
    res.data.resize(a.data.size() + b.data.size());
    for (size_t i = 0; i < a.data.size(); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b.data.size() || carry > 0; ++j) {
            uint64_t product = res.data[i + j] + (uint64_t) a.data[i] * b.get_digit(j, false) + carry;
            res.data[i + j] = (uint32_t) product;
            carry = product >> LOG2_BASE;
        }
    }

    refresh(res);
    return res;
}

big_integer operator/(big_integer a, int32_t b) {
    big_integer res = a / cast_to_unsigned(b);
    res.negative ^= b < 0;
    refresh(res);
    return res;
}

big_integer operator/(big_integer a, uint32_t b) {
    uint32_t carry = 0;
    for (size_t i = a.data.size() - 1; i != (size_t) (-1); --i) {
        uint64_t sum = ((uint64_t) carry << LOG2_BASE) + a.data[i];
        a.data[i] = (uint32_t) (sum / b);
        carry = sum % b;
    }

    refresh(a);
    return a;
}

big_integer operator/(big_integer a, big_integer const &b) {
    if (a.negative) {
        return -(-a / b);
    }
    if (b.negative) {
        return -(a / -b);
    }

    if (a < b) {
        return 0;
    }
    if (b.data.size() == 1) {
        return a / b.data[0];
    }

    auto factor = (uint32_t) ((1ull << LOG2_BASE) / ((uint64_t) (b.data.back() + 1)));
    big_integer r = a * factor, d = b * factor, res;
    res.negative = false;
    size_t n = d.data.size(), res_len = r.data.size() - n;
    res.data.resize(res_len);
    if (r >= d << LOG2_BASE * res_len) {
        res.data.push_back(1);
        r -= d << LOG2_BASE * res_len;
    }

    for (size_t i = res_len - 1; i != (size_t) (-1); --i) {
        size_t pos = n + i;
        r.data.resize(std::max(r.data.size(), pos + 1));

        uint64_t quotient = (((uint64_t) r.data[pos] << LOG2_BASE) + r.data[pos - 1]) / d.data[n - 1];
        uint32_t q = (quotient < UINT32_MAX ? quotient : UINT32_MAX);
        r -= (q * d) << (LOG2_BASE * i);

        while (r < 0) {
            --q;
            r += (d << (LOG2_BASE * i));
        }

        res.data[i] = q;
    }

    refresh(res);
    return res;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a - (a / b) * b;
}

big_integer bit_inverse(big_integer a) {
    ++a;
    for (uint32_t &i : a.data) {
        i = ~i;
    }

    return a;
}

uint32_t big_integer::get_digit(size_t pos, bool bit) const {
    auto out_of_range = (bit && negative ? UINT32_MAX : 0);
    return (pos >= data.size() ? out_of_range : data[pos]);
}

big_integer bit_operation(big_integer a, big_integer const &b, const std::function<uint32_t(uint32_t, uint32_t)> &bit_op) {
    big_integer x = (a.negative ? bit_inverse(a) : a);
    big_integer y = (b.negative ? bit_inverse(b) : b);

    big_integer res;
    res.data.resize(std::max(x.data.size(), y.data.size()));
    for (size_t i = 0; i < res.data.size(); ++i) {
        res.data[i] = (bit_op)(x.get_digit(i, true), y.get_digit(i, true));
    }
    res.negative = (bit_op)(x.negative, y.negative);
    if (res.negative) {
        res = bit_inverse(--res);
        --res;
    }

    return res;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return bit_operation(a, b, std::bit_and<uint32_t>());
}

big_integer operator|(big_integer a, big_integer const &b) {
    return bit_operation(a, b, std::bit_or<uint32_t>());
}

big_integer operator^(big_integer a, big_integer const &b) {
    return bit_operation(a, b, std::bit_xor<uint32_t>());
}

big_integer operator<<(big_integer a, int32_t b) {
    if (b < 0) {
        return a >> -b;
    }

    size_t shift = (size_t) b & (LOG2_BASE - 1);
    uint32_t carry = 0;
    for (uint32_t &i : a.data) {
        uint64_t shifted = ((uint64_t) i << shift) | carry;
        i = (uint32_t) shifted;
        carry = shifted >> LOG2_BASE;
    }
    if (carry > 0) {
        a.data.push_back(carry);
    }

    size_t div = (size_t) b / LOG2_BASE;
    a.data.resize(a.data.size() + div);
    for (size_t i = a.data.size() - 1; i != (size_t) (-1); --i) {
        a.data[i] = ((size_t) i >= div ? a.data[i - div] : 0);
    }

    return a;
}

big_integer operator>>(big_integer a, int32_t b) {
    if (b < 0) {
        return a << -b;
    }

    bool reminder_exists = false;
    size_t shift = (size_t) b & (LOG2_BASE - 1), div = (size_t) b / LOG2_BASE;
    uint32_t carry = 0;
    for (size_t i = 0; i < a.data.size(); ++i) {
        reminder_exists = a.data[i] != 0 && div > 0;
        a.data[i] = a.data[i + div];
    }
    a.data.resize(a.data.size() - div);
    for (size_t i = a.data.size() - 1; i != (size_t) (-1); --i) {
        uint64_t shifted = (uint64_t) a.data[i] << (LOG2_BASE - shift);
        a.data[i] = (shifted >> LOG2_BASE) | carry;
        carry = (uint32_t) shifted;
    }

    if (a.negative && (reminder_exists || carry != 0)) {
        --a;
    }

    refresh(a);
    return a;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return (a.negative == b.negative && a.data == b.data);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.negative ^ b.negative) {
        return a.negative;
    }
    if (a.data.size() != b.data.size()) {
        return a.negative ^ (a.data.size() < b.data.size());
    }

    for (size_t i = a.data.size() - 1; i != (size_t) (-1); --i) {
        if (a.data[i] != b.data[i]) {
            return (a.negative ^ (a.data[i] < b.data[i]));
        }
    }

    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

std::string to_string(big_integer const &a) {
    if (a == 0) {
        return "0";
    }

    std::string res;
    big_integer cpy = a;
    while (cpy != 0) {
        res += (char) ((cpy % 10).data[0] + '0');
        cpy /= 10;
    }

    if (a.negative) {
        res += '-';
    }
    std::reverse(res.begin(), res.end());
    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}