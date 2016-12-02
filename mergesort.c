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
	/*
	for (i=0;i<asize;i++)
		printf("%d| ", A[i]);
	putchar('\n');
	for (i=0;i<bsize;i++)
		printf("%d, ", B[i]);
	putchar('\n');

	i=0;j=0;k=0;
	*/
	C = (int *)malloc((asize+bsize)*sizeof(int));
	while (i < asize && j<bsize)
	{
		if (A[i] < B[j])
		{
			C[k] = A[i];
			k++;
			i++;
		}
		else
		{
			C[k] = B[j];
			k++;
			j++;
		}
	}
	while (i<asize)
	{
		C[k] = A[i];
		k++;
		i++;
	}
	while (j<bsize)
	{
		C[k] = B[j];
		k++;
		j++;
	}
	for (i=0;i<asize;i++)
	{
		A[i] = C[i];
	}
	for (i=0;i<bsize;i++)
	{
		B[i] = C[asize+i];
	}
	/*
	for (i=0;i<k;i++)
		printf("%d/ \n", C[i]);
	putchar('\n');
	*/
	return C;
}

void mergesort(int *A, int min, int max)
{
	// TODO: fill in the code here to recursive divide the array
	// into two halves, sort and merge them
	int mid;
	int i;
	//for (i=min;i<=max;i++)
	//	printf("%d, ", A[i]);
	//putchar('\n');
	//printf("mid max %d %d \n", min, max);
	if(min < max)
	{
		mid = (min + max)/2;
		mergesort(A, min, mid);
		mergesort(A, mid+1, max);
	//	printf("min min max %d %d %d\n", min, mid, max);
		merge(A+min, A+mid+1, mid-min+1, max-mid);
	}
}

int main(int argc, char **argv)
{
	int* data;
	int* sorted_data;
	int m, n, id, p, i, output;
	//Added by Alibek-
	int pre_size=0;
	int *sub_data;
	int *sub_data1;
	int level;
	//-----------------
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
	printf("%d\n", p);
	if(id==0)
	{
		pre_size = n/p;
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

	if(id==0)
	{
		MPI_Bcast(&pre_size,1,MPI_INT,0,MPI_COMM_WORLD);
		sub_data = (int *)malloc(pre_size*sizeof(int));
		MPI_Scatter(data,pre_size,MPI_INT,sub_data,pre_size,MPI_INT,0,MPI_COMM_WORLD);
		mergesort(sub_data, 0, pre_size-1);
	}
	else
	{
		MPI_Bcast(&pre_size,1,MPI_INT,0,MPI_COMM_WORLD);
		sub_data = (int *)malloc(pre_size*sizeof(int));
		MPI_Scatter(data,pre_size,MPI_INT,sub_data,pre_size,MPI_INT,0,MPI_COMM_WORLD);
		mergesort(sub_data, 0, pre_size-1);
	}

	level = 1;

	while( level < p )
	{
		if(id % (2*level) == 0)
		{
			if( id+level < p )
			{
				MPI_Recv(&m, 1 , MPI_INT,id+level,0,MPI_COMM_WORLD,&status);
				sub_data1 = (int *)malloc(m*sizeof(int));
				MPI_Recv(sub_data1,m,MPI_INT,id+level,0,MPI_COMM_WORLD,&status);
				sub_data = merge(sub_data,sub_data1, pre_size,m);
				pre_size = pre_size + m;
			}
		}
		else
		{
			int neighborhood = id-level;
			MPI_Send(&pre_size,1,MPI_INT,neighborhood,0,MPI_COMM_WORLD);
			MPI_Send(sub_data,pre_size,MPI_INT,neighborhood,0,MPI_COMM_WORLD);
			break;
		}
		level = level*2;
	}

	stop_time = clock();

	// Check correctness & print execution time
	sorted_data = sub_data;

	if (id == 0)
	{
		FILE * fp;

		printf("%d procs, %d items, %f seconds\n", p, n, (stop_time-start_time)/CLOCKS_PER_SEC);

		if(sorted_data==NULL){
			printf("error: sorted_data does not exist\n");
		}
		else{
			for(i = 0; i < n - 1; i++){
				//printf("%d\n", sorted_data[i]);
				if(sorted_data[i] > sorted_data[i+1])
					printf("error: sorted_data[%d] is greater than sorted_data[%d]\n",i,i+1);
			}
			printf("\n");
		}
	}
	MPI_Finalize();
}
