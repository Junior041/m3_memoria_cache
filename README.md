# Simulador de Memória Cache – Prática M3

Simulador de cache com suporte a **mapeamento direto** e **N-way set-associative** (políticas LRU e FIFO).

---

## Compilação

```bash
cmake -B build
cmake --build build
```

O executável será gerado em `build/m3_memoria_cache`.

---

## Uso

```
./build/m3_memoria_cache --cache-size <bytes> --block-size <bytes> --assoc <N>
                         --addr-bits <N> --input <arquivo>
                         [--policy LRU|FIFO] [--verbose]
```

### Parâmetros

| Parâmetro      | Descrição                                          | Exemplo     |
|----------------|----------------------------------------------------|-------------|
| `--cache-size` | Tamanho total da cache em bytes (potência de 2)    | `256`       |
| `--block-size` | Tamanho do bloco/linha em bytes (potência de 2)    | `16`        |
| `--assoc`      | Associatividade: 1 = direto, N = N-way             | `1`, `2`, `4` |
| `--addr-bits`  | Número de bits do endereço físico                  | `16`        |
| `--input`      | Arquivo com sequência de endereços                 | `test1.txt` |
| `--policy`     | Política de substituição (padrão: LRU)             | `LRU`, `FIFO` |
| `--verbose`    | Exibe trace detalhado de cada acesso               | —           |

---

## Exemplos de Uso

### Mapeamento Direto
```bash
./build/m3_memoria_cache \
  --cache-size 256 --block-size 16 --assoc 1 \
  --addr-bits 16 --input test1.txt --verbose
```

### 2-way Set-Associative com LRU
```bash
./build/m3_memoria_cache \
  --cache-size 1024 --block-size 32 --assoc 2 \
  --addr-bits 16 --input test2.txt
```

### 4-way com FIFO
```bash
./build/m3_memoria_cache \
  --cache-size 512 --block-size 8 --assoc 4 \
  --addr-bits 16 --input test2.txt --policy FIFO
```

---

## Formato do Arquivo de Endereços

- Um endereço por linha
- Aceita decimal (`256`) ou hexadecimal (`0x0100`)
- Linhas iniciadas com `#` são ignoradas (comentários)

```
# Exemplo
0x0010
0x0024
256
0x00FF
```

---

## Saída Esperada

```
=== Configuracao da Cache ===
  Tamanho total  : 256 bytes
  Tamanho bloco  : 16 bytes
  Associatividade: 1 (Mapeamento Direto)
  Conjuntos      : 16
  Bits de offset : 4
  Bits de index  : 4
  Bits de tag    : 8
  Bits de endereco: 16

=== Resultado da Simulacao ===
  Acessos totais : 10
  Cache hits     : 3  (30.00%)
  Cache misses   : 7  (70.00%)
```

Com `--verbose`, cada linha do trace exibe:
```
  Addr=0x0010  tag=  0  index= 1  offset= 0  -> MISS
  Addr=0x0110  tag=  1  index= 1  offset= 0  -> MISS
  Addr=0x0010  tag=  0  index= 1  offset= 0  -> MISS  (colisao: bloco 0x0110 expulso)
```

---

## Validação dos Bits (tabela da especificação)

| Cache | Bloco | Assoc | Addr | Offset | Index | Tag |
|-------|-------|-------|------|--------|-------|-----|
| 256   | 16    | 1     | 16   | 4      | 4     | 8   |
| 1024  | 32    | 2     | 16   | 5      | 4     | 7   |
| 512   | 8     | 4     | 16   | 3      | 4     | 9   |
| 2048  | 64    | 8     | 32   | 6      | 2     | 24  |
