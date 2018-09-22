
#include <iostream>
#include "spare_vector.h"

spare_vector::spare_vector() : _size(0), is_single_number(true) {
    data.small = 0;
}

spare_vector::spare_vector(spare_vector const &other) {
    _size = other._size;
    is_single_number = other.is_single_number;
    if (other.is_single_number) {
        data.small = other.data.small;
    } else {
        new(&data.big) std::shared_ptr<vector<uint32_t>>(other.data.big);
    }
}

size_t spare_vector::size() const {
    return _size;
}

uint32_t spare_vector::back() const {
    assert(_size > 0);
    return (is_single_number ? data.small : (*data.big.get())[_size - 1]);
}

void spare_vector::vec_from_number() {
    if (!is_single_number)
        return;
    is_single_number = false;
    uint32_t mas;
    mas = data.small;
    vector<uint32_t> *vect = new vector<uint32_t>(_size);
    new(&data.big) std::shared_ptr<std::vector<uint32_t>>(vect);
    (*data.big.get())[0] = mas;
}

void spare_vector::push_back(uint32_t x) {
    if (is_single_number && _size < 1) {
        data.small = x;
        _size++;
        return;
    }
    new_numb();
    vec_from_number();
    (*data.big.get()).push_back(x);
    _size++;
}

void spare_vector::pop_back() {
    assert(_size > 0);
    _size--;
    if (is_single_number) {
        return;
    }
    new_numb();
    (*data.big.get()).pop_back();
    if (data.big->size() == 1) {
        uint32_t tmp = data.big->back();
        data.big.reset();
        is_single_number = true;
        data.small = tmp;
    }
}

uint32_t &spare_vector::operator[](size_t ind) {
    assert(ind < _size);
    if (is_single_number)
        return data.small;
    new_numb();
    return (*data.big.get())[ind];
}

uint32_t const &spare_vector::operator[](size_t ind) const {
    assert(ind < _size);
    return (is_single_number ? data.small : (*data.big.get())[ind]);
}

spare_vector &spare_vector::operator=(spare_vector const &other) {
    _size = other._size;

    if (!is_single_number) {
        data.big.reset();
    }
    if (other.is_single_number) {

        data.small = other.data.small;
    } else {

        new(&data.big) std::shared_ptr<vector<uint32_t>>(other.data.big);

//        swap(tmp, data.big);
//        if (!is_single_number) {
//            tmp.reset();
//        }

    }
    is_single_number = other.is_single_number;
    return *this;
}

void spare_vector::clear() {
    new_numb();
    if (!is_single_number) {
        data.big.reset();
        is_single_number = true;
    }
    _size = 0;
}

void spare_vector::new_numb() {
    if (is_single_number || data.big.unique()) {
        return;
    }
    data.big = std::make_shared<std::vector<uint32_t >>(*data.big);
}

spare_vector::~spare_vector() {
    if (!is_single_number) {
        data.big.reset();
    }
}