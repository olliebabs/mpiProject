#include <stdio.h>
#include <stdbool.h>
#include <mpi.h>
#include <math.h>

//need to loop through 


bool is_prime(int n){

for (int i=2; i<n;i++){
if ((n%i) == 0){
return false;
}
}
return true;
}

void goldbach_range(int low, int max_number, int rank, int num_procs, int total_rows){
printf("\n\nnow inside the goldbach function\nrank number=%d\n", rank);

int prime_pairs[total_rows][2];


int counter=0;

for (int j = 2+(2*rank); j<=max_number; j+=(2*num_procs)){
	//int temp_even_number=2+(8*j);
//	printf("even number = %d\n",j);

	if (rank==0 && j==2){
		prime_pairs[counter][0]=1;
		prime_pairs[counter][1]=1;
	}
	for (int k=2; k<=j/2;k++){
//	printf("value of k=%d\n", k);
	if (is_prime(k) && is_prime(j-k)){
		//need to save the pairs into an array.
		prime_pairs[counter][0]=k;
		prime_pairs[counter][1]=j -k;
//		printf("the first prime pairs for %d are %d and %d\n",j,k,j-k);
		break;
		}
	}
	counter=counter+1;
	}

MPI_Send(&(prime_pairs[0][0]),total_rows * 2,MPI_INT,0,0,MPI_COMM_WORLD);

}

//main program:
//now need to split progarm into different sections:
int main(int argc, char *argv[]){

int rank, num_procs;

MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

int max_number = 8000;

int total_rows;
if (max_number % (2*num_procs) == 0){
total_rows = (max_number/(2*num_procs));
}else{
total_rows = ceil(max_number/(2*num_procs));
}

goldbach_range(2,max_number,rank, num_procs, total_rows);

//MPI_Barrier(MPI_COMM_WORLD);
//recieve the data back from the functions here:
//int [max_number/8][3][2] database;
int data_to_save [total_rows][2];
int database [total_rows * num_procs][2][num_procs];

for (int p = 0;p<=num_procs-1;p++){
	printf("recovering the data from processor number %d\n", p);
	MPI_Recv(&(data_to_save[0][0]),total_rows*2,MPI_INT,p,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for (int l=0;l<=total_rows;l++){
		database[l][0][p]=data_to_save[l][0];
		database[l][1][p]=data_to_save[l][1];
	}
}


//MPI_Gather(&(data_to_save[0][0]), (total_rows*num_procs) * 2, MPI_INT, &(database[0][0]), (total_rows*num_procs)*2, MPI_INT, 0, MPI_COMM_WORLD);

//MPI_Barrier(MPI_COMM_WORLD);
printf("\n\ncollecting back the data...\n\n");


if (rank == 0){
printf("data:\n");
for (int u=0; u< total_rows; u++){
	for (int m = 1; m<=3;m++){
		printf("pairs for %d are %d and %d\n", (2*u)+1 ,database[u][0][m], database[u][1][m]);
		}
	}
}


printf("\ngoodbye from process number %d\n", rank);


MPI_Finalize();
return 0;

}
