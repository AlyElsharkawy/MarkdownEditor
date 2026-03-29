#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

// This implementation was provided by Gemini. I will need to revise it and study it later
// when I better understand the black magic that is C++ templates

template <typename T, std::size_t N> class CircularStack 
{
  static_assert(N > 0, "Capacity must be greater than 0");

private:
  std::array<T, N> arr;
  std::size_t writeIndex = 0;
  std::size_t currentSize = 0;

public:
  void push(const T &value) 
  {
    arr[writeIndex] = value;
    writeIndex = (writeIndex + 1) % N;
    if (currentSize < N) 
    {
      currentSize++;
    }
  }

  // --- LIFO Iterator (Newest to Oldest) ---
  template <bool IsConst> class LifoIterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using ArrayType =
        std::conditional_t<IsConst, const std::array<T, N>, std::array<T, N>>;

  private:
    ArrayType *buffer_;
    std::size_t write_idx_;
    std::size_t count_;

  public:
    LifoIterator(ArrayType *buffer, std::size_t write_idx, std::size_t count)
        : buffer_(buffer), write_idx_(write_idx), count_(count) {}

    // Dereference: Start at write_idx, step back 1 to get the newest,
    // then step back by 'count_'. We add N to prevent negative numbers before
    // modulo.
    reference operator*() const {
      std::size_t physical_index = (write_idx_ + N - 1 - count_) % N;
      return (*buffer_)[physical_index];
    }

    pointer operator->() const {
      std::size_t physical_index = (write_idx_ + N - 1 - count_) % N;
      return &(*buffer_)[physical_index];
    }

    LifoIterator &operator++() {
      count_++;
      return *this;
    }

    LifoIterator operator++(int) {
      LifoIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const LifoIterator &a, const LifoIterator &b) {
      return a.count_ == b.count_;
    }

    friend bool operator!=(const LifoIterator &a, const LifoIterator &b) {
      return a.count_ != b.count_;
    }
  };

  using iterator = LifoIterator<false>;
  using const_iterator = LifoIterator<true>;

  iterator begin() { return iterator(&arr, writeIndex, 0); }
  iterator end() { return iterator(&arr, writeIndex, currentSize); }

  const_iterator begin() const { return const_iterator(&arr, writeIndex, 0); }
  const_iterator end() const {
    return const_iterator(&arr, writeIndex, currentSize);
  }

  std::size_t size() const { return currentSize; }
  std::size_t capacity() const { return N; }
};
