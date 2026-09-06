#include "Board.h"
#include <cmath>

Board::Board() { init(); }

// Inicialização do tabuleiro
void Board::init() {
  pegCount_ = 0;

  for (int l = 0; l < SIZE; ++l) {
    for (int c = 0; c < SIZE; ++c) {
      if (!ehPosicaoValida(l, c)) {
        grid_[l][c] = Cell::INVALID;
      } else if (l == 3 && c == 3) {
        grid_[l][c] = Cell::EMPTY; // Centro começa vazio
      } else {
        grid_[l][c] = Cell::PEG;
        ++pegCount_;
      }
    }
  }
}

Cell Board::getCelula(int l, int c) const {
  if (l < 0 || l >= SIZE || c < 0 || c >= SIZE)
    return Cell::INVALID;
  return grid_[l][c];
}

// Uma célula é válida se não for um dos 4 cantos 2x2 do grid 7x7
bool Board::ehPosicaoValida(int l, int c) const {
  return !((l < 2 || l > 4) && (c < 2 || c > 4));
}

// Destinos válidos
std::vector<Coord> Board::obterDestinos(int l, int c) const {
  std::vector<Coord> resultado;
  if (grid_[l][c] != Cell::PEG)
    return resultado;

  const int direcoes[4][2] = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
  for (auto &d : direcoes) {
    int nl = l + d[0], nc = c + d[1];
    int ml = l + d[0] / 2, mc = c + d[1] / 2;

    if (nl < 0 || nl >= SIZE || nc < 0 || nc >= SIZE)
      continue;
    if (grid_[nl][nc] != Cell::EMPTY)
      continue;
    if (grid_[ml][mc] != Cell::PEG)
      continue;

    resultado.push_back({nl, nc});
  }
  return resultado;
}

std::vector<Move> Board::obterTodosMovimentos() const {
  std::vector<Move> movimentos;
  for (int l = 0; l < SIZE; ++l)
    for (int c = 0; c < SIZE; ++c)
      if (grid_[l][c] == Cell::PEG)
        for (const auto &dest : obterDestinos(l, c))
          movimentos.push_back({{l, c}, dest});
  return movimentos;
}

bool Board::ehFimDeJogo() const { return obterTodosMovimentos().empty(); }

bool Board::ehVitoria() const {
  return pegCount_ == 1 && grid_[3][3] == Cell::PEG;
}

bool Board::ehQuaseVitoria() const {
  return pegCount_ == 1 && grid_[3][3] != Cell::PEG;
}

// Validação de movimento
bool Board::validarMovimento(const Move &m) const {
  int ol = m.origem.linha, oc = m.origem.coluna;
  int dl = m.destino.linha, dc = m.destino.coluna;

  // Limites do tabuleiro
  if (ol < 0 || ol >= SIZE || oc < 0 || oc >= SIZE)
    return false;
  if (dl < 0 || dl >= SIZE || dc < 0 || dc >= SIZE)
    return false;

  // Posições devem ser células válidas (não cantos)
  if (!ehPosicaoValida(ol, oc))
    return false;
  if (!ehPosicaoValida(dl, dc))
    return false;

  // Origem deve ter peça, destino deve ser vazio
  if (grid_[ol][oc] != Cell::PEG)
    return false;
  if (grid_[dl][dc] != Cell::EMPTY)
    return false;

  // Deve ser salto ortogonal de exatamente 2 casas
  int deslL = dl - ol, deslC = dc - oc;
  if (!((std::abs(deslL) == 2 && deslC == 0) ||
        (deslL == 0 && std::abs(deslC) == 2)))
    return false;

  // Deve haver peça no meio para capturar
  int ml = ol + deslL / 2, mc = oc + deslC / 2;
  if (grid_[ml][mc] != Cell::PEG)
    return false;

  return true;
}

// Movimento
bool Board::aplicarMovimento(const Move &m) {
  if (!validarMovimento(m))
    return false;

  int ol = m.origem.linha, oc = m.origem.coluna;
  int dl = m.destino.linha, dc = m.destino.coluna;
  int ml = (ol + dl) / 2, mc = (oc + dc) / 2;

  grid_[ol][oc] = Cell::EMPTY;
  grid_[ml][mc] = Cell::EMPTY;
  grid_[dl][dc] = Cell::PEG;
  --pegCount_;

  return true;
}

// Desfazer
Board::Snapshot Board::capturarEstado() const {
  Snapshot s;
  s.numPecas = pegCount_;
  for (int l = 0; l < SIZE; ++l)
    for (int c = 0; c < SIZE; ++c)
      s.grid[l][c] = grid_[l][c];
  return s;
}

void Board::restaurarEstado(const Snapshot &s) {
  pegCount_ = s.numPecas;
  for (int l = 0; l < SIZE; ++l)
    for (int c = 0; c < SIZE; ++c)
      grid_[l][c] = s.grid[l][c];
}
