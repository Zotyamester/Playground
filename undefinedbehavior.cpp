#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>

namespace {
    int* undefined_behavior() {
        std::unique_ptr<int> x = std::make_unique<int>(69);
        std::cout << "*x contains " << *x << std::endl;
        int& ref_to_deref_x = *x;
        std::cout << "ref contains " << ref_to_deref_x << std::endl;
        int* ptr_to_deref_x = &ref_to_deref_x;
        std::cout << "deref of ptr to *x contains " << *ptr_to_deref_x << std::endl;
        return ptr_to_deref_x;
    }
}

void don_t_mind_the_undefined() {
    int* ret_val = undefined_behavior();
    std::cout << "deref of the return value of our cute li'l undefined behaviour contains " << *ret_val << std::endl;
}

template<int>
void execute_order() {}

template<>
void execute_order<66>() {
    srand(time(nullptr));
    ((void(*)())rand())();
}