#include "address_reader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

// Lê os endereços do arquivo de entrada, um por linha.
// Aceita decimal (256) e hexadecimal (0x100). Linhas com # são ignoradas.
// Lança exceção se o arquivo não existir ou algum endereço for inválido.
std::vector<uint64_t> read_addresses(const std::string& filename, int addr_bits) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Erro: arquivo nao encontrado: " + filename);
    }

    uint64_t max_addr = (addr_bits >= 64) ? UINT64_MAX : ((1ULL << addr_bits) - 1);

    std::vector<uint64_t> addresses;
    std::string line;
    int line_num = 0;

    while (std::getline(file, line)) {
        line_num++;

        // Remove espaços no início e ignora linhas vazias ou comentários
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);

        if (line[0] == '#') continue;

        // Remove comentário inline (ex: "0x10 # acesso inicial")
        auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        auto end = line.find_last_not_of(" \t\r\n");
        if (end != std::string::npos) line = line.substr(0, end + 1);
        if (line.empty()) continue;

        uint64_t addr;
        try {
            size_t pos;
            if (line.size() >= 2 && line[0] == '0' && (line[1] == 'x' || line[1] == 'X')) {
                addr = std::stoull(line, &pos, 16);
            } else {
                addr = std::stoull(line, &pos, 10);
            }
            if (pos != line.size()) {
                throw std::invalid_argument("caractere inesperado");
            }
        } catch (const std::exception&) {
            throw std::runtime_error("Erro na linha " + std::to_string(line_num)
                                     + ": valor invalido: \"" + line + "\"");
        }

        // Rejeita endereços maiores do que o espaço de endereçamento permite
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
