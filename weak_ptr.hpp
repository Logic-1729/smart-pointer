#include<functional>
struct ptr_cnt {
    int shared_cnt = 1;
    int weak_cnt = 0;
};

template <typename T>
class SharedPtr {
  public:
    T* _p;
    ptr_cnt* num;

    void clear() {
        if (_p && (--num->shared_cnt == 0)) {
            delete _p;
            if (num->weak_cnt == 0) {
                delete num;
            }
        }
        _p = nullptr;
        num = nullptr;
    }

    SharedPtr() : _p(nullptr), num(nullptr) {}

    explicit SharedPtr(T* p) : _p(p), num(p ? new ptr_cnt() : nullptr) {}
    explicit SharedPtr(T* p, ptr_cnt* num__) : _p(p), num(num__) {}

    SharedPtr(const SharedPtr& temp) : _p(temp._p), num(temp.num) {
        if (num) {
            num->shared_cnt += 1;
        }
    }

    ~SharedPtr() { clear(); }

    int use_count() const { return num ? num->shared_cnt : 0; }

    T* get() const { return _p; }

    operator bool() const { return _p != nullptr; }

    T& operator*() const { return *_p; }

    T* operator->() const { return _p; }

    SharedPtr& operator=(const SharedPtr& temp) {
        if (this != &temp) {
            clear();
            _p = temp._p;
            num = temp.num;
            if (num) {
                num->shared_cnt += 1;
            }
        }
        return *this;
    }

    void reset() { clear(); }

    void reset(T* p) {
        clear();
        if (p) {
            _p = p;
            num = new ptr_cnt();
        }
    }
};

template <typename T, typename... Ts>
SharedPtr<T> make_shared(Ts&&... args) {
    return SharedPtr<T>(new T(std::forward<Ts>(args)...));
}

template <typename T>
class WeakPtr {
  public:
    T* _p;
    ptr_cnt* num;

    void swap(WeakPtr& temp) noexcept {
        std::swap(_p, temp._p);
        std::swap(num, temp.num);
    }

    void clear() {
        if (num && (--num->weak_cnt == 0) && num->shared_cnt == 0) {
            delete num;
        }
        _p = nullptr;num = nullptr;
    }

    WeakPtr() : _p(nullptr), num(nullptr) {}

    WeakPtr(const SharedPtr<T>& temp) noexcept : _p(temp._p), num(temp.num) {
        if (num) {
            num->weak_cnt += 1;
        }
    }

    WeakPtr(const WeakPtr& temp) noexcept : _p(temp._p), num(temp.num) {
        if (num) {
            num->weak_cnt += 1;
        }
    }

    WeakPtr(WeakPtr&& temp) noexcept : _p(temp._p), num(temp.num) {
        temp.reset();
    }

    ~WeakPtr() { clear(); }

    operator bool() const { return _p != nullptr; }

    int use_count() const { return num ? num->shared_cnt : 0; }

    WeakPtr& operator=(const WeakPtr& temp) noexcept {
        if (this != &temp) {
            clear();
            _p = temp._p;
            num = temp.num;
            if (num) {
                num->weak_cnt += 1;
            }
        }
        return *this;
    }

    WeakPtr& operator=(const SharedPtr<T>& temp) noexcept {
        clear();
        _p = temp._p;
        num = temp.num;
        if (num) {
            num->weak_cnt += 1;
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& temp) noexcept {
        if (this != &temp) {
            clear();
            _p = temp._p;
            num = temp.num;
            temp._p=nullptr;
            temp.num=nullptr;
        }
        return *this;
    }

    SharedPtr<T> lock() {
        if (expired()) {
            return SharedPtr<T>(nullptr);
        } else {
            if (num) {
                num->shared_cnt += 1;
            }
            return SharedPtr<T>(_p, num);
        }
    }

    bool expired() const { return !num || num->shared_cnt == 0; }

    void reset() { clear(); }
};
