#include <vector>
#include <iostream>
#include <string>
#include <gtest/gtest.h>

#include "Exceptions.h"
#include "MVector.h"

TEST(MVector, max_size)
{
    ASSERT_LE
    (
        sizeof(char) * MVector<char>().max_size(),
        static_cast<size_t>(-1)
    );

    ASSERT_LE
    (
        sizeof(double) * MVector<double>().max_size(),
        static_cast<size_t>(-1)
    );

    ASSERT_LE
    (
        sizeof(unsigned long long) * MVector<unsigned long long>().max_size(),
        static_cast<size_t>(-1)
    );

    class Tmp{};

    ASSERT_LE
    (
        sizeof(Tmp) * MVector<Tmp>().max_size(),
        static_cast<size_t>(-1)
    );
}

TEST(MVector, init)
{
    ASSERT_THROW(MVector<int>(-1), MVectorCapacityFail);
    
    ASSERT_NO_THROW(MVector<std::string>(100));
    ASSERT_NO_THROW(MVector<unsigned long long>(10000));
    ASSERT_NO_THROW(MVector<float>());

    MVector<int> a;
    ASSERT_EQ(a.size(), 0);
    ASSERT_NE(a.capacity(), 0);
    
    ASSERT_NO_THROW(MVector<int>(a));

    MVector<int> b(a);
    ASSERT_NE(a.data(), b.data());
}

TEST(MVector, reserve)
{
    MVector<double> a;
    a.reserve(10);

    ASSERT_EQ(a.size(), 0);
    ASSERT_EQ(a.capacity(), 10);

    double *prevData = a.data();

    a.reserve(100);
    ASSERT_EQ(a.size(), 0);
    ASSERT_EQ(a.capacity(), 100);

    double *curData = a.data();

    ASSERT_NE(prevData, curData);

    a.reserve(20);
    ASSERT_NE(a.capacity(), 20);
}

TEST(MVector, push_back)
{
    MVector<int> a;

    a.push_back(1);

    ASSERT_EQ(a.size(), 1);

    for(int i = 2; i <= 10; ++i)
        a.push_back(i);
    
    ASSERT_EQ(a.size(), 10);
    for(int i = 1; i <= 10; ++i)
        ASSERT_EQ(a.at(static_cast<size_t>(i - 1)), i);

    a.reserve(1000);
    ASSERT_EQ(a.size(), 10);
    ASSERT_EQ(a.capacity(), 1000);

    ASSERT_THROW(a.at(100), MVectorIndexOutOfRange);
}

TEST(MVector, frontAndback)
{
    MVector<unsigned long long> v;

    ASSERT_THROW(v.front(), MVectorEmpty);
    ASSERT_THROW(v.back(), MVectorEmpty);

    v.push_back(29384756);

    ASSERT_EQ(&v.front(), &v.back());

    v.front() = 2;
    ASSERT_EQ(v.at(0), 2);
}

int main(int argc, char ** argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}