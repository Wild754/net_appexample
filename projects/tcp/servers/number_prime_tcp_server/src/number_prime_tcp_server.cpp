#include "number_prime_tcp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Для Windows

// Функція для обчислення коренів квадратного рівняння
void solve_quadratic(double a, double b, double c, char* result) {
    double discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        double x1 = (-b + sqrt(discriminant)) / (2 * a);
        double x2 = (-b - sqrt(discriminant)) / (2 * a);
        sprintf(result, "Two roots: x1 = %.2f, x2 = %.2f", x1, x2);
    }
    else if (discriminant == 0) {
        double x = -b / (2 * a);
        sprintf(result, "One root: x = %.2f", x);
    }
    else {
        sprintf(result, "No real roots");
    }
}

void handle_connection(SOCKET socket, sockaddr_in* addr) {
    char* str_in_addr = inet_ntoa(addr->sin_addr);
    printf("[%s]>>%s\n", str_in_addr, "Established new connection");

    while (1) {
        double coefficients[3]; // Для зберігання коефіцієнтів a, b, c
        int rc = recv(socket, (char*)coefficients, sizeof(coefficients), 0); // Отримання коефіцієнтів

        if (rc > 0) {
            printf("[%s]: Received coefficients: a = %.2f, b = %.2f, c = %.2f\n",
                str_in_addr, coefficients[0], coefficients[1], coefficients[2]);

            char response[256] = "";
            solve_quadratic(coefficients[0], coefficients[1], coefficients[2], response); // Обчислення розв'язку

            // Надсилання результату клієнту
            rc = send(socket, response, strlen(response), 0);
            if (rc <= 0) {
                printf("Can't send response to client %s\n", str_in_addr);
                break;
            }
        }
        else {
            break;
        }
    }

    closesocket(socket); // Закриття сокета
    printf("[%s]>>%s\n", str_in_addr, "Closed incoming connection");
}

