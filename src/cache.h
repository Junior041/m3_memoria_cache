#pragma once
// Autor(es): [coloque seu nome aqui]
// Descricao: Declaracao da classe Cache
//
// Estrutura de dados:
//   Cache = vetor de conjuntos
//   Cada conjunto = vetor de linhas (vias)
//   Cada linha = { valida?, tag, counter }
//
//   sets_[set_index][way_index]
//
// O campo 'counter' serve a dois propositos dependendo da politica:
//   LRU  -> registra o instante do ultimo ACESSO  (hit ou miss)
//   FIFO -> registra o instante do CARREGAMENTO   (apenas miss)
// Em ambos os casos, a vitima e a linha de menor counter.

#include <vector>
#include <cstdint>
#include "config.h"

// Representa um bloco dentro da cache
struct CacheLine {
    bool     valid   = false;  // indica se a linha contem dados validos
    uint64_t tag     = 0;      // identifica qual bloco de memoria esta nesta linha
    int      counter = 0;      // usado pela politica de substituicao (ver nota acima)
};

// Estatisticas agregadas da simulacao
struct SimResult {
    int total_accesses = 0;
    int hits           = 0;
    int misses         = 0;
};

// Simulador de cache configuravel via struct Config
class Cache {
public:
    explicit Cache(const Config& cfg);

    // Simula a sequencia de acessos e retorna as estatisticas
    SimResult simulate(const std::vector<uint64_t>& addresses);

private:
    Config cfg_;
    std::vector<std::vector<CacheLine>> sets_; // sets_[conjunto][via]
    int global_time_  = 0; // relogio logico: incrementado a cada evento relevante
    int access_count_ = 0; // contador para numerar as linhas do trace verbose

    // Processa um unico acesso de memoria; retorna true se for hit
    bool access(uint64_t addr);

    // Decompoe o endereco em tag, index e offset usando operacoes de bit
    void decompose(uint64_t addr, uint64_t& tag, int& index, int& offset) const;

    // Retorna o indice da via vitima: prefere linha invalida, depois menor counter
    int find_victim(const std::vector<CacheLine>& set) const;
};
