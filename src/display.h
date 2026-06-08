#pragma once
// Autor(es): [coloque seu nome aqui]
// Descricao: Modulo de saida – todas as impressoes do programa estao aqui.
//            Separar output da logica facilita manutencao e testes.

#include <vector>
#include <cstdint>
#include "config.h"
#include "cache.h"

// Exibe os parametros calculados antes da simulacao comecar
void print_config(const Config& cfg);

// Exibe as estatisticas finais (hits, misses, percentuais)
void print_result(const SimResult& res);

// Exibe mensagem de uso correto do programa
void print_usage(const char* prog);

// Exibe uma linha do trace de acessos (chamado a cada acesso quando --verbose)
//   access_num   : numero sequencial do acesso (1, 2, 3...)
//   addr         : endereco acessado
//   tag/index/offset : campos decompostos do endereco
//   hit          : true = cache hit, false = cache miss
//   victim_way   : indice da via substituida (valido apenas no miss)
//   was_eviction : true se uma linha valida foi descartada
//   evicted_tag  : tag da linha descartada (valido se was_eviction == true)
void print_access(int access_num,
                  uint64_t addr,
                  uint64_t tag, int index, int offset,
                  bool hit,
                  int  victim_way,
                  bool was_eviction,
                  uint64_t evicted_tag);

// Exibe o estado atual de um conjunto (apos cada acesso, no modo verbose)
void print_set_state(int set_index, const std::vector<CacheLine>& set);
