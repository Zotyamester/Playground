#ifndef PLAYGROUND_SEGMENTTREE_HPP
#define PLAYGROUND_SEGMENTTREE_HPP

#include <cstddef>
#include <vector>

template<typename T>
class SegmentTree {
    size_t n;
    std::vector<T> data;

    void build(const std::vector<T>& array, size_t vertex, size_t tree_left, size_t tree_right) {
        if (tree_left == tree_right) {
            data[vertex] = array[tree_left];
        } else {
            size_t tree_mid = (tree_left + tree_right) / 2;
            build(array, 2 * vertex + 1, tree_left, tree_mid);
            build(array, 2 * vertex + 2, tree_mid + 1, tree_right);
            data[vertex] = data[2 * vertex + 1] + data[2 * vertex + 2];
        }
    }

    T query(size_t vertex, size_t tree_left, size_t tree_right, size_t left, size_t right) {
        if (left > right)
            return T();
        if (left == tree_left && right == tree_right)
            return data[vertex];

        size_t tree_mid = (tree_left + tree_right) / 2;
        return query(2 * vertex + 1, tree_left, tree_mid, left, std::min(right, tree_mid)) +
               query(2 * vertex + 2, tree_mid + 1, tree_right, std::max(left, tree_mid + 1), right);
    }
public:
    explicit SegmentTree(const std::vector<T>& array) : n(array.size()), data(4 * n) {
        build(array, 0, 0, n - 1);
    }

    T query(size_t left, size_t right) {
        return query(0, 0, n - 1, left, right);
    }
};

#endif //PLAYGROUND_SEGMENTTREE_HPP
