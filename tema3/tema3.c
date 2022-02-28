#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>


void topology (int rank, int cluster0_dim, int cluster1_dim, int cluster2_dim, int **clusters) 
{
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            printf("%d -> 0:", rank);
            for (int j = 0; j < cluster0_dim; j++) {
                if (j != cluster0_dim - 1) {
                    printf("%d,", clusters[i][j]);
                } else {
                    printf("%d ", clusters[i][j]);
                }
            }
        } else if (i == 1) {
            printf("1:");
            for (int j = 0; j < cluster1_dim; j++) {
                if (j != cluster1_dim - 1) {
                    printf("%d,", clusters[i][j]);
                } else {
                    printf("%d ", clusters[i][j]);
                }
            }
        } else {
            printf("2:");
            for (int j = 0; j < cluster2_dim; j++) {
                if (j != cluster2_dim - 1) {
                    printf("%d,", clusters[i][j]);
                } else {
                    printf("%d\n", clusters[i][j]);
                }
            }
        }
    }
}

void calculation(int *v, int N) 
{
    printf("Rezultat: ");
    for (int i = 0; i < N; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}


int main (int argc, char *argv[])
{
    int procs, rank;
    int **clusters = malloc(3 * sizeof(int**));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int value;
    MPI_Status status;

    int cluster0, cluster1, cluster2;

    if (rank == 0) {
        FILE *fp;
        char buff[255];
        fp = fopen("cluster0.txt", "r");

        if(fp == NULL) {
            printf("Nu s-a gasit fisierul!");   
            exit(1);             
        }

        fscanf(fp, "%s", buff);
        cluster0 = atoi(buff);

        // sends number of workers to the other coordinators
        MPI_Send(&cluster0, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
        printf("M(0,1)\n");
        MPI_Send(&cluster0, 1, MPI_INT, 2, 0, MPI_COMM_WORLD); 
        printf("M(0,2)\n");

        clusters[0] = malloc(cluster0 * sizeof(int));
        for (int i = 0; i < cluster0; i++) {
            fscanf(fp, "%s", buff);
            clusters[0][i] = atoi(buff);

            // sends workers' number to the other coordinators
            MPI_Send(&clusters[0][i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
            printf("M(0,1)\n");
            MPI_Send(&clusters[0][i], 1, MPI_INT, 2, 0, MPI_COMM_WORLD); 
            printf("M(0,2)\n");

            value = 0;
            MPI_Send(&value, 1, MPI_INT, clusters[0][i], 0, MPI_COMM_WORLD); 
            printf("M(0,%d)\n",  clusters[0][i]);

        }

        // receives number of workers from other coordinators
        MPI_Recv(&cluster1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        clusters[1] = malloc(cluster1 * sizeof(int));
        MPI_Recv(&cluster2, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
        clusters[2] = malloc(cluster2 * sizeof(int));
        
        // receives workers' number from other coordinators
        for (int i = 0; i < cluster1; i++) {
            MPI_Recv(&clusters[1][i], 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        }      
        
        for (int i = 0; i < cluster2; i++) {
            MPI_Recv(&clusters[2][i], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
        }

        // prints the topology
        topology(rank, cluster0, cluster1, cluster2, clusters);

        // sends the clusters dimension to the children
        for (int i = 0; i < cluster0; i++) {
            MPI_Send(&cluster0, 1, MPI_INT, clusters[0][i], 0, MPI_COMM_WORLD); 
            printf("M(0,%d)\n",  clusters[0][i]);

            MPI_Send(&cluster1, 1, MPI_INT, clusters[0][i], 0, MPI_COMM_WORLD); 
            printf("M(0,%d)\n",  clusters[0][i]);

            MPI_Send(&cluster2, 1, MPI_INT, clusters[0][i], 0, MPI_COMM_WORLD); 
            printf("M(0,%d)\n",  clusters[0][i]);
        }

        // sends the topology to the children
        for (int t = 0; t < cluster0; t++) {
            for (int i = 0; i < 3; i++) {
                if (i == 0) {
                    for (int j = 0; j < cluster0; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[0][t], 0, MPI_COMM_WORLD);  
                        printf("M(0,%d)\n",  clusters[0][t]);
                    }
                } else if (i == 1) {
                    for (int j = 0; j < cluster1; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[0][t], 0, MPI_COMM_WORLD); 
                        printf("M(0,%d)\n",  clusters[0][t]);
                    }
                } else {
                    for (int j = 0; j < cluster2; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[0][t], 0, MPI_COMM_WORLD); 
                        printf("M(0,%d)\n",  clusters[0][t]);
                    }
                }
            }
        }


        int N = atoi(argv[1]);
        int *v = malloc(N * sizeof(int));

        // generates the vector
        for (int i = 0; i < N; i++) {
            v[i] = i;
        }

        int number_of_workers = procs - 3;
        int *iter_per_worker = malloc(number_of_workers * sizeof(int));

        // calculates number of iterations per worker in a balanced way
        if (N % number_of_workers == 0) {
            for (int i = 0; i < number_of_workers; i++) {
                iter_per_worker[i] = N / number_of_workers;
            }
        } else {
            for (int i = 0; i < number_of_workers; i++) {
                if ( i < N % number_of_workers) {
                    iter_per_worker[i] = N / number_of_workers + 1;
                } else {
                    iter_per_worker[i] = N / number_of_workers;
                }
            }
        }

        int *iter_per_cluster = calloc(3, sizeof(int));

        // calculates number of iterations for the first cluster
        for (int i = 0; i < cluster0; i++) {
            iter_per_cluster[0] += iter_per_worker[i];
        }

        // calculates number of iterations for the second cluster
        // and sends the number of iteration for every worker from that cluster
        for (int i = cluster0; i < cluster0 + cluster1; i++) {
            iter_per_cluster[1] += iter_per_worker[i];
            MPI_Send(&iter_per_worker[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);  
            printf("M(0,1)\n");
        }

        // calculates number of iterations for the third cluster
        // and sends the number of iteration for every worker from that cluster
        for (int i = cluster0 + cluster1; i < number_of_workers; i++) {
            iter_per_cluster[2] += iter_per_worker[i];
            MPI_Send(&iter_per_worker[i], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);  
            printf("M(0,2)\n");
        }

        // first cluster directly sends parts of the vector to its children and receives it modified
        int count = 0;
        for (int j = 0; j < cluster0; j++) {
            MPI_Send(&iter_per_worker[clusters[0][j] - 3], 1, MPI_INT, clusters[0][j], 0, MPI_COMM_WORLD);  
            printf("M(0,%d)\n", clusters[0][j]);
            for (int i = 0; i < iter_per_worker[clusters[0][j] - 3]; i++) {
                MPI_Send(&v[count], 1, MPI_INT, clusters[0][j], 0, MPI_COMM_WORLD);  
                printf("M(0,%d)\n", clusters[0][j]);
                MPI_Recv(&v[count], 1, MPI_INT, clusters[0][j], clusters[0][j], MPI_COMM_WORLD, &status);
                count++;  
            }
        }    

        // sends to the second cluster its part of the vector and receives it modified
        for (int i = iter_per_cluster[0]; i < iter_per_cluster[0] + iter_per_cluster[1]; i++){
            MPI_Send(&v[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);  
            printf("M(0,1)\n");
            MPI_Recv(&v[i], 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        }


        // sends to the third cluster its part of the vector
        for (int i = iter_per_cluster[0] + iter_per_cluster[1]; i < N; i++){
            MPI_Send(&v[i], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);  
            printf("M(0,2)\n");
            MPI_Recv(&v[i], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
        }

        // prints the vector with the elements doubled
        calculation(v, N);

        fclose(fp);

    } else if (rank == 1) {
        FILE *fp;
        char buff[255];
        fp = fopen("cluster1.txt", "r");

        if(fp == NULL) {
            printf("File not found!");   
            exit(1);             
        }

        fscanf(fp, "%s", buff);
        cluster1 = atoi(buff);

        // sends number of workers to the other coordinators
        MPI_Send(&cluster1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD); 
        printf("M(1,0)\n");
        MPI_Send(&cluster1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD); 
        printf("M(1,2)\n");

        clusters[1] = malloc(cluster1 * sizeof(int));
        for (int i = 0; i < cluster1; i++) {
            fscanf(fp, "%s", buff);
            clusters[1][i] = atoi(buff);

            // sends workers' number to the other coordinators
            MPI_Send(&clusters[1][i], 1, MPI_INT, 0, 1, MPI_COMM_WORLD); 
            printf("M(1,0)\n");
            MPI_Send(&clusters[1][i], 1, MPI_INT, 2, 1, MPI_COMM_WORLD); 
            printf("M(1,2)\n");

            value = 1;
            MPI_Send(&value, 1, MPI_INT, clusters[1][i], 1, MPI_COMM_WORLD); 
            printf("M(1,%d)\n",  clusters[1][i]);
  
        }

        // receives number of workers from other coordinators
        MPI_Recv(&cluster0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        clusters[0] = malloc(cluster0 * sizeof(int));
        MPI_Recv(&cluster2, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
        clusters[2] = malloc(cluster2 * sizeof(int));
        
        // receives workers' number from other coordinators
        for (int i = 0; i < cluster0; i++) {
            MPI_Recv(&clusters[0][i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }

        for (int i = 0; i < cluster2; i++) {
            MPI_Recv(&clusters[2][i], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
        }

        // prints the topology
        topology(rank, cluster0, cluster1, cluster2, clusters);

        // sends the clusters dimension to the children
        for (int i = 0; i < cluster1; i++) {
            MPI_Send(&cluster0, 1, MPI_INT, clusters[1][i], 1, MPI_COMM_WORLD); 
            printf("M(1,%d)\n",  clusters[1][i]);

            MPI_Send(&cluster1, 1, MPI_INT, clusters[1][i], 1, MPI_COMM_WORLD); 
            printf("M(1,%d)\n",  clusters[1][i]);

            MPI_Send(&cluster2, 1, MPI_INT, clusters[1][i], 1, MPI_COMM_WORLD); 
            printf("M(1,%d)\n",  clusters[1][i]);
        }

        // sends the topology to the children
        for (int t = 0; t < cluster1; t++) {
            for (int i = 0; i < 3; i++) {
                if (i == 0) {
                    for (int j = 0; j < cluster0; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[1][t], 1, MPI_COMM_WORLD);   
                        printf("M(1,%d)\n", clusters[1][t]);
                    }
                } else if (i == 1) {
                    for (int j = 0; j < cluster1; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[1][t], 1, MPI_COMM_WORLD); 
                        printf("M(1,%d)\n", clusters[1][t]);
                    }
                } else {
                    for (int j = 0; j < cluster2; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[1][t], 1, MPI_COMM_WORLD); 
                        printf("M(1,%d)\n", clusters[1][t]);
                    }
                }
            }
        }

        // receives number of iterations per worker
        int *iter_per_worker = calloc(cluster1 , sizeof(int));
        for (int i = 0; i < cluster1; i++) {
            MPI_Recv(&iter_per_worker[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }

        // receives its corespondent part of the vector, sends it to its workers,
        // receives it modified and sends it back to the first cluster
        for (int i = 0; i < cluster1; i++) {
            MPI_Send(&iter_per_worker[i], 1, MPI_INT, clusters[1][i], 1, MPI_COMM_WORLD); 
            printf("M(1,%d)\n", clusters[1][i]);
            for (int j = 0; j < iter_per_worker[i]; j++) {
                int v;
                MPI_Recv(&v, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                MPI_Send(&v, 1, MPI_INT, clusters[1][i], 1, MPI_COMM_WORLD); 
                printf("M(1,%d)\n", clusters[1][i]);
                MPI_Recv(&v, 1, MPI_INT, clusters[1][i], clusters[1][i], MPI_COMM_WORLD, &status);
                MPI_Send(&v, 1, MPI_INT, 0, 1, MPI_COMM_WORLD); 
                printf("M(1,0)\n");
            }
        }

        fclose(fp);

    } else if (rank == 2) {
        FILE *fp;
        char buff[255];
        fp = fopen("cluster2.txt", "r");

        if(fp == NULL) {
            printf("Nu s-a gasit fisierul!");   
            exit(1);             
        }

        fscanf(fp, "%s", buff);
        int cluster2 = atoi(buff);
        
        // sends number of workers to the other coordinators
        MPI_Send(&cluster2, 1, MPI_INT, 0, 2, MPI_COMM_WORLD); 
        printf("M(2,0)\n");
        MPI_Send(&cluster2, 1, MPI_INT, 1, 2, MPI_COMM_WORLD); 
        printf("M(2,1)\n");

        clusters[2] = malloc(cluster2 * sizeof(int));
        for (int i = 0; i < cluster2; i++) {
            fscanf(fp, "%s", buff);
            clusters[2][i] = atoi(buff);

            // sends workers' number to the other coordinators
            MPI_Send(&clusters[2][i], 1, MPI_INT, 0, 2, MPI_COMM_WORLD); 
            printf("M(2,0)\n");
            MPI_Send(&clusters[2][i], 1, MPI_INT, 1, 2, MPI_COMM_WORLD); 
            printf("M(2,1)\n");
            
            value = 2;
            MPI_Send(&value, 1, MPI_INT, clusters[2][i], 2, MPI_COMM_WORLD); 
            printf("M(2,%d)\n",  clusters[2][i]);  

        }

        // receives number of workers from other coordinators
        MPI_Recv(&cluster0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        clusters[0] = malloc(cluster0 * sizeof(int));
        MPI_Recv(&cluster1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        clusters[1] = malloc(cluster1 * sizeof(int));
        
        // receives workers' number from other coordinators
        for (int i = 0; i < cluster0; i++) {
            MPI_Recv(&clusters[0][i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }

        for (int i = 0; i < cluster1; i++) {
            MPI_Recv(&clusters[1][i], 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        }

        // prints the topology
        topology(rank, cluster0, cluster1, cluster2, clusters);

        // sends the clusters dimension to the children
        for (int i = 0; i < cluster2; i++) {
            MPI_Send(&cluster0, 1, MPI_INT, clusters[2][i], 2, MPI_COMM_WORLD); 
            printf("M(2,%d)\n",  clusters[2][i]);

            MPI_Send(&cluster1, 1, MPI_INT, clusters[2][i], 2, MPI_COMM_WORLD); 
            printf("M(2,%d)\n",  clusters[2][i]);

            MPI_Send(&cluster2, 1, MPI_INT, clusters[2][i], 2, MPI_COMM_WORLD); 
            printf("M(2,%d)\n",  clusters[2][i]);
        }

        // sends the topology to the children
        for (int t = 0; t < cluster2; t++) {
            for (int i = 0; i < 3; i++) {
                if (i == 0) {
                    for (int j = 0; j < cluster0; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[2][t], 2, MPI_COMM_WORLD);    
                        printf("M(2,%d)\n", clusters[2][t]);
                    }
                } else if (i == 1) {
                    for (int j = 0; j < cluster1; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[2][t], 2, MPI_COMM_WORLD); 
                        printf("M(2,%d)\n", clusters[2][t]);
                    }
                } else {
                    for (int j = 0; j < cluster2; j++) {
                        MPI_Send(&clusters[i][j], 1, MPI_INT, clusters[2][t], 2, MPI_COMM_WORLD); 
                        printf("M(2,%d)\n", clusters[2][t]);
                    }
                }
            }
        }

        // receives number of iterations per worker
        int *iter_per_worker = calloc(cluster2, sizeof(int));
        for (int i = 0; i < cluster2; i++) {
            MPI_Recv(&iter_per_worker[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }        

        // receives its corespondent part of the vector, sends it to its workers,
        // receives it modified and sends it back to the first cluster
        for (int i = 0; i < cluster2; i++) {
            MPI_Send(&iter_per_worker[i], 1, MPI_INT, clusters[2][i], 2, MPI_COMM_WORLD); 
            printf("M(2,%d)\n", clusters[2][i]);
            for (int j = 0; j < iter_per_worker[i]; j++) {
                int v;
                MPI_Recv(&v, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                MPI_Send(&v, 1, MPI_INT, clusters[2][i], 2, MPI_COMM_WORLD); 
                printf("M(2,%d)\n", clusters[2][i]);
                MPI_Recv(&v, 1, MPI_INT, clusters[2][i], clusters[2][i], MPI_COMM_WORLD, &status);
                MPI_Send(&v, 1, MPI_INT, 0, 2, MPI_COMM_WORLD); 
                printf("M(2,0)\n");
            }
        }

        fclose(fp);
    } else {   
        // receives the parent's rank     
        MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        // receives clusters dimensions
        MPI_Recv(&cluster0, 1, MPI_INT, value, value, MPI_COMM_WORLD, &status);
        MPI_Recv(&cluster1, 1, MPI_INT, value, value, MPI_COMM_WORLD, &status);
        MPI_Recv(&cluster2, 1, MPI_INT, value, value, MPI_COMM_WORLD, &status);
        
        clusters[0] = malloc(cluster0 * sizeof(int));
        clusters[1] = malloc(cluster1 * sizeof(int));
        clusters[2] = malloc(cluster2 * sizeof(int));

        // receives the topology
        for (int i = 0; i < 3; i++) {
            if (i == 0) {
                for (int j = 0; j < cluster0; j++) {
                    MPI_Recv(&clusters[i][j], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
   
                }
            } else if (i == 1) {
                for (int j = 0; j < cluster1; j++) {
                    MPI_Recv(&clusters[i][j], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                }
            } else {
                for (int j = 0; j < cluster2; j++) {
                    MPI_Recv(&clusters[i][j], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                }
            }
        }

        // prints the topology
        topology(rank, cluster0, cluster1, cluster2, clusters);

        // receives the number of elements that must be modified
        int iter;
        MPI_Recv(&iter, 1, MPI_INT, value, value, MPI_COMM_WORLD, &status);

        // receives the elements, modifies them and sends them back 
        for (int i = 0; i < iter; i++) {
            int v_new;
            MPI_Recv(&v_new, 1, MPI_INT, value, value, MPI_COMM_WORLD, &status);
            v_new *= 2;
            MPI_Send(&v_new, 1, MPI_INT, value, rank, MPI_COMM_WORLD); 
            printf("M(%d,%d)\n", rank, value);

        }
    }

    MPI_Finalize();
}