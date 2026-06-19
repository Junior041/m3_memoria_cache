// display.cpp - formatacao das saidas: configuracao inicial, trace verbose
//               e relatorio final de hits/misses.
// Autores: Ismael Antonio da Silva Junior, Guilherme Thommy, Eduardo Leopoldo

#include "display.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

static std::string policy_name(ReplacementPolicy p) {
    return (p == ReplacementPolicy::LRU) ? "LRU" : "FIFO";
}

// Exibe os parâmetros da cache antes de iniciar a simulação
void print_config(const Config& cfg) {
    std::string mapping;
    if (cfg.assoc == 1) {
        mapping = "Mapeamento Direto";
    } else {
        mapping = std::to_string(cfg.assoc) + "-way Set-Associative ("
                  + policy_name(cfg.policy) + ")";
    }

    std::cout << "\n=== Configuracao da Cache ===\n";
    std::cout << "  Tamanho total  : " << cfg.cache_size  << " bytes\n";
    std::cout << "  Tamanho bloco  : " << cfg.block_size  << " bytes\n";
    std::cout << "  Associatividade: " << cfg.assoc << " (" << mapping << ")\n";
    std::cout << "  Conjuntos      : " << cfg.num_sets    << "\n";
    std::cout << "  Bits de offset : " << cfg.offset_bits << "\n";
    std::cout << "  Bits de index  : " << cfg.index_bits  << "\n";
    std::cout << "  Bits de tag    : " << cfg.tag_bits    << "\n";
    std::cout << "  Bits de endereco: " << cfg.addr_bits  << "\n";
}

// Exibe o resumo final com hits, misses e percentuais
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

// Mostra como usar o programa caso os argumentos estejam errados ou ausentes
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

// Imprime uma linha do trace: endereço, campos decompostos e resultado (HIT ou MISS)
void print_access(int access_num,
                  uint64_t addr,
                  uint64_t tag, int index, int offset,
                  bool hit,
                  int  victim_way,
                  bool was_removal,
                  uint64_t removed_tag)
{
    std::cout << "  Addr=0x" << std::hex << std::setw(4) << std::setfill('0') << addr
              << std::dec
              << "  tag=" << std::setw(3) << tag
              << "  index=" << std::setw(2) << index
              << "  offset=" << std::setw(2) << offset
              << "  -> ";

    if (hit) {
        std::cout << "HIT\n";
    } else {
        std::cout << "MISS";
        if (was_removal) {
            std::cout << "  (removido via " << victim_way << ", tag=" << removed_tag << ")";
        } else {
            std::cout << "  (carregado na via " << victim_way << ")";
        }
        std::cout << "\n";
    }
}

// Mostra o estado atual do conjunto após cada acesso (usado no modo verbose)
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
