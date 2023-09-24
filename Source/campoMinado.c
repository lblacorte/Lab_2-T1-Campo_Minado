#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#ifdef _WIN32 //Verifica se estamos compilando no Windows
#include <windows.h>
#endif
//Inclusão da biblioteca Allegro:
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

//Registro contendo as variáveis de cada ponto da matriz
typedef struct PONTO{
    bool mina; //Se o ponto tem mina ou não
    bool minaClicada; //Marca a mina clicada que resulta em game over
    bool revelado; //Se o ponto foi revelado pelo jogador ou não
    int minasVizinhas; //Número de casas vizinhas que contém minas
    bool bandeira; //Se o jogador marcou o ponto com bandeira
    bool interrogacao; //Se o jogador marcou o ponto com interrogação
    bool mostra; //Para quando o usuário quer avaliar as possibilidades de bombas
} Ponto;

//Registro contendo todas as variáveis do jogo:
typedef struct JOGO{
    Ponto*** matriz; //Matriz dinâmica de pontos dinâmicos que controla o tabuleiro do jogo
    int numLinhas; //Número de linhas a ser escolhido pelo usuário
    int numColunas; //Número de colunas a ser escolhido pelo usuário
    int numBombas; //Número de bombas a ser escolhido pelo usuário
    int numBandeiras; //Número de bandeiras que o usuário colocou relacionado ao número de bombas
    double tempo; //Conta o tempo decorrido desde o início da partida
    bool estado; //Controla o estado do jogo
    bool vitoria; //Se ganhou ou perdeu
} Jogo;

// Função para alocar a matriz dinamicamente (matriz dinâmica de pontos dinâmicos)
Ponto*** alocarMatriz(int nLin, int nCol) {
    Ponto*** matriz = (Ponto***) malloc(nLin * sizeof(Ponto**));
    for (int i = 0; i < nLin; i++) {
        matriz[i] = (Ponto**) malloc(nCol * sizeof(Ponto*));
        for (int j = 0; j < nCol; j++) {
            matriz[i][j] = (Ponto*) malloc(sizeof(Ponto));
            // Inicializando tudo como 0:
            matriz[i][j]->mina = false;
            matriz[i][j]->revelado = false;
            matriz[i][j]->minasVizinhas = 0;
            matriz[i][j]->bandeira = false;
            matriz[i][j]->interrogacao = false;
            matriz[i][j]->minaClicada = false;
            matriz[i][j]->mostra = false;
        }
    }
    return matriz;
}

// Função para liberar a matriz dinâmica de pontos dinâmicos
void liberarMatriz(Ponto*** matriz, int nLin, int nCol) {
    for (int i = 0; i < nLin; i++) {
        for (int j = 0; j < nCol; j++) {
            free(matriz[i][j]); // Libera cada ponto dinâmico
        }
        free(matriz[i]); // Libera cada linha
    }
    free(matriz); // Libera o vetor de linhas
}

// Função para colocar o determinado número de minas em posições aleatórias não iguais e diferentes da casa clicada
void aleatorizarMinas(Jogo* jogo, int linhaExcluida, int colunaExcluida) {
    srand(time(NULL));
    int indiceSorteado;
    int nMinasJaPosicionadas = 0;
    int nMinas = jogo->numBombas;
    int nLin = jogo->numLinhas;
    int nCol = jogo->numColunas;
    int* indicesJaSorteados = (int*) malloc(nLin * nCol * sizeof(int));
    // Inicializa o vetor de índices sorteados com 0
    for (int i = 0; i < nLin * nCol; i++) {
        indicesJaSorteados[i] = 0;
    }
    //Loop que controla o número de alterações já feitas na matriz
    while (nMinasJaPosicionadas < nMinas) {
        //Sorteia um índice aleatório que ainda não foi sorteado
        do {
            indiceSorteado = rand() % (nLin * nCol); //Gera um índice aleatório
        } while (indicesJaSorteados[indiceSorteado] == 1 || //Verifica se o índice já foi sorteado
                 (indiceSorteado / nCol == linhaExcluida && indiceSorteado % nCol == colunaExcluida)); //Verifica se o índice é igual ao excluído
        indicesJaSorteados[indiceSorteado] = 1; //Marca o índice como sorteado
        int linha = indiceSorteado / nCol; //Calcula a linha a partir do índice
        int coluna = indiceSorteado % nCol; //Calcula a coluna a partir do índice
        // Altera o valor da célula para true, indicando a presença de uma mina
        jogo->matriz[linha][coluna]->mina = true;
        nMinasJaPosicionadas++;
    }
    // Libera o vetor de índices sorteados
    free(indicesJaSorteados);
}

// Função para calcular o número de minas vizinhas
void calcularMinasVizinhas(Jogo* jogo) {
    int nLin = jogo->numLinhas;
    int nCol = jogo->numColunas;
    for (int linha = 0; linha < nLin; linha++) {
        for (int coluna = 0; coluna < nCol; coluna++) {
            if (!jogo->matriz[linha][coluna]->mina) {
                int minasVizinhas = 0;
                // Verifica as 8 casas vizinhas
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        int novaLinha = linha + i;
                        int novaColuna = coluna + j;
                        // Verifica se a nova linha e a nova coluna estão dentro dos limites da matriz
                        if (novaLinha >= 0 && novaLinha < nLin && novaColuna >= 0 && novaColuna < nCol) {
                            if (jogo->matriz[novaLinha][novaColuna]->mina) {
                                minasVizinhas++;
                            }
                        }
                    }
                }
                // Atribui o número de minas vizinhas à célula atual
                jogo->matriz[linha][coluna]->minasVizinhas = minasVizinhas;
            }
        }
    }
}

void testarVitoria(Jogo* jogo){
    for(int i = 0; i < jogo->numLinhas; i++){
        for(int j = 0; j < jogo->numColunas; j++){
            Ponto* ponto = jogo->matriz[i][j];
            if ((!ponto->revelado && !ponto->mina) || (ponto->mina && !ponto->bandeira)){
                return;
            }
        }
    }
    jogo->estado = false;
    jogo->vitoria = true;
}

void revelarVizinhas(Jogo* jogo, int linha, int coluna) {
    // Verifica se as coordenadas estão dentro dos limites da matriz
    if (linha < 0 || linha >= jogo->numLinhas || coluna < 0 || coluna >= jogo->numColunas) {
        return;
    }
    Ponto* pontoAtual = jogo->matriz[linha][coluna];
    // Verifica se a célula já foi revelada ou tem bandeira ou interrogação
    if (pontoAtual->revelado || pontoAtual->interrogacao || pontoAtual->bandeira) {
        return;
    }
    // Revela a célula atual
    pontoAtual->revelado = true;
    // Se a célula atual não tem minas vizinhas, revela as células vizinhas
    if (pontoAtual->minasVizinhas == 0) {
        // Chama a função recursivamente para todas as células vizinhas
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                revelarVizinhas(jogo, linha + i, coluna + j);
            }
        }
    }
    testarVitoria(jogo);
}

//Função padrão que exibe erro
void apontaErro(char* mensagem)
{
  printf("\nERRO\n%s\n\n", mensagem);
  exit(13);
}

//Função para criar uma janela
ALLEGRO_DISPLAY* inicializarJanela(){
    ALLEGRO_DISPLAY* janela = NULL;
    if (!al_init()) apontaErro("Não foi possível inicializar o Allegro");
    janela = al_create_display(800, 600); //Cria uma janela de 800x600 pixels
    if (!janela) apontaErro("Não foi possível inicializar a janela");
    if (!al_init_primitives_addon()) apontaErro("Falha ao inicializar addon de primitivas.\n");
    return janela;
}

//Função para fazer o carregamento de uma fonte em um determinado tamanho
ALLEGRO_FONT* carregarFonte(char* arqFonte, int tamFonte){
    ALLEGRO_FONT* fonte = NULL;
    al_init_font_addon();
    al_init_ttf_addon();
    fonte = al_load_ttf_font(arqFonte, tamFonte, 0);
    if (!fonte) apontaErro("Não foi possível inicializar a fonte.");
    return fonte;
}

//Registro de cores
typedef struct CORES{
    ALLEGRO_COLOR preto, branco, vermelho, verde, azul, amarelo, ciano, magenta, cinza, marrom, cinzaClaro, azulEscuro;
} Cores;

//Inicializando as cores com os respectivos códigos RGB e retornando
Cores inicializarCores() {
    Cores cores;
    cores.preto = al_map_rgb(0, 0, 0);
    cores.branco = al_map_rgb(255, 255, 255);
    cores.vermelho = al_map_rgb(255, 0, 0);
    cores.verde = al_map_rgb(0, 255, 0);
    cores.azul = al_map_rgb(0, 0, 255);
    cores.azulEscuro = al_map_rgb(0, 0, 150);
    cores.amarelo = al_map_rgb(200, 200, 50);
    cores.ciano = al_map_rgb(0, 255, 255);
    cores.magenta = al_map_rgb(255, 0, 255);
    cores.cinza = al_map_rgb(128, 128, 128);
    cores.marrom = al_map_rgb(139, 69, 19);
    cores.cinzaClaro = al_map_rgb(192, 192, 192);
    return cores;
}

//Definindo os alinhamentos de texto
#define esquerda ALLEGRO_ALIGN_LEFT
#define direita ALLEGRO_ALIGN_RIGHT
#define centro ALLEGRO_ALIGN_CENTER

// Função para pedir as definições do usuário e inicializar o jogo
void pedirDefinicoes(Jogo* jogo){
    int maxLinhas = 21;
    int maxColunas = 33;
    int maxMinas;
    int input;
    do {
        printf("\n****CAMPO MINADO****\n\n");
        printf("DIGITE AS DEFINIÇÕES:\n");
        printf("- Número de linhas (até %d) = ", maxLinhas);
        scanf("%d", &jogo->numLinhas);
        if (jogo->numLinhas > maxLinhas) {
            printf("Número de linhas excede o limite permitido. Pressione Enter para continuar.\n");
            while ((input = getchar()) != '\n' && input != EOF); // Limpa o buffer de entrada
            getchar(); // Aguarda o Enter
            continue; // Volta para o início do loop
        }
        printf("\n- Número de colunas (até %d) = ", maxColunas);
        scanf("%d", &jogo->numColunas);
        if (jogo->numColunas > maxColunas) {
            printf("Número de colunas excede o limite permitido. Pressione Enter para continuar.\n");
            while ((input = getchar()) != '\n' && input != EOF); // Limpa o buffer de entrada
            getchar(); // Aguarda o Enter
            continue; // Volta para o início do loop
        }
        maxMinas = (jogo->numLinhas * jogo->numColunas) - 2; // Atualiza o limite de minas
        printf("\n- Número de minas (até %d) = ", maxMinas);
        scanf("%d", &jogo->numBombas);
        if (jogo->numBombas > maxMinas) {
            printf("Número de minas excede o limite permitido. Pressione Enter para continuar.\n");
            while ((input = getchar()) != '\n' && input != EOF); // Limpa o buffer de entrada
            getchar(); // Aguarda o Enter
        }
        jogo->numBandeiras = jogo->numBombas;
    } while (jogo->numLinhas > maxLinhas || jogo->numColunas > maxColunas || jogo->numBombas > maxMinas);
    jogo->matriz = alocarMatriz(jogo->numLinhas, jogo->numColunas);
}

//Função que desenha um retângulo preenchido com Allegro
void desenharRetangulo(int x, int y, int largura, int altura, ALLEGRO_COLOR cor) {
    al_draw_filled_rectangle(x, y, x + largura, y + altura, cor);
}

// Estrutura para armazenar informações de posicionamento do desenho da matriz
typedef struct Posicionamento {
    int tamanhoCelula;
    int espacamento;
    int margemSuperior;
    int tamanhoTotal;
    int larguraMatriz;
    int alturaMatriz;
    int posXInicial;
    int posYInicial;
} Posicionamento;

// Função para calcular as informações de posicionamento
Posicionamento calcularPosicionamento(int numLinhas, int numColunas) {
    Posicionamento pos;
    pos.tamanhoCelula = 20;
    pos.espacamento = 2;
    pos.margemSuperior = 90;
    // Calcula o tamanho total das células incluindo o espaçamento e a borda (mesmo tamanho do espaçamento)
    pos.tamanhoTotal = pos.tamanhoCelula + 2 * pos.espacamento;
    pos.larguraMatriz = numColunas * pos.tamanhoTotal;
    pos.alturaMatriz = numLinhas * pos.tamanhoTotal;
    // Calcula a posição X inicial para centralizar a matriz
    pos.posXInicial = (800 - pos.larguraMatriz) / 2; // 800 é a largura da tela
    // Calcula a posição Y inicial respeitando a margem superior
    pos.posYInicial = pos.margemSuperior;
    return pos;
}

// Função para desenhar a matriz de retângulos
void desenharMatrizRetangulos(Cores cores, int numLinhas, int numColunas) {
    Posicionamento pos = calcularPosicionamento(numLinhas, numColunas);
    // Desenha um quadrado branco que serve como borda e fundo
    desenharRetangulo(pos.posXInicial, pos.posYInicial, pos.larguraMatriz, pos.alturaMatriz, cores.branco);
    // Loop para desenhar as células da matriz em cinza
    for (int i = 0; i < numLinhas; i++) {
        for (int j = 0; j < numColunas; j++) {
            int x = pos.posXInicial + j * pos.tamanhoTotal + pos.espacamento; // Calcula a coordenada X com espaçamento
            int y = pos.posYInicial + i * pos.tamanhoTotal + pos.espacamento; // Calcula a coordenada Y com espaçamento
            desenharRetangulo(x, y, pos.tamanhoCelula, pos.tamanhoCelula, cores.cinza);
        }
    }
}

void inicializarMouse(ALLEGRO_EVENT_QUEUE **filaEventosMouse) {
    if (!al_install_mouse()) {
        apontaErro("Erro ao inicializar o mouse.\n");
    }
    *filaEventosMouse = al_create_event_queue();
    if (!(*filaEventosMouse)) {
        apontaErro("Erro ao criar a fila de eventos do mouse.\n");
    }
    al_register_event_source(*filaEventosMouse, al_get_mouse_event_source());
}

void inicializarModuloImagem() {
    if (!al_init_image_addon()) {
        apontaErro("Erro ao inicializar o addon de imagem da Allegro.\n");
    }
}

void gameOver(int linha, int coluna, Jogo* jogo){
    jogo->estado = false;
    jogo->vitoria = false;
    jogo->matriz[linha][coluna]->minaClicada = true;
    jogo->matriz[linha][coluna]->revelado = true;
    for(int i = 0; i < jogo->numLinhas; i++){
        for(int j = 0; j < jogo->numColunas; j++){
            if(jogo->matriz[i][j]->mina){
                jogo->matriz[i][j]->revelado = true;
            }
        }
    }
}

// Função para tratar o clique com o botão esquerdo
void cliqueBotaoEsquerdo(int linha, int coluna, Jogo* jogo, bool primeiroClique, Cores cores) {
    Ponto* pontoClicado = jogo->matriz[linha][coluna];
    if(primeiroClique){
        aleatorizarMinas(jogo, linha, coluna);
        calcularMinasVizinhas(jogo);
    }
    else if(pontoClicado->mina){
        gameOver(linha, coluna, jogo);
    }
    // Verifica se a casa clicada não foi revelada
    if (!pontoClicado->revelado) {
        revelarVizinhas(jogo, linha, coluna);
    }
}

// Função para tratar o clique com o botão direito
void cliqueBotaoDireito(int linha, int coluna, Jogo* jogo) {
    if(!jogo->matriz[linha][coluna]->bandeira && !jogo->matriz[linha][coluna]->interrogacao){
        jogo->matriz[linha][coluna]->bandeira = true;
        jogo->numBandeiras--;
        testarVitoria(jogo);
    }
    else if(jogo->matriz[linha][coluna]->bandeira){
        jogo->matriz[linha][coluna]->bandeira = false;
        jogo->matriz[linha][coluna]->interrogacao = true;
        jogo->numBandeiras++;
    }
    else{
        jogo->matriz[linha][coluna]->interrogacao = false;
    }
}

//Função para exibir uma imagem
void exibirImagem(const char* enderecoImagem, float x, float y) {
    ALLEGRO_BITMAP* imagem = al_load_bitmap(enderecoImagem);
    if (!imagem) {
        apontaErro("Erro ao carregar a imagem\n");
    }
    al_draw_bitmap(imagem, x, y, 0);
    al_destroy_bitmap(imagem);
}

//Função para colocar os ícones
void exibirIcones(Jogo* jogo, Cores cores) {
    Posicionamento pos = calcularPosicionamento(jogo->numLinhas, jogo->numColunas);
    for (int linha = 0; linha < jogo->numLinhas; linha++) {
        for (int coluna = 0; coluna < jogo->numColunas; coluna++) {
            Ponto* celula = jogo->matriz[linha][coluna];
            int x = pos.posXInicial + coluna * pos.tamanhoTotal + pos.espacamento; // Calcula a coordenada X com espaçamento
            int y = pos.posYInicial + linha * pos.tamanhoTotal + pos.espacamento; // Calcula a coordenada Y com espaçamento
            if (celula->mostra){
                desenharRetangulo(x, y, pos.tamanhoCelula, pos.tamanhoCelula, cores.amarelo);
            }
            else if (celula->bandeira) {
                exibirImagem("bandeira.png", x, y);
            }
            else if (celula->interrogacao) {
                exibirImagem("interrogação.png", x, y);
            }
            else if (celula->revelado) {
                if (celula->minaClicada){
                    exibirImagem("minaVermelho.png", x, y);
                }
                else if (celula->mina) {
                    exibirImagem("mina.png", x, y);
                }
                else if (celula->minasVizinhas > 0) {
                    desenharRetangulo(x, y, pos.tamanhoCelula, pos.tamanhoCelula, cores.cinzaClaro);
                    ALLEGRO_FONT* fonte = carregarFonte("arial.ttf", 17);
                    switch(celula->minasVizinhas){
                        case 1: al_draw_text(fonte, cores.azul, x+10, y, centro, "1");
                            break;
                        case 2: al_draw_text(fonte, cores.verde, x+10, y, centro, "2");
                            break;
                        case 3: al_draw_text(fonte, cores.vermelho, x+10, y, centro, "3");
                            break;
                        case 4: al_draw_text(fonte, cores.azulEscuro, x+10, y, centro, "4");
                            break;
                        case 5: al_draw_text(fonte, cores.marrom, x+10, y, centro, "5");
                            break;
                        case 6: al_draw_text(fonte, cores.ciano, x+10, y, centro, "6");
                            break;
                        case 7: al_draw_text(fonte, cores.preto, x+10, y, centro, "7");
                            break;
                        case 8: al_draw_text(fonte, cores.cinza, x+10, y, centro, "8");
                            break;
                    }
                    al_destroy_font(fonte);
                }
                else {
                    desenharRetangulo(x, y, pos.tamanhoCelula, pos.tamanhoCelula, cores.cinzaClaro);
                }
            }
            else{
                desenharRetangulo(x, y, pos.tamanhoCelula, pos.tamanhoCelula, cores.cinza); //Volta ao normal
            }
        }
    }
}

//Função para realçar células adjacentes com possíveis bombas
int realcarAdjacentes(Jogo* jogo, int linha, int coluna, bool mostrar, Cores cores) {
    Ponto* pontoAtual;
    int cont = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int novaLinha = linha + i;
            int novaColuna = coluna + j;
            // Verifica se as novas coordenadas estão dentro dos limites da matriz
            if (novaLinha >= 0 && novaLinha < jogo->numLinhas && novaColuna >= 0 && novaColuna < jogo->numColunas) {
                pontoAtual = jogo->matriz[novaLinha][novaColuna];
                // Verifica se a célula atual não foi revelada e não tem bandeira ou interrogação
                if (!pontoAtual->revelado && !pontoAtual->bandeira && !pontoAtual->interrogacao) {
                    // Define a célula atual como realçada
                    if (mostrar) pontoAtual->mostra = true;
                    if (!mostrar) pontoAtual->mostra = false;
                    cont++;
                }
            }
        }
    }
    exibirIcones(jogo, cores);
    al_flip_display();
    return cont;
}

//Função para quando o botão esquerdo é segurado
void segurarBotaoEsquerdo(int linha, int coluna, Jogo* jogo, Cores cores){
    if (jogo->matriz[linha][coluna]->minasVizinhas > 0 && jogo->matriz[linha][coluna]->revelado){
        realcarAdjacentes(jogo, linha, coluna, true, cores);
    }
}

//Função para quando o botão esquerdo é largado, para voltar ao normal
void largarBotaoEsquerdo(int linha, int coluna, Jogo* jogo, Cores cores){
    realcarAdjacentes(jogo, linha, coluna, false, cores);
}

//Função para desenhar a contagem de bandeiras e tempo na tela
void desenharContagens (Jogo* jogo, Cores cores){
    ALLEGRO_FONT* fonte = carregarFonte("digital-7.ttf", 55);
    desenharRetangulo(50, 10, 110, 55, cores.preto);
    desenharRetangulo(650, 10, 110, 55, cores.preto);
    char *bandeira;
    char *tempo;
    bandeira = (char *)malloc(20 * sizeof(char));
    tempo = (char *)malloc(20 * sizeof(char));
    int tempoTruncado = (int)ceil(jogo->tempo);
    sprintf(bandeira, "%d", jogo->numBandeiras);
    sprintf(tempo, "%d", tempoTruncado);
    al_draw_text(fonte, cores.vermelho, 105, 10, centro, bandeira);
    al_draw_text(fonte, cores.vermelho, 705, 10, centro, tempo);
    free(bandeira);
    free(tempo);
    al_destroy_font(fonte);
}

//Função para contar o número de bandeiras ao redor de uma célula
int contarBandeiras(Jogo* jogo, int i, int j){
    int cont = 0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            int novaLinha = i + x;
            int novaColuna = j + y;
            // Verifica se as novas coordenadas estão dentro dos limites da matriz
            if (novaLinha >= 0 && novaLinha < jogo->numLinhas && novaColuna >= 0 && novaColuna < jogo->numColunas) {
                Ponto* pontoAoRedor = jogo->matriz[novaLinha][novaColuna];
                if (pontoAoRedor->bandeira) cont++;
            }
        }
    }
    return cont;
}

//Função para contar o número de células adjacentes não reveladas
int contarAdjacentes(Jogo* jogo, int i, int j){
    int cont = 0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            int novaLinha = i + x;
            int novaColuna = j + y;
            // Verifica se as novas coordenadas estão dentro dos limites da matriz
            if (novaLinha >= 0 && novaLinha < jogo->numLinhas && novaColuna >= 0 && novaColuna < jogo->numColunas) {
                Ponto* pontoAoRedor = jogo->matriz[novaLinha][novaColuna];
                if (!pontoAoRedor->revelado && !pontoAoRedor->bandeira && !pontoAoRedor->interrogacao) cont++;
            }
        }
    }
    return cont;
}

// Função para tentar resolver sozinho
void tentarResolver(Jogo* jogo, bool primeiroClique, Cores cores){
    if (primeiroClique){
        srand((time(NULL) & 0xFFFF) | (getpid() << 16));
        int indice = rand() % (jogo->numLinhas * jogo->numColunas);
        int linha = indice / jogo->numColunas;
        int coluna = indice % jogo->numColunas;
        cliqueBotaoEsquerdo(linha, coluna, jogo, primeiroClique, cores);
    }
    else{
        for (int i = 0; i < jogo->numLinhas; i++){
            for (int j = 0; j < jogo->numColunas; j++){
                Ponto* ponto = jogo->matriz[i][j];
                if (ponto->revelado && ponto->minasVizinhas > 0){
                    int numPossiveisMinas = contarAdjacentes(jogo, i, j) + contarBandeiras(jogo, i, j);
                    if (numPossiveisMinas <= ponto->minasVizinhas){
                        for (int x = -1; x <= 1; x++) {
                            for (int y = -1; y <= 1; y++) {
                                int novaLinha = i + x;
                                int novaColuna = j + y;
                                if (novaLinha >= 0 && novaLinha < jogo->numLinhas && novaColuna >= 0 && novaColuna < jogo->numColunas) {
                                    Ponto* pontoAoRedor = jogo->matriz[novaLinha][novaColuna];
                                    if (!pontoAoRedor->revelado && !pontoAoRedor->bandeira){
                                        pontoAoRedor->bandeira = true;
                                        (jogo->numBandeiras)--;
                                        testarVitoria(jogo);
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < jogo->numLinhas; i++){
            for (int j = 0; j < jogo->numColunas; j++){
                Ponto* ponto = jogo->matriz[i][j];
                if (ponto->revelado && ponto->minasVizinhas > 0){
                    int numBandeiras = contarBandeiras(jogo, i, j);
                    if (numBandeiras == ponto->minasVizinhas){
                        for (int x = -1; x <= 1; x++) {
                            for (int y = -1; y <= 1; y++) {
                                int novaLinha = i + x;
                                int novaColuna = j + y;
                                if (novaLinha >= 0 && novaLinha < jogo->numLinhas && novaColuna >= 0 && novaColuna < jogo->numColunas) {
                                    Ponto* pontoAoRedor = jogo->matriz[novaLinha][novaColuna];
                                    if (!pontoAoRedor->revelado && !pontoAoRedor->bandeira){
                                        cliqueBotaoEsquerdo(novaLinha, novaColuna, jogo, primeiroClique, cores);
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        bool temOpcao = false;
        for (int i = 0; i < jogo->numLinhas; i++){
            for (int j = 0; j < jogo->numColunas; j++){
                Ponto* ponto = jogo->matriz[i][j];
                if (!ponto->revelado && !ponto->bandeira && !ponto->interrogacao) temOpcao = true;
            }
        }
        if (temOpcao){
            Ponto* ponto;
            int linha;
            int coluna;
            srand((time(NULL) & 0xFFFF) | (getpid() << 16));
            do {
                int indice = rand() % (jogo->numLinhas * jogo->numColunas);
                linha = indice / jogo->numColunas;
                coluna = indice % jogo->numColunas;
                ponto = jogo->matriz[linha][coluna];
            } while (ponto->revelado || ponto->bandeira || ponto->interrogacao);
            cliqueBotaoEsquerdo(linha, coluna, jogo, primeiroClique, cores);
        }
    }
}

// Função para tratar os eventos do mouse
bool tratarEventosMouse(ALLEGRO_EVENT_QUEUE *filaEventosMouse, Jogo* jogo, bool primeiroClique, Cores cores, double tempoInicial, bool* jogando) {
    Posicionamento pos = calcularPosicionamento(jogo->numLinhas, jogo->numColunas);
    int tamanhoCelula = pos.tamanhoCelula;
    int tamanhoTotal = pos.tamanhoTotal;
    int posXInicial = pos.posXInicial;
    int posYInicial = pos.posYInicial;
    int larguraMatriz = pos.larguraMatriz;
    int alturaMatriz = pos.alturaMatriz;
    int espacamento = pos.espacamento;
    ALLEGRO_TIMER* timer = NULL;
    bool mouseSegurado = false;
    bool controle = true;
    int linhaClicada;
    int colunaClicada;
    double tempoAtual = 0;
    while (true) {
        ALLEGRO_EVENT evento;
        if(al_wait_for_event_timed(filaEventosMouse, &evento, 0.049)){
            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                // O usuário clicou no botão "X" da janela, encerra o jogo
                *jogando = false; // Encerra o jogo
                return false;
            }
            else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && !timer) {
                int mouseX = evento.mouse.x;
                int mouseY = evento.mouse.y;
                //Tratamento do botão de tentar resolver por máquina:
                if (mouseX >= 200 && mouseX <= 602 && mouseY >= 55 && mouseY <= 80){
                    tentarResolver(jogo, primeiroClique, cores);
                    return true;
                }
                // Verifica se o clique está dentro da área da matriz
                else if (mouseX >= posXInicial && mouseX < posXInicial + larguraMatriz &&
                mouseY >= posYInicial && mouseY < posYInicial + alturaMatriz) {
                    colunaClicada = (mouseX - posXInicial) / tamanhoTotal;
                    linhaClicada = (mouseY - posYInicial) / tamanhoTotal;
                    // Calcula as coordenadas do quadrado cinza clicado
                    int xQuadradoClicado = posXInicial + colunaClicada * tamanhoTotal + espacamento;
                    int yQuadradoClicado = posYInicial + linhaClicada * tamanhoTotal + espacamento;
                    if (mouseX >= xQuadradoClicado && mouseX < xQuadradoClicado + tamanhoCelula &&
                        mouseY >= yQuadradoClicado && mouseY < yQuadradoClicado + tamanhoCelula) {
                        if (evento.mouse.button & 1) { // Botão esquerdo do mouse
                            // Inicializa o temporizador para detectar a duração do clique
                            if (!timer) {
                                timer = al_create_timer(0.05); // Frequência desejada
                                al_register_event_source(filaEventosMouse, al_get_timer_event_source(timer));
                                al_start_timer(timer);
                                mouseSegurado = false;
                            }
                        } else if (evento.mouse.button & 2 && !jogo->matriz[linhaClicada][colunaClicada]->revelado) { // Botão direito do mouse
                            cliqueBotaoDireito(linhaClicada, colunaClicada, jogo);
                            return false;
                        }
                    }
                }
            } else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.button & 1) { // Botão esquerdo do mouse liberado
                    if (timer) { // Desliga o temporizador e trata o evento com base na duração do clique
                        al_stop_timer(timer);
                        al_destroy_timer(timer);
                        timer = NULL;
                        if (mouseSegurado && !primeiroClique && jogo->matriz[linhaClicada][colunaClicada]->revelado) {
                            // O botão esquerdo do mouse foi largado após ser mantido pressionado
                            largarBotaoEsquerdo(linhaClicada, colunaClicada, jogo, cores);
                            return false;
                        } else if (!jogo->matriz[linhaClicada][colunaClicada]->bandeira && !jogo->matriz[linhaClicada][colunaClicada]->interrogacao){
                            // O botão esquerdo do mouse foi clicado rapidamente
                            cliqueBotaoEsquerdo(linhaClicada, colunaClicada, jogo, primeiroClique, cores);
                            return true;
                        }
                    }
                }
            } else if (mouseSegurado && controle && !primeiroClique && jogo->matriz[linhaClicada][colunaClicada]->revelado){ //Se segurou o botão esquerdo do mouse
                controle = false;
                segurarBotaoEsquerdo(linhaClicada, colunaClicada, jogo, cores);
            } else if (evento.type == ALLEGRO_EVENT_TIMER) {
                if (timer) {
                    // Temporizador ativo, o botão esquerdo do mouse ainda está pressionado
                    mouseSegurado = true;
                }
            }
        }
        else if (!primeiroClique){
            tempoAtual = al_get_time();
            jogo->tempo = tempoAtual - tempoInicial;
            desenharContagens(jogo, cores);
            al_flip_display(); //Atualiza a tela
        }
    }
}

// Função para desenhar um botão
void desenharBotao(Cores cores, ALLEGRO_FONT* fonte, const char* texto, int x, int y, ALLEGRO_COLOR cor) {
    desenharRetangulo(x, y, 170, 40, cor);
    al_draw_text(fonte, cores.branco, x + 80, y + 10, centro, texto);
}

// Função para verificar se o mouse clicou em um botão
bool cliqueNoBotao(int mouseX, int mouseY, int x, int y) {
    return (mouseX >= x && mouseX <= x + 170 && mouseY >= y && mouseY <= y + 40);
}

// Função para reinicializar o jogo
void reinicializarJogo(Jogo* jogo) {
    // Liberar a matriz atual
    liberarMatriz(jogo->matriz, jogo->numLinhas, jogo->numColunas);
    // Recriar a matriz com as configurações iniciais
    jogo->matriz = alocarMatriz(jogo->numLinhas, jogo->numColunas);
    // Reinicializar todas as outras variáveis do jogo
    jogo->numBandeiras = jogo->numBombas;
    jogo->tempo = 0.0;
    jogo->estado = true;
    jogo->vitoria = false;
}

// Função para a tela final
void telaFinal(Jogo* jogo, ALLEGRO_EVENT_QUEUE *filaEventosMouse, Cores cores, bool* jogando) {
    al_flip_display();
    ALLEGRO_FONT* fonte = carregarFonte("arial.ttf", 50);
    if (jogo->vitoria)
        al_draw_text(fonte, cores.verde, 400, 300, centro, "VOCÊ VENCEU!");
    else
        al_draw_text(fonte, cores.vermelho, 400, 300, centro, "GAME OVER!");
    al_destroy_font(fonte);
    fonte = carregarFonte("arial.ttf", 20);
    desenharBotao(cores, fonte, "Jogar novamente", 200, 500, cores.verde);
    desenharBotao(cores, fonte, "Sair do jogo", 400, 500, cores.vermelho);
    al_destroy_font(fonte);
    // Loop para esperar o jogador clicar em um botão ou fechar a janela
    bool botaoClicado = false;
    while (!botaoClicado && *jogando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(filaEventosMouse, &evento);
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mouseX = evento.mouse.x;
            int mouseY = evento.mouse.y;
            if (cliqueNoBotao(mouseX, mouseY, 200, 500)) {
                // O jogador clicou em "Jogar Novamente"
                botaoClicado = true;
                *jogando = true; // Mantém o jogo ativo
            } else if (cliqueNoBotao(mouseX, mouseY, 400, 500) || evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                // O jogador clicou em "Sair" ou fechou a janela (clicou no botão "X")
                botaoClicado = true;
                *jogando = false; // Encerra o jogo
            }
        }
        al_flip_display();
    }
}

// Função para a tela de jogo
ALLEGRO_EVENT_QUEUE* telaJogo(Cores cores, Jogo* jogo, bool* jogando) {
    ALLEGRO_EVENT_QUEUE *filaEventosMouse = NULL;
    inicializarMouse(&filaEventosMouse);
    inicializarModuloImagem();
    ALLEGRO_FONT* fonte = carregarFonte("arial.ttf", 50);
    al_clear_to_color(cores.cinza);
    al_draw_text(fonte, cores.branco, 400, 0, centro, "CAMPO MINADO");
    al_destroy_font(fonte);
    fonte = carregarFonte("arial.ttf", 18);
    desenharRetangulo(200, 55, 402, 25, cores.azulEscuro);
    al_draw_text(fonte, cores.branco, 400, 55, centro, "Tentar resolver automaticamente por máquina");
    al_draw_text(fonte, cores.vermelho, 93, 68, centro, "Número de ");
    al_draw_text(fonte, cores.vermelho, 703, 68, centro, "Tempo decorrido");
    exibirImagem("bandeira.png", 142, 68);
    al_destroy_font(fonte);
    desenharMatrizRetangulos(cores, jogo->numLinhas, jogo->numColunas);
    bool primeiroClique = true;
    double tempoInicial = 0;
    while (jogo->estado && *jogando) {
        desenharContagens(jogo, cores);
        al_flip_display(); //Atualiza a tela
        bool botaoEsq = tratarEventosMouse(filaEventosMouse, jogo, primeiroClique, cores, tempoInicial, jogando);
        if (primeiroClique && botaoEsq){
            primeiroClique = false;
            tempoInicial = al_get_time();
        }
        exibirIcones(jogo, cores);
    }
    // Quando o jogo terminar, chama a tela final
    telaFinal(jogo, filaEventosMouse, cores, jogando);
    return filaEventosMouse;
}

int main() {
    #ifdef _WIN32
    //Código específico do Windows
    SetConsoleOutputCP(65001);
    #endif
    Cores cores = inicializarCores();
    Jogo jogo;
    jogo.estado = true;
    jogo.tempo = 0.0;
    pedirDefinicoes(&jogo);
    ALLEGRO_DISPLAY* janela = inicializarJanela();
    ALLEGRO_EVENT_QUEUE* filaEventosMouse;
    bool jogando = true;
    while (jogando) { //Loop principal do jogo
        al_flip_display();
        filaEventosMouse = telaJogo(cores, &jogo, &jogando); // Chama a tela do jogo
        if (jogando) {
            // Reinicializa o jogo se o jogador escolher "Jogar Novamente"
            reinicializarJogo(&jogo);
        }
    }
    al_destroy_event_queue(filaEventosMouse);
    al_shutdown_image_addon();
    al_uninstall_mouse();
    al_destroy_display(janela);
    al_shutdown_primitives_addon();
    al_uninstall_system();
    liberarMatriz(jogo.matriz, jogo.numLinhas, jogo.numColunas);
    return 0;
}