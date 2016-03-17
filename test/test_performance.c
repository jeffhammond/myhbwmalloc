#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <mpi.h>

#include "hbwmalloc.h"

int main(int argc, char * argv[])
{
    MPI_Init(&argc, &argv);

    size_t bytes = (argc>1) ? atol(argv[1]) : 1024*1024;

    int avail = hbw_check_available();
    printf("%s returned %s\n", "hbw_check_available", avail==0 ? "SUCCESS" : "FAILURE");
    assert(avail == 0);

    char * a = hbw_calloc(bytes,1);
    assert(a != NULL);

    char * b = hbw_calloc(bytes,1);
    assert(b != NULL);

    double t0 = MPI_Wtime();
    for (size_t i=0; i<bytes; i++) {
        b[i] = a[i];
    }
    double t1 = MPI_Wtime();
    double dt = t1-t0;
    printf("dt = %e, bw = %e B/s\n", dt, bytes/dt);

    hbw_free(b);
    hbw_free(a);

    MPI_Finalize();

    return 0;
}
