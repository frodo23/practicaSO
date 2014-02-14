#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/shm.h>

int getAleatorio(int maximo);

pthread_mutex_t sum_mutex; //mutex para evitar condiciones de carrera y deadlocks

int *arr_cuentas; //En cada posición del arreglo estará el valor de la cuenta
int *arr_cuentas_estado; //En cada posición del arreglo habra un cero si no se está utilizando la cuenta, 1 en caso contrario

int numeroDeHilos, cantidad_tiempo_a_correr, numero_cuentas, valor_inicial;

//Funcion que realiza transferencias aleatorias sobre dos cuentas
void *calcularTransferencias(){
	int cuentaA=getAleatorio(numero_cuentas);
	int cuentaB=getAleatorio(numero_cuentas);		
		
	while( arr_cuentas_estado[cuentaA] != 0 && arr_cuentas_estado[cuentaB] != 0 ){ //Mientras no encuentre dos cuentas desocupadas
		cuentaA=getAleatorio(numero_cuentas);
		cuentaB=getAleatorio(numero_cuentas);
	}
	
	pthread_mutex_lock(&sum_mutex); //Entra en zona segura para leer el arr_cuentas_estado	
	arr_cuentas_estado[cuentaA] = 1; //Se pone el estado de la cuenta como ocupado
	arr_cuentas_estado[cuentaB] = 1; //Se pone el estado de la cuenta como ocupado
	pthread_mutex_unlock(&sum_mutex);  //Sale de zona segura
	
	//Hacer las transferencias por N cantidad de tiempo		
	//Haciendo trasnferencias....
	
	//PROBAR EL SIGUIENTE CODIGO PARA EJECUTAR POR cantidad_tiempo_a_correr
	//AUN NO SE HAN HECHO PRUEBAS...
	int monto = 0;
	time_t start = time(NULL);
	for(;;)
	{
		monto = getAleatorio( arr_cuentas[cuentaA] ); //Se consulta un numero aleatorio entre 0 y el valor de la cuentaA
		arr_cuentas[cuentaA] = arr_cuentas[cuentaA] - monto; //Se retira en la cuentaA el monto aleatorio
		arr_cuentas[cuentaB] = arr_cuentas[cuentaB] + monto; //Se consigna en la cuentaB el monto retirado aleatorio de la cuentaA

		monto = getAleatorio( arr_cuentas[cuentaB] ); //Se consulta un numero aleatorio entre 0 y el valor de la cuentaB
		arr_cuentas[cuentaB] = arr_cuentas[cuentaB] - monto; //Se retira en la cuentaA el monto aleatorio
		arr_cuentas[cuentaA] = arr_cuentas[cuentaA] + monto; //Se consigna en la cuentaA el monto retirado aleatorio de la cuentaB
		
		if(time(NULL) > start + cantidad_tiempo_a_correr){ //Si ya ejecuto durante cantidad_tiempo_a_correr debe parar de hacer transferencias
			break;
		}
	}
	
	pthread_mutex_lock(&sum_mutex); //Entra en zona segura para leer el arr_cuentas_estado		
	arr_cuentas_estado[cuentaA] = 0; //Se pone el estado de la cuenta como desocupado
	arr_cuentas_estado[cuentaB] = 0; //Se pone el estado de la cuenta como desocupado
	pthread_mutex_unlock(&sum_mutex);  //Sale de zona segura

	int resu=0;
	return (void*)resu;

}

int getAleatorio(int maximo){
	srand(time(NULL));
    	int result = rand() % maximo;
	return result;
}

int main (int argn, char *arg[]){

	int x = 0;
	
	if (sscanf (arg[1], "%i", &numeroDeHilos) !=1 ) { printf ("error - not an integer"); }
	if (sscanf (arg[2], "%i", &cantidad_tiempo_a_correr) !=1 ) { printf ("error - not an integer"); }
	if (sscanf (arg[3], "%i", &numero_cuentas) !=1 ) { printf ("error - not an integer"); }
	if (sscanf (arg[4], "%i", &valor_inicial) !=1 ) { printf ("error - not an integer"); }
		
		
	pthread_mutex_init(&sum_mutex, NULL); //Inicia el mutex
	
	arr_cuentas = (int *)malloc(numero_cuentas*sizeof(int)); //Se crean tantas posiciones como numero de cuentas	
	//En todas las cuentas se pone el saldo como indice valor_inicial
	for ( x = 0; x < numero_cuentas; x++ ){
		arr_cuentas[x] = valor_inicial;	
	}
	
	arr_cuentas_estado = (int *)malloc(numero_cuentas*sizeof(int)); //Se crean tantas posiciones como numero de cuentas	
	//En todas las cuentas se pone 0, indicado que no se estan ocupando
	for ( x = 0; x < numero_cuentas; x++ ){
		arr_cuentas_estado[x] = 0;	
	}
	
	pthread_t hilos[numeroDeHilos]; //Se crean la cantidad de hilos que indica la variable numeroDeHilos
	for ( x = 0; x < numeroDeHilos; x++ ){
		pthread_create (&hilos[x], NULL, &calcularTransferencias, NULL);
	}
	
	int nada;
	for ( x = 0; x < numeroDeHilos; x++ ){
		pthread_join(hilos[x],(void*)&nada);
	}	
	
	
	int balanceTotal=0;
	for ( x = 0; x < numero_cuentas; x++ ){
		printf("Valor de la cuenta %d: %d\n", x, arr_cuentas[x]);
		balanceTotal = balanceTotal + arr_cuentas[x];
	}
	printf("\nBalance Total%d: \n", balanceTotal);

	pthread_mutex_destroy(&sum_mutex); 
	pthread_exit(NULL); 
	exit(0);
}
