#include "Game.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;
using namespace std::chrono;

Game::Game() = default;

void Game::executar() {
  limparTela();
  exibirBanner();
  pausar("Pressione ENTER para continuar...");

  int opcao;
  do {
    limparTela();
    exibirMenu();
    opcao = pedirOpcaoMenu();

    switch (opcao) {
    case 1: {
      string nome = pedirNomeJogador();
      iniciarPartida(nome);
      break;
    }
    case 2:
      limparTela();
      exibirRegras();
      pausar();
      break;
    case 3:
      limparTela();
      exibirRanking();
      pausar();
      break;
    case 4:
      limparTela();
      cout << "\n  Até a próxima!\n\n";
      break;
    default:
      cout << "\n  Opção inválida. Tente novamente.\n";
      pausar();
    }
  } while (opcao != 4);
}

void Game::iniciarPartida(const string &nomeJogador) {
  tabuleiro_ = Board{};
  numMovimentos_ = 0;
  dicasRestantes_ = MAX_DICAS;

  while (!pilhaDesfazer_.empty())
    pilhaDesfazer_.pop();

  tempoInicio_ = steady_clock::now();

  while (true) {
    limparTela();
    exibirEstatisticas();
    exibirTabuleiro();

    if (tabuleiro_.ehVitoria()) {
      exibirResultado(nomeJogador);
      salvarResultado(nomeJogador, "Vitória (peça no centro)");
      pausar();
      return;
    }
    if (tabuleiro_.ehQuaseVitoria()) {
      exibirResultado(nomeJogador);
      salvarResultado(nomeJogador, "Quase (1 peça, fora do centro)");
      pausar();
      return;
    }
    if (tabuleiro_.ehFimDeJogo()) {
      exibirResultado(nomeJogador);
      salvarResultado(nomeJogador, "Derrota");
      pausar();
      return;
    }

    if (!processarRodada())
      return;
  }
}

bool Game::processarRodada() {
  cout << "\n  [D]ica (" << dicasRestantes_ << " restante(s))  "
       << "[U]ndo  [S]air\n";
  cout << "  Comando (ou apenas ENTER para jogar): ";

  string linha;
  getline(cin, linha);

  linha.erase(0, linha.find_first_not_of(" \t"));
  if (!linha.empty())
    linha.erase(linha.find_last_not_of(" \t") + 1);

  if (linha.empty()) {
    Move m = pedirMovimento();
    if (!tabuleiro_.validarMovimento(m)) {
      cout << "\n  Movimento inválido! Verifique as coordenadas.\n";
      pausar();
      return true;
    }
    pilhaDesfazer_.push(
        {tabuleiro_.capturarEstado(), numMovimentos_, dicasRestantes_});
    tabuleiro_.aplicarMovimento(m);
    ++numMovimentos_;
    return true;
  }

  char cmd = static_cast<char>(toupper(linha[0]));
  switch (cmd) {
  case 'D':
    mostrarDica();
    return true;
  case 'U':
    desfazerMovimento();
    return true;
  case 'S':
    if (pedirSimNao("Tem certeza que quer sair? (s/n): "))
      return false;
    return true;
  default:
    cout << "\n  Comando desconhecido.\n";
    pausar();
    return true;
  }
}

void Game::mostrarDica() {
  if (dicasRestantes_ <= 0) {
    cout << "\n  Você não tem mais dicas disponíveis nesta partida.\n";
    pausar();
    return;
  }

  auto dica = Solver::obterDica(tabuleiro_);
  if (!dica) {
    cout << "\n  Nenhum movimento possível encontrado.\n";
    pausar();
    return;
  }

  --dicasRestantes_;
  cout << "\n Sugestão de Dica (" << dicasRestantes_ << " restante(s)):\n";
  cout << "     Mova a peça de (" << dica->origem.linha + 1 << ", "
       << dica->origem.coluna + 1 << ")"
       << " para (" << dica->destino.linha + 1 << ", "
       << dica->destino.coluna + 1 << ")\n";
  pausar();
}

void Game::desfazerMovimento() {
  if (pilhaDesfazer_.empty()) {
    cout << "\n  Nenhum movimento para desfazer.\n";
    pausar();
    return;
  }

  GameSnapshot foto = pilhaDesfazer_.top();
  pilhaDesfazer_.pop();

  tabuleiro_.restaurarEstado(foto.fotoTabuleiro);
  numMovimentos_ = foto.numMovimentos;
  dicasRestantes_ = foto.dicasRestantes;

  cout << "\n  Último movimento desfeito!\n";
  pausar();
}

string Game::pedirNomeJogador() {
  cout << "\n  Digite seu nome (ou apelido): ";
  string nome;
  getline(cin, nome);

  nome.erase(0, nome.find_first_not_of(" \t"));
  if (!nome.empty())
    nome.erase(nome.find_last_not_of(" \t") + 1);

  if (nome.empty())
    nome = "Jogador";
  if (nome.length() > 20)
    nome = nome.substr(0, 20);

  return nome;
}

Move Game::pedirMovimento() {
  cout << "\n  --- Nova Jogada ---\n";
  Coord de = pedirCoord("  Peça a mover (linha coluna, ex: 4 2): ");
  Coord para = pedirCoord("  Destino      (linha coluna, ex: 4 4): ");
  return {de, para};
}

Coord Game::pedirCoord(const string &prompt) {
  while (true) {
    cout << prompt;
    string linha;
    getline(cin, linha);

    stringstream ss(linha);
    int l, c;
    if (ss >> l >> c) {
      return {l - 1, c - 1};
    }
    cout << "  Entrada inválida. Digite dois números separados por espaço (ex: "
            "4 2).\n";
  }
}

int Game::pedirOpcaoMenu() {
  while (true) {
    cout << "  Opção: ";
    string linha;
    getline(cin, linha);
    stringstream ss(linha);
    int val;
    if (ss >> val)
      return val;
    cout << "  Entrada inválida. Digite um número de 1 a 4.\n";
  }
}

bool Game::pedirSimNao(const string &prompt) {
  cout << prompt;
  string linha;
  getline(cin, linha);
  if (!linha.empty()) {
    char c = static_cast<char>(tolower(linha[0]));
    return (c == 's' || c == 'y');
  }
  return false;
}

void Game::exibirBanner() const {
  cout << "-----------------------------------------\n";
  cout << "             R E S T A   1               \n";
  cout << "-----------------------------------------\n";
}

void Game::exibirMenu() const {
  exibirBanner();
  cout << "  1. Jogar Nova Partida\n";
  cout << "  2. Instruções e Regras\n";
  cout << "  3. Ver Ranking e Histórico\n";
  cout << "  4. Sair\n";
}

void Game::exibirTabuleiro() const {
  cout << "\n     ";
  for (int c = 1; c <= Board::SIZE; ++c)
    cout << " " << c;
  cout << "\n    +-----------------+\n";

  for (int l = 0; l < Board::SIZE; ++l) {
    cout << "  " << (l + 1) << " |";
    for (int c = 0; c < Board::SIZE; ++c) {
      Cell cel = tabuleiro_.getCelula(l, c);
      switch (cel) {
      case Cell::INVALID:
        cout << "  ";
        break;
      case Cell::EMPTY:
        cout << " .";
        break;
      case Cell::PEG:
        cout << " O";
        break;
      }
    }
    cout << " |\n";
  }
  cout << "    +-----------------+\n";
}

void Game::exibirEstatisticas() const {
  cout << "\n  Peças Restantes: " << tabuleiro_.obterNumPecas()
       << "   |   Movimentos: " << numMovimentos_
       << "   |   Tempo: " << formatarTempo(segundosDecorridos()) << "\n";
}

void Game::exibirRegras() const {
  cout << "-----------------------------------------------------\n";
  cout << "               INSTRUÇÕES E REGRAS                   \n";
  cout << "-----------------------------------------------------\n\n";
  cout << "  OBJETIVO:\n";
  cout << "    Remover as peças saltando sobre elas até restar\n";
  cout << "    apenas UMA peça no centro (resultado perfeito).\n\n";
  cout << "  REGRAS:\n";
  cout << "    - Uma peça pode saltar sobre outra adjacente\n";
  cout << "      (horizontal ou verticalmente) para uma casa vazia.\n";
  cout << "    - A peça sobre a qual se saltou é removida.\n";
  cout << "    - Não são permitidos saltos em diagonal.\n\n";
  cout << "  COMANDOS DURANTE O JOGO:\n";
  cout << "    - [D]ICA  : Sugere um movimento (máx. 3 por partida).\n";
  cout << "    - [U]NDO  : Desfaz a última jogada realizada.\n";
  cout << "    - [S]AIR  : Abandona a partida e volta ao menu.\n\n";
}

void Game::exibirResultado(const string &nome) const {
  cout << "-----------------------------------------------------\n";
  if (tabuleiro_.ehVitoria()) {
    cout << "   VITÓRIA PERFEITA! Sobrou 1 peça no centro!\n";
  } else if (tabuleiro_.ehQuaseVitoria()) {
    cout << "   QUASE! Sobrou 1 peça (fora do centro).\n";
  } else {
    cout << "   FIM DE JOGO! Restaram " << tabuleiro_.obterNumPecas()
         << " peças.\n";
  }
  cout << "-----------------------------------------------------\n";
  cout << "  Jogador    : " << nome << "\n";
  cout << "  Peças      : " << tabuleiro_.obterNumPecas() << "\n";
  cout << "  Movimentos : " << numMovimentos_ << "\n";
  cout << "  Tempo Total: " << formatarTempo(segundosDecorridos()) << "\n";
  cout << "-----------------------------------------------------\n";
}

void Game::exibirRanking() const {
  vector<RankingEntry> ranking;
  carregarRanking(ranking);

  cout << "-----------------------------------------------------\n";
  cout << "               HISTÓRICO E RANKING                   \n";
  cout << "-----------------------------------------------------\n";

  if (ranking.empty()) {
    cout << "\n  Nenhum resultado registrado ainda.\n\n";
    return;
  }

  cout << left << setw(4) << " # " << setw(16) << "Jogador" << setw(8)
       << "Peças" << setw(8) << "Movs" << setw(10) << "Tempo" << setw(20)
       << "Data/Hora"
       << "\n-----------------------------------------------------\n";

  int pos = 1;
  for (const auto &e : ranking) {
    cout << left << setw(4) << (to_string(pos++) + ".") << setw(16) << e.nome
         << setw(8) << e.pecas << setw(8) << e.movimentos << setw(10)
         << formatarTempo(e.segundosDecorridos) << setw(20) << e.dataHora
         << "\n";
  }
  cout << "-----------------------------------------------------\n";
}

void Game::salvarResultado(const string &nome, const string &resultado) const {
  ofstream file(ARQUIVO_RANKING, ios::app);
  if (!file.is_open())
    return;

  auto agora = system_clock::to_time_t(system_clock::now());
  char buf[30];
  strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", localtime(&agora));

  file << nome << ";" << resultado << ";" << tabuleiro_.obterNumPecas() << ";"
       << numMovimentos_ << ";" << segundosDecorridos() << ";" << buf << "\n";
}

void Game::carregarRanking(vector<RankingEntry> &out) const {
  out.clear();
  ifstream file(ARQUIVO_RANKING);
  if (!file.is_open())
    return;

  string linha;
  while (getline(file, linha)) {
    if (linha.empty())
      continue;
    stringstream ss(linha);
    string token;
    vector<string> campos;

    while (getline(ss, token, ';'))
      campos.push_back(token);

    if (campos.size() >= 6) {
      RankingEntry e;
      e.nome = campos[0];
      e.resultado = campos[1];
      e.pecas = stoi(campos[2]);
      e.movimentos = stoi(campos[3]);
      e.segundosDecorridos = stoll(campos[4]);
      e.dataHora = campos[5];
      out.push_back(e);
    }
  }

  sort(out.begin(), out.end(),
       [](const RankingEntry &a, const RankingEntry &b) {
         if (a.pecas != b.pecas)
           return a.pecas < b.pecas;
         return a.segundosDecorridos < b.segundosDecorridos;
       });
}

void Game::limparTela() const {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void Game::pausar(const string &msg) const {
  if (!msg.empty())
    cout << "\n  " << msg;
  else
    cout << "\n  Pressione ENTER para continuar...";
  string dummy;
  getline(cin, dummy);
}

string Game::formatarTempo(long long segundos) const {
  long long m = segundos / 60;
  long long s = segundos % 60;
  ostringstream oss;
  oss << setfill('0') << setw(2) << m << ":" << setfill('0') << setw(2) << s;
  return oss.str();
}

long long Game::segundosDecorridos() const {
  auto agora = steady_clock::now();
  return duration_cast<seconds>(agora - tempoInicio_).count();
}
