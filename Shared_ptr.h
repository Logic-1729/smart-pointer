#pragma GCC optimize("Ofast") 
#pragma GCC optimize(3)
#pragma GCC optimize("inline")
#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP
#include "exceptions.hpp"
#include <cstddef>
#include <iterator>

namespace sjtu {
template <class T> class deque {
    public:
        class block_node;
    private:
        block_node* head;
        block_node* tail;
        size_t size_;
        size_t block_size = 512;
    public:
        class list_node {
        public:
            list_node* pre;
            list_node* nxt;
            T* val;
            list_node():pre(nullptr),nxt(nullptr),val(nullptr){}
        };
        class block_node {
        public:
            block_node* pre;
            block_node* nxt;
            list_node* head;
            list_node* tail;
            size_t length ,index;
            block_node():pre(nullptr),nxt(nullptr),head(nullptr),tail(nullptr),length(0),index(0){}
        };
        class const_iterator;
        class iterator {
        private:
            deque<T>* deque_node;
            list_node* cur_node;
            size_t block_index;
            block_node* cur_block;
        public:
            iterator():deque_node(nullptr),cur_node(nullptr),block_index(0),cur_block(nullptr){}
            iterator(deque<T>* deque_node, list_node* cur_node,size_t block_index, block_node* cur_block):
                deque_node(deque_node), cur_node(cur_node), block_index(block_index), cur_block(cur_block) {}
            iterator(const iterator &other):
                deque_node(other.deque_node), cur_node(other.cur_node), block_index(other.block_index), cur_block(other.cur_block) {}
            iterator(const const_iterator &other):
                deque_node(other.deque_node), cur_node(other.cur_node), block_index(other.block_index), cur_block(other.cur_block) {}
            iterator operator+(const int &n) const {
                iterator tmp = *this;
                if (n < 0) return tmp - (-n) ;
                if (n == 0) return tmp;
                int remain = n;
                while (tmp.cur_block->nxt != deque_node->tail && tmp.block_index + remain >= tmp.cur_block->length) {
                    remain -= tmp.cur_block->length - tmp.block_index;
                    tmp.cur_block = tmp.cur_block->nxt;
                    tmp.cur_node = tmp.cur_block->head;
                    tmp.block_index = 0;
                }
                while (remain--) {
                    tmp.cur_node = tmp.cur_node->nxt;
                    ++tmp.block_index;
                }
                return tmp;
            }
            iterator operator-(const int &n) const {
                iterator tmp = *this;
                if (n < 0) return tmp + (-n) ;
                if (n == 0) return tmp;
                int remain = n;
                while (tmp.block_index < remain && tmp.cur_block->pre != tmp.deque_node->head) {
                    remain -= tmp.block_index + 1;
                    tmp.cur_block = tmp.cur_block->pre;
                    tmp.cur_node = tmp.cur_block->tail;
                    tmp.cur_node = tmp.cur_node->pre;
                    tmp.block_index = tmp.cur_block->length - 1;
                }
                while (remain--) {
                    tmp.cur_node = tmp.cur_node->pre;
                    --tmp.block_index;
                }
                return tmp;
            }

            size_t operator-(const iterator &rhs) const {
                if (deque_node != rhs.deque_node) throw std::out_of_range("out of range");
                size_t dist = 0;
                size_t l_index = cur_block->index,r_index = rhs.cur_block->index;
                if (l_index == r_index) return block_index - rhs.block_index;
                if (l_index > r_index) {
                    block_node* tmp_node = rhs.cur_block;
                    dist += tmp_node->length - rhs.block_index;
                    tmp_node = tmp_node->nxt;
                    while (tmp_node->index < cur_block->index) {
                        dist += tmp_node->length;
                        tmp_node = tmp_node->nxt;
                    }
                    dist += block_index;
                    return dist;
                }else {
                    return -(rhs - *this);
                }
            }
            iterator &operator+=(const int &n) {
                *this = *this + n;
                return *this;
            }
            iterator &operator-=(const int &n) {
                *this = *this - n;
                return *this;
            }
            iterator operator++(int) {
                iterator tmp = *this;
                if (block_index + 1 < cur_block->length || (block_index + 1 >= cur_block->length && cur_block->nxt == deque_node->tail)) {
                    ++block_index;
                    cur_node = cur_node->nxt;
                } else {
                    block_index = 0;
                    cur_block = cur_block->nxt;
                    cur_node = cur_block->head;
                }
                return tmp;
            }
            iterator& operator++() {
                if (block_index + 1 < cur_block->length || (block_index + 1 >= cur_block->length && cur_block->nxt == deque_node->tail)) {
                    ++block_index;
                    cur_node = cur_node->nxt;
                } else {
                    block_index = 0;
                    cur_block = cur_block->nxt;
                    cur_node = cur_block->head;
                }
                return *this;
            }
            iterator operator--(int) {
                iterator tmp = *this;
                if (block_index != 0 || cur_block->pre == deque_node->head) {
                    --block_index;
                    cur_node = cur_node->pre;
                }else {
                    cur_block = cur_block->pre;
                    block_index = cur_block->length - 1;
                    cur_node = cur_block->tail->pre;
                }
                return tmp;
            }
            iterator &operator--() {
                if (block_index != 0 || cur_block->pre == deque_node->head) {
                    block_index--;
                    cur_node = cur_node->pre;
                }else {
                    cur_block = cur_block->pre;
                    block_index = cur_block->length - 1;
                    cur_node = cur_block->tail->pre;
                }
                return *this;
            }
            T &operator*() const {
                if (cur_node == nullptr || cur_node->val == nullptr) throw std::out_of_range("out of range");
                return *cur_node->val;
            }
            T *operator->() const noexcept { return cur_node->val; }
            bool operator==(const iterator &rhs) const { return (cur_node == rhs.cur_node); }
            bool operator==(const const_iterator &rhs) const { return (cur_node == rhs.cur_node); }
            bool operator!=(const iterator &rhs) const { return (cur_node != rhs.cur_node); }
            bool operator!=(const const_iterator &rhs) const { return (cur_node != rhs.cur_node);}
            friend class deque<T>;
            friend class const_iterator;
        };
        class const_iterator {
        private:
            const deque<T>* deque_node = nullptr;
            list_node* cur_node = nullptr;
            size_t block_index = 0;
            block_node* cur_block = nullptr;
        public:
            const_iterator():deque_node(nullptr),cur_node(nullptr),block_index(0),cur_block(nullptr){}
            const_iterator(const deque<T>* deque_node, list_node* cur_node,size_t block_index, block_node* cur_block):
                deque_node(deque_node), cur_node(cur_node), block_index(block_index), cur_block(cur_block) {}
            const_iterator(const iterator &other):
                deque_node(other.deque_node), cur_node(other.cur_node), block_index(other.block_index), cur_block(other.cur_block) {}
            const_iterator(const const_iterator &other):
                deque_node(other.deque_node), cur_node(other.cur_node), block_index(other.block_index), cur_block(other.cur_block) {}
            const_iterator operator+(const int &n) const {
                const_iterator tmp = *this;
                if (n < 0) return tmp - (-n) ;
                if (n == 0) return tmp;
                int remain = n;
                while (tmp.cur_block->nxt != deque_node->tail && tmp.block_index + remain >= tmp.cur_block->length) {
                    remain -= tmp.cur_block->length - tmp.block_index;
                    tmp.cur_block = tmp.cur_block->nxt;
                    tmp.cur_node = tmp.cur_block->head;
                    tmp.block_index = 0;
                }
                while (remain--) {
                    tmp.cur_node = tmp.cur_node->nxt;
                    ++tmp.block_index;
                }
                return tmp;
            }
            const_iterator operator-(const int &n) const {
                const_iterator tmp = *this;
                if (n < 0) return tmp + (-n) ;
                if (n == 0) return tmp;
                int remain = n;
                while (tmp.block_index < remain && tmp.cur_block->pre != tmp.deque_node->head) {
                    remain -= tmp.block_index + 1;
                    tmp.cur_block = tmp.cur_block->pre;
                    tmp.cur_node = tmp.cur_block->tail;
                    tmp.cur_node = tmp.cur_node->pre;
                    tmp.block_index = tmp.cur_block->length - 1;
                }
                while (remain--) {
                    tmp.cur_node = tmp.cur_node->pre;
                    tmp.block_index--;
                }
                return tmp;
            }

            size_t operator-(const const_iterator &rhs) const {
                if (deque_node != rhs.deque_node) throw std::out_of_range("out of range");
                size_t dist = 0;
                size_t l_index = cur_block->index,r_index = rhs.cur_block->index;
                if (l_index == r_index) return block_index - rhs.block_index;
                if (l_index > r_index) {
                    block_node* tmp_node = rhs.cur_block;
                    dist += tmp_node->length - rhs.block_index;
                    tmp_node = tmp_node->nxt;
                    while (tmp_node->index < cur_block->index) {
                        dist += tmp_node->length;
                        tmp_node = tmp_node->nxt;
                    }
                    dist += block_index;
                    return dist;
                }else {
                    return -(rhs - *this);
                }
            }
            const_iterator &operator+=(const int &n) {
                *this = *this + n;
                return *this;
            }
            const_iterator &operator-=(const int &n) {
                *this = *this - n;
                return *this;
            }
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                if (block_index + 1 < cur_block->length || (block_index + 1 >= cur_block->length && cur_block->nxt == deque_node->tail)) {
                    ++block_index;
                    cur_node = cur_node->nxt;
                } else {
                    block_index = 0;
                    cur_block = cur_block->nxt;
                    cur_node = cur_block->head;
                }
                return tmp;
            }
            const_iterator &operator++() {
                if (block_index + 1 < cur_block->length || (block_index + 1 >= cur_block->length && cur_block->nxt == deque_node->tail)) {
                    ++block_index;
                    cur_node = cur_node->nxt;
                } else {
                    block_index = 0;
                    cur_block = cur_block->nxt;
                    cur_node = cur_block->head;
                }
                return *this;
            }
            const_iterator operator--(int) {
                const_iterator tmp = *this;
                if (block_index != 0 || cur_block->pre == deque_node->head) {
                    block_index--;
                    cur_node = cur_node->pre;
                }else {
                    cur_block = cur_block->pre;
                    block_index = cur_block->length - 1;
                    cur_node = cur_block->tail->pre;
                }
                return tmp;
            }
            const_iterator &operator--() {
                if (block_index != 0 || cur_block->pre == deque_node->head) {
                    block_index--;
                    cur_node = cur_node->pre;
                }else {
                    cur_block = cur_block->pre;
                    block_index = cur_block->length - 1;
                    cur_node = cur_block->tail->pre;
                }
                return *this;
            }
            T &operator*() const {
                if (cur_node == nullptr || cur_node->val == nullptr) throw std::out_of_range("out of range");
                return *cur_node->val;
            }
            T *operator->() const noexcept { return cur_node->val; }
            bool operator==(const iterator &rhs) const { return (cur_node == rhs.cur_node); }
            bool operator==(const const_iterator &rhs) const { return (cur_node == rhs.cur_node); }
            bool operator!=(const iterator &rhs) const { return (cur_node != rhs.cur_node); }
            bool operator!=(const const_iterator &rhs) const { return (cur_node != rhs.cur_node);}
            friend class deque<T>;
            friend class iterator;
        };

    deque():head(new block_node), tail(new block_node), size_(0){
        list_node* new_list_node = new list_node;
        block_node* new_node = new block_node;
        new_node->pre = head;
        new_node->nxt = tail;
        new_node->head = new_list_node;
        new_node->tail = new_list_node;
        new_node->length = 0;
        new_node->index = 1;
        head->nxt = new_node;tail->pre = new_node;
    }
    deque(const deque &other):head(new block_node), tail(new block_node), size_(other.size_) {
        block_node* other_ptr = other.head->nxt;
        block_node* ptr = head;
        while (other_ptr != other.tail) {
            block_node* new_node = new block_node;
            new_node->length = other_ptr->length;
            new_node->index = other_ptr->index;
            new_node->pre = ptr;
            ptr->nxt = new_node;
            list_node* other_list_node = other_ptr->head;
            if (other_ptr->head == other_ptr->tail) {
                list_node* new_list_node = new list_node;
                new_node->head = new_list_node;
                new_node->tail = new_list_node;
            } else {
                list_node* new_list_node = new list_node;
                new_node->head = new_list_node;
                new_list_node->val = new T(*other_list_node->val);
                other_list_node = other_list_node->nxt;
                while (other_list_node != other_ptr->tail){
                    list_node* tmp_list_node = new list_node;
                    new_list_node->nxt = tmp_list_node;
                    tmp_list_node->pre = new_list_node;
                    tmp_list_node->val = new T(*other_list_node->val);
                    new_list_node = tmp_list_node;
                    other_list_node = other_list_node->nxt;
                }
                list_node* tail = new list_node;
                new_node->tail = tail;
                tail->pre = new_list_node;
                new_list_node->nxt = tail;
            }
            other_ptr = other_ptr->nxt;
            ptr = new_node;
        }
        tail->pre = ptr;
        ptr->nxt = tail;
    }
    ~deque() {
        reset();
    }
    deque& operator=(const deque &other) {
        if (this == &other) return *this;
        reset();
        head = new block_node;
        tail = new block_node;
        size_ = other.size_;
        block_node* other_ptr = other.head->nxt;
        block_node* ptr = head;
        while (other_ptr != other.tail) {
            block_node* new_node = new block_node;
            new_node->length = other_ptr->length;
            new_node->index = other_ptr->index;
            new_node->pre = ptr;
            ptr->nxt = new_node;
            list_node* other_list_node = other_ptr->head;
            if (other_ptr->head == other_ptr->tail) {
                list_node* new_list_node = new list_node;
                new_node->head = new_list_node;
                new_node->tail = new_list_node;
            } else {
                list_node* new_list_node = new list_node;
                new_node->head = new_list_node;
                new_list_node->val = new T(*other_list_node->val);
                other_list_node = other_list_node->nxt;
                while (other_list_node != other_ptr->tail){
                    list_node* tmp_list_node = new list_node;
                    new_list_node->nxt = tmp_list_node;
                    tmp_list_node->pre = new_list_node;
                    tmp_list_node->val = new T(*other_list_node->val);
                    new_list_node = tmp_list_node;
                    other_list_node = other_list_node->nxt;
                }
                list_node* tail = new list_node;
                new_node->tail = tail;
                tail->pre = new_list_node;
                new_list_node->nxt = tail;
            }
            other_ptr = other_ptr->nxt;
            ptr = new_node;
        }
        tail->pre = ptr;
        ptr->nxt = tail;
        return *this;
    }

    T &at(const size_t &pos) {
        if (pos >= size_) throw std::out_of_range("out of range");
        int remain = pos;
        iterator tmp(this, head->nxt->head, 0, head->nxt);
        while (tmp.block_index + remain >= tmp.cur_block->length) {
            remain -= tmp.cur_block->length - tmp.block_index;
            tmp.cur_block = tmp.cur_block->nxt;
            tmp.cur_node = tmp.cur_block->head;
            tmp.block_index = 0;
        }
        while (remain--) {
            tmp.cur_node = tmp.cur_node->nxt;
            ++tmp.block_index;
        }
        return *tmp.cur_node->val;
    }
    const T &at(const size_t &pos) const {
        if (pos >= size_) throw std::out_of_range("out of range");
        int remain = pos;
        const_iterator tmp(this, head->nxt->head, 0, head->nxt);
        while (tmp.block_index + remain >= tmp.cur_block->length) {
            remain -= tmp.cur_block->length - tmp.block_index;
            tmp.cur_block = tmp.cur_block->nxt;
            tmp.cur_node = tmp.cur_block->head;
            tmp.block_index = 0;
        }
        while (remain--) {
            tmp.cur_node = tmp.cur_node->nxt;
            ++tmp.block_index;
        }
        return *tmp.cur_node->val;
    }
    T &operator[](const size_t &pos) {
        return at(pos);
    }
    const T &operator[](const size_t &pos) const {
        return at(pos);
    }
    const T &front() const {
        if (size_ == 0) throw std::out_of_range("out of range");
        return *(head->nxt->head->val);
    }
    const T &back() const {
        if (size_ == 0) throw std::out_of_range("out of range");
        return *(tail->pre->tail->pre->val);
    }

    iterator begin() {return iterator(this, head->nxt->head, 0, head->nxt); }

    const_iterator cbegin() const {return const_iterator(this, head->nxt->head, 0, head->nxt);}

    iterator end() {return iterator(this, tail->pre->tail, tail->pre->length, tail->pre);}

    const_iterator cend() const {return const_iterator(this, tail->pre->tail, tail->pre->length, tail->pre);}

    bool empty() const {return (size_ == 0);}

    size_t size() const {return size_;}

    void clear() {
        block_node* ptr = head->nxt;
        list_node* tmp_node;
        while (ptr != tail) {
            tmp_node = ptr->head;
            while (tmp_node != ptr->tail) {
                delete tmp_node->val;
                tmp_node = tmp_node->nxt;
                delete tmp_node->pre;
            }
            delete ptr->tail;
            ptr = ptr->nxt;
            delete ptr->pre;
        }
        block_node* new_node = new block_node;
        new_node->index = 1;
        new_node->length = 0;
        new_node->nxt = tail;
        new_node->pre = head;
        head->nxt = new_node;
        tail->pre = new_node;
        list_node* new_list_node = new list_node;
        new_node->head = new_list_node;
        new_node->tail = new_list_node;
        size_ = 0;
    }

    void reset() {
        block_node* ptr = head->nxt;
        delete head;
        list_node* tmp_node;
        while (!(ptr == tail)) {
            tmp_node = ptr->head;
            while (tmp_node != ptr->tail) {
                delete tmp_node->val;
                tmp_node = tmp_node->nxt;
                delete tmp_node->pre;
            }
            delete ptr->tail;
            ptr = ptr->nxt;
            delete ptr->pre;
        }
        delete tail;
    }

private:
    void merge(block_node* cur_block, block_node* nxt_block) {
        cur_block->tail->pre->nxt = nxt_block->head;
        nxt_block->head->pre = cur_block->tail->pre;
        delete cur_block->tail;
        cur_block->tail = nxt_block->tail;
        cur_block->length += nxt_block->length;
        cur_block->nxt = nxt_block->nxt;
        nxt_block->nxt->pre = cur_block;
        delete nxt_block;
        block_node* tmp = cur_block->nxt;
        while (tmp != tail) {
            tmp->index--;
            tmp = tmp->nxt;
        }
    }
    void check() {
        block_node* tmp = head->nxt;
        while (tmp->nxt != tail){
            if (tmp->length == 0) {
                delete tmp->tail;
                tmp->pre->nxt = tmp->nxt;
                tmp->nxt->pre = tmp->pre;
                block_node* tmp_node = tmp->nxt;
                delete tmp;
                while (tmp_node != tail) {
                    tmp_node->index--;
                    tmp_node = tmp_node->nxt;
                }
                return;
            }
            if (tmp->length + tmp->nxt->length <= (block_size >> 1)) {
                merge(tmp, tmp->nxt);
                if (tmp->nxt == tail) return;
            }
            tmp = tmp->nxt;
        }
        if (tmp->nxt == tail && tmp->pre != head) {
            if (tmp->length == 0) {
                delete tmp->tail;
                tmp->pre->nxt = tmp->nxt;
                tmp->nxt->pre = tmp->pre;
                block_node* tmp_node = tmp->nxt;
                delete tmp;
                while (tmp_node != tail) {
                    tmp_node->index--;
                    tmp_node = tmp_node->nxt;
                }
                return;
            }
        }
        return;
    }
    void split(list_node* cur_node, block_node* cur_block) {
        block_node* new_block = new block_node;
        new_block->index = cur_block->index + 1;
        new_block->pre = cur_block;
        new_block->nxt = cur_block->nxt;
        cur_block->nxt->pre = new_block;
        cur_block->nxt = new_block;
        new_block->length = 1;
        new_block->head = cur_node;
        new_block->tail = cur_block->tail;
        list_node* cur_tail = new list_node;
        cur_block->tail = cur_tail;
        cur_tail->pre = cur_node->pre;
        cur_node->pre->nxt = cur_tail;
        cur_node->pre = nullptr;
        cur_node = cur_node->nxt;
        while (cur_node != new_block->tail) {
            ++new_block->length;
            cur_node = cur_node->nxt;
        }
        cur_block->length -= new_block->length;
        while (new_block->nxt != tail){
            new_block = new_block->nxt;
            ++new_block->index;
        }
    }
    bool pointer_check(iterator pos) {
        if (pos.deque_node == nullptr || pos.cur_node == nullptr || pos.cur_block == nullptr || pos.block_index < 0) return true;
        block_node* tmp_block_node = head;
        while (tmp_block_node->nxt != tail) {
            tmp_block_node = tmp_block_node->nxt;
            if (tmp_block_node->index == pos.cur_block->index) {
                if (pos.block_index < tmp_block_node->length) return false; else return true;
            }
        }
        return true;
    }
    bool iterator_check(iterator pos) {
        if (pos == end()) return false;
        if (pos.deque_node == nullptr || pos.cur_node == nullptr || pos.cur_block == nullptr || pos.block_index < 0) return true;
        block_node* tmp_block_node = head;
        while (tmp_block_node->nxt != tail) {
            tmp_block_node = tmp_block_node->nxt;
            if (tmp_block_node->index == pos.cur_block->index) {
                if (pos.block_index < tmp_block_node->length) return false; else return true;
            }
        }
        return true;
    }
public:
    iterator insert(iterator pos, const T &value) {
        if (pos.deque_node != this || iterator_check(pos)) throw invalid_iterator();
        list_node* new_node = new list_node;
        ++pos.cur_block->length;
        ++size_;
        if (pos.block_index == 0) {
            new_node->nxt = pos.cur_node;
            pos.cur_block->head = new_node;
            new_node->val = new T(std::move(value));
            pos.cur_node->pre = new_node;
        } else {
            new_node->nxt = pos.cur_node;
            new_node->pre = pos.cur_node->pre;
            new_node->val = new T(std::move(value));
            new_node->pre->nxt = new_node;
            new_node->nxt->pre = new_node;
        }
        if (pos.cur_block->length >= block_size) {
            list_node* tmp_node = pos.cur_block->head;
            int tmp = 0;
            while (tmp < (block_size >> 1)) {
                tmp_node = tmp_node->nxt;
                ++tmp;
            }
            split(tmp_node, pos.cur_block);
            if (pos.block_index < (block_size >> 1)) { return iterator(this, new_node, pos.block_index, pos.cur_block);} 
            else {return iterator(this, new_node, pos.block_index - (block_size >> 1), pos.cur_block->nxt);}
        } else { return iterator(this, new_node, pos.block_index, pos.cur_block);}
    }
    iterator erase(iterator pos) {
        if (size_ == 0) throw container_is_empty();
        if (pos.deque_node != this || pointer_check(pos)) throw invalid_iterator();
        iterator tmp = pos + 1;
        int flag = 0;
        if (tmp == end()) flag = 1;
        size_--;
        if (pos.block_index == 0) {
            pos.cur_block->head = pos.cur_node->nxt;
            pos.cur_node->nxt->pre = nullptr;
            pos.cur_block->length--;
        } else {
            pos.cur_node->pre->nxt = pos.cur_node->nxt;
            pos.cur_node->nxt->pre = pos.cur_node->pre;
            pos.cur_block->length--;
        }
        delete pos.cur_node->val;
        delete pos.cur_node;
        check();
        if (flag) return end();
        size_t ind = 0;
        list_node* tmp_list_node = tmp.cur_node;
        while (tmp_list_node->pre != nullptr) {
            ++ind;
            tmp_list_node = tmp_list_node->pre;
        }
        tmp.block_index = ind;
        block_node* tmp_block_node = head->nxt;
        while (tmp_block_node != tail) {
            if (tmp_block_node->head == tmp_list_node) {
                tmp.cur_block = tmp_block_node;
                return tmp;
            }
            tmp_block_node = tmp_block_node->nxt;
        }
    }
    void push_back(const T &value) {
        iterator pos = end();
        list_node* new_node = new list_node;
        ++pos.cur_block->length;
        ++size_;
        if (pos.block_index == 0) {
            new_node->nxt = pos.cur_node;
            pos.cur_block->head = new_node;
            new_node->val = new T(std::move(value));
            pos.cur_node->pre = new_node;
        } else {
            new_node->nxt = pos.cur_node;
            new_node->pre = pos.cur_node->pre;
            new_node->val = new T(std::move(value));
            new_node->pre->nxt = new_node;
            new_node->nxt->pre = new_node;
        }
        if (pos.cur_block->length >= block_size) {
            list_node* tmp_node = pos.cur_block->head;
            int tmp = 0;
            while (tmp < (block_size >> 1)) {
                tmp_node = tmp_node->nxt;
                ++tmp;
            }
            split(tmp_node, pos.cur_block);
        }
    }
    void pop_back() {
        if (size_ == 0) std::out_of_range("out of range");
        iterator pos = end() - 1;
        --size_;
        if (pos.block_index == 0) {
            if (pos.cur_block->pre == head) {
                pos.cur_block->head = pos.cur_node->nxt;
                pos.cur_node->nxt->pre = nullptr;
                pos.cur_block->length--;
            } else {
                delete pos.cur_block->tail;
                pos.cur_block->pre->nxt = tail;
                tail->pre = pos.cur_block->pre;
                delete pos.cur_block;
            }
        } else {
            pos.cur_node->pre->nxt = pos.cur_node->nxt;
            pos.cur_node->nxt->pre = pos.cur_node->pre;
            pos.cur_block->length--;
            if (pos.cur_block->pre != head && pos.cur_block->pre->length + pos.cur_block->length <= (block_size >> 1)) {
                merge(pos.cur_block->pre, pos.cur_block);
            }
        }
        delete pos.cur_node->val;
        delete pos.cur_node;
    }
    void push_front(const T &value) {
        iterator pos = begin();
        list_node* new_node = new list_node;
        ++pos.cur_block->length;
        ++size_;
        new_node->nxt = pos.cur_node;
        pos.cur_block->head = new_node;
        new_node->val = new T(std::move(value));
        pos.cur_node->pre = new_node;
        if (pos.cur_block->length >= block_size) {
            list_node* tmp_node = pos.cur_block->head;
            int tmp = 0;
            while (tmp < (block_size >> 1)) {
                tmp_node = tmp_node->nxt;
                ++tmp;
            }
            split(tmp_node, pos.cur_block);
        }
    }
    void pop_front() {
        if (size_ == 0) std::out_of_range("out of range");
        iterator pos = begin();
        --size_;
        pos.cur_block->head = pos.cur_node->nxt;
        pos.cur_node->nxt->pre = nullptr;
        --pos.cur_block->length;
        delete pos.cur_node->val;
        delete pos.cur_node;
        if (pos.cur_block->length == 0 && pos.cur_block->nxt != tail) {
            delete pos.cur_block->tail;
            head->nxt = pos.cur_block->nxt;
            pos.cur_block->nxt->pre = head;
            delete pos.cur_block;
            block_node* tmp_node = head->nxt;
            while (tmp_node != tail) {
                --tmp_node->index;
                tmp_node = tmp_node->nxt;
            }
        } else if (pos.cur_block->length != 0 && pos.cur_block->nxt != tail) {
            if (pos.cur_block->length + pos.cur_block->nxt->length <= (block_size >> 1))
            merge(pos.cur_block, pos.cur_block->nxt);
        }
    }
};

}

#endif