#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <vector>
#include "spare_vector.h"
using std::vector;

class big_integer {
public:
    vector<uint32_t > value;
    //spare_vector value;
    bool sign;

    void clear_pref();
    void addition(size_t len);
    size_t length() const;
    bool is_zero() const;
    big_integer increment() const;
    big_integer invert() const;
    big_integer convert_to_add_two() const;
    big_integer convert_from_add_two() const;

    big_integer div_by_ten(uint32_t *ost) const;

    static int compare(big_integer const &a, big_integer const &b);

    static void mul_long_short(big_integer & res, big_integer const & a, uint32_t const b);
    static bool prefix_compare(big_integer const &v, big_integer const &check, size_t const pref_len);
    static void prefix_sub(big_integer &v, big_integer const &check, size_t const pref_len);

    static big_integer bit_op(big_integer const &left, big_integer const &right, uint32_t (*f)(uint32_t a, uint32_t b));

    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    explicit big_integer(std::string const& str);

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);
};


big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);

#endif // BIG_INTEGER_H
