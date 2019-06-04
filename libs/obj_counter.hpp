#pragma once

#include <atomic>
#include <mutex>
#include <sstream>
#include <map>

#include <execinfo.h>
#include <boost/core/demangle.hpp>

static std::atomic<size_t> obj_counter_longest_class_name(0);

class ConstrBt {
 public:
  ConstrBt() {
    //void *addresses[kMaxBtSize];

    bt_size_ = backtrace(bt_addresses_, kMaxBtSize);
    //backtrace_ = backtrace_symbols(addresses, bt_size_);
  }

  std::string getBt() {
    std::stringstream bt;
    bt << std::hex;
    for (size_t i = 0; i < bt_size_; ++i) {
      bt << bt_addresses_[i] << std::endl;
    }

    return bt.str();
  }

 private:
  static constexpr size_t kMaxBtSize = 100;

  //char **backtrace_;
  void *bt_addresses_[kMaxBtSize];
  size_t bt_size_{0};
};

template <typename T>
struct ObjCounter : public ConstrBt {

  ObjCounter() noexcept {
    std::lock_guard<std::mutex> lock(counter_mu_);
    object_id_ = objects_created++;
    objects_alive_.emplace(std::make_pair(object_id_, this));
  }

  static std::string getStats() {
    std::lock_guard<std::mutex> lock(counter_mu_);
    const size_t class_name_padding_length =
        obj_counter_longest_class_name.load(std::memory_order_relaxed)
        - class_name_.size();
    std::stringstream ss;
    ss << class_name_ << ": " << std::string(class_name_padding_length, '.')
       << " created " << objects_created
       << ", alive :" << objects_alive_.size();

    ss << std::endl << "Living objects' backtraces:" << std::endl;
    for (const auto &id_and_ptr : objects_alive_) {
      ss << id_and_ptr.first << ":" << std::endl << id_and_ptr.second->getBt();
    }
    ss << std::endl << "End of living objects' backtraces." << std::endl;
    return ss.str();
  }

 protected:
  ~ObjCounter()  // objects should never be removed through pointers of this type
  {
    std::lock_guard<std::mutex> lock(counter_mu_);
    objects_alive_.erase(object_id_);
  }

 private:
  static std::string class_name_;

  static std::mutex counter_mu_;
  static std::map<size_t, ConstrBt *> objects_alive_;
  static size_t objects_created;
  size_t object_id_;
};

template <typename T>
std::atomic<size_t> ObjCounter<T>::objects_created(0);
template <typename T>
std::atomic<size_t> ObjCounter<T>::objects_alive(0);
template <typename T>
std::mutex ObjCounter<T>::get_stats_mu_;
template <typename T>
std::string ObjCounter<T>::class_name_([] {
  std::string class_name = boost::core::demangle(typeid(T).name());
  const size_t my_class_name_length = class_name.size();
  size_t cur_max_length = obj_counter_longest_class_name;
  while (not std::atomic_compare_exchange_weak(
      &obj_counter_longest_class_name,
      &cur_max_length,
      std::max(my_class_name_length, cur_max_length)))
    ;
  return class_name;
}());
template <typename T>
std::mutex ObjCounter<T>::counter_mu_;
template <typename T>
std::map<size_t, ConstrBt *> ObjCounter<T>::objects_alive_;
