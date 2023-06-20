// created by CSba7822 on 2021/10/18 exc04 task 3

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>


// ich gehe in meinem Fall davon aus, dass die Inputnamen nicht zu lange sind
const size_t CLIENT_NAME_MAX_LENGTH = 50;
const char* PIPE_DIR = "/tmp/math_client_";

int main(int argc, char** argv) {
    // Wenn kein clientname angegeben wird, kann das Programm nicht funktionieren
    if (argc != 2) {
        fprintf(stderr, "Eingabe der folgenden Form: './math_client <name>'");
        return EXIT_FAILURE;
    }
    // Die namend pipe muss logeischerweise gleich heißen wie in Server/Client weil sonst kann es keine verbindung anlegen
    char* fifo_path = malloc(strlen(PIPE_DIR) + CLIENT_NAME_MAX_LENGTH + 1);
    if (fifo_path == NULL) {
        fprintf(stderr, "Kein speicher konnte alloziert werden");
        return EXIT_FAILURE;
    }
    strcpy(fifo_path, PIPE_DIR);
    strcat(fifo_path, argv[1]);
    // fifo wird mit write only aufgerufen
    int fifo_fd = open(fifo_path, O_WRONLY);
    if (fifo_fd < 0) {
        fprintf(stderr, "Fifo konnte nicht gefunden werden");
        free(fifo_path);
        return EXIT_FAILURE;
    }

    char expression[PIPE_BUF];
    while (1) {
        printf("Expression:\n");
        // fgets holt die Eingabe vom STDIN und schreibt sie in expression
        fgets(expression, PIPE_BUF, stdin);
        // Abbruchbedingung, wenn der User nur einen Zeilenumbruch eingibt, beendet sich das Programm
        if (expression[0] == '\n') {
            break;
        }
        // im Array muss an der letzten Stelle noch die '\0' eingefügt werden
        expression[strlen(expression) - 1] = '\0';
        // der Userinput wird in die Pipe geschrieben, und sollte ein Fehler aufgetreten sein, wird die Pipe geschlossen und das Programm beendet
        if (write(fifo_fd, expression, strlen(expression) + 1) == -1) {
            printf("Schreiben in die Pipe fehlgeschlagen");
            free(fifo_path);
            close(fifo_fd);
            return EXIT_FAILURE;
        };
    }
    // aller allozierter Speicher muss wieder freigegeben werden
    free(fifo_path);
    close(fifo_fd);
    return EXIT_SUCCESS;
}