/**
 * @author Marek Kovalčík, xkoval14@stud.fit.vutbr.cz
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define BUFSIZE 1024

/*
 * Pomocné proměnné
 */
bool isReflect = false;
bool isMeter   = false;

// hodnoty přejaté z argumentů
int port = 0;
char *host = NULL;
int velikostSondy = 0;
int dobaMereni = 0;

// pomocné proměnné pro kontrolu zadání povinných argumentů
int hostUsed = -1;
int portUsed = -1;
int sUsed = -1;
int tUsed = -1;

// proměnné pro měření
double prumernaPrenosRychlost  = 0.0;
double maximalniPrenosRychlost = 0.0;
double minimalniPrenosRychlost = 0.0;
double standardniOdchylka      = 0.0;
double prumernyRTTpaketu       = 0.0;

/*
 * Deklarace funkcí
 */
void printHelp();
bool checkArguments(int, char **);
void reflecting();
void measurementing();

/*
 * Funkce pro vypsání nápovědy
 */
void printHelp(){
    fprintf(stderr, "---------------------------------------------------\n");
    fprintf(stderr, "USAGE: reflect program: ./ipk-mtrip reflect -p PORT\n");
    fprintf(stderr, "\t meter program: ./ipk-mtrip meter -h HOST -p PORT -s SONDA -t TIME\n");
    fprintf(stderr, "\t\t-p PORT \t... port number\n");
    fprintf(stderr, "\t\t-h HOST \t... IP or Domain name of remote host\n");
    fprintf(stderr, "\t\t-s SONDA\t... amount of data in packet\n");
    fprintf(stderr, "\t\t-t TIME \t... overall time of measurement\n");
}

/*
 * Funkce pro kontrolu a parsování argumentů
 */
bool checkArguments(int argc, char **argv){
    int c;
    while ((c = getopt (argc, argv, ":h:p:s:t:")) != -1)
        switch (c){
            case 'h': hostUsed = (int)atol(optarg);
                host = optarg;
                break;
            case 'p': portUsed = (int)atol(optarg);
                if ((port = strtod(optarg, NULL)) <= 0) {
                    fprintf(stderr, "ERROR: Spatne zadane parametry. PORT musi byt kladne cislo > 0 a < 65536\n");
                    return false;
                }else if(port > 65535){
                    fprintf(stderr, "ERROR: Spatne zadane parametry. PORT musi byt kladne cislo > 0 a < 65536\n");
                    return false;
                }else
                    break;
            case 's': sUsed = (int)atol(optarg);
                if ((velikostSondy = strtod(optarg, NULL)) <= 0) {
                    fprintf(stderr, "ERROR: Spatne zadane parametry. Velikost sondy musi byt kladne cislo > 0 \n");
                    return false;
                }
                break;
            case 't': tUsed = (int)atol(optarg);
                if ((dobaMereni = strtod(optarg, NULL)) <= 0) {
                    fprintf(stderr, "ERROR: Spatne zadane parametry. Cas mereni musi byt kladne cislo > 0 \n");
                    return false;
                }
                break;
            case '?':
                fprintf(stderr, "ERROR: Neplatny prepinac\n");
                return false;
            default:
                break;
        }
    // Program je spuštěn jako reflektor
    if(argc == 4){
        if(strcmp(argv[3], "reflect") == 0){
            isReflect = true; isMeter = false;
            if (portUsed != -1)
                return true;
        }
        fprintf(stderr, "ERROR: Spatne zadane argumenty pro reflektor\n");
        return false;

    // Program je spuštěn jako meter
    }else if(argc == 10){
        if(strcmp(argv[9], "meter") == 0){
            isReflect = false; isMeter = true;
            if (hostUsed != -1)
                if (portUsed != -1)
                    if(sUsed != -1)
                        if(tUsed != -1)
                            return true;
        }
        fprintf(stderr, "ERROR: Spatne zadane argumenty pro meter\n");
        return false;

    // Program je spuštěn se špatnými parametry
    }else{
        fprintf(stderr, "ERROR: Spatne zadane argumenty\n");
        return false;
    }

    return true;
}

/*
 * Funkce reflektování (implementace serveru)
 */
void reflecting(){
    char buf[BUFSIZE];
    int server_socket, bytestx, bytesrx;
    socklen_t clientlen;
    struct sockaddr_in client_address, server_address;
    int optval;
    const char * hostaddrp;
    struct hostent *hostp;

    /* Vytvoreni soketu */
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }
    /* potlaceni defaultniho chovani rezervace portu ukonceni aplikace */
    optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    /* adresa serveru, potrebuje pro prirazeni pozadovaneho portu */
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)port);

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        perror("ERROR: binding");
        exit(EXIT_FAILURE);
    }

    while(1){
        printf("INFO: Ready.\n");
        /* prijeti odpovedi a jeji vypsani */
        clientlen = sizeof(client_address);
        bytesrx = recvfrom(server_socket, buf, BUFSIZE, 0, (struct sockaddr *) &client_address, &clientlen);
        if (bytesrx < 0)
            perror("ERROR: recvfrom:");

        hostp = gethostbyaddr((const char *)&client_address.sin_addr.s_addr, sizeof(client_address.sin_addr.s_addr), AF_INET);

        hostaddrp = inet_ntoa(client_address.sin_addr);
        printf("Message (%lu) from %s:  %s\n", strlen(buf), hostaddrp, buf);


        /* odeslani zpravy zpet klientovi  */
        bytestx = sendto(server_socket, buf, strlen(buf), 0, (struct sockaddr *) &client_address, clientlen);
        if (bytestx < 0)
            perror("ERROR: sendto:");
    }

}

/*
 * Funkce měření (implementace klienta)
 */
void measurementing(){
    int client_socket, bytestx, bytesrx;
    socklen_t serverlen;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buf[BUFSIZE];

    /* 2. ziskani adresy serveru pomoci DNS */

    if ((server = gethostbyname(host)) == NULL) {
        fprintf(stderr,"ERROR: no such host as %s\n", host);
        exit(EXIT_FAILURE);
    }

    /* 3. nalezeni IP adresy serveru a inicializace struktury server_address */
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port);

    /* tiskne informace o vzdalenem soketu */
    printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    /* Vytvoreni soketu */
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    /* nacteni zpravy od uzivatele */
    bzero(buf, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

    /* odeslani zpravy na server */
    serverlen = sizeof(server_address);
    bytestx = sendto(client_socket, buf, strlen(buf), 0, (struct sockaddr *) &server_address, serverlen);
    if (bytestx < 0)
        perror("ERROR: sendto");

    /* prijeti odpovedi a jeji vypsani */
    bytesrx = recvfrom(client_socket, buf, BUFSIZE, 0, (struct sockaddr *) &server_address, &serverlen);
    if (bytesrx < 0)
        perror("ERROR: recvfrom");
    printf("Echo from server: %s", buf);
}

/*
 * Hlavní funkce programu
 */
int main(int argc, char **argv) {

    // Kontrola argumentů
    if(checkArguments(argc, argv) != true) {
        printHelp();
        exit(EXIT_FAILURE);
    }

    // Program je spuštěn jako reflect
    if(isReflect == true) {
        printf("Program je spusten jako reflektor\n");
        printf("\tport: %d\n", port);
        reflecting();
    }

    // Program je spuštěn jako meter
    if(isMeter == true) {
        printf("Program je spusten jako meter\n");
        printf("\thost: %s | port: %d | sonda: %d | time: %d\n", host, port, velikostSondy, dobaMereni);
        measurementing();
    }

    // Výpis výsledků měření na stdout
    printf("Vysledky mereni ipk-mtrip\n-----------------------------\n");
    printf("Prumerna prenosova rychlost :\t%lf Mbit/s\n", prumernaPrenosRychlost);
    printf("Maximalni prenosova rychlost:\t%lf Mbit/s\n", maximalniPrenosRychlost);
    printf("Minimalni prenosova rychlost:\t%lf Mbit/s\n", minimalniPrenosRychlost);
    printf("Standardni odchylka         :\t%lf Mbit/s\n", standardniOdchylka);
    printf("Prumerny RTT paketu         :\t%lf Mbit/s\n", prumernyRTTpaketu);


    exit(EXIT_SUCCESS);
}