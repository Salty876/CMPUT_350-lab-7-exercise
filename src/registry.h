#ifndef REGISTRY_H
#define REGISTRY_H

#include <vector>
template <typename T>
class Registry {
public:

    Registry() : data(std::vector<T> {}), recycledKeys(std::vector<size_t> {}), validSlots(std::vector<bool> {}), active(static_cast<size_t>(0)) {};

    // copy store
    size_t store(const T& value) {
        if (!recycledKeys.empty()) {
            size_t ID = recycledKeys.back();
            recycledKeys.pop_back();

            data.at(ID) = value;
            validSlots.at(ID) = true;
            size_t active = active + static_cast<size_t>(1);
        }
        else{
            size_t ID = active;
            data.push_back(value);
            validSlots.push_back(true);
            size_t active = active + static_cast<size_t>(1);
        }
        
    }

    // move store
    size_t store(const T&& value) {
        if (!recycledKeys.empty()) {
            size_t ID = recycledKeys.back();
            recycledKeys.pop_back();

            data.at(ID) = std::move(value);
            validSlots.at(ID) = true;
            size_t active = active + static_cast<size_t>(1);
        }
        else{
            size_t ID = active;
            data.push_back(std::move(value));
            validSlots.push_back(true);
            size_t active = active + static_cast<size_t>(1);
        }
    }

    void remove(size_t ID) {
        T val = data.at(ID);
        data.at(ID) = nullptr;
        delete val;
        val = nullptr;
        validSlots.at(ID) = false;
        recycledKeys.push_back(ID);
        size_t active = active - static_cast<size_t>(1);
    }

    T& operator[](size_t ID) {
        return data.at(ID);
    }

    const T& operator[](size_t ID) const {
        return data.at(ID);
    }

    bool valid(size_t ID) {
        return validSlots.at(ID);
    }

    size_t size() {
        return active;
    }

private:
    std::vector<T> data;
    std::vector<size_t> recycledKeys;
    std::vector<bool> validSlots;
    size_t active;

};

#endif  // REGISTRY_H
