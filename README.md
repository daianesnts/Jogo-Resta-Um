# Resta 1 

<div align="center">

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=flat&logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=flat&logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=flat&logo=javascript&logoColor=black)

**O clássico jogo de tabuleiro onde o objetivo é deixar apenas uma peça no centro!**

</div>

## Sobre o Projeto

Este projeto começou como um trabalho acadêmico do 2º período, quando cursei a disciplina de Programação 2, sem orientação a objeto. 

Agora, no 6º período, o projeto foi transformado em um **Em algo mais moderno e jogável diretamente no navegador**, sem necessidade de instalação ou compilador!

>**Nota**: Todo o código original refatorado em **C++ Orientado a Objeto** permanece salvo e preservado na pasta `/cpp` para fins de registro acadêmico.

## Como Jogar no Navegador

Basta abrir o arquivo **`index.html`** em qualquer navegador.

### Recursos do Jogo:

- **Design**: Interface minimalista e tátil, sem distrações.
- **Desfazer (Undo)**: Desfaça jogadas a qualquer momento sem perder o tempo corrido.
- **Sistema de Dicas**: Até 3 sugestões por partida.
- **Ranking Local**: Salva automaticamente suas melhores partidas no próprio navegador.
- **Temporizador e Contador**: Acompanhe o tempo e a quantidade de movimentos em tempo real.

## Como Jogar pelo Terminal

A versão original em C++ ainda pode ser compilada e executada pelo terminal. É necessário ter um compilador compatível com C++17 instalado, como o **g++**.

### Windows

Na pasta raiz do projeto, execute:

```powershell
g++ -std=c++17 cpp\Board.cpp cpp\Game.cpp cpp\Solver.cpp cpp\main.cpp -o resta_um.exe

.\resta_um.exe
```

### Linux ou macOS

Na pasta raiz do projeto, execute:

```bash
g++ -std=c++17 cpp/Board.cpp cpp/Game.cpp cpp/Solver.cpp cpp/main.cpp -o resta_um

./resta_um
```

## Estrutura do Repositório

```
Jogo-Resta-Um/
├── .gitignore
├── LICENSE
├── README.md  
├── index.html        # Página principal do jogo
├── style.css         # Estilização 
├── game.js           # Lógica do jogo 
└── cpp/              # Código fonte original em C++
    ├── Board.h/.cpp
    ├── Game.h/.cpp
    ├── Solver.h/.cpp
    └── main.cpp
```

---
Desenvolvido em grupo no 2° período e aprimorado por **Daiane Santos** no 6º período de Sistemas de Informação.
