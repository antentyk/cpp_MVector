#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>

class MVectorMemoryFail : public std::exception{
public:
    const char *what() const throw(){
        return "Erorr occured while manipulating with memory";
    }
};

class MVectorCapacityFail : public std::exception{
public:
    const char *what() const throw(){
        return "Capacity of vector is too big";
    }
};

class MVectorIndexOutOfRange : public std::exception{
public:
    const char *what() const throw(){
        return "Invalid subscript of MVector";
    }
};

class MVectorEmpty : public std::exception{
public:
    const char *what() const throw(){
        return "Cannot get a value because MVector is empty";
    }
};

#endif