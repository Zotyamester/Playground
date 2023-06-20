#ifndef PLAYGROUND_HEAP_HPP
#define PLAYGROUND_HEAP_HPP

#include <iostream>
#include <vector>

template<typename T, typename CMP>
class Heap {
private:
    static const CMP cmp;
    std::vector<T> data;

    size_t up_heap(size_t node) {
        size_t parent;
        while (node > 0 && cmp(data[node], data[parent = (node - 1) / 2])) {
            std::swap(data[node], data[parent]);
            node = parent;
        }
        return node;
    }

    size_t down_heap(size_t node) {
        size_t left, right;
        while ((left = 2 * node + 1) < data.size()) {
            right = left + 1;

            size_t extreme_child = (right == data.size() || cmp(data[left], data[right])) ? left : right;
            if (cmp(data[node], data[extreme_child]))
                break;

            std::swap(data[node], data[extreme_child]);
            node = extreme_child;
        }
        return node;
    }

public:
    Heap() = default;

    Heap(std::initializer_list<T> values) : data(values) {
        for (size_t node = data.size() - 1; node > 0; node--)
            if (up_heap(node) == node)  // if can't go upwards
                down_heap(node);        // go downwards
    }

    void insert(const T& value) {
        data.push_back(value);
        up_heap(data.size() - 1);
    }

    T& extreme() { return data[0]; }

    const T& extreme() const { return data[0]; }

    void extreme_remove() {
        data[0] = data[data.size() - 1];
        data.pop_back();
        down_heap(0);
    }

    bool empty() const { return data.empty(); }
};

template<typename T> using MinHeap = Heap<T, std::less<T>>;

template<typename T> using MaxHeap = Heap<T, std::greater<T>>;

template<typename T, typename CMP> const CMP Heap<T, CMP>::cmp;

#endif //PLAYGROUND_HEAP_HPP
