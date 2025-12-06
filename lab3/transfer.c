#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2)
    {
        if (world_rank == 0)
        {
            printf("This program requires at least 2 MPI processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    if (world_rank == 0)
    {
        char *filename = (argc > 1) ? argv[1] : "test_mpi.txt";

        FILE *fp = fopen(filename, "rb");
        if (!fp)
        {
            fp = fopen(filename, "w");
            if (fp)
            {
                fprintf(fp, "This is a dummy file for MPI transfer.");
                fclose(fp);
                fp = fopen(filename, "rb");
            }
        }

        if (fp)
        {
            fseek(fp, 0, SEEK_END);
            long filesize = ftell(fp);
            rewind(fp);

            char *file_content = (char *)malloc(filesize);
            if (file_content)
            {
                fread(file_content, 1, filesize, fp);

                MPI_Send(filename, strlen(filename) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

                MPI_Send(file_content, filesize, MPI_CHAR, 1, 1, MPI_COMM_WORLD);

                free(file_content);
            }
            fclose(fp);
        }
    }
    else if (world_rank == 1)
    {
        MPI_Status status;
        int count;

        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &count);

        char *filename = (char *)malloc(count);
        MPI_Recv(filename, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &count);

        char *file_content = (char *)malloc(count);
        MPI_Recv(file_content, count, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        char save_name[256];
        snprintf(save_name, sizeof(save_name), "received_%s", filename);

        FILE *fp = fopen(save_name, "wb");
        if (fp)
        {
            fwrite(file_content, 1, count, fp);
            fclose(fp);
        }

        free(filename);
        free(file_content);
    }

    MPI_Finalize();
    return 0;
}