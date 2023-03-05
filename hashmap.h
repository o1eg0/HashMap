#include <bits/stdc++.h>

// Можете взять посылку 81543350 за основу, если необходимо лучшее быстродействие
template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
private:
    using KeyValue = std::pair<KeyType, ValueType>;
    using ConstKeyValue = std::pair<const KeyType, ValueType>;

    struct data {
        ConstKeyValue info;
        size_t index{}, hash_index{};
        size_t dist{};

        data(KeyValue info, size_t index, size_t hash_index, size_t dist) : info(info), index(index),
                                                                            hash_index(hash_index), dist(dist) {}
    };

    size_t n = 0;
    size_t capacity = 2;
    size_t first_index = capacity;

    std::vector<data *> arr = {nullptr, nullptr};

    Hash hash;

    void rebuild() {
        std::vector<ConstKeyValue> copy;
        for (auto it = begin(); it != end(); ++it)
            copy.push_back({it->first, it->second});

        size_t new_cap = capacity * 2;
        clear();

        capacity = new_cap;
        first_index = capacity;
        arr.resize(capacity, nullptr);

        for (size_t i = 0; i < copy.size(); ++i)
            insert(copy[i]);
    }

    size_t find_index(KeyType key) const {
        size_t h = hash(key) % capacity;

        for (size_t i = h;; ++i) {
            if (i >= capacity) i -= capacity;
            if (arr[i] == nullptr)
                return capacity;
            if (arr[i]->hash_index == h and arr[i]->info.first == key)
                return i;
        }
    }

public:
    //---------- begin constructors ----------

    explicit HashMap(Hash h = Hash()) : hash(h) {}

    HashMap(std::initializer_list<KeyValue> l, Hash h = Hash()) : hash(h) {
        for (auto &x: l)
            insert(x);
    }

    HashMap(HashMap const &copy) {
        *this = copy;
    }

    template<class Iterator>
    HashMap(Iterator Begin, Iterator End, Hash h = Hash()) : hash(h) {
        for (auto &it = Begin; it != End; ++it)
            insert(*it);
    }

    ~HashMap() {
        for (size_t i = 0; i < arr.size(); ++i)
            if (arr[i]) {
                delete arr[i];
                arr[i] = nullptr;
            }
    }

    //---------- end constructors ----------


    //---------- begin const iterator ----------

    class const_iterator {
    private:
        size_t index{};
        const std::vector<data *> *orig;
    public:
        const_iterator() = default;

        const_iterator(size_t index, const std::vector<data *> *orig) : index(index), orig(orig) {}

        const_iterator(const const_iterator &it) : index(it.index), orig(it.orig) {}

        bool operator==(const const_iterator &it) {
            return index == it.index;
        }

        bool operator!=(const const_iterator &it) {
            return index != it.index;
        }

        const_iterator operator++() {
            if (index == orig->size()) return *this;
            ++index;
            while (index < orig->size() && (*orig)[index] == nullptr) ++index;
            return *this;
        }

        const_iterator operator++(int) {
            if (index == orig->size()) return *this;
            auto copy = *this;
            ++index;
            while (index < orig->size() && (*orig)[index] == nullptr) ++index;
            return copy;
        }

        const ConstKeyValue &operator*() {
            return (*orig)[index]->info;
        }

        const ConstKeyValue *operator->() {
            return &((*orig)[index]->info);
        }
    };


    const_iterator begin() const {
        return const_iterator(first_index, &arr);
    }

    const_iterator end() const {
        return const_iterator(capacity, &arr);
    }

    const_iterator find(KeyType key) const {
        return const_iterator(find_index(key), &arr);
    }

    //---------- end const iterator ----------

    //---------- begin iterator ----------

    class iterator {
    private:
        size_t index = 0;
        std::vector<data *> *orig = nullptr;
    public:
        iterator() = default;

        iterator(size_t index, std::vector<data *> *orig) : index(index), orig(orig) {}

        iterator(iterator &it) : index(it.index), orig(it.orig) {}

        bool operator==(const iterator &it) {
            return index == it.index && orig == it.orig;
        }

        bool operator!=(const iterator &it) {
            return index != it.index || orig != it.orig;
        }

        iterator operator++() {
            if (index == orig->size()) return *this;
            ++index;
            while (index < orig->size() && (*orig)[index] == nullptr) ++index;
            return *this;
        }

        iterator operator++(int) {
            if (index == orig->size()) return *this;
            auto copy = *this;
            ++index;
            while (index < orig->size() && (*orig)[index] == nullptr) ++index;
            return copy;
        }

        const ConstKeyValue &operator*() {
            return (*orig)[index]->info;
        }

        ConstKeyValue *operator->() {
            return &((*orig)[index])->info;
        }

        iterator(const iterator &iterator1) {
            index = iterator1.index;
            orig = iterator1.orig;
        }
    };

    iterator begin() {
        return iterator(first_index, &arr);
    }

    iterator end() {
        return iterator(capacity, &arr);
    }

    iterator find(KeyType key) {
        return iterator(find_index(key), &arr);
    }

    //---------- end iterator ----------

    //---------- begin const functions ----------

    size_t size() const {
        return n;
    }

    bool empty() const {
        return n == 0;
    }

    Hash hash_function() const {
        return hash;
    }

    const ValueType &at(const KeyType &key) const {
        size_t i = find_index(key);
        if (i == capacity)
            throw std::out_of_range("this element is not in the HashMap");
        return arr[i]->info.second;
    }

    //---------- end const functions ----------

    //---------- begin functions ----------


    void insert(KeyValue el) {
        while (n * 2 >= capacity) rebuild();

        size_t h = hash(el.first) % capacity;
        data *new_el = new data(el, 0, h, 0);
        for (new_el->index = h;; ++new_el->index) {
            if (new_el->index >= capacity) new_el->index -= capacity;

            if (arr[new_el->index] == nullptr) {
                arr[new_el->index] = new_el;

                first_index = std::min(first_index, new_el->index);
                ++n;
                return;
            }

            if (arr[new_el->index]->hash_index == h && arr[new_el->index]->info.first == el.first) {
                delete new_el;
                return;
            }

            if (arr[new_el->index]->dist < new_el->dist)
                std::swap(arr[new_el->index], new_el);
            ++new_el->dist;
        }
    }

    void erase(KeyType key) {
        size_t h = hash(key) % capacity;

        for (size_t i = h;; ++i) {
            if (i >= capacity) i -= capacity;

            if (arr[i] == nullptr)
                return;

            if (arr[i]->hash_index == h && arr[i]->info.first == key) {
                --n;
                delete arr[i];
                arr[i] = nullptr;

                size_t cnt = 1;
                for (size_t j = i + 1;; ++j) {
                    if (j >= capacity) j -= capacity;

                    if (arr[j] == nullptr)
                        break;

                    if (arr[j]->dist < cnt) {
                        ++cnt;
                        continue;
                    }

                    std::swap(arr[i], arr[j]);
                    arr[i]->index = i;
                    arr[i]->dist -= cnt;

                    i = j;
                    cnt = 1;
                }

                first_index = capacity;
                for (size_t k = 0; k < capacity; ++k) {
                    if (arr[k] != nullptr) {
                        first_index = k;
                        return;
                    }
                }
            }
        }
    }

    void clear() {
        n = 0;
        capacity = 2;
        first_index = capacity;

        for (size_t i = 0; i < arr.size(); ++i)
            if (arr[i]) {
                delete arr[i];
                arr[i] = nullptr;
            }
        arr = {nullptr, nullptr};
    }

    ValueType &operator[](KeyType key) {
        size_t index = find_index(key);
        if (index == capacity) {
            insert({key, ValueType()});
            return arr[find_index(key)]->info.second;
        } else
            return arr[index]->info.second;
    }

    HashMap &operator=(const HashMap &copy) {
        if (&copy == this) return *this;

        clear();

        n = copy.n;
        capacity = copy.capacity;
        first_index = copy.first_index;

        arr.resize(copy.capacity, nullptr);

        for (size_t i = 0; i < copy.arr.size(); ++i){
            auto ptr = copy.arr[i];
            arr[i] = ptr ? new data(ptr->info, ptr->index, ptr->hash_index, ptr->dist) : nullptr;
        }
        return *this;
    }

//    ---------- end functions ----------
};
