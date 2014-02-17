#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <sys/shm.h>

int main (int argn, char *args[]){
	
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
	
	pid_t wpid;
	int status;
	FILE *file;
	file= fopen( args[2], "r" );
	if( file == NULL ){
		printf("Error al abrir el archivo");
		exit(-1);
	}

	char line [ 128 ];
	int x = 0;
	int balances[150]; //Arreglo que contiene en cada posicion, el calculo de balance que debe arrojar la ejecucion del programa

	while ( fgets ( line, sizeof line, file ) != NULL ) /* Se lee una linea */
	{
		//Se parsean los parametros que se mandaran al programa
		char numHijos[100];
		char tiempo[100];
		char cuentas[100];
		char valorIni[100];
		int repeticiones=1;
		char *pch;
		pch = strtok(line," ");
		sscanf(pch, "%s", numHijos);
		pch = strtok (NULL, " ");
		sscanf(pch, "%s", tiempo);
		pch = strtok (NULL, " ");
		sscanf(pch, "%s", cuentas);
		pch = strtok (NULL, " ");
		sscanf(pch, "%s", valorIni);
		pch = strtok (NULL, " ");
		sscanf(pch, "%d", &repeticiones);

		int k=0;
		//El balance debe ser valorIni*cuentas
		int vali;
		sscanf(valorIni,"%d",&vali);
		int cuenti;
		sscanf(cuentas,"%d",&cuenti);		

		
		for ( k = 0; k < repeticiones; k++ ){
			balances[x+1] = vali*cuenti;
			pid_t pidhijo = fork();
			x++;
		
			if( pidhijo < 0 ){
				printf("Error al crear el hijo");
				exit(-1);
			}else if( pidhijo == 0 ){ //Hijo
				char pos[100] = "";
				sprintf(pos,"%d",x);
				char *parametrosHijo2 [] = { args[1],numHijos,tiempo,cuentas,valorIni, pos, 0, NULL };	
				int control2 = execv(parametrosHijo2[0], parametrosHijo2);
				if( control2 < 0 ){
					printf("error en execvp\n");
				}
				exit(0);
			
			}
		}
	}
	fclose(file);

	while ((wpid = wait(&status)) > 0){ //Padre espera a todos los hijos

	}
	
	int k=1;
	for(k=1; k<=x; k++){
		printf("Resultado Programa: %d\n",Memoria[k]);
		printf("Resultado que debio dar: %d\n",balances[k]);
		if( Memoria[k] != balances[k] ){
			printf("NO FUNCIONO\n\n");
		}else{
			printf("FUNCIONO\n\n");
		}
	}

	//Terminada de usar la memoria compartida, la liberamos.
	shmdt ((char *)Memoria);
	shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);

	exit(0);
}
