#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>//proceosos
#include <signal.h>//señales
#include <stdio.h>
#include <sys/shm.h>//memoria
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/sem.h>//semaforos
#include <sys/msg.h>//buzones


#include "pist2.h"

#define DIM 26 // Dimension maxima para el array de procesos
pid_t pistoleros[DIM]; // Array de procesos

//--------------------------------------------------------
// DEFINICION DE ESTRUCTURAS DE SEMAFOROS Y BUZONES
//--------------------------------------------------------

union semun {
    int val; // Valor para SETVAL
    struct semid_ds *buf; // Buffer para IPC_STAT, IPC_SET
    unsigned short *array; // Array para GETALL, SETALL
    struct seminfo *__buf; // Buffer para IPC_INFO (LINUX)
}; // Estructura general para los semaforos

typedef struct mensaje {
    long Id_Mensaje;
    int hijo; 
} mensaje; // estructura mensaje para buzón

//--------------------------------------------------------
// PROTOTIPOS
//--------------------------------------------------------
void finalizarPrograma();
void handler();






//--------------------------------------------------------
// VARIABLES GLOBALES 
//--------------------------------------------------------

int memoria;
char *ptr;
int semaforo;
int buzon;
int n_pistoleros;
int procesos_vivos = 0;
struct sigaction sig;
int seed;


//--------------------------------------------------------
// MAIN
//--------------------------------------------------------

int main(int argc, char *argv[]){

    union semun sem1, sem2;
    struct mensaje mensaje;
    int velocidad;

    ///// SEÑAL PARA FINALIZAR (CRTL+C)
    sig.sa_handler = handler;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    sigaddset(&sig.sa_mask, SIGINT);
    if (sigaction(SIGINT, &sig, NULL) == -1) {
        perror("Error en la manejadora SIGINT (CTRL+C)\n");
        exit(-1);
    }

    ///// COMPROBACIÓN DE PARÁMETROS
    for(int i=0; i<DIM; i++){
        pistoleros[i]=-2;
    }

    if(argc>3){
        perror("Error. Demasiados parametros introducidos\n");
        exit(-1);
    }

    n_pistoleros = atoi(argv[1])/*+2*/;
    velocidad = atoi(argv[2]);
    printf("El numero de pistoleros es %d.", n_pistoleros-2);
    printf("La velocidad de ejecucion es %d.", velocidad);
    if(argv[3]!=NULL){
        seed = atoi(argv[3]);
        printf("La semilla para generar numeros aleatorios es %d.", seed);
    } else {
		seed = 0;
	}

    if(n_pistoleros<0 || n_pistoleros>26){
        perror("Numero de procesos incorrecto.\n");
        exit(-1);
    }

    if(velocidad < 0){
        perror("Velocidad incorrecta. Introduce una velocidad positiva.\n");
        exit(-1);
    }

    ///// CREACIÓN DE ZONA DE MEMORIA COMPARTIDA
    memoria = -1;
	ptr = (char *) -1;

	if((memoria = shmget(IPC_PRIVATE,256,IPC_CREAT|0600))==-1){ // Reserva de memoria compartida
		perror("No se ha podido crear la memoria compartida\n");
		exit(-1);
	}

	if((ptr = (char *) shmat(memoria, NULL, 0)) == NULL){ // Puntero de memoria compartida
		perror("No se ha podido crear el puntero de memoria compartida\n");
		exit(-1);
	}

	///// CREACIÓN DE SEMAFOROS
	semaforo=semget(IPC_PRIVATE,12,IPC_CREAT | 0700); //creo un conjunto de semaforos	











	///// CREACIÓN DE BUZON
	buzon=msgget(IPC_PRIVATE, 0666 | IPC_CREAT); 
		if(buzon == -1) {
    		perror("No se puede crear el buzon");
    		exit(-1);
		} 


	///// COMIENZO DE LA EJECUCION DEL PROGRAMA
	if (PIST_inicio(n_pistoleros, velocidad,semaforo, ptr, 0) == -1){
		perror("Error al iniciar la libreria 'libpist.a'\n");
		exit(-1);
	}

	pistoleros[0] = getpid(); // Guardamos el pid del padre

	fork(); // Proceso semaforico

	if(pistoleros[0]!=getpid()){
		cicloSemaforico();
	}

	///// CREACION DE PROCESOS
	char letras_asignadas = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "Ñ", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y"];
	int pistolero;
	int i = 0;

	if(pistoleros[0]==getpid()){
		do {
			if(procesos_vivos < num_procesos -2){
				pistolero = PIST_nuevoPistolero(letras_asignadas[i]);
				i++;
				procesos_vivos++;

				if(pistoleros[0]!=getpid())break;

			} else {

			}
			
		} while(pistoleros[0]==getpid());
	}



}
void finalizarPrograma(){
	

	int i, valor_devuelto;
	
	if(pistoleros[0]==getpid()){


		if (PIST_fin() == -1) {
			perror("Error al finalizar el programa\n");
			exit(-1);
		}


		if (semctl(semaforo, 0, IPC_RMID) == -1) {
			perror("Error al eliminar el conjunto de semaforos\n");
			exit(-1);
		} else
			printf("Conjunto de semáforos eliminado correctamente\n");
			
			
		if((msgctl(buzon, IPC_RMID, NULL)==-1)){
			perror("Error al eliminar el buzon");
			exit(-1); 
			
		} else printf("Buzon correctamente eliminado\n");
		

		if (shmctl(memoria, IPC_RMID, 0) == -1) {
			perror("Error al borrar la zona de memoria compartida\n");
			exit(-1);
			
		} else printf("Memoria compartida correctamente borrada\n");
	
	exit(0);
	
	} else exit(0);

	
}

void handler(){ //función manejadora para finalizar el programa
finalizarPrograma();
}
//Funcion para crear procesos
int crear(){

int i;
	
	if(pistoleros[0]==getpid()){
	
		for(i=1; i < n_pistoleros; i++ ){
			
			if(pistoleros[i]==-2){
				
				fork();
				
				if(pistoleros[0]!=getpid()){
					pistoleros[i]=getpid();
					PIST_nuevoPistolero();
					
				
				return i;
			
				}
			}
		}
	}
}

