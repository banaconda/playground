#pragma once

template <typename TKey, typename TValue> //
class UnorderedMap
{
    std::list<std::pair<TKey, TValue>> map;

  public:
    void
    insert(TKey key, TValue value)
    {
        std::pair<TKey, TValue> *p = get(key);
        if (p == nullptr) {
            map.push_back(std::make_pair(key, value));
        } else {
            p->second = value;
        }
    }

    void
    remove(TKey key)
    {
        for (auto &e : map) {
            if (key == e.first) {
                map.erase(e);
                return;
            }
        }
    }

    std::pair<TKey, TValue> *
    get(TKey key)
    {
        for (auto &e : map) {
            if (key == e.first) {
                return &e;
            }
        }
        return nullptr;
    }

    TValue *
    getValue(TKey key)
    {
        std::pair<TKey, TValue> *p = get(key);
        if (p != nullptr) {
            return &p->second;
        } else {
            return nullptr;
        }
    }
};