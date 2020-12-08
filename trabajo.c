#include <stdio.h>
#include <stdlib.h>

typedef struct{
	short int ETQ;
	short int Datos[8];
} T_LINEA_CACHE;

void acierto(int hex);
void fallo(int hex, unsigned char RAM[1024],T_LINEA_CACHE *cache, int *numFallos, int *tiempoglobal);

void main(){


	T_LINEA_CACHE cache[4];


	//Pongo las etiquetas a 0xFF y los datos a 0 de la cache

	int i,j;

	for(i=0; i<4; i++){
		cache[i].ETQ = 0xFF;
		for(j=0; j<8; j++){
			cache[i].Datos[j] = 0;
		}
	}

	//Se lee los datos de RAM.bin y se meten en la variable RAM
	
	FILE *datosRAM;
	unsigned char RAM[1024];

	datosRAM = fopen("RAM.bin", "r");
	//Compruebo que existe
	if(datosRAM == NULL){
		printf("El fichero RAM.bin no existe");
		exit(-1);
	}

	fgets(RAM, 1024, (FILE*)datosRAM);
	
	fclose(datosRAM);

	//Leo accesos_memoria
	
	FILE *Faccesos_memoria;
	Faccesos_memoria = fopen("accesos_memoria.txt", "r");
	
	//Compruebo que existe
	if(Faccesos_memoria == NULL){
		printf("El fichero accesos_memoria.txt no existe");
		exit(-1);
	}
		
	char accesos_memoria[5];

	//Creo las variables que faltan

	int hex, tiempoglobal=0, numfallos=0;

	//Empieza el simulador

	while(1){
		if(fgets(accesos_memoria,7,Faccesos_memoria)==NULL){
			break; //Esto es para cuando termine
		}
		
		hex = (int)strtol(accesos_memoria, NULL, 16); //Hex es la direccion de memoria

		//Compruebo si ya esta cargado
		
		if(cache[hex>>3&3].ETQ == hex>>5&31){
			printf("\n[CARGADO]");
			
			acierto(hex);

		}else{
			printf("\n[NO CARGADO]\n");
			fallo(hex, RAM, &cache[hex>>3&3], &numfallos, &tiempoglobal);
		}

		for(i=0; i<4; i++){
			printf("\nETQ: %02X   Datos: ", cache[i].ETQ);
			for(j=0; j<8; j++){
				printf("%02X ", cache[i].Datos[j]);
			}
		}
		
		
		printf("\n\nETQ: %02X   Linea: %02X   Palabra: %02X   Bloque: %02X\n", hex>>5&31, hex>>3&3, hex&7, hex>>3&127);

	}

}


void acierto(int hex){
	return;
}

void fallo(int hex,unsigned char RAM[1024], T_LINEA_CACHE *cache, int *numFallos, int *tiempoglobal){
	*numFallos++;
	*tiempoglobal=+10;
	
	int bloque = (hex>>3&127) * 8;

	printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X Linea %02X Palabra %02X Bloque %02X", *tiempoglobal, *numFallos, hex, hex>>5&31, hex>>3&3, hex&7, hex>>3&127);
	printf("\nCargando el bloque %02X en la linea %02X", hex>>3&127, hex>>3&3);

	cache->ETQ = hex>>5&31;
	printf("\n");
	int i;
	for(i=7; i>-1; i--){
		cache->Datos[i] = RAM[bloque+7-i];
	}
	
	






	return;
}


