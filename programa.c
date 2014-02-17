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

	//printf("\nCuentaA: %d, CuentaB: %d",cuentaA,cuentaB);
		
	while( arr_cuentas_estado[cuentaA] == 1 || arr_cuentas_estado[cuentaB] == 1 || cuentaA == cuentaB){ //Mientras no encuentre dos cuentas desocupadas
		//printf("\nWhile CuentaA: %d, CuentaB: %d",cuentaA,cuentaB);
		cuentaA=getAleatorio(numero_cuentas);
		cuentaB=getAleatorio(numero_cuentas);
		
	}
	//printf("\nTratando de bloquear cuentas... quede con %d, %d",cuentaA,cuentaB);

	pthread_mutex_lock(&sum_mutex); //Entra en zona segura para leer el arr_cuentas_estado	
	arr_cuentas_estado[cuentaA] = 1; //Se pone el estado de la cuenta como ocupado
	arr_cuentas_estado[cuentaB] = 1; //Se pone el estado de la cuenta como ocupado
	pthread_mutex_unlock(&sum_mutex);  //Sale de zona segura
	
	//Hacer las transferencias por N cantidad de tiempo		
	//printf("\nHaciendo trasnferencias....");
	
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
	//printf("\nTermino transferencias");
	pthread_mutex_lock(&sum_mutex); //Entra en zona segura para leer el arr_cuentas_estado		
	arr_cuentas_estado[cuentaA] = 0; //Se pone el estado de la cuenta como desocupado
	arr_cuentas_estado[cuentaB] = 0; //Se pone el estado de la cuenta como desocupado
	pthread_mutex_unlock(&sum_mutex);  //Sale de zona segura

	int resu=0;
	return (void*)resu;

}

int getAleatorio(int maximo) {
	static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	int res;
	int error;
	if (error = pthread_mutex_lock(&lock)){
		printf("\nERROR MUTEX");
		return error; 
	}
	res = rand() % maximo;
	pthread_mutex_unlock(&lock);
	if(res<0)
		res=0;
	
	return res;
}

int main (int argn, char *arg[]){
	
	int soyEjecucion = -1;
	int x = 0;
	if( argn > 3 ){
		if (sscanf (arg[1], "%i", &numeroDeHilos) !=1 ) { printf ("error numeroDeHilos- not an integer %s",arg[1]); exit(0); }
		if (sscanf (arg[2], "%i", &cantidad_tiempo_a_correr) !=1 ) { printf ("error tiempo- not an integer"); exit(0);  }
		if (sscanf (arg[3], "%i", &numero_cuentas) !=1 ) { printf ("error cuentas - not an integer"); exit(0); }
		if (sscanf (arg[4], "%i", &valor_inicial) !=1 ) { printf ("error valor inicial- not an integer"); exit(0); }
		if( argn == 6 )		
			if (sscanf (arg[5], "%i", &soyEjecucion) !=1 ) { printf ("error valor inicial- not an integer"); soyEjecucion = -1;}
	}

	//printf("\nParametros: hilos=%d,tiempo=%d,cuentas=%d,vi=%d", numeroDeHilos,cantidad_tiempo_a_correr,numero_cuentas,valor_inicial);
	
	if(numero_cuentas < 3){
		printf("Deben haber al menos 2 cuentas");
		exit(0);
	}
		
		
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
	printf("\nBalance Total%d: \n\n\n", balanceTotal);

	if( soyEjecucion != -1 ){
		//La idea es crear una memoria compartida donde se guarde el balanceTotal para que sea leído por el programa de prueba
		//para memoria compartida
		key_t Clave;
		int Id_Memoria;
		int *Memoria = NULL;
		Clave = 134;
		Id_Memoria = shmget (Clave, sizeof(int)*150, 0777 | IPC_CREAT);
		if (Id_Memoria == -1){
			printf("No consigo Id para memoria compartida");
			exit (0);
		}
		Memoria = (int *)shmat (Id_Memoria, (char *)0, 0);
		if (Memoria == NULL){
			printf("No consigo memoria compartida");
			exit (0);
		}
	
		Memoria[soyEjecucion] = balanceTotal;
		//printf("\nGUARDO EN MEMORIA %d",soyEjecucion);

		//Liberamos recursos
		if (Id_Memoria != -1){
			shmdt ((char *)Memoria);
		}
	}
	
	
	pthread_mutex_destroy(&sum_mutex); 
	pthread_exit(NULL); 
	exit(0);
}

