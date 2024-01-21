//
// Created by creeper on 10/26/23.
//

#ifndef RENDERCRAFT_CORE_INCLUDE_CORE_MEMORY_H_
#define RENDERCRAFT_CORE_INCLUDE_CORE_MEMORY_H_
#include <Core/core.h>
#include <list>
#include <vector>
#include <memory>

namespace rdcraft {
template <typename Base>
class PolymorphicVector : NonCopyable {
  public:
    // a copy constructor and a move constructor
    PolymorphicVector() = default;
    PolymorphicVector(PolymorphicVector&& other) noexcept {
      m_objects = std::move(other.m_objects);
    }
    PolymorphicVector& operator=(PolymorphicVector&& rhs) noexcept {
      m_objects = std::move(rhs.m_objects);
      return *this;
    }
    template <typename Derived>
    Base* allocate() {
      m_objects.push_back(std::make_unique<Derived>());
      return m_objects.back().get();
    }
    template <typename Derived>
    Base* allocate(const Derived& derived) {
      m_objects.push_back(std::make_unique<Derived>(derived));
      return m_objects.back().get();
    }
    template <typename Derived, typename... Ts>
    Base* construct(Ts&&... args) {
      m_objects.push_back(std::make_unique<Derived>(std::forward<Ts>(args)...));
      return m_objects.back().get();
    }
    template <typename Derived, typename... Ts>
    Derived* constructDerived(Ts&&... args) {
      auto ptr = std::make_unique<Derived>(std::forward<Ts>(args)...);
      m_objects.push_back(std::move(ptr));
      return ptr.get();
    }
    const Base* operator()(int i) const {
      return m_objects[i].get();
    }
    Base* operator()(int i) {
      return m_objects[i].get();
    }
    std::vector<std::unique_ptr<Base>>& objects() {
      return m_objects;
    }
    const std::vector<std::unique_ptr<Base>>& objects() const {
      return m_objects;
    }
    bool empty() const { return m_objects.empty(); }
    int size() const { return m_objects.size(); }

  private:
    std::vector<std::unique_ptr<Base>> m_objects;
};

constexpr int kDefaultBlockSize = 64;

// use a list of blocks to manage objects of fixed size
template <typename T, int BlockSize = kDefaultBlockSize>
class MemoryPool : NonCopyable {
  public:
    MemoryPool() = default;
    MemoryPool(MemoryPool&& other) noexcept {
      blocks = std::move(other.blocks);
      other.blocks = nullptr;
      other.ptr = 0;
    }
    MemoryPool& operator=(MemoryPool&& rhs) noexcept {
      blocks = std::move(rhs.blocks);
      return *this;
    }
    T* allocate() {
      if (blocks.empty() || ptr == BlockSize) {
        blocks.emplace_back();
      }

      T* ret = blocks.back().data() + ptr;
      ptr = ptr == BlockSize ? 0 : ptr + 1;
      return ret;
    }
    template <typename... Ts>
    T* construct(Ts... args) {
      if (blocks.empty() || ptr == BlockSize) {
        std::array<T, BlockSize> new_block = {T(args...)};
        blocks.push_back(std::move(new_block));
        ptr = 1;
        return blocks.back().data();
      }
      T* ret = blocks.back().data() + (ptr++);
      *ret = T(args...);
      return ret;
    }
    void clear() {
      blocks.clear();
    }
    bool empty() const {
      return blocks.empty();
    }
    int size() const {
      return blocks.size() * BlockSize;
    }

  private:
    std::list<std::array<T, BlockSize>> blocks;
    int ptr = 0; // used size of the tail block
};
}
#endif //RENDERCRAFT_CORE_INCLUDE_CORE_MEMORY_H_