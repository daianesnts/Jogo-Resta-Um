#include "Game.h"
#include <clocale>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
  // Suporte para acentuação UTF-8 no terminal
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#else
  std::setlocale(LC_ALL, "");
#endif

  try {
    Game game;
    game.executar();
  } catch (const std::exception &e) {
    std::cerr << "\n  Erro fatal: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
