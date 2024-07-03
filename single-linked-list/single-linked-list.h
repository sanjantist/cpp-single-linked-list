#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <utility>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type &val, Node *next) : value(val), next_node(next) {}
        Type value;
        Node *next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node *node) : node_(node) {}

       public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType *;
        using reference = ValueType &;

        BasicIterator() = default;
        BasicIterator(const BasicIterator<Type> &other) noexcept
            : node_(other.node_) {}

        BasicIterator &operator=(const BasicIterator &rhs) = default;
        [[nodiscard]] bool operator==(
            const BasicIterator<const Type> &rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator==(
            const BasicIterator<Type> &rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator!=(
            const BasicIterator<Type> &rhs) const noexcept {
            return !(*this == rhs);
        }
        [[nodiscard]] bool operator!=(
            const BasicIterator<const Type> &rhs) const noexcept {
            return !(*this == rhs);
        }
        BasicIterator &operator++() noexcept {
            if (node_) {
                node_ = node_->next_node;
            }
            return *this;
        }
        BasicIterator operator++(int) noexcept {
            BasicIterator temp(*this);
            ++(*this);
            return temp;
        }
        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }
        [[nodiscard]] pointer operator->() const noexcept {
            return &(node_->value);
        }

       private:
        Node *node_ = nullptr;
    };

   public:
    using value_type = Type;
    using reference = value_type &;
    using const_reference = const value_type &;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }
    [[nodiscard]] Iterator end() noexcept { return Iterator(nullptr); }
    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }
    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] Iterator before_begin() noexcept { return Iterator(&head_); }
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node *>(&head_));
    }
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node *>(&head_));
    }

    Iterator InsertAfter(ConstIterator pos, const Type &value) {
        Node *new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator(new_node);
    }

    void PopFront() noexcept {
        if (head_.next_node != nullptr) {
            Node *temp_node = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete temp_node;
            --size_;
        }
    }
    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (pos.node_ && pos.node_->next_node) {
            Node *temp_node = pos.node_->next_node;
            pos.node_->next_node = pos.node_->next_node->next_node;
            delete temp_node;
            --size_;
            return Iterator(pos.node_->next_node);
        }
        return end();
    }

    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        assert(size_ == 0 && head_.next_node == nullptr);

        InitFromRange(values.begin(), values.end());
    }
    SingleLinkedList(const SingleLinkedList &other) {
        assert(size_ == 0 && head_.next_node == nullptr);

        InitFromRange(other.begin(), other.end());
    }

    SingleLinkedList &operator=(const SingleLinkedList &rhs) {
        if (this != &rhs) {
            SingleLinkedList temp(rhs);
            swap(temp);
        }
        return *this;
    }
    void swap(SingleLinkedList &other) {
        if (&other == this) {
            return;
        }
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    ~SingleLinkedList() { Clear(); }

    [[nodiscard]] size_t GetSize() const noexcept { return size_; }

    [[nodiscard]] bool IsEmpty() const noexcept { return size_ == 0; }

    void PushFront(const Type &value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node *prev_first_node = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete prev_first_node;
        }
        size_ = 0;
    }

   private:
    Node head_ = *new Node;
    size_t size_ = 0;

    template <typename Iter>
    void InitFromRange(Iter begin, Iter end) {
        Node **tail = &head_.next_node;
        size_t count = 0;
        while (begin != end) {
            *tail = new Node(*begin, nullptr);
            tail = &(*tail)->next_node;
            ++begin;
            ++count;
        }
        size_ = count;
    }
};

template <typename Type>
void swap(SingleLinkedList<Type> &lhs, SingleLinkedList<Type> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type> &lhs,
                const SingleLinkedList<Type> &rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type> &lhs,
                const SingleLinkedList<Type> &rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type> &lhs,
               const SingleLinkedList<Type> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                        rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type> &lhs,
                const SingleLinkedList<Type> &rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type> &lhs,
               const SingleLinkedList<Type> &rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type> &lhs,
                const SingleLinkedList<Type> &rhs) {
    return !(lhs < rhs);
}