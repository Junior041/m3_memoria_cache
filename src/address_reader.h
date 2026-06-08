#pragma once
// Autor(es): [coloque seu nome aqui]
// Descricao: Leitura de enderecos de memoria a partir de arquivo texto

#include <vector>
#include <string>
#include <cstdint>

// Le enderecos (decimal ou hexadecimal com prefixo 0x) de um arquivo.
// Linhas iniciadas com '#' sao ignoradas.
// Lanca std::runtime_error em caso de arquivo nao encontrado,
// valor invalido ou endereco fora do espaco de addr_bits bits.
std::vector<uint64_t> read_addresses(const std::string& filename, int addr_bits);
