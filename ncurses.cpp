#include <ncurses.h>
#include <string>

int main() {
    std::string inputText;
    int ch; // Para armazenar o caractere lido
    int linha = 2;
    
    // Inicialização do ncurses
    initscr(); // Inicializa a tela do ncurses
    start_color(); // Start
    raw();
    cbreak(); // Desabilita o buffer de linha, permitindo entrada imediata
    noecho(); // Desabilita a exibição automática de entrada do teclado
    keypad(stdscr, TRUE); // Permite a leitura de teclas de função, setas, etc.

    // Limpa a tela atual
    clear();

    mvprintw(0, 0, "Inicializou o ncurses");
    refresh(); //

    // Laço principal do programa
    do{

        // Imprime a prompt e a entrada atual do usuário
        mvprintw(1, 0, "Digite o inicio da palavra: ");
        ch = getchar();
        mvprintw(linha, 0, "%c", ch);
        linha++;

        // Atualiza a tela para mostrar as mudanças
        refresh();

    } while(ch != '\n');

    // Finaliza o ncurses
    endwin();

    return 0;
}
