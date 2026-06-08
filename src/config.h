#pragma once
// Autor(es): [coloque seu nome aqui]
// Disciplina: Organizacao de Computadores – Pratica M3
// Descricao: Configuracao do simulador de cache

#include <string>

// Politica de substituicao utilizada quando todas as vias de um conjunto estao ocupadas
enum class ReplacementPolicy {
    LRU,  // Least Recently Used  – substitui o bloco menos recentemente acessado
    FIFO  // First In, First Out  – substitui o bloco carregado ha mais tempo
};

// Configuracao completa da cache e da simulacao
struct Config {
    // -----------------------------------------------------------------------
    // Parametros fornecidos pelo usuario (via linha de comando)
    // -----------------------------------------------------------------------
    int  cache_size = 0;    // tamanho total da cache em bytes
    int  block_size = 0;    // tamanho de cada bloco/linha em bytes
    int  assoc      = 1;    // grau de associatividade (1 = mapeamento direto, N = N-way)
    int  addr_bits  = 16;   // numero de bits do endereco fisico

    std::string      input_file;
    bool             verbose = false;
    ReplacementPolicy policy = ReplacementPolicy::LRU;

    // -----------------------------------------------------------------------
    // Campos calculados automaticamente em validate_and_compute()
    // -----------------------------------------------------------------------
    int num_sets    = 0;   // numero de conjuntos  = cache_size / (block_size * assoc)
    int offset_bits = 0;   // bits para enderecar byte dentro do bloco = log2(block_size)
    int index_bits  = 0;   // bits para selecionar o conjunto          = log2(num_sets)
    int tag_bits    = 0;   // bits restantes para identificar o bloco  = addr_bits - offset - index
};
