# Lab 2 - T1 | Leandro Brum da Silva Lacorte | Campo Minado

## Descrição:
Este trabalho consiste de um jogo clássico Campo Minado, programado totalmente em C com a biblioteca gráfica Allegro 5, feito para rodar tanto em Windows quanto em Linux. Ele possui todo o funcionamento padrão do jogo, utiliza o mouse para jogar, tem interface e comandos simples e uma IA que tenta resolver o jogo sozinha. Serviu para exercitar o conceito de alocação dinâmica de variáveis simples, vetores e matrizes de Laboratório de Programação II.

## Instalação da biblioteca gráfica:
Para poder rodar o código, é necessário ter instalado a biblioteca Allegro 5. Existem vários métodos para isso, como por terminal ou Code::Blocks, mas o mais simples é feito pelo Visual Studio Code. E desse último método que tentarei fazer um tutorial simples. Eu explicarei como fazer para Windows, mas o processo também pode ser feito no Linux, seguindo a mesma base, com as divergências do funcionamento do SO.

Pode-se usar também esse tutorial simples que segue a mesma base: https://www.youtube.com/watch?v=7mmJUahxEWk

Os arquivos utilizados nesse tutorial e página foram baixados dos sites oficiais das ferramentas: https://www.msys2.org/ e https://github.com/liballeg/allegro5

Primeiramente, é claro, é necessário ter instalado o Visual Studio Code. Vale frisar que também deve ser instalada a extensão de C/C++ para o VSCode, bem como o GCC. Após isso, basta seguir as etapas abaixo.

### Instalação do MSYS2:
Caso não o tenha instalado, e necessário primeiramente instalar o MSYS2, para ter acesso ao MinGW, que são ferramentas GNU para Windows. Para verificar se não o tem instalado, basta procurar no caminho padrão, na raiz do disco, se há a pasta "msys64". Para instalá-lo, é só abrir o executável que deixei em "Allegro\msys2-x86_64-20230718.exe" e realizar os passos de instalação corretamente, escolhendo a pasta que preferir.

### Instalação do Allegro 5:
Após instalar o MSYS2, só falta instalar o Allegro. Para isso, siga os passos descritos abaixo em ordem:
  - Extraia o arquivo que deixei em "Allegro\allegro-x86_64-w64-mingw32-gcc-12.1.0-posix-seh-dynamic-5.2.8.0.zip" onde bem preferir;
  - Abra a pasta que foi extraída e copie os três diretórios dentro dela: "bin", "include" e "lib";
  - Navegue para o diretório onde você instalou o MSYS2 (por padrão na raiz do disco, na pasta "msys64"), entre na pasta "mingw64" e cole tudo o que copiou.

Feito isso, o Allegro deverá estar instalado e pronto para uso.

## Compilamento e execução do programa:
Após ter instalado o Allegro, basta abrir a pasta "Source" aqui presente no Visual Studio Code, com todos os arquivos dentro, e executar o seguinte comando no terminal:
```
gcc -Wall -o campoMinado campoMinado.c -lallegro_font -lallegro_color -lallegro_ttf -lallegro_image -lallegro_primitives -lallegro
```
Isso irá compilar o programa e gerar o arquivo "campoMinado.exe".
Após, basta executar esse arquivo gerado, com:
```
./campoMinado
```

## Implementação:
  - Todas as instruções no enunciado do trabalho foram realizadas;
  - Utilizei alocação dinâmica para as matrizes e também alguns vetores;
  - O programa é bem organizado em muitas funções, cada uma com seu objetivo;
  - O usuário pode escolher o tamanho da matriz e o número de minas, respeitando o limite de exibição da janela e a jogabilidade;
  - O tempo decorrido desde o primeiro clique que inicia o jogo é contado e exibido na tela;
  - Tem um botão para o computador tentar resolver o jogo de maneira inteligente. Fiz uma "IA" que faz uma análise de acordo com os números de minas e bandeiras ao redor de cada ponto da matriz, coloca bandeiras e faz jogadas, sendo que quase sempre ganha;
  - Ele conta e mostra na tela o número de bandeiras que faltam ser colocadas para preencher o número de minas do jogo;
  - Além disso, implementei uma funcionalidade muito útil do campo minado, para quando ficar segurando com o botão esquerdo do mouse em um número (que representa o número de minas adjacentes), as casas adjacentes onde é possível haver minas ficam destacadas, assim você pode analisar onde há conflito de posições que podem haver minas em casas diferentes, comparar umas com as outras, além de perceber caso o número de minas possível é igual ao número de minas adjacentes. Quando o botão esquerdo do mouse é solto, tudo volta ao normal, sendo apenas uma alteração visual para ajudar o jogador;
  - Há vários outros arquivos junto do principal (.c), que servem para exibir imagens PNG de ícones do jogo e utilizar fontes para escrever na tela;
  - Assim, além das imagens de minas para quando o usuário perde, também implementei ícones de bandeira e interrogação, que são ativos quando o jogador clica com o botão direito em uma casa não revelada, e servem para marcação. E também, quando o jogador clica em uma mina, além de mostrar onde estavam todas as outras, a que ele clicou fica com fundo vermelho;
  - A aleatorização das minas só ocorre após o primeiro clique, impossibilitando o usuário de clicar em uma mina na primeira jogada;
  - Foram utilizadas várias structs para organização melhor das váriaveis comuns. Tem structs para controlar jogo, controlar exibição do tabuleiro, controlar posicionamento...
  - Quando o jogador ganha ou perde, são apresentados dois botões, permitindo jogar novamente ou fechar o jogo;
  - Quando o jogador clica com o botão esquerdo em uma casa não revelada que não é mina, revelam-se todas as posições adjacentes que não são minas, e são reveladas também as posições adjacentes dessas posições que não são minas, fazendo uma recursão e construindo uma espécie de cercado de números;
  - O jogo é bem otimizado e ocorre sem travamentos;
  - O jogador pode pedir para a máquina tentar resolver automaticamente uma jogada a qualquer momento, sendo que ela faz a jogada sem trapaças e joga inteligentemente analisando os padrões de jogo como o jogador deveria;
  - É preciso marcar todas as minas com bandeira para vencer, além de revelar todas as casas sem minas.