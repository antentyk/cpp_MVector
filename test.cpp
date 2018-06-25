#include <queue>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <iterator>

#include <gtest/gtest.h>

#include "MVector.hpp"

template class MVector<int>;
template class MVector<double>;
template class MVector<MVector<int>>;
template class MVector<std::queue<std::string>>;

TEST(MVector, initialization){
    MVector<int> v1;
    ASSERT_EQ(v1.size(), 0);
    ASSERT_EQ(v1.capacity(), v1.kMinCapacity);

    MVector<int> v2(10);
    ASSERT_EQ(v2.size(), 0);
    ASSERT_EQ(v2.capacity(), 10);

    auto v2beg = v2.begin();

    MVector<int> v3(std::move(v2));
    ASSERT_EQ(v3.size(), 0);
    ASSERT_EQ(v3.capacity(), 10);
    ASSERT_EQ(v3.begin(), v2beg);
    ASSERT_NE(v3.begin(), v2.begin());
    ASSERT_EQ(v2.size(), 0);
    ASSERT_EQ(v2.capacity(), v3.kMinCapacity);

    MVector<int> v4(v3);
    ASSERT_EQ(v4.size(), v3.size());
    ASSERT_EQ(v4.capacity(), v3.capacity());
    ASSERT_NE(v4.begin(), v3.begin());
}

TEST(MVector, pushBackPopBackSubscriptFrontBack){
    MVector<int> v(100);
    ASSERT_TRUE(v.empty());

    ASSERT_THROW(v.front(), MVectorEmptyError);
    ASSERT_THROW(v.back(), MVectorEmptyError);

    for(size_t i = 0; i < 100; ++i){
        ASSERT_EQ(v.size(), i);
        v.push_back(i);
        ASSERT_EQ(v.at(i), i);
        ASSERT_EQ(v[i], i);
        ASSERT_THROW(v.at(i + 1), MVectorOutOfBoundsError);
    }

    ASSERT_EQ(v.front(), 0);

    for(size_t i = 0; i < 100; ++i){
        ASSERT_EQ(v.size(), 100 - i);
        ASSERT_EQ(v.back(), 100 - i - 1);
        ASSERT_EQ(v.front(), 0);
        v.pop_back();
    }
}

TEST(MVector, initializationInitialValue){
    MVector<int> v(100, 5);

    for(size_t i = 0; i < 100; ++i)
        ASSERT_EQ(v.at(i), 5);
}

TEST(MVector, initializationIterators){
    MVector<int> v;
    for(size_t i = 0; i < 10; ++i)
        v.push_back(i);
    
    MVector<int> v1(v.begin() + 1, v.begin() + 3);

    ASSERT_EQ(v1.size(), 2);
    ASSERT_EQ(v1.capacity(), 2);

    ASSERT_EQ(v1.front(), 1);
    ASSERT_EQ(v1.back(), 2);
    ASSERT_NE(&v1.front(), &v.at(1));
    ASSERT_NE(&v1.back(), &v.at(2));
}

TEST(MVector, copyConstructors){
    MVector<int> v;
    
    for(size_t i = 0; i < 10; ++i)
        v.push_back(i);
    
    MVector<int> v1(v);

    ASSERT_EQ(v.size(), v1.size());
    ASSERT_EQ(v.capacity(), v1.capacity());
    ASSERT_NE(v.begin(), v1.begin());

    for(size_t i = 0; i < 10; ++i){
        ASSERT_NE(&v.at(i), &v1.at(i));
        ASSERT_EQ(v.at(i), v1.at(i));
    }

    MVector<int> v2(std::move(v1));
    ASSERT_EQ(v1.size(), 0);
    ASSERT_EQ(v1.capacity(), v1.kMinCapacity);

    for(size_t i = 0; i < 10; ++i)
        ASSERT_EQ(v2.at(i), i);
}

TEST(MVector, data){
    MVector<int> v;

    for(size_t i = 0; i < 10; ++i)
        v.push_back(i);
    
    auto data = v.data();
    ASSERT_EQ(*data, 0);
    *data = 1;
    ASSERT_EQ(v.front(), 1);
}

TEST(MVector, max_size){
    MVector<char> vChar;
    MVector<int> vInt;
    ASSERT_EQ(vChar.max_size(), static_cast<size_t>(-1));
    ASSERT_EQ(vInt.max_size(), vChar.max_size() / sizeof(int));
}

TEST(MVector, reserve){
    MVector<int> v;
    
    for(int i = 0; i < 10; ++i)
        v.push_back(i);
    
    v.reserve(10000);
    ASSERT_EQ(v.size(), 10);
    ASSERT_EQ(v.capacity(), 10000);

    for(size_t i = 0; i < 10; ++i)
        ASSERT_EQ(v.at(i), i);
    
    v.reserve(1);
    ASSERT_NE(v.capacity(), 1);
    ASSERT_EQ(v.size(), 10);
    ASSERT_EQ(v.capacity(), 10000);

    for(size_t i = 0; i < 10; ++i)
        ASSERT_EQ(v.at(i), i);
    
    MVector<int> v1;
    ASSERT_EQ(v1.capacity(), v1.kMinCapacity);
    v1.reserve(100);
    ASSERT_TRUE(v1.empty());
    ASSERT_EQ(v1.capacity(), 100);
}

TEST(MVector, shrink_to_fit){
    MVector<int> v;
    v.shrink_to_fit();
    ASSERT_NE(v.capacity(), 0);

    for(int i = 0; i < 10; ++i)
        v.push_back(i);
    
    v.reserve(100);

    ASSERT_EQ(v.size(), 10);
    ASSERT_EQ(v.capacity(), 100);

    v.shrink_to_fit();
    ASSERT_EQ(v.size(), 10);
    ASSERT_EQ(v.capacity(), 10);
}

TEST(MVector, resize){
    MVector<int> v;

    for(int i = 0; i < 10; ++i)
        v.push_back(i);
    
    v.resize(5);
    ASSERT_EQ(v.size(), 5);

    for(size_t i = 0; i < 5; ++i)
        ASSERT_EQ(v.at(i), i);
    
    v.resize(10);
    ASSERT_EQ(v.size(), 10);

    for(size_t i = 0; i < 5; ++i)
        ASSERT_EQ(v.at(i), i);
    for(size_t i = 5; i < 10; ++i)
        ASSERT_EQ(v.at(i), 0);
    
    v.resize(20, 100);
    
    for(size_t i = 0; i < 5; ++i)
        ASSERT_EQ(v.at(i), i);
    for(size_t i = 5; i < 10; ++i)
        ASSERT_EQ(v.at(i), 0);
    for(size_t i = 10; i < 20; ++i)
        ASSERT_EQ(v.at(i), 100);
    
    v.resize(0, 1000);
    ASSERT_TRUE(v.empty());
    ASSERT_NE(v.capacity(), 0);

    MVector<int> v1;
    v1.resize(0, 10);
    ASSERT_NE(v1.capacity(), 0);
    ASSERT_TRUE(v1.empty());
}

TEST(MVector, initializer_list){
    MVector<int> v{1, 2, 3};
    ASSERT_EQ(v.size(), 3);
    ASSERT_EQ(v.capacity(), 3);

    for(size_t i = 0; i < v.size(); ++i)
        ASSERT_EQ(v.at(i), i + 1);
    
    MVector<int> v1{};
    ASSERT_TRUE(v1.empty());
}

TEST(MVector, erase){
    MVector<int> v{1,2,3};

    v.erase(v.begin());
    ASSERT_EQ(v.size(), 2);
    ASSERT_EQ(v.front(), 2);
    ASSERT_EQ(v.back(), 3);

    MVector<int> v1{1,2,3,4,5,6,7,8,9};
    v1.erase(v1.begin() + 1, v1.begin() + 3);
    ASSERT_EQ(v1.size(), 7);
    ASSERT_EQ(v1.at(0), 1);
    ASSERT_EQ(v1.at(1), 4);
    ASSERT_EQ(v1.at(2), 5);
    ASSERT_EQ(v1.at(3), 6);
    ASSERT_EQ(v1.at(4), 7);
    ASSERT_EQ(v1.at(5), 8);
    ASSERT_EQ(v1.at(6), 9);
}

TEST(MVector, insert){
    MVector<int> v{1,2,3};

    auto it = v.insert(v.begin(), 0);
    ASSERT_EQ(*it, 0);
    ASSERT_EQ(it, v.begin());

    ASSERT_EQ(v.size(), 4);
    ASSERT_EQ(v.front(), 0);
    ASSERT_EQ(v.at(1), 1);
    ASSERT_EQ(v.at(2), 2);
    ASSERT_EQ(v.at(3), 3);

    v.clear();
    v = MVector<int>({1,2,3});
    it = v.insert(v.begin() + 1, 3, 0);
    ASSERT_EQ(it, v.begin() + 1);
    ASSERT_EQ(v.size(), 6);
    ASSERT_GE(v.capacity(), v.size());
    ASSERT_EQ(v.at(0), 1);
    for(int i = 1; i < 1 + 3; ++i)
        ASSERT_EQ(v.at(i), 0);
    ASSERT_EQ(v.back(), 3);
    ASSERT_EQ(*(v.end() - 2), 2);

    v.clear();
    v = {1, 2, 3};
    it = v.insert(v.end(), 0, 1);
    ASSERT_EQ(v.size(), 3);
    ASSERT_EQ(v.back(), 3);

    it = v.insert(v.end(), 100);
    ASSERT_EQ(it, v.end() - 1);
    ASSERT_EQ(v.size(), 4);
    ASSERT_EQ(v.back(), 100);

    v.clear();
    v = {1, 2, 3};
    v.reserve(6);
    v.insert(v.begin(), v.begin(), v.end());
    ASSERT_EQ(v.size(), 6);
    ASSERT_EQ(v.at(0), 1);
    ASSERT_EQ(v.at(1), 2);
    ASSERT_EQ(v.at(2), 3);
    ASSERT_EQ(v.at(3), 1);
    ASSERT_EQ(v.at(4), 2);
    ASSERT_EQ(v.at(5), 3);
}

TEST(MVector, equality){
    MVector<int> v{1,2,3,4};
    ASSERT_TRUE(v == v);
    ASSERT_FALSE(v != v);

    MVector<int> v1 = v;
    ASSERT_NE(v.data(), v1.data());
    ASSERT_TRUE(v == v1);
    ASSERT_FALSE(v != v1);

    v1.reserve(100000);
    ASSERT_TRUE(v == v1);
    v.push_back(0);
    ASSERT_FALSE(v == v1);
}

TEST(MVector, sort){
    MVector<int> v{6,4,5,3,2,1};
    MVector<int> expected{1,2,3,4,5,6};

    std::sort(v.begin(), v.end());

    ASSERT_TRUE(v == expected);    
}

TEST(MVector, backInserter){
    MVector<int> v;

    auto it = std::back_inserter(v);

    MVector<int> expected;

    ASSERT_TRUE(v == expected);

    expected.push_back(1);
    it = 1;
    ASSERT_TRUE(v == expected);
}

TEST(MVector, reverse){
    MVector<int> v{1, 3, 2};
    MVector<int> expected{2, 3, 1};

    std::reverse(v.begin(), v.end());
    ASSERT_TRUE(v == expected);

    std::reverse(v.begin() + 1, v.end());
    std::swap(expected.back(), expected.at(1));

    ASSERT_TRUE(v == expected);
}

TEST(MVector, swap){
    MVector<int> a{1, 2, 3};
    MVector<int> b{2, 1};

    auto aData = a.data();
    auto bData = b.data();

    a.swap(b);

    ASSERT_EQ(a.size(), 2);
    ASSERT_EQ(b.size(), 3);
    ASSERT_EQ(a.data(), bData);
    ASSERT_EQ(b.data(), aData);
}

TEST(MVector, front_inserter){
    MVector<int> v;

    auto it = std::front_inserter(v);

    it = 1;
    ASSERT_TRUE(v == MVector<int>({1}));
    it = 2;
    ASSERT_TRUE(v == MVector<int>({2, 1}));
    it = 5;
    ASSERT_TRUE(v == MVector<int>({5, 2, 1}));
}

TEST(MVector, reverseIterators){
    MVector<int> v;
    ASSERT_TRUE(v.rbegin() == v.rend());
    v.push_back(1);
    v.push_back(2);

    MVector<int> res;
    for(auto it = v.crbegin(); it != v.crend(); ++it)
        res.push_back(*it);
    
    std::reverse(v.begin(), v.end());
    ASSERT_TRUE(v == res);
}

int main(int argc, char ** argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
