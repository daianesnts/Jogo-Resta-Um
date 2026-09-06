#pragma once
#include <vector>

// Representação de uma célula do tabuleiro
enum class Cell { INVALID, EMPTY, PEG };

// Coordenada simples (linha, coluna)
struct Coord {
  int linha, coluna;
  bool operator==(const Coord &o) const {
    return linha == o.linha && coluna == o.coluna;
  }
};

// Peça em origem pula sobre a do meio e cai em destino
struct Move {
  Coord origem, destino;
};

class Board {
public:
  static constexpr int SIZE = 7;
  static constexpr int INITIAL_PEGS = 32; // 33 casas válidas - 1 centro vazio

  Board(); // Inicializa o tabuleiro com centro vazio

  Cell getCelula(int l, int c) const;
  int obterNumPecas() const { return pegCount_; }
  bool ehPosicaoValida(int l, int c) const;

  std::vector<Coord> obterDestinos(int l, int c) const;

  std::vector<Move> obterTodosMovimentos() const;

  bool ehFimDeJogo() const;    // Sem movimentos possíveis
  bool ehVitoria() const;      // 1 peça, no centro
  bool ehQuaseVitoria() const; // 1 peça, fora do centro

  bool aplicarMovimento(const Move &m);

  bool validarMovimento(const Move &m) const;

  struct Snapshot {
    Cell grid[SIZE][SIZE];
    int numPecas;
  };
  Snapshot capturarEstado() const;
  void restaurarEstado(const Snapshot &s);

private:
  Cell grid_[SIZE][SIZE];
  int pegCount_;

  void init();
};
