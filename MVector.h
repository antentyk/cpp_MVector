#ifndef MVECTOR_H_
#define MVECTOR_H_

#include <new>

#include "Exceptions.h"

template<typename T>
class MVector
{
public:
    explicit MVector(size_t capacity):
        data_(nullptr)
    {
        if(capacity > max_size())
            throw MVectorCapacityFail();

        rawInit(capacity < kMinCapacity? kMinCapacity: capacity);
    }

    MVector(): MVector(0)
    {}

    MVector(const MVector &rhs):
        data_(nullptr)
    {
        rawInit(rhs.capacity_);

        size_ = rhs.size_;

        for(size_t i = 0; i < size_; ++i)
            data_[i] = rhs.data_[i];
    }

    MVector& operator=(const MVector& rhs)
    {
        if(&rhs == this)
            return *this;
        
        rawClear();

        rawInit(rhs.capacity_);

        size_ = rhs.size_;

        for(size_t i = 0; i < size_; ++i)
            data_[i] = rhs.data_[i];
        
        return *this;
    }

    ~MVector()
    {
        rawClear();
    }

    size_t max_size()
    {
        size_t result = -1;
        result /= sizeof(T);
        return result;
    }

    const T* data() const {return data_;}
    T* data(){return data_;}
    size_t size() const {return size_;}
    size_t capacity() const {return capacity_;}

    const T& at(size_t index) const{
        if(index >= size_)
            throw MVectorIndexOutOfRange();
        
        return data_[index];
    }
    T& at(size_t index){
        if(index >= size_)
            throw MVectorIndexOutOfRange();
        
        return data_[index];
    }
    const T& operator[](size_t index) const{return at(index);}
    T& operator[](size_t index){return at(index);}

    void clear(){
        rawClear();
        rawInit(kMinCapacity);
    }

    void reserve(size_t newCapacity)
    {
        if(newCapacity <= capacity_)
            return;
        
        MVector<T> tmp(*this);

        rawClear();
        rawInit(newCapacity);

        size_ = tmp.size();

        for(size_t i = 0; i < size_; ++i)
            data_[i] = tmp[i];
    }

    void push_back(const T &val)
    {
        if(size_ < capacity_)
        {
            new(data_ + size_) T(val);
            size_++;
            return;
        }

        size_t newCapacity = capacity_;
        newCapacity *= 2; // didn't handle corner cases

        reserve(newCapacity);
        push_back(val);
    }

    bool empty() const{return size_ == 0;}

    const T& front() const{
        if(empty())
            throw MVectorEmpty();
        
        return at(0);
    }
    T& front(){
        if(empty())
            throw MVectorEmpty();
        
        return at(0);
    }

    const T& back() const{
        if(empty())
            throw MVectorEmpty();
        
        return at(size() - 1);
    }
    T& back(){
        if(empty())
            throw MVectorEmpty();
        
        return at(size() - 1);
    }

private:
    static const size_t kMinCapacity = 8;

    size_t
        size_,
        capacity_;
    
    T* data_;

    void rawClear()
    {
        if(data_ == nullptr)
            return;

        for(;size_ >= 1; --size_)
            (data_ + (size_ - 1))->~T();
        
        ::operator delete(data_);

        data_ = nullptr;
        capacity_ = 0;
    }

    void rawInit(size_t capacity)
    {
        size_ = 0;
        capacity_ = capacity;

        data_ = static_cast<T*>(::operator new(capacity_ * sizeof(T), std::nothrow));

        if(data_ == nullptr)
            throw MVectorMemoryFail();
    }
};

#endif