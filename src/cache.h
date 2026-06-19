// cache.h - tipos e interface publica da classe Cache.
// Autores: Ismael Antonio da Silva Junior, Guilherme Thommy, Eduardo Leopoldo
#pragma once

#include <vector>
#include <cstdint>
#include "config.h"

struct CacheLine {
    bool     valid   = false;
    uint64_t tag     = 0;
    int      counter = 0;  // LRU: atualizado a cada acesso; FIFO: definido só no carregamento
};

struct SimResult {
    int total_accesses = 0;
    int hits           = 0;
    int misses         = 0;
};

class Cache {
public:
    explicit Cache(const Config& cfg);

    SimResult simulate(const std::vector<uint64_t>& addresses);

private:
    Config cfg_;
    std::vector<std::vector<CacheLine>> sets_;  // sets_[conjunto][via]

    int global_time_  = 0;
    int access_count_ = 0;

    bool access(uint64_t addr);
    void decompose(uint64_t addr, uint64_t& tag, int& index, int& offset) const;
    static int  find_victim(const std::vector<CacheLine>& set) ;
};
