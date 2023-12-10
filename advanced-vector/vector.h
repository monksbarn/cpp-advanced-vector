#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <memory>


template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory& rhs) = delete;

    RawMemory(RawMemory&& other) noexcept;

    RawMemory& operator=(RawMemory&& rhs) noexcept;

    T* operator+(size_t offset) noexcept;

    const T* operator+(size_t offset) const noexcept;

    const T& operator[](size_t index) const noexcept;

    T& operator[](size_t index) noexcept;

    void Swap(RawMemory& other) noexcept;

    const T* GetAddress() const noexcept;

    T* GetAddress() noexcept;

    size_t Capacity() const;

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector {
public:
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

public:

    Vector() = default;
    explicit Vector(size_t size) :
        data_(size),
        size_(size)
    {
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }

    Vector(const Vector& other);

    Vector(Vector&& other) noexcept;

    void Reserve(size_t new_capacity);

    void Resize(size_t new_size);

    template<typename U>
    void PushBack(U&& val);

    void PopBack() noexcept;

    template <typename... Args>
    T& EmplaceBack(Args&&... args);

    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args);

    iterator Erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<T>);

    iterator Insert(const_iterator pos, const T& value);
    iterator Insert(const_iterator pos, T&& value);

    size_t Size() const noexcept;

    size_t Capacity() const noexcept;

    void Swap(Vector& other) noexcept;

    Vector& operator=(const Vector& rhs);

    Vector& operator=(Vector&& rhs) noexcept;

    const T& operator[](size_t index) const noexcept;

    T& operator[](size_t index) noexcept;

    ~Vector();

private:
    RawMemory<T> data_;
    size_t size_ = 0;

    void CopyOrMoveN(T* data, size_t size, T* new_data) {
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(data, size, new_data);
        }
        else {
            std::uninitialized_copy_n(data, size, new_data);
        }
    }

    void CopyOrMove(T* data, T* new_data) {
        if constexpr (std::is_move_constructible_v<T>) {
            std::uninitialized_move_n(data, 1, new_data);
        }
        else {
            std::uninitialized_copy_n(data, 1, new_data);
        }
    }

    bool TryPush(T* data, T& value, bool is_rvalue) {
        bool result = false;
        if (is_rvalue) {
            if constexpr (std::is_move_constructible_v<T>) {
                try {
                    new(data + size_) T(std::move(value));
                    result = true;
                }
                catch (...) {
                    throw;
                }
            }
        }
        else {
            if constexpr (std::is_copy_constructible_v<T>) {
                try {
                    new(data + size_) T(value);
                    result = true;
                }
                catch (...) {
                    throw;
                }
            }
        }
        return result;
    }
};


//=========================RAw_MEMORY==========================================

template<typename T>
RawMemory<T>::RawMemory(RawMemory&& other) noexcept {
    buffer_ = other.buffer_;
    capacity_ = other.capacity_;
    other.buffer_ = nullptr;
}

template<typename T>
RawMemory<T>& RawMemory<T>::operator=(RawMemory&& rhs) noexcept {
    buffer_ = rhs.buffer_;
    capacity_ = rhs.capacity_;
    rhs.buffer_ = nullptr;
    return *this;
}

template<typename T>
T* RawMemory<T>::operator+(size_t offset) noexcept {
    // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
    assert(offset <= capacity_);
    return buffer_ + offset;
}

template<typename T>
const T* RawMemory<T>::operator+(size_t offset) const noexcept {
    return const_cast<RawMemory&>(*this) + offset;
}

template<typename T>
const T& RawMemory<T>::operator[](size_t index) const noexcept {
    return const_cast<RawMemory&>(*this)[index];
}

template<typename T>
T& RawMemory<T>::operator[](size_t index) noexcept {
    assert(index < capacity_);
    return buffer_[index];
}

template<typename T>
void RawMemory<T>::Swap(RawMemory& other) noexcept {
    std::swap(buffer_, other.buffer_);
    std::swap(capacity_, other.capacity_);
}

template<typename T>
const T* RawMemory<T>::GetAddress() const noexcept {
    return buffer_;
}

template<typename T>
T* RawMemory<T>::GetAddress() noexcept {
    return buffer_;
}

template<typename T>
size_t RawMemory<T>::Capacity() const {
    return capacity_;
}

//========================VECTOR=================================

template<typename T>
typename Vector<T>::iterator Vector<T>::begin() noexcept {
    return data_.GetAddress();
}
template<typename T>
typename Vector<T>::iterator Vector<T>::end() noexcept {
    return begin() + size_;
}
template<typename T>
typename Vector<T>::const_iterator Vector<T>::begin() const noexcept {
    return data_.GetAddress();
}
template<typename T>
typename Vector<T>::const_iterator Vector<T>::end() const noexcept {
    return begin() + size_;
}
template<typename T>
typename Vector<T>::const_iterator Vector<T>::cbegin() const noexcept {
    return begin();
}
template<typename T>
typename Vector<T>::const_iterator Vector<T>::cend() const noexcept {
    return end();
}

template<typename T>
Vector<T>::Vector(const Vector& other) :
    data_(other.size_),
    size_(other.size_)
{
    std::uninitialized_copy_n(other.data_.GetAddress(), other.size_, data_.GetAddress());
}
template<typename T>
Vector<T>::Vector(Vector&& other) noexcept :
    data_(std::move(other.data_)),
    size_(std::move(other.size_)) {
}

template<typename T>
void Vector<T>::Reserve(size_t new_capacity)
{
    if (new_capacity > data_.Capacity())
    {
        RawMemory<T> new_data(new_capacity);
        CopyOrMoveN(data_.GetAddress(), size_, new_data.GetAddress());
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }
}

template<typename T>
void Vector<T>::Resize(size_t new_size) {
    if (new_size < size_) {
        std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);
    }
    else {
        Reserve(new_size);
        std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
    }
    size_ = new_size;
}

template<typename T>
template<typename U>
void Vector<T>::PushBack(U&& val) {
    decltype(auto) value = std::forward<U>(val);
    constexpr bool is_rvalue = std::is_same_v<T&&, decltype(value)>;
    if (size_ == Capacity()) {
        RawMemory<T> new_data(size_ == 0 ? 1 : 2 * size_);
        CopyOrMoveN(data_.GetAddress(), size_, new_data.GetAddress());
        if (TryPush(new_data.GetAddress(), value, is_rvalue)) {
            std::destroy_n(data_.GetAddress(), size_++);
            data_.Swap(new_data);
        }
    }
    else {
        if (TryPush(data_.GetAddress(), value, is_rvalue)) {
            ++size_;
        }
    }
}

template<typename T>
template <typename... Args>
T& Vector<T>::EmplaceBack(Args&&... args) {
    if (size_ == Capacity()) {
        RawMemory<T> new_data(size_ == 0 ? 1 : 2 * size_);
        CopyOrMoveN(data_.GetAddress(), size_, new_data.GetAddress());
        try {
            new(new_data.GetAddress() + size_) T(std::forward<Args>(args)...);
        }
        catch (...) {
            throw;
        }
        std::destroy_n(data_.GetAddress(), size_++);
        data_.Swap(new_data);
    }
    else {
        try {
            new(data_.GetAddress() + size_) T(std::forward<Args>(args)...);
        }
        catch (...) {
            throw;
        }
        ++size_;
    }
    return data_[size_ - 1];
}

template<typename T>
void Vector<T>::PopBack() noexcept {
    std::destroy_at(end() - 1);
    --size_;
}

template<typename T>
template <typename... Args>
typename Vector<T>::iterator Vector<T>::Emplace(const_iterator pos, Args&&... args) {
    size_t cp_pos = pos - begin();
    size_t cp_length = end() - pos;
    if (size_ == Capacity()) {
        RawMemory<T> new_data(size_ == 0 ? 1 : 2 * size_);
        try {
            new(new_data.GetAddress() + cp_pos) T(std::forward<Args>(args)...);
        }
        catch (...) {
            throw;
        }
        CopyOrMoveN(begin(), cp_pos, new_data.GetAddress());
        CopyOrMoveN(begin() + cp_pos, cp_length, new_data + cp_pos + 1);
        std::destroy_n(begin(), size_++);
        data_.Swap(new_data);
    }
    else {
        T value(std::forward<Args>(args)...);
        CopyOrMove(end() - 1, end());
        if constexpr (std::is_move_assignable_v<T>) {
            try {
                std::move_backward(begin() + cp_pos, end() - 1, end());
                data_[cp_pos] = std::move(value);
            }
            catch (...) {
                throw;
            }
        }
        else {
            try {
                std::copy_backward(begin() + cp_pos, end() - 1, end());
                data_[cp_pos] = value;
            }
            catch (...) {
                throw;
            }
        }
        ++size_;
    }
    return begin() + cp_pos;
}

template<typename T>
typename Vector<T>::iterator Vector<T>::Erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<T>) {
    size_t cp_pos = pos - begin() + 1;
    if constexpr (std::is_nothrow_move_assignable_v<T>) {
        std::move(begin() + cp_pos, end(), begin() + cp_pos - 1);
    }
    else {
        std::copy(begin() + cp_pos, end() - pos - 1, begin() + cp_pos - 1);
    }
    std::destroy_at(end() - 1);
    --size_;
    return begin() + cp_pos - 1;
}
template<typename T>
typename Vector<T>::iterator Vector<T>::Insert(const_iterator pos, const T& value) {
    return  Emplace(pos, value);
}

template<typename T>
typename Vector<T>::iterator Vector<T>::Insert(const_iterator pos, T&& value) {
    return Emplace(pos, std::move(value));
}

template<typename T>
Vector<T>::~Vector()
{
    if (data_.GetAddress()) {
        std::destroy_n(data_.GetAddress(), size_);
    }
}

template<typename T>
size_t Vector<T>::Size() const noexcept
{
    return size_;
}

template<typename T>
size_t Vector<T>::Capacity() const noexcept
{
    return data_.Capacity();
}

template<typename T>
void Vector<T>::Swap(Vector& other) noexcept {
    data_.Swap(other.data_);
    std::swap(size_, other.size_);
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs) {
    if (this != &rhs) {
        if (rhs.size_ <= size_) {
            for (size_t i = 0; i < rhs.size_; ++i) {
                data_[i] = rhs.data_[i];
            }
            std::destroy_n(data_ + rhs.size_, size_ - rhs.size_);
            size_ = rhs.size_;
        }
        else if (rhs.size_ <= Capacity()) {
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = rhs.data_[i];
            }
            std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, rhs.size_ - size_, data_.GetAddress() + size_);
            size_ = rhs.size_;
        }
        else {
            Vector rhs_copy(rhs);
            Swap(rhs_copy);
        }
    }
    return*this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector&& rhs) noexcept {

    if (size_ == rhs.size_)
    {
        Swap(rhs);
    }
    else
    {
        std::destroy_n(data_.GetAddress(), size_);
        data_ = std::move(rhs.data_);
        size_ = rhs.size_;
    }
    return *this;
}

template<typename T>
const T& Vector<T>::operator[](size_t index) const noexcept
{
    return const_cast<Vector&>(*this)[index];
}

template<typename T>
T& Vector<T>::operator[](size_t index) noexcept
{
    assert(index < size_);
    return data_[index];
}