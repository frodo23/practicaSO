#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>

pthread_mutex_t sum_mutex; //mutex para evitar condiciones de carrera y deadlocks

int *arr_cuentas; //En cada posición del arreglo estará el valor de la cuenta

//Funcion que realiza transferencias aleatorias sobre dos cuentas
void *calcularTransferencias(){
		pthread_mutex_lock(&sum_mutex); //Entra en zona segura
		
		
		
		pthread_mutex_unlock(&sum_mutex);  //Sale de zona segura
}

int main (int argn, char *args[]){

	int x = 0;
	int numeroDeHilos, cantidad_tiempo_a_correr, numero_cuentas, valor_inicial;
	
	if (sscanf (arg[1], "%i", &numeroDeHilos) !=1 ) { printf ("error - not an integer"); }
	if (sscanf (arg[2], "%i", &cantidad_tiempo_a_correr) !=1 ) { printf ("error - not an integer"); }
	if (sscanf (arg[3], "%i", &numero_cuentas) !=1 ) { printf ("error - not an integer"); }
	if (sscanf (arg[4], "%i", &valor_inicial) !=1 ) { printf ("error - not an integer"); }
	
	arr_cuentas = (int *)malloc(numero_cuentas*sizeof(int)); //Se crean tantas posiciones como numero de cuentas
	
	//En todas las cuentas se pone el saldo como indice valor_inicial
	for ( x = 0; x < numero_cuentas; x++ ){
		arr_cuentas[x] = valor_inicial;	
	}
	
	pthread_mutex_init(&sum_mutex, NULL); //Inicia el mutex
	
	
	for ( x = 0; x < numero_cuentas; x++ ){
		printf("Valor de la cuenta %d: %d\n", x, arr_cuentas[x]);
	}
		
	exit(0);
}
