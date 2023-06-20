#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include <cstddef>

// Suppose we have a generic collection of some sort.
// That is, a collection implemented as a template-class
// in which the template argument determines what kind
// of data it holds.
template<typename T>
class SomeGenericCollection {
private:
    // here, we're looking at a vector-ish collection
    // hence the pointer and the n (denoting its capacity/actual size)
    T* data;
    size_t n;

public:
    // functions, operators etc.

private:
    // By making the iterator implementation itself a generic class, we can
    // make it so that it's capable of holding a pointer to either const
    // or non-const data.
    //
    // And luckily, users of this class may not even have to know, that this
    // trick is happening, as we can hide it, by making its accessibility
    // private. This works because the concrete iterator types are just
    // aliases for the const/non-const T instantiation of the class.
    template<typename ITEM = T>
    class inner_iterator {
    private:
        ITEM* ptr;
    public:
        explicit inner_iterator(ITEM* ptr = nullptr) : ptr(ptr) {}

        ITEM& operator*() const {
            return *ptr;
        }

        ITEM* operator->() const {
            return ptr;
        }

        inner_iterator& operator++() {
            ++ptr;
            return *this;
        }

        inner_iterator operator++(int) {
            inner_iterator original = *this;
            ++ptr;
            return original;
        }

        bool operator==(inner_iterator rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator!=(inner_iterator rhs) const {
            return ptr != rhs.ptr;
        }
    };

public:
    // aliasing the instantiation for the non-const version
    using iterator = inner_iterator<T>;

    iterator begin() { return iterator(data); }
    iterator end() { return iterator(data + n); }

    // aliasing the instantiation for the const version
    using const_iterator = inner_iterator<const T>;

    const_iterator begin() const { return const_iterator(data); }
    const_iterator end() const { return const_iterator(data + n); }
};

#endif // ITERATOR_HPP
