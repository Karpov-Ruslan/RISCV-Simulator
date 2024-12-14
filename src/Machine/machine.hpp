#pragma once

#include <type_traits>
#include <fstream>
#include <bitset>
#include <iostream>
#include <string_view>
#include <cstdint>
#include <unordered_map>
#include <map>
#include <list>
#include <functional>

#include <defines.hpp>

namespace RISCVS {

#define MACHINE_ATTR template<typename T>

class Machine {
public:
    using MemoryRefType = int32_t;

    Machine();

    ~Machine();

    MACHINE_ATTR void Store(const MemoryRefType virtualMemoryAddress, const T& data) {
        const auto pageVirtualAddress = TranslateMemoryAddrIntoPageAddr(virtualMemoryAddress);

        // The entire date lies in one memory page.
        const MemoryRefType pageOffset = virtualMemoryAddress - pageVirtualAddress;
        auto pagePtr = pageCache.AcquireValue(pageVirtualAddress);
        if (pagePtr != nullptr) {
            // Page cache hit
            T* dst = reinterpret_cast<T*>(&(pagePtr->bytes[pageOffset]));
            *dst = data;
            return;
        }

        // No page cache hit
        PageType page = {};
        auto physicalPageAddressPtr = tlbCache.AcquireValue(pageVirtualAddress);
        if (physicalPageAddressPtr != nullptr) {
            // TLB cache hit
            page = PhysicalLoad<PageType>(*physicalPageAddressPtr);
        } else {
            // No TLB cache hit
            auto physicalPageAddress = PageTableSearch(pageVirtualAddress);
            tlbCache.InsertValue(pageVirtualAddress, physicalPageAddress);
            page = PhysicalLoad<PageType>(physicalPageAddress);
        }

        T* dst = reinterpret_cast<T*>(&(pageCache.InsertValue(pageVirtualAddress, page, [this](const VirtualPageRefType& pageVirtualAddr, const PageType& page) {
            const MemoryRefType physicalPageAddr = PageTableSearch(pageVirtualAddr);
            PhysicalStore<PageType>(physicalPageAddr, page);
        })->bytes[pageOffset]));
        *dst = data;
    }

    MACHINE_ATTR T Load(const MemoryRefType virtualMemoryAddress) {
        const auto pageVirtualAddress = TranslateMemoryAddrIntoPageAddr(virtualMemoryAddress);

        // The entire date lies in one memory page.
        const MemoryRefType pageOffset = virtualMemoryAddress - pageVirtualAddress;
        auto pagePtr = pageCache.AcquireValue(pageVirtualAddress);
        if (pagePtr != nullptr) {
            // Page cache hit
            T* dst = reinterpret_cast<T*>(&(pagePtr->bytes[pageOffset]));
            return *dst;
        }

        // No page cache hit
        PageType page = {};
        auto physicalPageAddressPtr = tlbCache.AcquireValue(pageVirtualAddress);
        if (physicalPageAddressPtr != nullptr) {
            // TLB cache hit
            page = PhysicalLoad<PageType>(*physicalPageAddressPtr);
        } else {
            // No TLB cache hit
            auto physicalPageAddress = PageTableSearch(pageVirtualAddress);
            tlbCache.InsertValue(pageVirtualAddress, physicalPageAddress);
            page = PhysicalLoad<PageType>(physicalPageAddress);
        }
        pageCache.InsertValue(pageVirtualAddress, page, [this](const VirtualPageRefType& pageVirtualAddr, const PageType& page) {
            const MemoryRefType physicalPageAddr = PageTableSearch(pageVirtualAddr);
            PhysicalStore<PageType>(physicalPageAddr, page);
        });
        T* dst = reinterpret_cast<T*>(&(page.bytes[pageOffset]));
        return *dst;
    }

private:
    constexpr static uint32_t SYSTEM_MEMORY_ADDR = 0x40000000; // 256MB
    constexpr static uint32_t SYSTEM_MEMORY_LENGTH = 0x4000000; // 64MB (At this moment - unused)
    // PAGE_BASE in bytes. Used for PAGE_SIZE and page memory operations
    constexpr static uint32_t PAGE_BASE = 12; // Linux page base
    // PAGE_SIZE in bytes. Must be more than 8
    constexpr static uint32_t PAGE_SIZE = 1 << PAGE_BASE; // Linux page size
    using VirtualPageRefType = MemoryRefType;
    using PhysicalPageRefType = MemoryRefType;

    struct PageType {
        uint8_t bytes[PAGE_SIZE] = {};
    };

    // For page caches and TLB
    template<typename Key, typename T, uint32_t MaxSize>
    class LruCache {
        struct CacheListElement {
            T value;
            Key key;
        };

        using ListCache = std::list<CacheListElement>;
        ListCache cacheList;
        using ListIt = typename ListCache::iterator;
        std::unordered_map<Key, ListIt> cacheMap;

    public:
        T* AcquireValue(const Key& key) {
            if (auto it = cacheMap.find(key); it != cacheMap.end()) {
                cacheList.splice(cacheList.begin(), cacheList, it->second);
                return &(it->second->value);
            }

            return nullptr;
        }

        T* InsertValue(const Key& key, const T& value, const std::function<void(const Key&, const T&)>& pullLastElement = [](const Key&, const T&){}) {
            if (auto it = cacheMap.find(key); it != cacheMap.end()) {
                // Cache hit situation
                cacheList.splice(cacheList.begin(), cacheList, it->second);
                cacheList.begin()->value = value;
            } else {
                // There are no cache hit situation
                cacheList.push_front(CacheListElement{value, key});
                cacheMap[key] = cacheList.begin();

                if (cacheList.size() > MaxSize) {
                    // Cache overflow, so must delete element
                    auto lastElem = std::prev(cacheList.end());
                    auto endKey = lastElem->key;
                    cacheMap.erase(endKey);

                    // Valid only for page cache
                    pullLastElement(lastElem->key, lastElem->value);

                    cacheList.pop_back();
                }
            }

            return &(cacheMap.find(key)->second->value);
        }
    };

    template <typename T, bool allowSystemMemoryStore = true> void PhysicalStore(const MemoryRefType physicalMemoryAddress, const T& data) {
        // std::cerr << "MemoryRef: " << static_cast<uint32_t>(physicalMemoryAddress) << '\n';
        ram.seekp(static_cast<uint32_t>(physicalMemoryAddress));

        if (ram.fail()) {
            throw std::runtime_error("seekp failed\n");
        }

        using bitset = std::bitset<8*sizeof(T)>;
        ram.write(reinterpret_cast<const char*>(&data), sizeof(T));

        if constexpr (allowSystemMemoryStore) {
            if (physicalMemoryAddress >= SYSTEM_MEMORY_ADDR && physicalMemoryAddress < SYSTEM_MEMORY_ADDR + SYSTEM_MEMORY_LENGTH) {
                throw std::runtime_error("Sytem memory store is unacceptable");
            }
        }

        if (ram.fail()) {
            throw std::runtime_error("write failed\n");
        }
    }

    MACHINE_ATTR T PhysicalLoad(const MemoryRefType physicalMemoryAddress) {
        ram.seekg(static_cast<uint32_t>(physicalMemoryAddress));

        if (ram.fail()) {
            throw std::runtime_error("seekp failed\n");
        }

        T ret;
        ram.read(reinterpret_cast<char*>(&ret), sizeof(T));

        if (ram.fail()) {
            if (ram.bad()) {
                std::cerr << ram.rdstate() << std::endl;
                std::cerr << ram.gcount() << std::endl;
                throw std::runtime_error("read failed\n");
            } else {
                ram.clear();
            }
        }

        return ret;
    }

    // Helped functions
    static MemoryRefType TranslateMemoryAddrIntoPageAddr(MemoryRefType memoryAddr);

    PhysicalPageRefType PageTableSearch(VirtualPageRefType pageVirtualAddr);

    // Page Table structure:
    // Virtual Page Addr <-> Physical Page Addr
    void InitSystemMemory();

    const char* RAM_PATH = "../ram/ram.bin";

    //////////////////////////////////////////////////// MMU Module ////////////////////////////////////////////////////
    LruCache<VirtualPageRefType, PageType, 256> pageCache;
    LruCache<VirtualPageRefType, PhysicalPageRefType, 1024> tlbCache;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // RAM (it is file in fact)
    std::fstream ram;
};

}
