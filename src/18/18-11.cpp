#include <cassert>
#include <cstddef>
#include <memory>
#include <random>
#include <limits>
#include <vector>
#include <iostream>

template <typename T>
struct Node {
    T key;
    std::vector<std::shared_ptr<Node>> forward;
    Node(T key, size_t level) : key {std::move(key)} {
        forward.resize(level + 1);
    }
};

std::mt19937 gen(std::random_device{}());

template <typename T>
size_t node_level(const std::vector<std::shared_ptr<Node<T>>>& floor) {
    size_t level = 1;
    if (floor.front() && floor.front()->key == std::numeric_limits<T>::max()) {
        return level;
    }
    for (const auto& node : floor) {
        if (node && node->key != std::numeric_limits<T>::max()) {
            level++;
        } else {
            break;
        }
    }
    return level;
}

template <typename T>
struct SkipList {
    size_t levels;
    float prob;
    size_t curr_level;
    std::shared_ptr<Node<T>> head;
    std::shared_ptr<Node<T>> NIL;

    SkipList(size_t levels, float prob) : levels {levels}, prob {prob} {
        curr_level = 0;
        head = std::make_shared<Node<T>>(std::numeric_limits<T>::min(), levels);
        NIL = std::make_shared<Node<T>>(std::numeric_limits<T>::max(), levels);
        for (auto& pred : head->forward) {
            pred = NIL;
        }
    }

    size_t random_level() {
        size_t level = 1;
        std::uniform_real_distribution<> dis(0.0, 1.0);
        while (dis(gen) < prob && level < levels) {
            level++;
        }
        return level;
    }

    std::shared_ptr<Node<T>> search(const T& key) {
        auto curr = head;
        size_t curr_max = node_level(head->forward);
        for (size_t i = curr_max; i-- > 0;) {
            while (curr->forward[i] && curr->forward[i]->key < key) {
                curr = curr->forward[i];
            }
        }
        curr = curr->forward.front();
        if (curr->key == key) {
            return curr;
        } else {
            return nullptr;
        }
    }

    void insert(const T& key) {
        auto curr = search(key);
        if (curr) return;

        std::vector<std::shared_ptr<Node<T>>> update(head->forward);
        size_t curr_max = node_level(head->forward);
        curr = head;
        for (size_t i = curr_max; i-- > 0;) {
            while (curr->forward[i] && curr->forward[i]->key < key) {
                curr = curr->forward[i];
            }
            update[i] = curr;
        }
        curr = curr->forward.front();
        size_t new_level = 1;
        if (curr->key != key) {
            new_level = random_level();
            size_t current_level = node_level(update);

            if (new_level > current_level) {
                for (size_t i = current_level + 1; i < new_level; ++i) {
                    update[i] = head;
                }
            }
            curr = std::make_shared<Node<T>>(key, new_level);
        }
        for (size_t i = 0; i < new_level; ++i) {
            curr->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = curr;
        }
    }

    void erase(const T& key) {
        std::vector<std::shared_ptr<Node<T>>> update(head->forward);
        size_t curr_max = node_level(head->forward);
        auto curr = head;
        for (size_t i = curr_max; i-- > 0; ) {
            while (curr->forward[i] && curr->forward[i]->key < key) {
                curr = curr->forward[i];
            }
            update[i] = curr;
        }
        curr = curr->forward.front();
        if (curr->key == key) {
            for (size_t i = 0; i < update.size(); ++i) {
                if (update[i]->forward[i] != curr) {
                    break;
                }
                update[i]->forward[i] = curr->forward[i];
            }
            curr = nullptr;
        }
    }
};

template <typename T>
std::ostream& operator<< (std::ostream& ostr, const SkipList<T>& skipList) {
    auto head = skipList.head;
    ostr << "{";
    while (head->forward.front()) {
        ostr << "Key : " << head->forward.front()->key
        << ", Level : " << node_level(head->forward);
        head = head->forward.front();
        if (head->forward.front()) {
            ostr << "\n";
        }
    }
    ostr << "}\n";
    return ostr;
}

int main() {
    SkipList<int> skipList(16, 0.5);

    for (int i = 0; i < 10; i++) {
        skipList.insert(i);
    }
    std::cout << skipList;

    auto five = skipList.search(5);
    assert(five);

    skipList.insert(8);
    std::cout << skipList;

    skipList.erase(8);
    auto eight = skipList.search(8);
    assert(!eight);
    std::cout << skipList;
}