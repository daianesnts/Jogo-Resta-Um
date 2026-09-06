#pragma once
#include "Board.h"
#include "Solver.h"
#include <chrono>
#include <stack>
#include <string>

struct GameSnapshot {
  Board::Snapshot fotoTabuleiro;
  int numMovimentos;
  int dicasRestantes;
};

// Entrada do ranking salvo em arquivo
struct RankingEntry {
  std::string nome;
  std::string resultado;
  int pecas;
  int movimentos;
  long long segundosDecorridos;
  std::string dataHora;
};

class Game {
public:
  static constexpr int MAX_DICAS = 3;

  Game();
  void executar(); // Inicia o loop principal do menu

private:
  Board tabuleiro_;
  std::stack<GameSnapshot> pilhaDesfazer_;
  int numMovimentos_ = 0;
  int dicasRestantes_ = MAX_DICAS;
  std::chrono::steady_clock::time_point tempoInicio_;

  void iniciarPartida(const std::string &nomeJogador);
  bool processarRodada();
  void mostrarDica();
  void desfazerMovimento();

  // Entrada de dados
  std::string pedirNomeJogador();
  Move pedirMovimento();
  Coord pedirCoord(const std::string &prompt);
  int pedirOpcaoMenu();
  bool pedirSimNao(const std::string &prompt);

  // Display
  void exibirBanner() const;
  void exibirMenu() const;
  void exibirTabuleiro() const;
  void exibirEstatisticas() const;
  void exibirRegras() const;
  void exibirRanking() const;
  void exibirResultado(const std::string &nome) const;

  // Utilitários
  void limparTela() const;
  void pausar(const std::string &msg = "") const;
  std::string formatarTempo(long long segundos) const;
  long long segundosDecorridos() const;

  // Ranking
  void salvarResultado(const std::string &nome,
                       const std::string &resultado) const;
  void carregarRanking(std::vector<RankingEntry> &out) const;

  static constexpr const char *ARQUIVO_RANKING = "resultado.txt";
};
