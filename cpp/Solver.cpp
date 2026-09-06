#include "Solver.h"

int Solver::contarMovimentosApos(Board tabuleiro, const Move &m) {
  tabuleiro.aplicarMovimento(m);
  return static_cast<int>(tabuleiro.obterTodosMovimentos().size());
}

std::optional<Move> Solver::obterDica(const Board &tabuleiro) {
  const auto movimentos = tabuleiro.obterTodosMovimentos();
  if (movimentos.empty())
    return std::nullopt;

  const Move *melhor = &movimentos[0];
  int melhorPontuacao = -1;

  for (const auto &m : movimentos) {
    int pontuacao = contarMovimentosApos(tabuleiro, m);
    if (pontuacao > melhorPontuacao) {
      melhorPontuacao = pontuacao;
      melhor = &m;
    }
  }

  return *melhor;
}
