#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024


int client_sockets[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int server_socket;


void signal_handler(int sig) {
    printf("\nServidor sendo encerrado...\n");
    
    
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            close(client_sockets[i]);
            client_sockets[i] = 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
    
    close(server_socket);
    
    
    pthread_mutex_destroy(&clients_mutex);
    
    exit(0);
}


void broadcast_message(char *message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int sock = client_sockets[i];
        if (sock != 0 && sock != sender_socket) {
            if (send(sock, message, strlen(message), MSG_NOSIGNAL) < 0) {
                
                printf("Cliente desconectado durante broadcast\n");
                close(sock);
                client_sockets[i] = 0;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


void *handle_client(void *arg) {
    int client_socket = *((int *) arg);
    free(arg);
    char buffer[BUFFER_SIZE];
    int read_size;

    
    while ((read_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[read_size] = '\0';
        
        
        if (buffer[read_size - 1] == '\n') {
            buffer[read_size - 1] = '\0';
        }
        
        printf("Mensagem recebida: %s\n", buffer);
        
        
        char formatted_message[BUFFER_SIZE + 1];
        snprintf(formatted_message, sizeof(formatted_message), "%s\n", buffer);
        
        
        broadcast_message(formatted_message, client_socket);
    }

    if (read_size == 0) {
        printf("Cliente desconectado.\n");
    } else if (read_size == -1) {
        perror("Erro ao receber mensagem");
    }

    
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(client_socket);
    printf("Thread do cliente finalizada.\n");
    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;
    int opt = 1;

    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket criado com sucesso.\n");

    
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Erro ao configurar SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }

    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro no bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Bind realizado com sucesso.\n");

    
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Erro no listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Aguardando conexões na porta %d...\n", PORT);

    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) > 0) {
        printf("Conexão aceita de %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        
        pthread_mutex_lock(&clients_mutex);
        int added = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = client_socket;
                added = 1;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        if (!added) {
            char *message = "Servidor cheio. Tente novamente mais tarde.\n";
            send(client_socket, message, strlen(message), 0);
            close(client_socket);
            printf("Conexão rejeitada - servidor cheio.\n");
            continue;
        }

        
        int *new_sock = malloc(sizeof(int));
        if (new_sock == NULL) {
            perror("Erro ao alocar memória");
            
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == client_socket) {
                    client_sockets[i] = 0;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            close(client_socket);
            continue;
        }
        
        *new_sock = client_socket;
        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_sock) < 0) {
            perror("Erro ao criar thread");
            free(new_sock);
            
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == client_socket) {
                    client_sockets[i] = 0;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            close(client_socket);
            continue;
        }
        
        
        pthread_detach(thread_id);
    }

    if (client_socket < 0) {
        perror("Erro ao aceitar conexão");
    }

    
    close(server_socket);
    pthread_mutex_destroy(&clients_mutex);
    return 0;
}
