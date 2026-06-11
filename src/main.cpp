#include <iostream>
#include <string>
#include <stdexcept>

#include "config.h"
#include "cache.h"
#include "display.h"
#include "address_reader.h"

// n & (n-1) zera o bit mais baixo; se o resultado for 0, só havia um bit ligado
static bool is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

static int log2_int(int n) {
    int bits = 0;
    while (n > 1) { n >>= 1; bits++; }
    return bits;
}

// Valida os parâmetros e calcula os campos derivados (bits de offset, index e tag).
// Lança std::invalid_argument se algo estiver errado.
static void validate_and_compute(Config& cfg) {
    if (!is_power_of_two(cfg.cache_size))
        throw std::invalid_argument("--cache-size deve ser potencia de 2 (atual: "
                                    + std::to_string(cfg.cache_size) + ")");

    if (!is_power_of_two(cfg.block_size))
        throw std::invalid_argument("--block-size deve ser potencia de 2 (atual: "
                                    + std::to_string(cfg.block_size) + ")");

    if (!is_power_of_two(cfg.assoc) || cfg.assoc < 1)
        throw std::invalid_argument("--assoc deve ser potencia de 2 >= 1 (atual: "
                                    + std::to_string(cfg.assoc) + ")");

    if (cfg.addr_bits < 1 || cfg.addr_bits > 64)
        throw std::invalid_argument("--addr-bits deve estar entre 1 e 64");

    if (cfg.block_size > cfg.cache_size)
        throw std::invalid_argument("--block-size nao pode ser maior que --cache-size");

    cfg.num_sets = cfg.cache_size / (cfg.block_size * cfg.assoc);

    if (cfg.num_sets < 1)
        throw std::invalid_argument("Configuracao invalida: numero de conjuntos < 1. "
                                    "Reduza a associatividade ou aumente o tamanho da cache.");

    if (!is_power_of_two(cfg.num_sets))
        throw std::invalid_argument("Numero de conjuntos (" + std::to_string(cfg.num_sets)
                                    + ") nao e potencia de 2. Verifique os parametros.");

    cfg.offset_bits = log2_int(cfg.block_size);
    cfg.index_bits  = log2_int(cfg.num_sets);
    cfg.tag_bits    = cfg.addr_bits - cfg.offset_bits - cfg.index_bits;

    if (cfg.tag_bits <= 0)
        throw std::invalid_argument("Configuracao invalida: bits de tag <= 0. "
                                    "Use --addr-bits maior ou reduza o tamanho da cache.");
}

// Lê os argumentos da linha de comando e preenche a struct Config.
// Lança exceção se algum argumento obrigatório faltar ou for inválido.
static Config parse_args(int argc, char* argv[]) {
    Config cfg;
    bool has_cache = false, has_block = false, has_input = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--verbose") { cfg.verbose = true; continue; }

        if (i + 1 >= argc)
            throw std::invalid_argument("Argumento sem valor: " + arg);

        std::string val = argv[++i];

        if      (arg == "--cache-size") { cfg.cache_size = std::stoi(val); has_cache = true; }
        else if (arg == "--block-size") { cfg.block_size = std::stoi(val); has_block = true; }
        else if (arg == "--assoc")      { cfg.assoc      = std::stoi(val); }
        else if (arg == "--addr-bits")  { cfg.addr_bits  = std::stoi(val); }
        else if (arg == "--input")      { cfg.input_file = val;            has_input = true; }
        else if (arg == "--policy") {
            if      (val == "LRU")  cfg.policy = ReplacementPolicy::LRU;
            else if (val == "FIFO") cfg.policy = ReplacementPolicy::FIFO;
            else throw std::invalid_argument("Politica invalida: '" + val + "'. Use LRU ou FIFO.");
        } else {
            throw std::invalid_argument("Argumento desconhecido: " + arg);
        }
    }

    if (!has_cache || !has_block || !has_input)
        throw std::invalid_argument("Parametros obrigatorios faltando: --cache-size, --block-size, --input");

    return cfg;
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            print_usage(argv[0]);
            return 1;
        }

        Config cfg = parse_args(argc, argv);
        validate_and_compute(cfg);
        print_config(cfg);

        auto addresses = read_addresses(cfg.input_file, cfg.addr_bits);

        if (cfg.verbose) {
            std::cout << "=== Acessos ===\n";
        }

        Cache     cache(cfg);
        SimResult result = cache.simulate(addresses);
        print_result(result);

    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
