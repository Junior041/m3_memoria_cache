// Autor(es): [coloque seu nome aqui]
// Descricao: Implementacao do leitor de arquivo de enderecos

#include "address_reader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

std::vector<uint64_t> read_addresses(const std::string& filename, int addr_bits) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Erro: arquivo nao encontrado: " + filename);
    }

    // Mascara para checar se o endereco cabe em addr_bits bits
    uint64_t max_addr = (addr_bits >= 64) ? UINT64_MAX : ((1ULL << addr_bits) - 1);

    std::vector<uint64_t> addresses;
    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        line_num++;

        // Remove espacos e tabulacoes do inicio da linha
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue; // linha vazia
        line = line.substr(start);

        // Ignora comentarios
        if (line[0] == '#') continue;

        // Remove possiveis comentarios inline (ex: "0x10 # bloco X")
        auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        // Remove espacos ao final
        auto end = line.find_last_not_of(" \t\r\n");
        if (end != std::string::npos) line = line.substr(0, end + 1);
        if (line.empty()) continue;

        // Faz o parse do endereco
        uint64_t addr;
        try {
            size_t pos;
            if (line.size() >= 2 && line[0] == '0' && (line[1] == 'x' || line[1] == 'X')) {
                addr = std::stoull(line, &pos, 16);
            } else {
                addr = std::stoull(line, &pos, 10);
            }
            // Verifica se todo o token foi consumido
            if (pos != line.size()) {
                throw std::invalid_argument("caractere inesperado");
            }
        } catch (const std::exception&) {
            throw std::runtime_error("Erro na linha " + std::to_string(line_num)
                                     + ": valor invalido: \"" + line + "\"");
        }

        if (addr > max_addr) {
            std::ostringstream oss;
            oss << "Erro na linha " << line_num
                << ": endereco 0x" << std::hex << addr
                << " excede o espaco de " << std::dec << addr_bits << " bits";
            throw std::runtime_error(oss.str());
        }

        addresses.push_back(addr);
    }

    return addresses;
}
