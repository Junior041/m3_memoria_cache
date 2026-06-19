// display.h - funcoes de impressao da configuracao, do trace e do resultado.
// Autores: Ismael Antonio da Silva Junior, Guilherme Thommy, Eduardo Leopoldo
#pragma once

#include <vector>
#include <cstdint>
#include "config.h"
#include "cache.h"

void print_config(const Config& cfg);
void print_result(const SimResult& res);
void print_usage(const char* prog);
void print_access(int access_num,
                  uint64_t addr,
                  uint64_t tag, int index, int offset,
                  bool hit,
                  int  victim_way,
                  bool was_removal,
                  uint64_t removed_tag);
void print_set_state(int set_index, const std::vector<CacheLine>& set);
