# Lab 2 - T1 | Leandro Brum da Silva Lacorte | Campo Minado

## Descrição:
Este trabalho consiste de um jogo clássico Campo Minado, programado totalmente em C com a biblioteca gráfica Allegro 5, feito para rodar tanto em Windows quanto em Linux. Ele possui todo o funcionamento padrão do jogo, utiliza o mouse para jogar, tem interface e comandos simples e uma IA que tenta resolver o jogo sozinha. Serviu para exercitar o conceito de alocação dinâmica de variáveis simples, vetores e matrizes de Laboratório de Programação II.

## Instalação da biblioteca gráfica:
Para poder rodar o código, é necessário ter instalado a biblioteca Allegro 5. Existem vários métodos para isso, como por terminal ou Code::Blocks, mas o mais simples é feito pelo Visual Studio Code. E desse último método que tentarei fazer um tutorial simples. Eu explicarei como fazer para Windows, mas o processo também pode ser feito no Linux, seguindo a mesma base, com as divergências do funcionamento do SO.

Pode-se usar também esse tutorial simples que segue a mesma base: https://www.youtube.com/watch?v=7mmJUahxEWk

Os arquivos utilizados nesse tutorial e página foram baixados dos sites oficiais das ferramentas: https://www.msys2.org/ e https://github.com/liballeg/allegro5

Primeiramente, é claro, é necessário ter instalado o Visual Studio Code. Vale frisar que também deve ser instalada a extensão de C/C++ para o VSCode. Após isso, basta seguir as etapas abaixo.

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
