#pragma once

#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <concepts>

// This object pool is meant for use in multi-threaded environment where Object type is not a thread safe
// Many threads can share ObjectPool and have a unique ownership of an instance of Object
// with getObject call, which ensures that calling thread is the only owner of returned instance
template<typename Object>
class ObjectPool {
public:
    template<typename ... Args>
    explicit ObjectPool(std::size_t size, Args&& ... args) {
        if (size == 0) {
            throw std::runtime_error{"Object pool has to contain at least one object."};
        }
        objects.reserve(size);
        for(std::size_t i = 0; i < size; ++i) {
            objects.push_back(std::make_shared<Object>(args...));
        }
    }

    explicit ObjectPool(std::vector<std::shared_ptr<Object>> objects): objects(std::move(objects)) {
        if (this->objects.empty()) {
            throw std::runtime_error{"Object pool has to contain at least one object."};
        }
    }

    explicit ObjectPool(std::vector<std::unique_ptr<Object>> objects): objects(std::move(objects)) {
        if (this->objects.empty()) {
            throw std::runtime_error{"Object pool has to contain at least one object."};
        }
    }

    std::shared_ptr<Object> getObject() {
        auto size = objects.size();

        while (true) {
            for (std::size_t i = last_given_object_index.load(); i < size; ++i) {
                auto object = objects[i];
                if (object.use_count() == 2) { // 1 use count for "object" variable, and 1 for vector, if equal to 2, then only ObjectPool owns the object
                    last_given_object_index.store((i + 1) % size);
                    return object;
                }
            }
            last_given_object_index.store(0);
            std::this_thread::yield();
        }
    }

    template<typename Callable>
    void invokeOnAllThreadSafe(Callable&& callable) {
        auto claimed_objects = claimAll();
        for(auto& object: claimed_objects) {
            callable(object);
        }
    }

private:
    std::vector<std::shared_ptr<Object>> claimAll() {
        // ensures that only one thread can claim all connections at once
        std::lock_guard lock {claim_all_objects_mutex};
        std::vector<std::shared_ptr<Object>> claimed_objects {};

        claimed_objects.reserve(objects.size());

        for (std::size_t i = 0; i < objects.size(); ++i) {
            claimed_objects.push_back(getObject());
        }
        return claimed_objects;
    }

    std::mutex claim_all_objects_mutex;
    std::atomic_size_t last_given_object_index{0};
    std::vector<std::shared_ptr<Object>> objects;
};