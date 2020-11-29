#ifndef OOP_EXERCISE_05_STACK_H
#define OOP_EXERCISE_05_STACK_H

#include "Rectangle.h"
#include "Allocator.h"

template<class T, class ALLOCATOR>
class Stack {
private:
    struct TNode;

    using allocator_type = typename ALLOCATOR::template rebind<TNode>::other;

    struct deleter {
        allocator_type stack_node_deleter;

        deleter() : stack_node_deleter() {};
        deleter(allocator_type* another_deleter) : stack_node_deleter(another_deleter) {}

        /* std::shared_ptr uses operator() to delete memory */
        void operator() (void* ptr) {
            stack_node_deleter.deallocate((TNode*)ptr, 1);
        }
    };

    struct TNode {
        T data;
        std::shared_ptr<TNode> next;

        TNode() noexcept : data(), next(nullptr) {};
        explicit TNode(const T & elem) noexcept : data(elem), next(nullptr) {}

        friend bool operator != (const TNode & lhs, const TNode & rhs) {
            return &lhs.data != &rhs.data;
        }

        friend bool operator == (const TNode & lhs, const TNode & rhs) {
            return &lhs.data == &rhs.data;
        }

        friend std::ostream & operator << (std::ostream & out, const TNode & node) {
            out << node.data;
            return out;
        }
    };

public:
    class iterator {
    private:
        std::shared_ptr<TNode> ptr;
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_reference = const T&;

        iterator() : ptr(nullptr) {}
        iterator(const std::shared_ptr<TNode> & another_ptr) : ptr(another_ptr) {}

        bool is_null() {
            return ptr == nullptr;
        }

        void unvalidate() {
            ptr = nullptr;
        }

        iterator & operator ++ () {
            if (this->ptr != nullptr) {
                this->ptr = this->ptr->next;
                return *this;
            } else {
                throw(std::runtime_error("Iterator points to nullptr!"));
            }
        }

        bool operator != (const iterator & other_iterator) {
            return &other_iterator.ptr->data != &this->ptr->data;
        }

        friend std::ostream & operator << (std::ostream & out, const iterator & it) {
            out << *(it.ptr);
            return out;
        }

        TNode & operator * () {
            return *ptr;
        }
    };

private:
    std::shared_ptr<TNode> top_node;
    deleter stack_deleter;

public:
    Stack() noexcept : top_node() {};

    iterator begin() {
        return iterator(top_node);
    }

    iterator end() {
        return iterator(nullptr);
    }

    void pop() {
        if (top_node) {
            top_node = top_node->next;
        } else {
            throw(std::runtime_error("Stack is empty!"));
        }
    }

    void push(const T & elem) {
        TNode* new_node = stack_deleter.stack_node_deleter.allocate(sizeof(TNode));
        stack_deleter.stack_node_deleter.construct(new_node, elem);
        std::shared_ptr<TNode> new_node_shared(new_node, stack_deleter);
        new_node_shared->next = top_node;
        top_node = new_node_shared;
    }

    T top() {
        if (top_node) {
            return top_node->data;
        } else {
            throw(std::runtime_error("Stack is empty!"));
        }
    }

    void erase(iterator it) {
        if (it.is_null()) {
            throw(std::runtime_error("Iterator points to nullptr!"));
        } else {
            if (*it == *top_node) {
                top_node = top_node->next;
            } else {
                std::shared_ptr<TNode> prev_node = top_node;
                while (*prev_node->next != *it) {
                    prev_node = prev_node->next;
                }
                prev_node->next = prev_node->next->next;
                (*it).next = nullptr;
            }
            it.unvalidate();
        }
    }

    void insert(iterator it, const T & elem) {
        TNode* new_node = stack_deleter.stack_node_deleter.allocate(sizeof(TNode));
        stack_deleter.stack_node_deleter.construct(new_node, elem);
        std::shared_ptr<TNode> new_node_shared(new_node, stack_deleter);
        if (top_node) {
            if (*it == *top_node) {
                new_node_shared->next = top_node;
                top_node = new_node_shared;
                it.unvalidate();
                return;
            }
            std::shared_ptr<TNode> prev_node = top_node;
            while (*prev_node->next != *it) {
                prev_node = prev_node->next;
            }
            if (it.is_null()) {
                prev_node->next = new_node_shared;
            } else {
                new_node_shared->next = prev_node->next;
                prev_node->next = new_node_shared;
                // std::swap(prev_node->data, prev_node->next->data);
            }
        } else {
            top_node = new_node_shared;
        }
        it.unvalidate();
    }
};

#endif //OOP_EXERCISE_05_STACK_H

