// Autor(es): [coloque seu nome aqui]
// Descricao: Implementacao das funcoes de output do simulador

#include "display.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

// -------------------------------------------------------------------------
// Converte o enum de politica para string legivel
// -------------------------------------------------------------------------
static std::string policy_name(ReplacementPolicy p) {
    return (p == ReplacementPolicy::LRU) ? "LRU" : "FIFO";
}

// -------------------------------------------------------------------------
// Configuracao
// -------------------------------------------------------------------------

void print_config(const Config& cfg) {
    std::string mapping;
    if (cfg.assoc == 1) {
        mapping = "Mapeamento Direto";
    } else {
        mapping = std::to_string(cfg.assoc) + "-way Set-Associative"
                  " | Politica: " + policy_name(cfg.policy);
    }

    std::cout << "\n=== Configuracao da Cache ===\n";
    std::cout << "  Tamanho total    : " << cfg.cache_size  << " bytes\n";
    std::cout << "  Tamanho do bloco : " << cfg.block_size  << " bytes\n";
    std::cout << "  Associatividade  : " << cfg.assoc << " (" << mapping << ")\n";
    std::cout << "  Conjuntos        : " << cfg.num_sets    << "\n";
    std::cout << "  Bits do endereco : " << cfg.addr_bits   << "\n";
    std::cout << "  Decomposicao     : "
              << "[TAG=" << cfg.tag_bits << " bits]"
              << " [INDEX=" << cfg.index_bits << " bits]"
              << " [OFFSET=" << cfg.offset_bits << " bits]\n\n";
}

// -------------------------------------------------------------------------
// Resultado
// -------------------------------------------------------------------------

void print_result(const SimResult& res) {
    double hit_pct  = res.total_accesses > 0
                      ? 100.0 * res.hits   / res.total_accesses : 0.0;
    double miss_pct = res.total_accesses > 0
                      ? 100.0 * res.misses / res.total_accesses : 0.0;

    std::cout << "\n=== Resultado da Simulacao ===\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Acessos totais : " << res.total_accesses << "\n";
    std::cout << "  Cache hits     : " << std::setw(4) << res.hits
              << "  (" << hit_pct  << "%)\n";
    std::cout << "  Cache misses   : " << std::setw(4) << res.misses
              << "  (" << miss_pct << "%)\n\n";
}

// -------------------------------------------------------------------------
// Uso
// -------------------------------------------------------------------------

void print_usage(const char* prog) {
    std::cerr << "Uso: " << prog
              << " --cache-size <bytes>"
              << " --block-size <bytes>"
              << " --assoc <N>"
              << " --addr-bits <N>"
              << " --input <arquivo>"
              << " [--policy LRU|FIFO]"
              << " [--verbose]\n\n"
              << "Exemplos:\n"
              << "  " << prog << " --cache-size 256 --block-size 16 --assoc 1"
                               << " --addr-bits 16 --input test1.txt --verbose\n"
              << "  " << prog << " --cache-size 1024 --block-size 32 --assoc 2"
                               << " --addr-bits 16 --input test2.txt --policy LRU\n";
}

// -------------------------------------------------------------------------
// Trace de acesso individual
// -------------------------------------------------------------------------

void print_access(int access_num,
                  uint64_t addr,
                  uint64_t tag, int index, int offset,
                  bool hit,
                  int  victim_way,
                  bool was_eviction,
                  uint64_t evicted_tag)
{
    // Linha principal: numero, endereco, campos decompostos e resultado
    std::cout << "[#" << std::setw(3) << std::setfill(' ') << access_num << "]"
              << "  Addr=0x" << std::hex << std::setw(4) << std::setfill('0') << addr
              << std::dec
              << "  tag=" << std::setw(3) << tag
              << "  index=" << std::setw(2) << index
              << "  offset=" << std::setw(2) << offset
              << "  ";

    if (hit) {
        std::cout << "HIT\n";
    } else {
        std::cout << "MISS";
        if (was_eviction) {
            std::cout << "  (substituiu via " << victim_way
                      << ", tag=" << evicted_tag << ")";
        } else {
            std::cout << "  (carregado em via " << victim_way << ")";
        }
        std::cout << "\n";
    }
}

// -------------------------------------------------------------------------
// Estado do conjunto apos o acesso
// -------------------------------------------------------------------------

void print_set_state(int set_index, const std::vector<CacheLine>& set) {
    std::cout << "       Conj." << std::setw(2) << set_index << ": ";
    for (int w = 0; w < static_cast<int>(set.size()); w++) {
        std::cout << "[via" << w << ": ";
        if (set[w].valid) {
            std::cout << "tag=" << std::setw(3) << set[w].tag << " VALIDO]";
        } else {
            std::cout << "---    VAZIO ]";
        }
        if (w + 1 < static_cast<int>(set.size())) std::cout << " ";
    }
    std::cout << "\n";
}
