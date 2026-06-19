// cache.cpp - logica central do simulador: decomposicao de endereco,
//             busca de hit/miss e politicas de substituicao (LRU/FIFO).
// Autores: Ismael Antonio da Silva Junior, Guilherme Thommy, Eduardo Leopoldo

#include "cache.h"
#include "display.h"

Cache::Cache(const Config& cfg) : cfg_(cfg) {
    sets_.resize(cfg.num_sets, std::vector<CacheLine>(cfg.assoc));
}

// Quebra o endereço nos três campos: [ TAG | INDEX | OFFSET ]
// Os bits menos significativos são o offset (posição dentro do bloco),
// depois vem o index (qual conjunto da cache) e o resto é a tag.
void Cache::decompose(uint64_t addr, uint64_t& tag, int& index, int& offset) const {
    uint64_t offset_mask = (1ULL << cfg_.offset_bits) - 1;
    uint64_t index_mask  = (1ULL << cfg_.index_bits)  - 1;

    offset = static_cast<int>(addr & offset_mask);
    index  = static_cast<int>((addr >> cfg_.offset_bits) & index_mask);
    tag    = addr >> (cfg_.offset_bits + cfg_.index_bits);
}

// Escolhe qual via será substituída.
// Primeiro aproveita vias vazias (cache não está cheia ainda).
// Se todas ocupadas, expulsa a de menor counter — mas o significado do counter
// depende da política: no FIFO ele marca quando o bloco ENTROU (nunca muda),
// no LRU ele marca quando o bloco foi USADO POR ÚLTIMO (atualizado a cada hit).
// Ou seja, a lógica aqui é idêntica para ambas; o que muda é como o counter
// foi escrito antes de chegar aqui.
int Cache::find_victim(const std::vector<CacheLine>& set) {
    for (int i = 0; i < static_cast<int>(set.size()); i++) {
        if (!set[i].valid) return i;
    }

    int victim = 0;
    for (int i = 1; i < static_cast<int>(set.size()); i++) {
        if (set[i].counter < set[victim].counter) victim = i;
    }
    return victim;
}

// Processa um único acesso à cache.
// Retorna true se foi hit, false se foi miss.
bool Cache::access(uint64_t addr) {
    uint64_t tag;
    int index, offset;
    decompose(addr, tag, index, offset);

    auto& set = sets_[index];
    access_count_++;

    // Verifica se o bloco já está na cache (hit)
    for (auto& line : set) {
        if (line.valid && line.tag == tag) {
            // LRU renova o counter a cada acesso; FIFO não faz nada aqui,
            // mantendo o counter original de quando o bloco entrou
            if (cfg_.policy == ReplacementPolicy::LRU) {
                line.counter = global_time_++;
            }
            if (cfg_.verbose) {
                print_access(access_count_, addr, tag, index, offset,
                             true, 0, false, 0);
                print_set_state(index, set);
            }
            return true;  // HIT
        }
    }

    // Miss: escolhe a vítima e carrega o novo bloco
    int      victim_way  = find_victim(set);
    bool     was_removal = set[victim_way].valid;
    uint64_t removed_tag = set[victim_way].tag;

    set[victim_way].valid   = true;
    set[victim_way].tag     = tag;
    set[victim_way].counter = global_time_++;  // registra o momento de entrada

    if (cfg_.verbose) {
        print_access(access_count_, addr, tag, index, offset,
                     false, victim_way, was_removal, removed_tag);
        print_set_state(index, set);
    }
    return false;  // MISS
}

// Percorre todos os endereços e conta hits e misses
SimResult Cache::simulate(const std::vector<uint64_t>& addresses) {
    SimResult result;
    result.total_accesses = static_cast<int>(addresses.size());

    for (uint64_t addr : addresses) {
        if (access(addr)) result.hits++;
        else              result.misses++;
    }

    return result;
}
