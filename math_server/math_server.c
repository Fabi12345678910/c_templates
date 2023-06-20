// created by CSba7822 on 2021/10/18 exc04 task 3

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>

// ich gehe in meinem Fall davon aus, dass die Inputnamen nicht zu lange sind
const size_t CLIENT_NAME_MAX_LENGTH = 50;
const char* PIPE_DIR = "/tmp/math_client_";

typedef struct Client {
    char* fifo_path;
    char* name;
    int fd;
} Client;


void free_connections(int amount_connections, Client* client_list) {
    if (client_list != NULL) {
        for (int i = 0; i < amount_connections; ++i) {
            if (client_list[i].fd >= 0) {
                close(client_list[i].fd);
            }
            if (client_list[i].name != NULL) {
                free(client_list[i].name);
            }
            if (client_list[i].fifo_path != NULL) {
                // pipe muss noch unlinked werden, bevor der Speicherplatz freigegeben wird
                unlink(client_list[i].fifo_path);
                free(client_list[i].fifo_path);
            }
        }
        free(client_list);
    }
}

Client* generate_connections(int amount_of_clients, char** names) {
    // erstellt Speicherplatz für alle Benutzer, die in argv sind
    Client* new_clients = malloc(sizeof(*new_clients) * amount_of_clients);
    // Fehlermeldung
    if (new_clients == NULL) {
        fprintf(stderr, "Kann keinen speicher für die Klienten allozieren");
        // Wenn 1 Benutzer keine Pipe anlegen kann, werden alle Benutzer gefreet und das Programm beendet
        free_connections(amount_of_clients, new_clients);
        exit(EXIT_FAILURE);
    }
    // erstellt für jedes argv-Element einen nutzer
    for (int i = 0; i < amount_of_clients; ++i) {
        // Path ist immer PIPE_DIR ++ Clientname
        new_clients[i].fifo_path = malloc(strlen(PIPE_DIR) + CLIENT_NAME_MAX_LENGTH);
        if (new_clients[i].fifo_path == NULL) {
            fprintf(stderr, "Kann keinen speicher für die Klienten allozieren");
            free_connections(amount_of_clients, new_clients);
            exit(EXIT_FAILURE);
        }
        // kopiert und konkateniert die Strings in .fifo_path
        strcpy(new_clients[i].fifo_path, PIPE_DIR);
        strcat(new_clients[i].fifo_path, names[i]);
        // erstellt neue named pipe; 0666 alle erhalten lese und schreibzugriff; name der pipe == path
        if (mkfifo(new_clients[i].fifo_path, 0666) == -1) {
            fprintf(stderr, "Kann keine Pipe erstellen");
            free_connections(amount_of_clients, new_clients);
            exit(EXIT_FAILURE);
        }
        // wir öffnen die gerade erstellte pipe mit readonly, weil das Programm nur die Inputs der Clients beachten muss
        new_clients[i].fd = open(new_clients[i].fifo_path, O_RDONLY);
        if (new_clients[i].fd == -1) {
            fprintf(stderr, "Kann keine Pipe erstellen");
            free_connections(amount_of_clients, new_clients);
            exit(EXIT_FAILURE);
        }
        // wenn Benutzer erstellt wird das mit connected signalisiert
        printf("%s connected!\n\n", names[i]);

        new_clients[i].name = malloc(sizeof(char*) * CLIENT_NAME_MAX_LENGTH + 1);
        if (new_clients[i].name == NULL) {
            fprintf(stderr, "Kann keinen speicher für die Klienten allozieren");
            free_connections(amount_of_clients, new_clients);
            exit(EXIT_FAILURE);
        }
        // Wir müssen dem struct noch den namen des Clients zuweisen
        strcpy(new_clients[i].name, names[i]);
    }
    return new_clients;
}

// -1 error; 0 ok
int calculation(double* result, char* expression) {
    char* endpointer;
    // die erste übergebene Zahl wird eingelesen
    double first_value = strtod(expression, &endpointer);
    if (endpointer == expression) {
        return -1;
    }
    // Kontrolliert ob eine weiter Eingaben(Rechenzeichen + 2.te Zahl vorhanden sind) vorhanden ist 
    if (expression + strlen(expression) - endpointer < 4) {
    return -1;
    }
    // wir überspringen zwei leerzeichen und ein Rechenzeichen, um bei der nächsten Zahl zu landen
    char* endpointer2 = endpointer + 3;
    double secound_value = strtod(endpointer2, &endpointer);
    // Fehler, dass keine Zahl vorhanden ist
    if (endpointer == endpointer2) return -1;
    // der endpointer2 wird um 2 vermindert, weil ein Leerzeichen dazwischen ist
    switch (*(endpointer2 - 2)) {
    case '+':
        *result = first_value + secound_value;
        return 0;
    case '-':
        *result = first_value - secound_value;
        return 0;
    case '*':
        *result = first_value * secound_value;
        return 0;
    case '/':
        *result = first_value / secound_value;
        return 0;
    default:
    }
    return -1;
}


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Bitte Namen von Klienten angeben: './math_server <name1> <name2> ...'\n");
        return EXIT_FAILURE;
    }
    // argc wird um 1 erniedrigt und argv um 1 erhöht, weil sonst der Dateiname drinn wäre
    Client* connections = generate_connections(argc - 1, argv + 1);
    // Poll = wartet auf einen fd, um eine Eingabe/Ausgabe-Aktion durchzuführen
    // pollstruct = struct pollfd {int   fd; /* file descriptor */ short events; /* requested events */ short revents;    /* returned events */
    // events == 0 heißt kann nur die Aktionen POLLHUP, POLLERR, and POLLNVAL retrunen in revents

    struct pollfd* pollfd = malloc((argc - 1) * sizeof(struct pollfd));
    // Fehlerbehandlung
    if (pollfd == NULL) {
        free_connections(argc - 1, connections);
        fprintf(stderr, "Kann keinen Speicher für poll allozieren");
        return EXIT_FAILURE;
    }
    // POLLIN == Daten müssen gelesen werden, und jeder Clienten fd wird ein pollfd zugewiesen
    for (int i = 0; i < argc - 1; ++i) {
        pollfd[i].events = POLLIN;
        pollfd[i].fd = connections[i].fd;
    }
    // PIPE_BUF vordefinierte konstante
    char* expression = malloc(PIPE_BUF * sizeof(*expression));
    bool server_cont = true;
    // äußer while-schleife checked ob noch ein Benutzer verbunden ist
    while (server_cont) {
        server_cont = false;
        for (int i = 0; i < argc - 1; ++i) {
            // Diese Bedingung ist nur wahr, wenn der Client disconnected ist
            if (connections[i].fd < 0){
                continue;
            } 
            // wenn eine fd nicht auf -1 gesetzt wurde(zumindest 1 Client noch nicht disconnect hat), wird server_cont(inue) auf true gesetzt und der server läuft weiter
            server_cont = true;
            // int poll(struct pollfd *fds, nfds_t nfds, int timeout); poll wartet auf eine fd um eine I/O action auszuführen
            // argc - 1 länge von pollfd; 0 für die Millisekunden das Signal blockt
            if (poll(pollfd, argc - 1, 0) == -1) {
                free(pollfd);
                free_connections(argc - 1, connections);
                fprintf(stderr, "Poll kann nicht ausgeführt werden\n");
                return EXIT_FAILURE;
            };

            for (int i = 0; i < argc - 1; ++i) {
                // POLLHUP impliziert, dasss die Pipe vom Client geschlossen wurde, => Client disconnectete
                if (pollfd[i].revents == POLLHUP) {
                    printf("%s disconnected.\n\n", connections[i].name);
                    close(connections[i].fd);
                    // wenn connection geschlossen wurde, wird der file descriptor auf -1 gesetzt
                    connections[i].fd = -1;
                    pollfd[i].fd = -1;
                    break;
                }
                // POLLIN signalisiert, das neue Daten sind vorhanden in der Pipe
                if (pollfd[i].revents == POLLIN) {
                    // revents wird wieder auf 0 gesetzt, um zu verhindern, das der Input öfter gelesen wird
                    pollfd[i].revents = 0;
                    
                    if (read(pollfd[i].fd, expression, PIPE_BUF) == -1) {
                        printf("Aus der Pipe konnte nicht gelesen werden");
                        free(pollfd);
                        free(expression);
                        free_connections(argc - 1, connections);
                        return EXIT_FAILURE;
                    };
                    // führt die eigentliche Berechnung aus
                    double result;
                    if (!calculation(&result, expression)) {
                        printf("%s: %s = %g\n\n", connections[i].name, expression, result);
                    }
                    else {
                        printf("%s: %s is malformed.\n\n", connections[i].name, expression);
                    }
                    // der Buffer wird wieder mit der Nullterminierung gefüllt, damit es zu keinem Problem kommt, dass eine Nachricht 2x ausgelesen wird
                    for (int i = 0; i < PIPE_BUF; ++i) {
                        expression[i] = '\0';
                    }
                }
            }
        }
    }
    // zum Schluss muss noch der ganze Speicherplatz gefreet werden
    free(pollfd);
    free(expression);
    free_connections(argc - 1, connections);
    return EXIT_SUCCESS;
}