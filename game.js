'use strict';


// CONSTANTES

const TAMANHO = 7;
const INVALIDO = null;
const VAZIO = 0;
const PECA = 1;
const MAX_DICAS = 3;
const TEMPO_DICA_MS = 4500;
const CHAVE_RANKING = 'resta1_v2_ranking';
const MAX_RANKING = 10;

// LÓGICA DO TABULEIRO 

function ehPosicaoValida(l, c) {
  return !((l < 2 || l > 4) && (c < 2 || c > 4));
}

function criarTabuleiroInicial() {
  return Array.from({ length: TAMANHO }, (_, l) =>
    Array.from({ length: TAMANHO }, (_, c) => {
      if (!ehPosicaoValida(l, c)) return INVALIDO;
      if (l === 3 && c === 3) return VAZIO;
      return PECA;
    })
  );
}

function contarPecas(tabuleiro) {
  return tabuleiro.flat().filter(v => v === PECA).length;
}

/**
 * Retorna todas as posições de destino válidas para a peça em (l, c).
 * Um movimento válido salta exatamente 2 casas ortogonalmente sobre uma peça para uma casa vazia.
 */
function obterDestinosValidos(tabuleiro, l, c) {
  if (tabuleiro[l][c] !== PECA) return [];
  const direcoes = [[-2, 0], [2, 0], [0, -2], [0, 2]];
  return direcoes.reduce((acc, [dl, dc]) => {
    const nl = l + dl, nc = c + dc;
    const ml = l + dl / 2, mc = c + dc / 2;
    if (
      nl >= 0 && nl < TAMANHO && nc >= 0 && nc < TAMANHO &&
      tabuleiro[nl][nc] === VAZIO &&
      tabuleiro[ml][mc] === PECA
    ) {
      acc.push({ linha: nl, coluna: nc });
    }
    return acc;
  }, []);
}

function obterTodosMovimentos(tabuleiro) {
  const movimentos = [];
  for (let l = 0; l < TAMANHO; l++)
    for (let c = 0; c < TAMANHO; c++)
      if (tabuleiro[l][c] === PECA)
        obterDestinosValidos(tabuleiro, l, c).forEach(destino =>
          movimentos.push({ origem: { linha: l, coluna: c }, destino })
        );
  return movimentos;
}

function aplicarMovimento(tabuleiro, ol, oc, dl, dc) {
  const t = tabuleiro.map(linha => [...linha]);
  t[ol][oc] = VAZIO;
  t[(ol + dl) / 2][(oc + dc) / 2] = VAZIO;
  t[dl][dc] = PECA;
  return t;
}

function calcularDica(tabuleiro) {
  const movimentos = obterTodosMovimentos(tabuleiro);
  if (!movimentos.length) return null;

  let melhorMovimento = movimentos[0];
  let melhorPontuacao = -1;

  for (const mov of movimentos) {
    const apos = aplicarMovimento(tabuleiro, mov.origem.linha, mov.origem.coluna, mov.destino.linha, mov.destino.coluna);
    const pontuacao = obterTodosMovimentos(apos).length;
    if (pontuacao > melhorPontuacao) { melhorPontuacao = pontuacao; melhorMovimento = mov; }
  }
  return melhorMovimento;
}

// ESTADO DO JOGO

const jogo = {
  tabuleiro: null,
  selecionada: null,
  destinosValidos: [],
  pilhaUndo: [],
  movimentos: 0,
  dicasRestantes: MAX_DICAS,
  tempoSegundos: 0,
  refCronometro: null,
  ativo: false,
  movimentoDica: null,
  refTimerDica: null,
};


const $ = id => document.getElementById(id);

const dom = {
  tabuleiro: $('board'),
  contagemPecas: $('peg-count'),
  contagemMovimentos: $('move-count'),
  cronometro: $('timer'),
  contagemDicas: $('hint-count'),
  instrucao: $('instruction'),
  btnNovaPartida: $('btn-new-game'),
  btnDesfazer: $('btn-undo'),
  btnDica: $('btn-hint'),
  btnRanking: $('btn-ranking'),
  // Resultado
  modalResultado: $('result-modal'),
  iconeResultado: $('result-icon'),
  tituloResultado: $('result-title'),
  textoResultado: $('result-text'),
  pecasResultado: $('result-pieces'),
  movimentosResultado: $('result-moves'),
  tempoResultado: $('result-time'),
  nomeJogador: $('player-name'),
  btnSalvarRanking: $('btn-save-ranking'),
  btnPularRanking: $('btn-skip-ranking'),
  // Ranking
  modalRanking: $('ranking-modal'),
  tabelaRanking: $('ranking-table'),
  corpoRanking: $('ranking-body'),
  rankingVazio: $('empty-ranking'),
  btnFecharRanking: $('btn-close-ranking'),
  btnLimparRanking: $('btn-clear-ranking'),
  // Confirmação
  modalConfirmacao: $('confirm-modal'),
  btnConfirmarSim: $('btn-confirm-yes'),
  btnConfirmarNao: $('btn-confirm-no'),
};


// CRONOMETRO

function iniciarCronometro() {
  pararCronometro();
  jogo.refCronometro = setInterval(() => {
    jogo.tempoSegundos++;
    dom.cronometro.textContent = formatarTempo(jogo.tempoSegundos);
  }, 1000);
}

function pararCronometro() {
  clearInterval(jogo.refCronometro);
  jogo.refCronometro = null;
}

function formatarTempo(s) {
  const m = Math.floor(s / 60).toString().padStart(2, '0');
  const seg = (s % 60).toString().padStart(2, '0');
  return `${m}:${seg}`;
}


// RENDERIZACAO

function renderizar() {
  const fragmento = document.createDocumentFragment();

  for (let l = 0; l < TAMANHO; l++) {
    for (let c = 0; c < TAMANHO; c++) {
      const val = jogo.tabuleiro[l][c];

      const celula = document.createElement('div');
      celula.className = 'cell';
      celula.dataset.r = l;
      celula.dataset.c = c;

      if (val === INVALIDO) {
        celula.classList.add('invalid');

      } else if (val === PECA) {
        celula.classList.add('peg');

        if (jogo.selecionada?.linha === l && jogo.selecionada?.coluna === c) {
          celula.classList.add('selected');
        }
        if (jogo.movimentoDica?.origem.linha === l && jogo.movimentoDica?.origem.coluna === c) {
          celula.classList.add('hint-from');
        }

        const peca = document.createElement('div');
        peca.className = 'piece';
        celula.appendChild(peca);

      } else { // VAZIO
        celula.classList.add('empty');

        if (jogo.destinosValidos.some(d => d.linha === l && d.coluna === c)) {
          celula.classList.add('valid-dest');
        }
        if (jogo.movimentoDica?.destino.linha === l && jogo.movimentoDica?.destino.coluna === c) {
          celula.classList.add('hint-to');
        }

        const buraco = document.createElement('div');
        buraco.className = 'hole';
        celula.appendChild(buraco);
      }

      celula.addEventListener('click', () => tratarCliqueCelula(l, c));
      fragmento.appendChild(celula);
    }
  }

  dom.tabuleiro.innerHTML = '';
  dom.tabuleiro.appendChild(fragmento);

  // Atualizar estatísticas
  const pecas = contarPecas(jogo.tabuleiro);
  dom.contagemPecas.textContent = pecas;
  dom.contagemMovimentos.textContent = jogo.movimentos;
  dom.contagemDicas.textContent = jogo.dicasRestantes;
  dom.btnDesfazer.disabled = jogo.pilhaUndo.length === 0;
  dom.btnDica.disabled = jogo.dicasRestantes === 0 || !jogo.ativo;

  // Texto de instrução
  if (jogo.movimentoDica) {
    dom.instrucao.textContent = 'Sugestão destacada — mova a peça indicada!';
  } else if (jogo.selecionada) {
    dom.instrucao.textContent = 'Clique no destino verde ou na peça novamente para cancelar';
  } else if (jogo.ativo) {
    dom.instrucao.textContent = 'Selecione uma peça e depois clique no destino';
  } else {
    dom.instrucao.textContent = '';
  }
}


// MANIPULAÇÃO DE ENTRADA

function tratarCliqueCelula(l, c) {
  if (!jogo.ativo) return;

  limparDica(false);

  const val = jogo.tabuleiro[l][c];

  // Clique em um destino válido: executa o movimento
  if (jogo.selecionada && jogo.destinosValidos.some(d => d.linha === l && d.coluna === c)) {
    executarMovimento(jogo.selecionada.linha, jogo.selecionada.coluna, l, c);
    return;
  }

  // Clique em uma peça
  if (val === PECA) {
    if (jogo.selecionada?.linha === l && jogo.selecionada?.coluna === c) {
      // Mesma peça: desmarcar
      jogo.selecionada = null;
      jogo.destinosValidos = [];
    } else {
      // Nova peça: selecionar e calcular destinos
      jogo.selecionada = { linha: l, coluna: c };
      jogo.destinosValidos = obterDestinosValidos(jogo.tabuleiro, l, c);
    }
    renderizar();
    return;
  }

  // Clique em vazio ou inválido: desmarcar
  jogo.selecionada = null;
  jogo.destinosValidos = [];
  renderizar();
}


// EXECUÇÃO DE MOVIMENTO

function executarMovimento(ol, oc, dl, dc) {
  jogo.pilhaUndo.push({
    tabuleiro: jogo.tabuleiro.map(linha => [...linha]),
    movimentos: jogo.movimentos,
    dicasRestantes: jogo.dicasRestantes,
  });

  jogo.tabuleiro = aplicarMovimento(jogo.tabuleiro, ol, oc, dl, dc);
  jogo.movimentos++;
  jogo.selecionada = null;
  jogo.destinosValidos = [];

  renderizar();
  verificarFimDeJogo();
}

function verificarFimDeJogo() {
  const pecas = contarPecas(jogo.tabuleiro);
  const temMovimentos = obterTodosMovimentos(jogo.tabuleiro).length > 0;

  if (pecas === 1 || !temMovimentos) {
    pararCronometro();
    jogo.ativo = false;
    setTimeout(() => exibirModalResultado(pecas), 350);
  }
}


// DESFAZER

function desfazerMovimento() {
  if (!jogo.pilhaUndo.length) return;

  const estavaEncerrado = !jogo.ativo;
  const foto = jogo.pilhaUndo.pop();

  jogo.tabuleiro = foto.tabuleiro;
  jogo.movimentos = foto.movimentos;
  jogo.dicasRestantes = foto.dicasRestantes;
  jogo.selecionada = null;
  jogo.destinosValidos = [];
  limparDica(false);

  if (estavaEncerrado) {
    dom.modalResultado.hidden = true;
    jogo.ativo = true;
    iniciarCronometro();
  }

  renderizar();
}


// SISTEMA DE DICAS

function mostrarDica() {
  if (!jogo.dicasRestantes || !jogo.ativo) return;

  const dica = calcularDica(jogo.tabuleiro);
  if (!dica) return;

  limparDica(false);

  jogo.dicasRestantes--;
  jogo.movimentoDica = dica;
  jogo.selecionada = dica.origem;
  jogo.destinosValidos = obterDestinosValidos(jogo.tabuleiro, dica.origem.linha, dica.origem.coluna);

  renderizar();

  jogo.refTimerDica = setTimeout(() => limparDica(true), TEMPO_DICA_MS);
}

function limparDica(re_renderizar) {
  clearTimeout(jogo.refTimerDica);
  jogo.refTimerDica = null;
  jogo.movimentoDica = null;
  if (re_renderizar) renderizar();
}


// NOVA PARTIDA

function iniciarNovaPartida() {
  pararCronometro();
  limparDica(false);

  jogo.tabuleiro = criarTabuleiroInicial();
  jogo.selecionada = null;
  jogo.destinosValidos = [];
  jogo.pilhaUndo = [];
  jogo.movimentos = 0;
  jogo.dicasRestantes = MAX_DICAS;
  jogo.tempoSegundos = 0;
  jogo.ativo = true;

  dom.cronometro.textContent = formatarTempo(0);
  iniciarCronometro();
  renderizar();
}


// EXIBE O RESULTADO

function exibirModalResultado(pecas) {
  const ehVitoria = pecas === 1;
  const ehVitoriaCentro = ehVitoria && jogo.tabuleiro[3][3] === PECA;

  let icone, titulo, texto;
  if (ehVitoriaCentro) {
    icone = '';
    titulo = 'Você Venceu!';
    texto = 'Incrível! Uma peça no centro — resultado perfeito!';
  } else if (ehVitoria) {
    icone = '';
    titulo = 'Quase Perfeito!';
    texto = 'Restou uma peça, mas não estava no centro. Muito perto!';
  } else {
    icone = '';
    titulo = 'Fim de Jogo!';
    texto = `Restaram ${pecas} peças sem movimentos possíveis. Tente novamente!`;
  }

  dom.iconeResultado.textContent = icone;
  dom.tituloResultado.textContent = titulo;
  dom.textoResultado.textContent = texto;
  dom.pecasResultado.textContent = pecas;
  dom.movimentosResultado.textContent = jogo.movimentos;
  dom.tempoResultado.textContent = formatarTempo(jogo.tempoSegundos);
  dom.nomeJogador.value = '';

  dom.modalResultado.hidden = false;
  setTimeout(() => dom.nomeJogador.focus(), 300);
}


// EXIBE O RANKING

function exibirModalRanking() {
  const dados = carregarRanking();

  if (!dados.length) {
    dom.rankingVazio.hidden = false;
    dom.tabelaRanking.hidden = true;
  } else {
    dom.rankingVazio.hidden = true;
    dom.tabelaRanking.hidden = false;
    dom.corpoRanking.innerHTML = '';

    dados.forEach((entrada, i) => {
      const tr = document.createElement('tr');
      if (i === 0) tr.classList.add('rank-gold');
      tr.innerHTML = `
        <td>${i + 1}</td>
        <td>${escaparHtml(entrada.nome)}</td>
        <td>${entrada.pecas}</td>
        <td>${entrada.movimentos}</td>
        <td class="mono">${formatarTempo(entrada.tempo)}</td>
        <td>${entrada.data}</td>
      `;
      dom.corpoRanking.appendChild(tr);
    });
  }

  dom.modalRanking.hidden = false;
}


// PERSISTÊNCIA DO RANKING 

function carregarRanking() {
  try {
    return JSON.parse(localStorage.getItem(CHAVE_RANKING)) || [];
  } catch {
    return [];
  }
}

function salvarEntradaRanking(nome) {
  const dados = carregarRanking();
  const entrada = {
    nome: (nome.trim() || 'Anônimo').slice(0, 20),
    pecas: contarPecas(jogo.tabuleiro),
    movimentos: jogo.movimentos,
    tempo: jogo.tempoSegundos,
    data: new Date().toLocaleDateString('pt-BR'),
  };
  dados.push(entrada);
  dados.sort((a, b) => a.pecas - b.pecas || a.tempo - b.tempo);
  if (dados.length > MAX_RANKING) dados.length = MAX_RANKING;
  localStorage.setItem(CHAVE_RANKING, JSON.stringify(dados));
}

function limparRanking() {
  localStorage.removeItem(CHAVE_RANKING);
}

function escaparHtml(str) {
  return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

// OPCOES DO MENU 

dom.btnNovaPartida.addEventListener('click', () => {
  if (jogo.ativo && jogo.movimentos > 0) {
    dom.modalConfirmacao.hidden = false;
  } else {
    iniciarNovaPartida();
  }
});

dom.btnDesfazer.addEventListener('click', desfazerMovimento);
dom.btnDica.addEventListener('click', mostrarDica);
dom.btnRanking.addEventListener('click', exibirModalRanking);

dom.btnSalvarRanking.addEventListener('click', () => {
  salvarEntradaRanking(dom.nomeJogador.value);
  dom.modalResultado.hidden = true;
  iniciarNovaPartida();
});

dom.btnPularRanking.addEventListener('click', () => {
  dom.modalResultado.hidden = true;
  iniciarNovaPartida();
});

dom.btnFecharRanking.addEventListener('click', () => {
  dom.modalRanking.hidden = true;
});

dom.btnLimparRanking.addEventListener('click', () => {
  if (window.confirm('Limpar todo o histórico de ranking?')) {
    limparRanking();
    dom.modalRanking.hidden = true;
  }
});

dom.btnConfirmarSim.addEventListener('click', () => {
  dom.modalConfirmacao.hidden = true;
  iniciarNovaPartida();
});

dom.btnConfirmarNao.addEventListener('click', () => {
  dom.modalConfirmacao.hidden = true;
});

dom.modalRanking.addEventListener('click', e => {
  if (e.target === dom.modalRanking) dom.modalRanking.hidden = true;
});

dom.modalConfirmacao.addEventListener('click', e => {
  if (e.target === dom.modalConfirmacao) dom.modalConfirmacao.hidden = true;
});

document.addEventListener('keydown', e => {
  if (e.key !== 'Escape') return;
  if (!dom.modalRanking.hidden) { dom.modalRanking.hidden = true; return; }
  if (!dom.modalConfirmacao.hidden) { dom.modalConfirmacao.hidden = true; return; }
  if (!dom.modalResultado.hidden) {
    dom.modalResultado.hidden = true;
    iniciarNovaPartida();
  }
});

dom.nomeJogador.addEventListener('keydown', e => {
  if (e.key === 'Enter') dom.btnSalvarRanking.click();
});

// INICIALIZAÇÃO

iniciarNovaPartida();
