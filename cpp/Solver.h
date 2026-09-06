#pragma once
#include "Board.h"
#include <optional>

class Solver {
public:
  // Retorna a melhor dica de movimento para o estado atual do tabuleiro, se
  // houver movimentos disponíveis
  static std::optional<Move> obterDica(const Board &tabuleiro);

private:
  static int contarMovimentosApos(Board tabuleiro, const Move &m);
};
