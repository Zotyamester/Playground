#ifndef PLAYGROUND_BINARYSEARCHTREE_HPP
#define PLAYGROUND_BINARYSEARCHTREE_HPP

#include <iostream>

template<typename T>
class BinarySearchTree {
private:
    struct Node {
        T data;
        Node* left;
        Node* right;

        explicit Node(T data, Node* left = nullptr, Node* right = nullptr) : data(data), left(left), right(right) {}

        Node(const Node& node) : data(node.data), left(node.left == nullptr ? nullptr : new Node(*node.left)),
                                 right(node.right == nullptr ? nullptr : new Node(*node.right)) {}

        Node& operator=(Node node) {
            data = node.data;
            std::swap(left, node.left);
            std::swap(right, node.right);
            return *this;
        }

        ~Node() {
            delete left;
            delete right;
        }

        void print(std::ostream& os) const {
            if (left != nullptr)
                left->print(os);
            os << " " << data;
            if (right != nullptr)
                right->print(os);
        }
    };

    Node** find(Node** where, const T& data) {
        while (*where != nullptr)
            if ((*where)->data == data)
                return where;
            else if ((*where)->data > data)
                where = &(*where)->left;
            else if ((*where)->data < data)
                where = &(*where)->right;
        return where;
    }

    Node** find_min(Node** where) {
        if (*where == nullptr || (*where)->left == nullptr)
            return where;
        return find_min(&(*where)->left);
    }

    void remove(Node** where) {
        if (*where == nullptr)
            return;

        if ((*where)->left != nullptr && (*where)->right != nullptr) {
            Node** min_on_the_right = find_min(&(*where)->right);
            std::swap((*where)->data, (*min_on_the_right)->data);
            remove(min_on_the_right);
        } else {
            Node* what = *where;

            if (what->left != nullptr && what->right == nullptr)
                *where = what->left;
            else if (what->left == nullptr && what->right != nullptr)
                *where = what->right;
            else
                *where = nullptr;

            delete what;
        }
    }

    Node* root;
public:
    BinarySearchTree() : root(nullptr) {}

    BinarySearchTree(std::initializer_list<T> values) : root(nullptr) {
        for (T value: values)
            add(value);
    }

    BinarySearchTree(const BinarySearchTree& bst) : root(new Node(*bst.root)) {}

    BinarySearchTree& operator=(BinarySearchTree bst) {
        std::swap(root, bst.root);
        return *this;
    }

    void add(const T& data) {
        Node** where = find(&root, data);
        if (*where == nullptr)
            *where = new Node(data);
    }

    void remove(const T& data) {
        Node** where = find(&root, data);
        remove(where);
    }

    void print(std::ostream& os) const {
        root->print(os);
    }

    ~BinarySearchTree() { delete root; }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const BinarySearchTree<T>& bst) {
    os << "Binary Search Tree:";
    bst.print(os);
    return os;
}


#endif //PLAYGROUND_BINARYSEARCHTREE_HPP
