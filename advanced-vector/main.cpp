#include "vector.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

constexpr std::size_t SIZE = 8u;
constexpr int MAGIC = 42;

template<typename Container>
void Print(const Container& v) {
    std::cout << "Vector size: " << v.size() << std::endl;
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i] << std::endl;
    }
    std::cout << "END\n" << std::endl;
}

template <typename U>
void TestInsertImpl(Vector<int>& v, size_t capacity, size_t offset, U&& val) {
    auto pos = v.cbegin() + offset;

    bool equal = v.Size() == v.Capacity();
    Vector<int>::iterator iter = v.Insert(pos, std::forward<U>(val));
    assert(v.Size() == SIZE + 1);
    assert(v.Capacity() == capacity);
    assert(v[offset - 1] == MAGIC - 1);
    assert(v[offset] == MAGIC);
    assert(v[offset + 1] == MAGIC + 1);
    assert(*iter == MAGIC);
    if (equal) {
        assert(iter != pos);
    }
    else {
        assert(iter == pos);
    }
    assert(iter == v.cbegin() + offset);
}

void TestInsert() {
    {
        int a{ MAGIC };
        Vector<int> v(SIZE);
        v[0] = MAGIC - 1;
        v[1] = MAGIC + 1;
        TestInsertImpl(v, 2 * SIZE, 1u, a);
    } {
        int a{ MAGIC };
        Vector<int> v(SIZE);
        v[0] = MAGIC - 1;
        v[1] = MAGIC + 1;
        v.Reserve(SIZE + 1);
        TestInsertImpl(v, SIZE + 1, 1u, a);
    } {
        int a{ MAGIC };
        Vector<int> v(SIZE);
        v[0] = MAGIC - 1;
        v[1] = MAGIC + 1;
        v.Reserve(2 * SIZE);
        TestInsertImpl(v, 2 * SIZE, 1u, a);
    } {
        int a{ MAGIC };
        Vector<int> v(SIZE);
        v[0] = MAGIC - 1;
        v[1] = MAGIC + 1;
        TestInsertImpl(v, 2 * SIZE, 1u, move(a));
    } {
        int a{ MAGIC };
        Vector<int> v(SIZE);
        v[0] = MAGIC - 1;
        v[1] = MAGIC + 1;
        v.Reserve(SIZE + 1);
        TestInsertImpl(v, SIZE + 1, 1u, move(a));
    } {
        int a{ MAGIC };
        Vector<int> v(SIZE);
        v[0] = MAGIC - 1;
        v[1] = MAGIC + 1;
        v.Reserve(2 * SIZE);
        TestInsertImpl(v, 2 * SIZE, 1u, move(a));
    }
}

int main() {


    TestInsert();


    return 0;
}