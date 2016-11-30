#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

int* merge(int *A, int *B, int asize, int bsize);
void mergesort(int *A, int min, int max);

double start_time, stop_time;

int* merge(int *A, int *B, int asize, int bsize) {
	// TODO: fill in the code here to merge the sorted arrays
	int *C;
	int i=0;
	int j=0;
	int k=0;
	C = (int *)malloc(asize+bsize);
	while (i < asize && j<bsize)
	{
		if (A[i] < B[j])
		{
			C[k] = A[i];
			++k;
			++i;
		}
		else
		{
			C[k] = B[j];
			++k;
			++j;
		}
	}
	while (i<asize)
	{
		C[k] = A[i];
		++k;
		++i;
	}
	while (j<bsize)
	{
		C[k] = B[j];
		++k;
		++j;
	}
	return C;
}

void mergesort(int *A, int min, int max)
{
	// TODO: fill in the code here to recursive divide the array
	// into two halves, sort and merge them

}

int main(int argc, char **argv)
{
	int* data;
	int* sorted_data;
	int m, n, id, p, i, output;

	MPI_Status status;

	if (argc != 2) {
		printf("usage: %s <num_items>\n", argv[0]);
		return 1;
	}

	// Number of items to be sorted
	n = atoi(argv[1]);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if(id==0)
	{
		// data generation
		srandom(0);
		// Make sure that n is a multiple of p
		data = (int *)malloc(n*sizeof(int));
		for(i=0; i<n; i++)
			data[i] = random();
	}

	start_time = clock();

	// TODO: fill in the code here to
	// (1) distribute the data across the processes
	// (2) sort the data
	// (3) merge sorted data

	stop_time = clock();

	// Check correctness & print execution time
	if (id == 0)
	{
		FILE * fp;

		printf("%d procs, %d items, %f seconds\n", p, n, (stop_time-start_time)/CLOCKS_PER_SEC);

		if(sorted_data==NULL){
			printf("error: sorted_data does not exist\n");
		}
		else{
			for(i = 0; i < n - 1; i++){
				if(sorted_data[i] > sorted_data[i+1])
					printf("error: sorted_data[%d] is greater than sorted_data[%d]\n",i,i+1);
			}
		}
	}
	MPI_Finalize();
}
