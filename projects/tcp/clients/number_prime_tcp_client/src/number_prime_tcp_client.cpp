#include "number_prime_tcp_client.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Для Windows

void exit_handler();
SOCKET client_socket;

int main(int argc, char* argv[]) {
    atexit(common_exit_handler);
    atexit(exit_handler);

    short port;
    char host[128] = "";
    bool parse_cmd_result = parse_cmd(argc, argv, host, &port);

    if (!parse_cmd_result || !host || !strlen(host)) {
        printf("Invalid host or port. Usage %s -h host -p port\n", argv[0]);
        return -1;
    }

    common_init_handler();

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket <= 0) {
        error_msg("Can't create socket");
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    resolve_addr(host, &server_addr.sin_addr);

    // Підключення до сервера
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(sockaddr))) {
        char err_msg[128] = "";
        sprintf(err_msg, "Can't connect to the server %s:%d", host, port);
        error_msg(err_msg);
        return -1;
    }

    printf("Connection to the server %s:%d success\n", host, port);

    char control = '\0';

    do {
        double a, b, c;
        printf("%s", "Enter coefficient a: ");
        if (scanf("%lf", &a) != 1) {
            printf("Invalid input\n");
            return -1;
        }
        printf("%s", "Enter coefficient b: ");
        if (scanf("%lf", &b) != 1) {
            printf("Invalid input\n");
            return -1;
        }
        printf("%s", "Enter coefficient c: ");
        if (scanf("%lf", &c) != 1) {
            printf("Invalid input\n");
            return -1;
        }

        // Відправка коефіцієнтів на сервер
        double coefficients[3] = { a, b, c };
        int sc = send(client_socket, (char*)coefficients, sizeof(coefficients), 0);
        if (sc <= 0) {
            char err_msg[128] = "";
            sprintf(err_msg, "Can't send data to the server %s:%d", host, port);
            error_msg(err_msg);
            return -1;
        }

        // Отримання відповіді від сервера
        char resp[256] = "";
        sc = recv(client_socket, resp, sizeof(resp), 0);
        if (sc <= 0) {
            printf("Server is not able\n");
            return -1;
        }

        printf("Server response: %s\n", resp);

        // Запит на повторення
        printf("\nDo you want to break? (Y/Any): ");
        getchar();  // щоб зчитати зайвий символ нового рядка
        control = getchar();

    } while (toupper(control) != 'Y');

    printf("Bye!\n");

    return 0;
}

void exit_handler() {
    close_socket(client_socket);
}
