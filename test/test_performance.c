#include "hbwmallocconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if HAVE_MPI
#include <mpi.h>
#endif

#include "hbwmalloc.h"

static inline void barrier(void)
{
#if HAVE_MPI
    MPI_Barrier(MPI_COMM_WORLD);
#endif
}

static inline double wtime(void)
{
#if HAVE_MPI
    return MPI_Wtime();
#else
    return 0.0;
#endif
}

int main(int argc, char * argv[])
{
    int me = 0;

#ifdef HAVE_MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
#endif

    int avail = hbw_check_available();
    int consensus = avail;
#ifdef HAVE_MPI
    if (avail<0) avail = -avail; /* just look at absolute value since only zero is success */
    MPI_Allreduce(&avail, &consensus, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
#endif
    if (me==0) {
        printf("%s returned %s\n", "hbw_check_available", consensus==0 ? "SUCCESS" : "FAILURE");
    }
    assert(avail == 0);

    size_t bytes = (argc>1) ? atol(argv[1]) : 1024*1024;

    {
        char * a = calloc(bytes,1);
        assert(a != NULL);

        char * b = calloc(bytes,1);
        assert(b != NULL);

        barrier();
        double t0 = wtime();
        for (size_t i=0; i<bytes; i++) {
            b[i] = a[i];
        }
        barrier();
        double t1 = wtime();
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

        barrier();
        double t0 = wtime();
        for (size_t i=0; i<bytes; i++) {
            b[i] = a[i];
        }
        barrier();
        double t1 = wtime();
        double dt = t1-t0;
        if (me==0) {
            printf("hbw heap: dt = %e, bw = %e B/s\n", dt, bytes/dt);
        }

        hbw_free(b);
        hbw_free(a);
    }

#ifdef HAVE_MPI
    MPI_Finalize();
#endif

    return 0;
}
