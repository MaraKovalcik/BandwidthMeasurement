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

/*
 * Deklarace funkcí
 */
void printHelp();
bool checkArguments(int, char **);

/*
 * Funkce pro vypsání nápovědy
 */
void printHelp(){
    fprintf(stderr, "---------------------------------------------------\n");
    fprintf(stderr, "USAGE: reflect program: ./ipk-mtrip reflect -p PORT\n");
    fprintf(stderr, "\t meter program: ./ipk-mtrip meter -h HOST -p PORT -s SONDA -t TIME\n");
    fprintf(stderr, "\t\t-p PORT\t... port number\n");
    fprintf(stderr, "\t\t-h HOST\t... IP or Domain name of remote host\n");
    fprintf(stderr, "\t\t-s SONDA\t... amount of data in packet\n");
    fprintf(stderr, "\t\t-t TIME\t... overall time of measurement\n");
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
        // TODO
        printf("Program je spusten jako reflektor\n");
        printf("\tport: %d\n", port);
    }

    // Program je spuštěn jako meter
    if(isMeter == true) {
        // TODO
        printf("Program je spusten jako meter\n");
        printf("\thost: %s | port: %d | sonda: %d | time: %d\n", host, port, velikostSondy, dobaMereni);
    }


    exit(EXIT_SUCCESS);
}