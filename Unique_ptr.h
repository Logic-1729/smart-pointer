template <typename T>
class UniquePtr{
public:
    T* _p;

    void clear(){
        delete _p;
        _p=nullptr;
    }

    UniquePtr() {
        _p = nullptr;
    }

    UniquePtr(T* p) noexcept{
        _p = p;
    }

    UniquePtr(UniquePtr &temp)=delete;

    UniquePtr(UniquePtr &&temp){
        _p = temp._p;
        temp._p = nullptr;
    }

    ~UniquePtr() {
        clear();
    }

    T* get() const{
        return _p;
    }

    operator bool() const{
        return (_p!=nullptr);
    }

    T& operator*() const{
        return *_p;
    }

    T* operator->() const{
        return _p;
    }

    UniquePtr& operator=(UniquePtr& temp)=delete;

    UniquePtr& operator=(UniquePtr&& temp)noexcept{
        if (&temp!=this){
            delete _p;
            _p = temp._p;
            temp._p = nullptr;
            return *this;
        }
        return *this;
    }

    void reset() {
        clear();
    }

    void reset(T* p) {
        if (_p == p) return;
        delete _p;_p=p;
    }

    T*release(){
        T*temp=_p;
        _p=nullptr;
        return temp;
    }
};

template <typename T, typename... Ts>
UniquePtr<T> make_unique(Ts &&...args){
    return UniquePtr<T>(new T(std::forward<Ts>(args)...));
}