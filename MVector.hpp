#ifndef MVECTOR_H_
#define MVECTOR_H_

#include <cstddef>
#include <iterator>
#include <algorithm>
#include <new>
#include <utility>
#include <initializer_list>

#include "Exceptions.hpp"

template<class T>
class MVector{
public:
    static const size_t kMinCapacity;

    typedef T* iterator;
    typedef T const * const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef T value_type;

    inline size_t size() const noexcept{ return size_; }
    inline size_t capacity() const noexcept{ return capacity_; }

    inline iterator begin() noexcept{ return data_; }
    inline iterator end() noexcept{ return data_ + size_; }

    inline const_iterator cbegin() const noexcept{ return data_; }
    inline const_iterator cend() const noexcept{ return data_ + size_; }

    inline reverse_iterator rbegin() noexcept{ return std::reverse_iterator<iterator>(end()); }
    inline reverse_iterator rend() noexcept{ return std::reverse_iterator<iterator>(begin()); }

    inline const_reverse_iterator crbegin() const noexcept{ return std::reverse_iterator<const_iterator>(cend()); }
    inline const_reverse_iterator crend() const noexcept{ return std::reverse_iterator<const_iterator>(cbegin()); }  

    const T& front() const{
        if(empty())
            throw MVectorEmptyError();
        
        return *cbegin();
    }
    T& front(){
        if(empty())
            throw MVectorEmptyError();
        
        return *begin();
    }

    const T& back() const{
        if(empty())
            throw MVectorEmptyError();
        
        return *(cend() - 1);
    }
    T& back(){
        if(empty())
            throw MVectorEmptyError();
        
        return *(end() - 1);
    }

    MVector(): MVector(kMinCapacity){}

    MVector(const std::initializer_list<T> &lst): MVector(lst.size()){
        size_ = lst.size();
        
        auto iter = begin();
        for(auto &it: lst)
            construct(iter++, it);
    }

    MVector(size_t size, const T& val): MVector(size){
        size_ = size;
        construct(begin(), end(), val);
    }

    MVector(const_iterator begin, const_iterator end){
        if(begin > end)
            throw MVectorIteratorError();

        size_ = end - begin;
        capacity_ = std::max(size_, kMinCapacity);
        data_ = getMemory(capacity());

        iterator current = this->begin();    
        for(; begin != end; ++begin, ++current)
            construct(current, *begin);
    }

    MVector(size_t capacity){
        capacity = std::max(capacity, kMinCapacity);
        data_ = getMemory(capacity);
        capacity_ = capacity;
    }

    MVector(const MVector<T>& rhs):MVector(rhs.capacity_){
        std::copy(rhs.cbegin(), rhs.cend(), begin());
        size_ = rhs.size();
        capacity_ = rhs.capacity();
    }

    MVector(MVector<T>&& rhs){
        T* newRhsData = getMemory(kMinCapacity);

        this->~MVector();
        size_ = rhs.size();
        capacity_ = rhs.capacity();
        data_ = rhs.data_;

        rhs.size_ = 0;
        rhs.capacity_ = kMinCapacity;
        rhs.data_ = newRhsData;
    }

    MVector<T>& operator=(const MVector<T>& rhs){
        T* newData = getMemory(rhs.capacity());

        this->~MVector();

        size_ = rhs.size();
        capacity_ = rhs.capacity();
        data_ = newData;

        std::copy(rhs.cbegin(), rhs.cend(), begin());

        return *this;
    }

    MVector<T>& operator=(MVector<T>&& rhs){
        T* newRhsData = getMemory(kMinCapacity);

        this->~MVector();
        size_ = rhs.size();
        capacity_ = rhs.capacity();
        data_ = rhs.data_;

        rhs.size_ = 0;
        rhs.capacity_ = kMinCapacity;
        rhs.data_ = newRhsData;

        return *this;
    }

    ~MVector(){
        destroy(begin(), end());
        ::operator delete(data_);
    }

    const T& at(size_t i) const{
        if(i >= size())
            throw MVectorOutOfBoundsError();
        
        return *(data_ + i);
    }
    T& at(size_t i){
        if(i >= size())
            throw MVectorOutOfBoundsError();
        
        return *(data_ + i);
    }

    inline const T& operator[](size_t i) const { return *(data_ + i); }
    inline T& operator[](size_t i) { return *(data_ + i); }

    void push_back(const T& val){
        const T& elem = isWithin(val)? T(val): val;
        if(capacity() == size())
            reserve(2 * capacity());
        ++size_;
        construct(end() - 1, elem);
    }
    void push_front(const T& val){
        insert(begin(), val);
    }

    inline bool empty() const noexcept{ return size_ == 0; }

    void pop_back(){
        if(empty())
            throw MVectorEmptyError();
        
        destroy(end() - 1);
        --size_;
    }

    inline T* data() noexcept{ return data_; }
    inline T const * data() const noexcept{ return data_; }

    inline size_t max_size() const noexcept{ return static_cast<size_t>(-1) / sizeof(T); }

    void reserve(size_t newCapacity){
        if(newCapacity < capacity())
            return;
        
        grow(newCapacity);
    }

    void shrink_to_fit(){
        size_t newCapacity = empty()? kMinCapacity: size();
        if(capacity() == newCapacity)return;
        
        T* newData = getMemory(newCapacity);
        construct(newData, newData + size(), begin());

        this->~MVector<T>();

        capacity_ = newCapacity;
        data_ = newData;
    }

    void clear(){
        erase(begin(), end());
    }

    void swap(MVector<T> &rhs) noexcept {
        std::swap(size_, rhs.size_);
        std::swap(capacity_, rhs.capacity_);
        std::swap(data_, rhs.data_);
    }

    void resize(size_t count){
        if(count < size())
        {
            destroy(begin() + count, end());
            size_ = count;
            return;
        }
        if(count > size())return resize(count, T());
    }
    void resize(size_t count, const T& val){
        if(count < size())return resize(count);
        if(count > size()){
            reserve(count);
            size_t oldCount = size();
            size_ = count;
            construct(begin() + oldCount, end(), val);
        }
    }

    void erase(iterator first, iterator last){
        destroy(first, last);
        if(last < end())
            shiftLeft(last, end(), first);
        size_ -= (last - first);
    }
    void erase(iterator pos){
        return erase(pos, pos + 1);
    }

    iterator insert(iterator pos, const T& val){
        return insert(pos, 1, val);
    }
    iterator insert(iterator pos, size_t cnt, const T& val){
        size_t n = pos - begin();

        const T& elem = isWithin(val)? T(val): val;

        if(size() + cnt > capacity())
            reserve(std::max(capacity() * 2, size() + cnt));
        pos = begin() + n;

        size_ += cnt;
        shiftRight(pos, end() - cnt, end());
        construct(pos, pos + cnt, elem);

        return pos;
    }

    iterator insert(iterator pos, const_iterator first, const_iterator last){
        size_t cnt = last - first;
        size_t n = pos - begin();

        if(size() + cnt > capacity())
            reserve(std::max(capacity() * 2, size() + cnt));
        pos = begin() + n;

        size_ += cnt;
        shiftRight(pos, end() - cnt, end());
        construct(pos, pos + cnt, first);

        return pos;
    }

    bool operator==(const MVector<T> &rhs) const{
        if(rhs.size() != size())return false;

        for(size_t i = 0; i < size(); ++i)
            if(rhs[i] != operator[](i))
                return false;
        
        return true;
    }
    bool operator!=(const MVector<T> &rhs) const{
        return !(this->operator==(rhs));
    }

private:
    static T* getMemory(size_t capacity){
        T* result = static_cast<T*>(::operator new(sizeof(T) * capacity, std::nothrow));
        if(result == nullptr)
            throw MVectorMemoryAllocationError();
        return result;
    }

    void destroy(iterator first, iterator last){
        while(first != last)
            (first++)->~T();
    }

    inline void destroy(iterator first){ return destroy(first, first + 1); }

    void construct(T* first, T* last, const T& val){
        while(first != last)
            new(first++) T(val);
    }

    void construct(T* first, T* last, const_iterator pos){
        while(first < last)
            new(first++) T(*(pos++));
    }

    inline void construct(iterator first, const T& val){ construct(first, first + 1, val); }

    inline bool isWithin(const T& val){ return begin() <= &val && &val < end(); }

    void grow(size_t newCapacity){
        T* newData = getMemory(newCapacity);  
        
        if(newCapacity < size()){
            destroy(begin() + newCapacity, end());
            size_ = newCapacity;
        }

        std::copy(cbegin(), cend(), newData);

        this->~MVector();
        
        data_ = newData;
        capacity_ = newCapacity;
    }

    void shiftLeft(iterator first, iterator last, T* leftPos){
        while(first < last){
            construct(leftPos++, *(first++));
            destroy(first);
        }
    }

    void shiftRight(iterator first, iterator last, T* rightPos){
        --last;
        --rightPos;
        while(last >= first){
            construct(rightPos--, *(last--));
            destroy(last);
        }
    }

    size_t
        size_{0},
        capacity_{0};
    
    T *data_{nullptr};
};

template<class T>
const size_t MVector<T>::kMinCapacity = 1;

#endif