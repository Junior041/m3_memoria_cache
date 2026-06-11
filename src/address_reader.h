#pragma once

#include <vector>
#include <string>
#include <cstdint>

// Lê endereços de um arquivo (decimal ou hex com 0x). Linhas com # são ignoradas.
std::vector<uint64_t> read_addresses(const std::string& filename, int addr_bits);
