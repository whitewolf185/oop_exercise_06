#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP
#include <stack>

template<class T, std::size_t BLOCK_SIZE>
class linear_allocator_t {
private:
    std::stack<T*> st;
    T* buffer;
public:
    using allocator_type = linear_allocator_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;

    T* allocate(const std::size_t & n) {
        if (buffer == nullptr) {
            buffer = new T[BLOCK_SIZE];
            for (std::size_t i = 0; i < BLOCK_SIZE; ++i) {
                st.push(&buffer[i]);
            }
        }
        printf("Allocatind %lu bytes in block with %lu free bytes\n", n, st.size());
        if (st.size() < n) {
            throw(std::bad_alloc());
        } else {
            T* p = st.top();
            for (std::size_t i = 0; i < n; ++i) {
                st.pop();
            }
            return p;
        }
    }

    template<class OTHER_T, class... ARGS>
    void construct(OTHER_T* p, ARGS... arguments) {
        *p =  OTHER_T(std::forward<ARGS>(arguments)...);
    }

    void deallocate(pointer, std::size_t) {
        ;
    }

    linear_allocator_t() : st(), buffer(nullptr) {
        static_assert(BLOCK_SIZE > 0);
    }

    explicit linear_allocator_t(const linear_allocator_t<T, BLOCK_SIZE> & another_allocator) : linear_allocator_t() {
        buffer = new T[BLOCK_SIZE];
        for (std::size_t i = 0; i < BLOCK_SIZE; ++i) {
            buffer[i] = another_allocator.buffer[i];
            st.push(&buffer[i]);
        }
    }

    ~linear_allocator_t() {
        delete[] buffer;
    }

    template<class OTHER_T>
    class rebind {
    public:
        using other = linear_allocator_t<OTHER_T, BLOCK_SIZE>;
    };
};

#endif /* ALLOCATOR_HPP */
