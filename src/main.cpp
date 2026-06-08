// Autor(es): [coloque seu nome aqui]
// Disciplina: Organizacao de Computadores – Pratica M3
// Descricao: Simulador de memoria cache
//            Suporta mapeamento direto e N-way set-associative (LRU ou FIFO)
//
// Uso:
//   ./m3_memoria_cache --cache-size <bytes> --block-size <bytes> --assoc <N>
//                      --addr-bits <N> --input <arquivo>
//                      [--policy LRU|FIFO] [--verbose]

#include <iostream>
#include <string>
#include <stdexcept>

#include "config.h"
#include "cache.h"
#include "display.h"
#include "address_reader.h"

// -------------------------------------------------------------------------
// Utilitarios de validacao numerica
// -------------------------------------------------------------------------


// Verifica se n e potencia de 2 — necessario porque os calculos de bits
// usam log2, que so e inteiro para potencias de 2.
static bool is_power_of_two(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// Calcula log2 de n assumindo que n e potencia de 2.
// Equivale a contar quantas vezes n pode ser dividido por 2.
// Ex: log2_int(16) = 4, pois 16 >> 4 = 1.
static int log2_int(int n) {
    int bits = 0;
    while (n > 1) { n >>= 1; bits++; }
    return bits;
}

// -------------------------------------------------------------------------
// Validacao dos parametros e calculo dos campos derivados
// -------------------------------------------------------------------------

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

    // Por que num_sets = cache_size / (block_size * assoc)?
    //   A cache possui (cache_size / block_size) linhas no total.
    //   Com associatividade N, cada conjunto agrupa N linhas (N vias).
    //   Portanto: num_sets = total_de_linhas / assoc
    cfg.num_sets = cfg.cache_size / (cfg.block_size * cfg.assoc);

    if (cfg.num_sets < 1)
        throw std::invalid_argument("Configuracao invalida: numero de conjuntos < 1. "
                                    "Reduza a associatividade ou aumente o tamanho da cache.");

    if (!is_power_of_two(cfg.num_sets))
        throw std::invalid_argument("Numero de conjuntos (" + std::to_string(cfg.num_sets)
                                    + ") nao e potencia de 2. Verifique os parametros.");

    // Por que offset_bits = log2(block_size)?
    //   O bloco contem block_size bytes. Para enderecar qualquer posicao dentro dele
    //   sao necessarios log2(block_size) bits. Ex: bloco de 16 bytes -> 4 bits (0..15).
    cfg.offset_bits = log2_int(cfg.block_size);

    // Por que index_bits = log2(num_sets)?
    //   Precisamos de bits suficientes para selecionar entre todos os conjuntos.
    //   Ex: 16 conjuntos -> 4 bits (0..15).
    cfg.index_bits = log2_int(cfg.num_sets);

    // Por que tag_bits = addr_bits - offset_bits - index_bits?
    //   Os tres campos (tag, index, offset) juntos compoem o endereco completo.
    //   A tag ocupa todos os bits que sobram apos reservar offset e index.
    //   Ela e quem diferencia blocos distintos que mapeiam no mesmo conjunto.
    cfg.tag_bits = cfg.addr_bits - cfg.offset_bits - cfg.index_bits;

    if (cfg.tag_bits <= 0)
        throw std::invalid_argument("Configuracao invalida: bits de tag <= 0. "
                                    "Use --addr-bits maior ou reduza o tamanho da cache.");
}

// -------------------------------------------------------------------------
// Parsing dos argumentos da linha de comando
// -------------------------------------------------------------------------

static Config parse_args(int argc, char* argv[]) {
    Config cfg;
    bool has_cache = false, has_block = false, has_input = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--verbose") { cfg.verbose = true; continue; }

        if (i + 1 >= argc)
            throw std::invalid_argument("Argumento sem valor: " + arg);

        std::string val = argv[++i];

        if (arg == "--cache-size") {
            cfg.cache_size = std::stoi(val);
            has_cache = true;
        } else if (arg == "--block-size") {
            cfg.block_size = std::stoi(val);
            has_block = true;
        } else if (arg == "--assoc") {
            cfg.assoc = std::stoi(val);
        } else if (arg == "--addr-bits") {
            cfg.addr_bits = std::stoi(val);
        } else if (arg == "--input") {
            cfg.input_file = val;
            has_input = true;
        } else if (arg == "--policy") {
            if (val == "LRU")       cfg.policy = ReplacementPolicy::LRU;
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
            std::cout << "=== Trace de Acessos ===\n";
        }

        Cache    cache(cfg);
        SimResult result = cache.simulate(addresses);
        print_result(result);

    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
