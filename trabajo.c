#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_RAM 1024
#define MAX_TEXTO 100
#define TAM_DIR 5
#define NUM_LINEAS 4

typedef struct{
	short int ETQ;
	short int Datos[8];
} T_LINEA_CACHE;

void acierto(int hex, unsigned char RAM[], int *tiempoglobal, char *texto); //Trata los aciertos
void fallo(int hex, unsigned char RAM[],T_LINEA_CACHE *cache, int *numfallos, int *tiempoglobal, char *texto); //Trata los fallos
void printCache(T_LINEA_CACHE cache[]); //Imprime los resultados actuales de la caché
void resetCache(T_LINEA_CACHE *cache); //Resetea la caché (Las etiquetas a FF y los datos a 00)

void main(){

	T_LINEA_CACHE cache[NUM_LINEAS];

	//Pongo las etiquetas a 0xFF y los datos a 0 de la cache
	resetCache(cache);

	//Se lee los datos de RAM.bin y se meten en la variable RAM
	FILE *datosRAM;
	unsigned char RAM[MAX_RAM];

	datosRAM = fopen("RAM.bin", "r");

	//Compruebo que existe
	if(datosRAM == NULL){
		printf("El fichero RAM.bin no existe");
		exit(-1);
	}

	fgets(RAM, MAX_RAM, (FILE*)datosRAM);
	fclose(datosRAM);

	//Leo accesos_memoria
	FILE *Faccesos_memoria;
	Faccesos_memoria = fopen("accesos_memoria.txt", "r");
	
	//Compruebo que existe
	if(Faccesos_memoria == NULL){
		printf("El fichero accesos_memoria.txt no existe");
		exit(-1);
	}
		
	char accesos_memoria[TAM_DIR];

	//Creo las variables que faltan
	int hex, tiempoglobal=0, numfallos=0, accesos;
	char texto[MAX_TEXTO];
	strcpy(texto, "");

	//Empieza el simulador
	for(accesos=0;fgets(accesos_memoria,7,Faccesos_memoria)!=NULL; accesos++){
		
		hex = (int)strtol(accesos_memoria, NULL, 16); //Hex es la direccion de memoria

		//Compruebo si ya esta cargado
		tiempoglobal++;
		
		if(cache[hex>>3&3].ETQ == hex>>5&31){

			acierto(hex, RAM, &tiempoglobal, texto);

		}else{
			
			fallo(hex, RAM, &cache[hex>>3&3], &numfallos, &tiempoglobal, texto);
			
			acierto(hex, RAM, &tiempoglobal, texto);
		}
	
		printCache(cache);
		sleep(2);
	}

	printf("\n\nNumero total de accesos: %d, numero de fallos: %d, tiempo medio de accesos: %.2f", accesos, numfallos, ((float) tiempoglobal/accesos), tiempoglobal);
	printf("\n\nTexto: %s", texto);
}


void acierto(int hex, unsigned char RAM[], int *tiempoglobal, char *texto){

	//Guardamos en el buffer el dato de la RAM
	char buff[3];
	buff[0]=RAM[((hex>>3&127)*8)+(hex&7)];
	strncat(texto,  buff, 1);

	printf("\nT: %d, Acierto de CACHE, ADDR %04X ETQ %X Linea %02X Palabra %02X DATO %02X\n", *tiempoglobal, hex, hex>>5&31, hex>>3&3, hex&7, RAM[((hex>>3&127)*8)+(hex&7)]);
}

void fallo(int hex,unsigned char RAM[], T_LINEA_CACHE *cache, int *numfallos, int *tiempoglobal, char *texto){

	//Se incrementa el número de fallos, al cargar el dato aumenta en 10 el tiempo global y cargamos el bloque correspondiente a la línea de caché
	*numfallos = *numfallos + 1;
	int bloque = (hex>>3&127) * 8;

	printf("\nT: %d, Fallo de CACHE %d, ADDR %04X ETQ %X Linea %02X Palabra %02X Bloque %02X", *tiempoglobal, *numfallos, hex, hex>>5&31, hex>>3&3, hex&7, bloque);
	
	*tiempoglobal+=10;

	printf("\nCargando el bloque %02X en la linea %02X", hex>>3&127, hex>>3&3);

	cache->ETQ = hex>>5&31;
	int i;
	for(i=7; i>-1; i--){
		cache->Datos[i] = RAM[bloque+7-i];
	}
}

void printCache(T_LINEA_CACHE cache[]){

	//Imprimimos las líneas de caché mediante un bucle doble, interpretándolas como una matriz
	int i,j;

	for(i=0; i<4; i++){
		printf("\nETQ: %02X   Datos: ", cache[i].ETQ);
		for(j=0; j<8; j++){
			printf("%02X ", cache[i].Datos[j]);
		}
	}
	printf("\n");
}

void resetCache(T_LINEA_CACHE *cache){

	//Al igual que en la función anterior sobreescribimos los datos de las líneas para "resetearlas"
	int i,j;

	for(i=0; i<4; i++){
		cache[i].ETQ = 0xFF;
		for(j=0; j<8; j++){
			cache[i].Datos[j] = 0;
		}
	}
}
