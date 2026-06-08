// Autor(es): [coloque seu nome aqui]
// Descricao: Logica central do simulador de cache

#include "cache.h"
#include "display.h"

// -------------------------------------------------------------------------
// Construtor
// -------------------------------------------------------------------------

Cache::Cache(const Config& cfg) : cfg_(cfg) {
    // Cria num_sets conjuntos, cada um com 'assoc' linhas (vias) inicialmente vazias
    sets_.resize(cfg.num_sets, std::vector<CacheLine>(cfg.assoc));
}

// -------------------------------------------------------------------------
// Decomposicao do endereco
// -------------------------------------------------------------------------

void Cache::decompose(uint64_t addr, uint64_t& tag, int& index, int& offset) const {
    // O endereco fisico e dividido em tres campos (da direita para a esquerda):
    //
    //   [ TAG | INDEX | OFFSET ]
    //   |<-- tag_bits -->|<-- index_bits -->|<-- offset_bits -->|
    //
    // OFFSET (bits menos significativos):
    //   Identifica o byte dentro do bloco de cache.
    //   Tamanho: log2(block_size) bits — ex: bloco de 16 bytes precisa de 4 bits (0..15).
    //   Extracao: mascara os 'offset_bits' bits menos significativos.
    //   Mascara:  (1 << offset_bits) - 1  ->  ex: 4 bits = 0b00001111 = 0x0F
    //
    // INDEX:
    //   Identifica qual conjunto da cache sera verificado.
    //   Tamanho: log2(num_sets) bits.
    //   Extracao: desloca o endereco 'offset_bits' posicoes e aplica mascara de index.
    //
    // TAG (bits mais significativos):
    //   Distingue blocos diferentes que mapeiam no mesmo conjunto.
    //   Extracao: desloca o endereco (offset_bits + index_bits) posicoes para a direita.
    //   Nao precisa de mascara — os bits sobrantes sao a propria tag.

    uint64_t offset_mask = (1ULL << cfg_.offset_bits) - 1;
    uint64_t index_mask  = (1ULL << cfg_.index_bits)  - 1;

    offset = static_cast<int>(addr & offset_mask);
    index  = static_cast<int>((addr >> cfg_.offset_bits) & index_mask);
    tag    = addr >> (cfg_.offset_bits + cfg_.index_bits);
}

// -------------------------------------------------------------------------
// Escolha da vitima
// -------------------------------------------------------------------------

int Cache::find_victim(const std::vector<CacheLine>& set) const {
    // Estrategia de escolha em duas etapas:
    //
    // 1. Linha invalida (vazia): carregamos o bloco sem descartar nada.
    //    Isso acontece quando a cache ainda nao esta cheia.
    for (int i = 0; i < static_cast<int>(set.size()); i++) {
        if (!set[i].valid) return i;
    }

    // 2. Todas as linhas validas: escolhe a de menor counter.
    //    - LRU:  counter = instante do ultimo ACESSO  -> menor = usado ha mais tempo
    //    - FIFO: counter = instante do CARREGAMENTO   -> menor = carregado ha mais tempo
    //    A mesma logica de "menor counter = vitima" serve para os dois casos.
    int victim = 0;
    for (int i = 1; i < static_cast<int>(set.size()); i++) {
        if (set[i].counter < set[victim].counter) victim = i;
    }
    return victim;
}

// -------------------------------------------------------------------------
// Processamento de um acesso de memoria
// -------------------------------------------------------------------------

bool Cache::access(uint64_t addr) {
    uint64_t tag;
    int index, offset;
    decompose(addr, tag, index, offset);

    auto& set = sets_[index];
    access_count_++;

    // --- Verifica se o bloco ja esta na cache (HIT) ---
    for (auto& line : set) {
        if (line.valid && line.tag == tag) {

            // Diferenca fundamental entre LRU e FIFO no hit:
            //   LRU:  atualiza o counter — registra "acabei de usar esta linha agora"
            //   FIFO: NAO atualiza — o counter fica fixo desde o carregamento original
            // Isso determina qual linha sera descartada no proximo miss neste conjunto.
            if (cfg_.policy == ReplacementPolicy::LRU) {
                line.counter = global_time_++;
            }

            if (cfg_.verbose) {
                print_access(access_count_, addr, tag, index, offset,
                             /*hit=*/true, /*victim_way=*/0, /*was_eviction=*/false, 0);
                print_set_state(index, set);
            }
            return true; // HIT
        }
    }

    // --- Bloco nao encontrado: MISS — escolhe vitima e carrega o novo bloco ---
    int  victim_way  = find_victim(set);
    bool was_eviction = set[victim_way].valid;        // havia um bloco valido?
    uint64_t evicted_tag = set[victim_way].tag;       // tag do bloco descartado

    set[victim_way].valid   = true;
    set[victim_way].tag     = tag;
    // Tanto LRU quanto FIFO registram o instante de CARGA no counter ao carregar o bloco.
    // No proximo hit sobre esta linha:
    //   LRU vai sobrescrever este counter com um valor mais recente.
    //   FIFO vai manter este counter intacto, garantindo ordem de chegada.
    set[victim_way].counter = global_time_++;

    if (cfg_.verbose) {
        print_access(access_count_, addr, tag, index, offset,
                     /*hit=*/false, victim_way, was_eviction, evicted_tag);
        print_set_state(index, set);
    }
    return false; // MISS
}

// -------------------------------------------------------------------------
// Simulacao da sequencia completa de acessos
// -------------------------------------------------------------------------

SimResult Cache::simulate(const std::vector<uint64_t>& addresses) {
    SimResult result;
    result.total_accesses = static_cast<int>(addresses.size());

    for (uint64_t addr : addresses) {
        if (access(addr)) {
            result.hits++;
        } else {
            result.misses++;
        }
    }

    return result;
}
