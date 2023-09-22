// Copyright 2023 JohnCorn
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once
#include <string>
#include <list>
#include <unordered_map>
#include <memory_resource>
#include <chrono>
#include <cassert>

template <typename TValue, typename TKey, std::size_t page_size = 4096>
struct LRUCache final
{
    /// @brief constructor
    /// @param max_size - max count of resources loaded
    explicit LRUCache(std::size_t max_size) : max_size(max_size) {}

    /// get resource by key
    TValue &get(const TKey &key)
    {
        KeyIter key_iter = keys.find(key);
        ResourceIter res_iter;
        if (key_iter != keys.end()) // is loaded
        {
            res_iter = key_iter->second;
            resources.splice(resources.begin(), resources, res_iter);
        }
        else // is must be loaded
        {
            if (resources.size() >= max_size)
            {
                auto& last = resources.back();
                keys.erase(last.key_iterator);
                resources.pop_back();
            }
            resources.emplace_front(key);
            res_iter = resources.begin();
            std::pair<KeyIter, bool> insertion = keys.insert({key, res_iter});
            assert(insertion.second == true); // check if insertion happend
            res_iter->key_iterator = insertion.first;
        }
        return *res_iter->value;
    }

    void put(const TKey& key, TValue& value)
    {
        
    }
private:
    // -------------------- Types ---------------
    struct Resource;
    using ResourcesQueue = std::pmr::list<Resource>;
    using ResourceIter = typename ResourcesQueue::iterator;
    using Keys = std::pmr::unordered_map<TKey, ResourceIter>;
    using KeyIter = typename Keys::iterator;
    struct Resource
    {
        float time_to_construct;        /// in milliseconds
        size_t size_in_bytes;           /// resource's size in bytes
        std::unique_ptr<TValue> value;  /// pointer on data
        KeyIter key_iterator;           /// iterator on key mapped in unordered_map

        /// constructor for resource
        explicit Resource(const TKey &key)
        {
            using namespace std::chrono;
            auto start = steady_clock::now();
            value = std::make_unique<TValue>(key, size_in_bytes);
            auto end = steady_clock::now();
            duration<float, std::milli> elapsed_ms = end - start;
            time_to_construct = elapsed_ms.count();
        }
    };

    //------------------------- Data ----------------------
    std::size_t max_size;   /// max count of resources loaded
    std::pmr::monotonic_buffer_resource pool { page_size }; /// pool for the list and the map
    ResourcesQueue resources{&pool};    /// list of resources. Last resource prepend to delete first
    Keys keys{&pool};   /// map with keys and iterators on resource in the list

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;
};