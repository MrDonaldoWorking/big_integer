#ifndef BIGINT_OPT_VECTOR_H
#define BIGINT_OPT_VECTOR_H

/*
 * Beware the awful English
 */

/*
 * Mere small object optimisation explanation:
 * vector has already some elements
 * if it's not enough, create a large "array" through pointer
 */

/*
 * Mere copy on write optimisation explanation:
 * if one object is complete copy of other,
 * they have a pointer pointing to the same object,
 * and if it changes, a copy is already created and changed
 */

#include <memory>
#include <vector>
#include <assert.h>

template <typename T>
struct opt_vector {
public:
    opt_vector();
    explicit opt_vector(size_t new_len);
    opt_vector(size_t new_len, T new_val);
    ~opt_vector();

    size_t size() const;
    T operator[](size_t pos) const;
    T &operator[](size_t pos);
    T back() const;

    void resize(size_t new_len);
    void push_back(T new_val);
    void pop_back();

    bool operator==(opt_vector const& other) const;

private:
    size_t len;
    T small_data;
    std::shared_ptr<std::vector<T>> big_data;

    inline bool is_small() const;
    void make_unique();
};

#endif //BIGINT_OPT_VECTOR_H
