#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <mpi.h>

#include "hbwmalloc.h"

int main(int argc, char * argv[])
{
    MPI_Init(&argc, &argv);

    int me;
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    size_t bytes = (argc>1) ? atol(argv[1]) : 1024*1024;

    int avail = hbw_check_available();
    printf("%s returned %s\n", "hbw_check_available", avail==0 ? "SUCCESS" : "FAILURE");
    assert(avail == 0);

    {
        char * a = calloc(bytes,1);
        assert(a != NULL);

        char * b = calloc(bytes,1);
        assert(b != NULL);

        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();
        for (size_t i=0; i<bytes; i++) {
            b[i] = a[i];
        }
        MPI_Barrier(MPI_COMM_WORLD);
        double t1 = MPI_Wtime();
        double dt = t1-t0;
        if (me==0) {
            printf("def heap: dt = %e, bw = %e B/s\n", dt, bytes/dt);
        }

        free(b);
        free(a);
    }
    {
        char * a = hbw_calloc(bytes,1);
        assert(a != NULL);

        char * b = hbw_calloc(bytes,1);
        assert(b != NULL);

        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();
        for (size_t i=0; i<bytes; i++) {
            b[i] = a[i];
        }
        MPI_Barrier(MPI_COMM_WORLD);
        double t1 = MPI_Wtime();
        double dt = t1-t0;
        if (me==0) {
            printf("hbw heap: dt = %e, bw = %e B/s\n", dt, bytes/dt);
        }

        hbw_free(b);
        hbw_free(a);
    }

    MPI_Finalize();

    return 0;
}
