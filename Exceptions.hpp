#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>

class MVectorError : public std::exception{
public:
    const char * what() const noexcept{
        return "Error while operating with MVector";
    };
};

class MVectorMemoryAllocationError : public MVectorError{
public:
    const char * what() const noexcept{
        return "Error while allocating memory for MVector";
    };
};

class MVectorOutOfBoundsError : public MVectorError{
public:
    const char * what() const noexcept{
        return "Invalid subscript";
    }
};

class MVectorEmptyError : public MVectorError{
public:
    const char * what() const noexcept{
        return "MVector is empty";
    }
};

class MVectorIteratorError : public MVectorError{
public:
    const char * what() const noexcept{
        return "Invalid iterators for the interval";
    }
};

#endif