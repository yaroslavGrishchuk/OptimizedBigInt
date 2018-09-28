#include "big_integer.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>


const uint32_t MAX = UINT32_MAX;
const uint64_t MORE_MAX = uint64_t(MAX) + 1;

size_t big_integer::length() const
{
    return value.size();
}

bool big_integer::is_zero() const
{
    return value.size() == 0;
}

void big_integer::clear_pref()
{
    while (length() > 0 && value.back() == 0)
    {
        value.pop_back();
    }
}

big_integer big_integer::increment() const
{
    big_integer res(*this);
    bool carry = true;
    uint64_t sum;
    for (size_t i = 0; i < res.length(); i++)
    {
        sum = uint64_t(res.value[i]) + carry;
        res.value[i] = (uint32_t) sum;
        carry = (bool) (sum & MORE_MAX);
    }
    return res;
}

big_integer big_integer::invert() const
{
    big_integer res(*this);
    for (size_t i = 0; i < res.length(); i++)
    {
        res.value[i] = ~res.value[i];
    }
    return res;
}

big_integer big_integer::convert_to_add_two() const
{
    big_integer res(*this);
    res.value.push_back(0);
    if (!sign)
    {
        return res;
    }
    res = res.invert().increment();
    return res;
}

big_integer big_integer::convert_from_add_two() const
{
    big_integer res(*this);
    if (res.sign)
    {
        res = res.invert().increment();
    }
    res.clear_pref();
    return res;
}

void big_integer::addition(size_t len)
{
    while (length() < len) {
        value.push_back(sign ? MAX : 0);
    }
}

big_integer::big_integer() : sign(false) {}

big_integer::big_integer(big_integer const &other) : value(other.value), sign(other.sign) {}

big_integer::big_integer(int a)
{
    if (a == 0)
    {
        sign = false;
        return;
    }
    if (a == INT32_MIN)
    {
        value.push_back(2147483648);
    }
    else
    {
        value.push_back(static_cast<uint32_t>(std::abs(a)));
    }
    sign = (a < 0);
}

big_integer::big_integer(const std::string &str)
{
    sign = false;
    for (size_t i = (str[0] == '-' ? 1 : 0); i < str.length(); i++)
    {
        mul_long_short(*this, *this, 10);
        *this += (int) (str[i] - '0');
    }
    sign = (str[0] == '-');
    clear_pref();
}

big_integer &big_integer::operator=(const big_integer &other) = default;

big_integer &big_integer::operator+=(const big_integer &rhs)
{
    big_integer first = convert_to_add_two(),
            second = rhs.convert_to_add_two();
    size_t len = std::max(first.length(), second.length());
    first.addition(len);
    second.addition(len);
    bool carry = false;
    for (size_t i = 0; i < len; i++)
    {
        uint64_t buf = uint64_t(first.value[i]) + second.value[i] + carry;
        first.value[i] = uint32_t(buf);
        carry = buf > MAX;
    }
    first.sign = static_cast<bool>(first.value.back() >> 31);
    return *this = first.convert_from_add_two();
}

big_integer &big_integer::operator-=(const big_integer &rhs)
{
    return (*this) += (-rhs);
}

big_integer &big_integer::operator*=(const big_integer &rhs)
{
    if (is_zero() || rhs.is_zero())
    {
        return *this = big_integer();
    }
    big_integer res;
    size_t len = std::max(length(), rhs.length());
    res.addition(2 * len);
    uint64_t carry, mul, buf;
    for (size_t i = 0; i < length(); ++i)
    {
        carry = 0;
        for (size_t j = 0; j < rhs.length(); ++j)
        {
            mul = uint64_t(value[i]) * rhs.value[j];
            buf = (mul & MAX) + carry + res.value[i + j];
            res.value[i + j] = uint32_t(buf);
            carry = (mul >> 32) + (buf >> 32);
        }
        res.value[i + rhs.length()] += carry;
    }
    res.sign = sign ^ rhs.sign;
    res.clear_pref();
    return *this = res;
}

big_integer &big_integer::operator/=(const big_integer &rhs)
{
    big_integer res;
    if (length() < rhs.length())
        return *this = res;

    big_integer remainder(*this);
    big_integer divisor(rhs);

    auto k = uint32_t(MORE_MAX / (uint64_t(divisor.value.back()) + 1));
    mul_long_short(remainder, remainder, k);
    mul_long_short(divisor, divisor, k);

    remainder.value.push_back(0);
    size_t pref_len = divisor.length() + 1;
    size_t dividend_len = remainder.length();
    res.addition(dividend_len - pref_len + 1);

    big_integer mul;
    uint32_t quotient;

    for (size_t i = pref_len; i <= dividend_len; ++i)
    {
        quotient = std::min(uint32_t(
                ((uint64_t(remainder.value.back()) << 32) + remainder.value[remainder.length() - 2]) /
                divisor.value.back()), MAX);

        mul_long_short(mul, divisor, quotient);
        while (!prefix_compare(remainder, mul, pref_len))
        {
            quotient--;
            mul_long_short(mul, divisor, quotient);
        }
        res.value[res.length() + pref_len - 1 - i] = quotient;
        prefix_sub(remainder, mul, pref_len);
        if (remainder.value.back() == 0)
        {
            remainder.value.pop_back();
        }
    }

    res.sign = sign ^ rhs.sign;
    res.clear_pref();
    return *this = res;
}

void big_integer::mul_long_short(big_integer &res, big_integer const &a, uint32_t const b)
{
    size_t len = a.length();
    res.addition(len + 1);
    uint64_t buf, carry = 0;
    for (size_t i = 0; i < len; ++i)
    {
        buf = uint64_t(a.value[i]) * b + carry;
        res.value[i] = uint32_t(buf);
        carry = (buf >> 32);
    }
    res.value[len] = uint32_t(carry);
    res.clear_pref();
}

bool big_integer::prefix_compare(big_integer const &v, big_integer const &check, size_t const pref_len)
{
    int len = int(v.length() - pref_len);
    uint32_t digit;
    for (int i = int(v.length() - 1); i >= len; i--)
    {
        digit = (static_cast<uint32_t>(i - len) < static_cast<uint32_t>(check.length()) ? check.value[i - len] : 0);
        if (v.value[i] > digit)
            return true;
        if (v.value[i] < digit)
            return false;
    }
    return true;
}

void big_integer::prefix_sub(big_integer &v, big_integer const &check, size_t const pref_len)
{
    size_t len = v.length() - pref_len;
    bool carry = false;
    uint32_t digit, sub;
    for (size_t i = 0; i < pref_len; ++i)
    {
        digit = (i < check.length() ? check.value[i] : 0);
        sub = v.value[len + i] - digit - carry;
        carry = v.value[len + i] < digit + carry;
        v.value[len + i] = sub;
    }
}

big_integer &big_integer::operator%=(const big_integer &rhs)
{
    return *this = (*this - *this / rhs * rhs);
}

big_integer big_integer::bit_op(big_integer const &left, big_integer const &right, uint32_t (*f)(uint32_t a, uint32_t b))
{
    big_integer first = left.convert_to_add_two(),
            second = right.convert_to_add_two();
    size_t len = std::max(first.length(), second.length());
    first.addition(len);
    second.addition(len);
    for (size_t i = 0; i < first.length(); ++i)
    {
        first.value[i] = f(first.value[i], second.value[i]);
    }
    first.sign = static_cast<bool>(f(static_cast<uint32_t>(first.sign), static_cast<uint32_t>(second.sign)));
    return first.convert_from_add_two();
}

big_integer &big_integer::operator&=(big_integer const &rhs)
{
    return *this = bit_op(*this, rhs, [](uint32_t a, uint32_t b) { return a & b; });
}

big_integer &big_integer::operator|=(big_integer const &rhs)
{
    return *this = bit_op(*this, rhs, [](uint32_t a, uint32_t b) { return a | b; });
}

big_integer &big_integer::operator^=(big_integer const &rhs)
{
    return *this = bit_op(*this, rhs, [](uint32_t a, uint32_t b) { return a ^ b; });
}

big_integer &big_integer::operator<<=(int rhs)
{
    if (sign)
    {
        *this = ~*this;
        *this = (*this <<= rhs) += 1;
        sign = true;
        return *this;
    }
    int shift;
    while (rhs > 0)
    {
        shift = std::min(rhs, 32);
        rhs -= shift;
        uint64_t result;
        uint64_t carry = 0;
        for (size_t i = 0; i < length(); i++)
        {
            uint32_t &digit = value[i];
            result = ((uint64_t(digit)) << shift);
            digit = (uint32_t) ((result & MAX) + carry);
            carry = (result >> 32);
        }
        if (carry != 0)
        {
            value.push_back((uint32_t) (carry));
        }
    }
    return *this;
}

big_integer &big_integer::operator>>=(int rhs)
{
    if (sign)
    {
        *this = ~*this;
        *this = (*this >>= rhs) += 1;
        sign = true;
        return *this;
    }
    int shift;
    while (rhs > 0)
    {
        shift = std::min(rhs, 32);
        rhs -= shift;
        uint64_t carry = 0;
        uint64_t result;
        for (size_t i = length(); i--;)
        {
            uint64_t carry1 = value[i] & (uint32_t(1 << (shift)) - 1);
            result = (value[i] >> shift);
            value[i] = uint32_t((result) + (carry << (32 - shift)));
            carry = carry1;
        }
    }
    clear_pref();
    return *this;
}

big_integer big_integer::operator+() const
{
    return *this;
}

big_integer big_integer::operator-() const
{
    big_integer res(*this);
    if (!res.is_zero())
    {
        res.sign ^= true;
    }
    return res;
}

big_integer big_integer::operator~() const
{
    big_integer res(*this);
    res++;
    res.sign ^= 1;
    return res;
}

big_integer &big_integer::operator++()
{
    return *this += 1;
}

big_integer big_integer::operator++(int)
{
    big_integer res = *this;
    *this += 1;
    return res;
}

big_integer &big_integer::operator--()
{
    return *this -= 1;
}

big_integer big_integer::operator--(int)
{
    big_integer res = *this;
    *this -= 1;
    return res;
}

int big_integer::compare(big_integer const &a, big_integer const &b)
{
    if (a.is_zero() && b.is_zero())
        return 0;
    if (a.is_zero())
        return b.sign ? 1 : -1;
    if (b.is_zero())
        return a.sign ? -1 : 1;
    if (a.sign != b.sign) {
        return a.sign ? -1 : 1;
    }
    big_integer first(a);
    big_integer second(b);
    size_t l = std::max(a.length(), b.length());
    first.addition(l);
    second.addition(l);
    for (size_t i = l; i--;)
    {
        if (first.value[i] > second.value[i])
        {
            return a.sign ? -1 : 1;
        }
        else if (first.value[i] < second.value[i])
        {
            return a.sign ? 1 : -1;
        }
    }
    return 0;
}

big_integer operator+(big_integer a, big_integer const &b)
{
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b)
{
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b)
{
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b)
{
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b)
{
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b)
{
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b)
{
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b)
{
    return a ^= b;
}

big_integer operator<<(big_integer a, int b)
{
    return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b)
{
    return big_integer::compare(a, b) == 0;
}

bool operator!=(big_integer const &a, big_integer const &b)
{
    return big_integer::compare(a, b) != 0;
}

bool operator<(big_integer const &a, big_integer const &b)
{
    return big_integer::compare(a, b) == -1;
}

bool operator>(big_integer const &a, big_integer const &b)
{
    return big_integer::compare(a, b) == 1;
}

bool operator<=(big_integer const &a, big_integer const &b)
{
    return big_integer::compare(a, b) != 1;
}

bool operator>=(big_integer const &a, big_integer const &b)
{
    return big_integer::compare(a, b) != -1;
}

big_integer big_integer::div_by_ten(uint32_t *ost) const
{
    uint64_t carry = 0;
    big_integer res;
    res.addition(length());
    for (int i = static_cast<int>(length() - 1); i >= 0; i--)
    {
        uint64_t cur = value[i] + carry * MORE_MAX;
        res.value[i] = uint32_t(cur / 10);
        carry = cur % 10;
    }
    //res.sign = !sign;
    *ost = uint32_t(carry);
    res.clear_pref();
    return res;
}

std::string to_string(big_integer const &a)
{
    if (a.is_zero())
        return std::string("0");
    std::string res;
    big_integer tmp = a;
    while (!tmp.is_zero())
    {
        uint32_t cur;
        tmp = tmp.div_by_ten(&cur);
        res.push_back('0' + char(cur));
    }
    if (a.sign)
        res.push_back('-');
    std::reverse(res.begin(), res.end());
    return res;
}