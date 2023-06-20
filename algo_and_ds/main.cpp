#include <iostream>
#include <string>
#include <vector>
#include "SegmentTree.hpp"

void lexicographic_sort(std::vector<std::string>& strings, size_t k) {
    if (k == 0)
        return;

    std::vector<std::vector<std::string>> buckets(256);
    do {
        k--;

        for (auto& bucket : buckets)
            bucket.clear();

        for (auto&& str : strings)
            buckets[k >= str.size() ? 0 : str[k]].emplace_back(str);

        strings.clear();

        for (auto& bucket : buckets)
            for (auto&& str : bucket)
                strings.emplace_back(str);
    } while (k > 0);
}

//int main() {
//    BinarySearchTree<int> bst = {5, 3, 7, 2, 1, 4, 6, 9, 8, 10};
//    bst.add(0);
//    bst.remove(5);
//    std::cout << bst << std::endl;
//
//    MinHeap<int> h = {5, 3, 7, 2, 1, 4, 6, 9, 8, 10};
//    for (size_t i = 0; i < 10; i++) {
//        std::cout << h.extreme() << " ";
//        h.extreme_remove();
//    }
//    std::cout << std::endl;
//
//    std::vector<std::string> strings = { "cab", "bab", "bcb", "b", "aba", "aab", "aaa", "a" };
//    lexicographic_sort(strings, 3);
//    for (const auto& str : strings)
//        std::cout << str << " ";
//    std::cout << std::endl;
//
//    std::vector<int> array = {5, 3, 7, 2, 1, 4, 6, 9, 8, 10};
//    SegmentTree<int> tree(array);
//    std::cout << tree.query(1, 3) << std::endl;
//
//    return 0;
//}
