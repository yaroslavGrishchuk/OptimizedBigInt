

#ifndef BIGINT_SPARE_VECTOR_H
#define BIGINT_SPARE_VECTOR_H

#include <vector>
#include <memory>
#include <cassert>
#include <cstring>

using std::shared_ptr;
using std::vector;

class spare_vector {

    union data {
        uint32_t small;
        shared_ptr<vector<uint32_t>> big;

        data() {};

        ~data() {};
    } data;

    size_t _size;
    bool is_single_number;


    void vec_from_number();

    void new_numb();

public:

    spare_vector();

    ~spare_vector();

    spare_vector &operator=(spare_vector const &other);

    spare_vector(spare_vector const &other);

    size_t size() const;

    uint32_t back() const;

    void push_back(uint32_t x);

    void pop_back();

    uint32_t &operator[](size_t ind);

    uint32_t const &operator[](size_t ind) const;

    void clear();

#endif //BIGINT_SPARE_VECTOR_H
};