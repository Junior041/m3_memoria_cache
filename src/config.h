#pragma once

#include <string>

enum class ReplacementPolicy {
    LRU,   // descarta o bloco menos recentemente usado
    FIFO   // descarta o bloco mais antigo na ordem de chegada
};

struct Config {
    // parâmetros fornecidos pelo usuário
    int  cache_size = 0;
    int  block_size = 0;
    int  assoc      = 1;
    int  addr_bits  = 16;

    std::string       input_file;
    bool              verbose = false;
    ReplacementPolicy policy  = ReplacementPolicy::LRU;

    // campos calculados a partir dos parâmetros acima
    int num_sets    = 0;   // cache_size / (block_size * assoc)
    int offset_bits = 0;   // log2(block_size)
    int index_bits  = 0;   // log2(num_sets)
    int tag_bits    = 0;   // addr_bits - offset_bits - index_bits
};
