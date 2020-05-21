#pragma once
#include <memory>
#include <type_traits>
#include <mutex>
#include <array>
#include <utility>

template <class T, int N>
class file_circulaire_concurrente {
    std::mutex ins_mutex, ext_mutex;
    
    struct buffer {
        static auto type() {
            return std::make_unique<T[]>(N);
        }
    };
    using buf_type = decltype(buffer::type());
    buf_type buf = buffer::type();
    int ins_pt = 0;
    int ext_pt = 0;
    static int next(int n) {
        return ++n, n == N ? 0 : n;
    }
    static int prev(int n) {
        return n == 0 ? N - 1 : n - 1;
    }
    bool full() const noexcept {
        return next(ins_pt) == ext_pt;
    }
    bool empty() const noexcept {
        return ins_pt == ext_pt;
    }
public:
    void push(const T& obj) {
        std::lock_guard<std::mutex> _{ ins_mutex };
        buf[ins_pt] = obj;
        ins_pt = next(ins_pt);
    }
    void pop() {
        std::lock_guard<std::mutex> _{ ext_mutex };
        ext_pt = next(ext_pt);
    }
    T top() {
        std::lock_guard<std::mutex> _{ ext_mutex };
        return buf[ext_pt];
    }
    T pop_element() {
        std::lock_guard<std::mutex> _{ ext_mutex };
        T elmt = buf[ext_pt];
        ext_pt = next(ext_pt);
        return elmt;
    }
    bool try_push(const T& obj) {
        std::lock_guard<std::mutex> _{ ins_mutex };
        std::lock_guard<std::mutex> __{ ext_mutex };
        if (full()) return false;
        buf[ins_pt] = obj;
        ins_pt = next(ins_pt);
        return true;
    }
    bool try_pop(T& obj) {
        std::lock_guard<std::mutex> _{ ext_mutex };
        std::lock_guard<std::mutex> __{ ins_mutex };
        if (empty()) return false;
        obj = buf[ext_pt];
        ext_pt = next(ext_pt);
        return true;
    }
    int size() {
        std::lock_guard<std::mutex> _{ ins_mutex };
        std::lock_guard<std::mutex> __{ ext_mutex };
        if (ins_pt > ext_pt) {
            return ins_pt - ext_pt;
        }
        else {
            return N - (ins_pt - ext_pt);
        }
    }
};