#ifndef OPTIONAL_COMPAT_H
#define OPTIONAL_COMPAT_H

#include <stdexcept>
#include <utility>

// Simple std::optional replacement for GCC 6.3.0 compatibility
namespace std {

struct nullopt_t {
    explicit nullopt_t() = default;
};

constexpr nullopt_t nullopt{};

template<typename T>
class optional {
private:
    bool has_value_;
    union {
        char dummy_;
        T value_;
    };

public:
    optional() : has_value_(false), dummy_('\0') {}
    
    optional(nullopt_t) : has_value_(false), dummy_('\0') {}
    
    optional(const T& value) : has_value_(true), value_(value) {}
    
    optional(T&& value) : has_value_(true), value_(std::move(value)) {}
    
    optional(const optional& other) : has_value_(other.has_value_) {
        if (has_value_) {
            new(&value_) T(other.value_);
        } else {
            dummy_ = '\0';
        }
    }
    
    optional& operator=(const optional& other) {
        if (this != &other) {
            if (has_value_) {
                value_.~T();
            }
            has_value_ = other.has_value_;
            if (has_value_) {
                new(&value_) T(other.value_);
            } else {
                dummy_ = '\0';
            }
        }
        return *this;
    }
    
    ~optional() {
        if (has_value_) {
            value_.~T();
        }
    }
    
    bool has_value() const { return has_value_; }
    
    explicit operator bool() const { return has_value_; }
    
    T& operator*() { return value_; }
    const T& operator*() const { return value_; }
    
    T* operator->() { return &value_; }
    const T* operator->() const { return &value_; }
    
    T& value() { 
        if (!has_value_) throw std::runtime_error("optional has no value");
        return value_; 
    }
    
    const T& value() const { 
        if (!has_value_) throw std::runtime_error("optional has no value");
        return value_; 
    }
};

} // namespace std

#endif // OPTIONAL_COMPAT_H