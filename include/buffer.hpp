
#pragma once
#include <Arduino.h>
#include <stdint.h>

template<typename T, int32_t length_>
class buffer_t {

  public:

    // Total length of the data buffer
    constexpr static int32_t length{length_};

    // Type of this data buffer
    using this_type = buffer_t<T, length_>;

    // Constructs a data buffer
    explicit buffer_t() = default;

    // Constructs a copy of another data buffer
    template<typename U, int32_t other_length>
    explicit buffer_t(const buffer_t<U, other_length>& other) {
      const int32_t copy_cnt = min(this->length_, other.size());
      for (int32_t i = copy_cnt - 1; i >= 0; i--) {
        push(static_cast<T>(other[i]));
      }
    }

    // Sets this data buffer's contents equal to annother
    template<typename U, int32_t other_length>
    buffer_t& operator = (const buffer_t<U, other_length> & other) {
      this->head_ = 0;
      this->wrap_flag_ = 0;
      const int32_t copy_cnt = min(length_, other.size());
      for (int32_t i = copy_cnt - 1; i >= 0; i--) {
        push(static_cast<T>(other[i]));
      }
      return *this;
    }

    // Adds data to the buffer
    void push(const T& value) {
      if (head_ == length_) {
        wrap_flag_ = 1;
        head_ = 0;
      }
      this->data_[head_] = value;
      head_++;
    }

    // Accesses data in the buffer (FIFO)
    T& operator [] (int32_t index) {
      if (index < 0 || index >= size()) {
        return data_[0];
      } 
      if (index >= head_) {
        return data_[length_ - (index - (head_ - 1))];
      } else {
        return data_[(head_ - 1) - index];
      }
    }

    // Accesses data in the buffer (FIFO)
    const T& operator [] (int32_t index) const {
      if (index < 0 || index >= size()) {
        return data_[0];
      } 
      if (index >= head_) {
        return data_[length_ - (index - (head_ - 1))];
      } else {
        return data_[(head_ - 1) - index];
      }
    }

    // Removes all data from the buffer
    void clear() {
      head_ = 0;
      wrap_flag_ = 0;
    }

    // Gets the current size of the buffer
    int32_t size() const {
      if (wrap_flag_) {
        return length_;
      } else {
        return head_;
      }
    }

  private:

    // Fields
    int32_t head_{0};
    bool wrap_flag_{false};
    T data_[length_]{0};

};