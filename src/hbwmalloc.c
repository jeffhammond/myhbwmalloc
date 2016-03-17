#include <stdio.h>
#include <stdlib.h>

#include <errno.h> /* hbw_check_available() return codes */

#include <numa.h>

#ifdef HAVE_PTHREAD
#include <pthread.h> /* pthread_once */
#endif

#ifdef HAVE_MPI
#include <mpi.h>
#endif

#include "hbwmalloc.h"
#include "mspace.h"

/* INTERNAL API */

#ifdef HAVE_PTHREAD
/* see http://pubs.opengroup.org/onlinepubs/007908775/xsh/pthread_once.html */
pthread_once_t myhbwmalloc_once_control = PTHREAD_ONCE_INIT;
#endif

int myhbwmalloc_verbose;
int myhbwmalloc_hardfail;
int myhbwmalloc_numa_node;
size_t myhbwmalloc_slab_size;
void * myhbwmalloc_slab;
mspace myhbwmalloc_mspace;

static void myhbwmalloc_final(void)
{
    if (myhbwmalloc_mspace != NULL) {
        size_t bytes_avail = destroy_mspace(myhbwmalloc_mspace);
        if (myhbwmalloc_verbose) {
            printf("hbwmalloc: destroy_mspace returned = %zu\n", bytes_avail);
        }
    }
    if (myhbwmalloc_slab != NULL) {
        numa_free(myhbwmalloc_slab, myhbwmalloc_slab_size);
    }
}

static void myhbwmalloc_init(void)
{
    /* verbose printout? */
    myhbwmalloc_verbose = 0;
    {
        char * env_char = getenv("HBWMALLOC_VERBOSE");
        if (env_char != NULL) {
            myhbwmalloc_verbose = 1;
        }
    }

    /* fail hard or soft? */
    myhbwmalloc_hardfail = 1;
    {
        char * env_char = getenv("HBWMALLOC_SOFTFAIL");
        if (env_char != NULL) {
            myhbwmalloc_hardfail = 0;
        }
    }

    /* detect and configure use of NUMA memory nodes */
    {
        int max_numa_nodes = numa_max_node();
        /* FIXME this is a hack.  assumes HBW is only numa node 1. */
        if (max_numa_nodes == 2) {
            myhbwmalloc_numa_node = 1;
        } else {
            fprintf(stderr,"hbwmalloc: we support only 2 numa nodes, not %d\n", max_numa_nodes);
        }

        if (myhbwmalloc_verbose) {
            for (int i=0; i<max_numa_nodes; i++) {
                unsigned max_numa_cpus = numa_num_configured_cpus();
                struct bitmask * mask = numa_bitmask_alloc( max_numa_cpus );
                int rc = numa_node_to_cpus(i, mask);
                if (rc != 0) {
                    fprintf(stderr, "hbwmalloc: numa_node_to_cpus failed\n");
                } else {
                    for (unsigned j=0; j<max_numa_cpus; j++) {
                        int bit = numa_bitmask_isbitset(mask,j);
                        printf("hbwmalloc: numa node %d cpu %d = %d", i, j, bit);
                    }
                }
                numa_bitmask_free(mask);
            }
            fflush(stdout);
        }
    }

#if 0 /* unused */
    /* see if the user specifies a slab size */
    size_t slab_size_requested = 0;
    {
        char * env_char = getenv("HBWMALLOC_slab_size_requested");
        if (env_char!=NULL) {
            long units = 1L;
            if      ( NULL != strstr(env_char,"G") ) units = 1000000000L;
            else if ( NULL != strstr(env_char,"M") ) units = 1000000L;
            else if ( NULL != strstr(env_char,"K") ) units = 1000L;
            else                                     units = 1L;

            int num_count = strspn(env_char, "0123456789");
            memset( &env_char[num_count], ' ', strlen(env_char)-num_count);
            slab_size_requested = units * atol(env_char);
        }
        if (myhbwmalloc_verbose) {
            printf("hbwmalloc: requested slab_size_requested = %zu\n", slab_size_requested);
        }
    }
#endif

    /* see what libnuma says is available */
    size_t myhbwmalloc_slab_size;
    {
        int node = myhbwmalloc_numa_node;
        long long freemem;
        long long maxmem = numa_node_size64(node, &freemem);
        if (myhbwmalloc_verbose) {
            printf("hbwmalloc: numa_node_size64 says maxmem=%lld freemem=%lld for numa node %d\n", 
                    maxmem, freemem, node);
        }
        myhbwmalloc_slab_size = freemem;
    }

    /* assume threads, disable if MPI knows otherwise, then allow user to override. */
    int multithreaded = 1;
#ifdef HAVE_MPI
    int nprocs;
    {
        int is_init, is_final;
        MPI_Initialized(&is_init);
        MPI_Finalized(&is_final);
        if (is_init && !is_final) {
            MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
        }

        /* give equal portion to every MPI process */
        myhbwmalloc_slab_size /= nprocs;

        /* if the user initializes MPI with MPI_Init or
         * MPI_Init_thread(MPI_THREAD_SINGLE), they assert there
         * are no threads at all, which means we can skip the
         * malloc mspace lock.
         *
         * if the user lies to MPI, they deserve any bad thing
         * that comes of it. */
        int provided;
        MPI_Query_thread(&provided);
        if (provided==MPI_THREAD_SINGLE) {
            multithreaded = 0;
        } else {
            multithreaded = 1;
        }

        if (myhbwmalloc_verbose) {
            printf("hbwmalloc: MPI processes = %d (threaded = %d)\n", nprocs, multithreaded);
            printf("hbwmalloc: myhbwmalloc_slab_size = %d\n", myhbwmalloc_slab_size);
        }
    }
#endif

    /* user can assert that hbwmalloc and friends need not be thread-safe */
    {
        char * env_char = getenv("HBWMALLOC_LOCKLESS");
        if (env_char != NULL) {
            multithreaded = 0;
            if (myhbwmalloc_verbose) {
                printf("hbwmalloc: user has disabled locking in mspaces by setting HBWMALLOC_LOCKLESS\n");
            }
        }
    }

    /* set to NULL before trying to initialize.  if we return before
     * successful creation of the mspace, then it will still be NULL,
     * and we can use that in subsequent library calls to determine
     * that the library failed to initialize. */
    myhbwmalloc_mspace = NULL;

    /* set the atexit handler that will destroy the mspace and free the numa allocation */
    atexit(myhbwmalloc_final);

    myhbwmalloc_slab = numa_alloc_onnode( myhbwmalloc_slab_size, myhbwmalloc_numa_node);
    if (myhbwmalloc_slab==NULL) {
        fprintf(stderr, "hbwmalloc: numa_alloc_onnode returned NULL\n");
        return;
    } else {
        /* part (less than 128*sizeof(size_t) bytes) of this space is used for bookkeeping,
         * so the capacity must be at least this large */
        if (myhbwmalloc_slab_size < 128*sizeof(size_t)) {
            fprintf(stderr, "hbwmalloc: not enough space for mspace bookkeeping\n");
            return;
        }

        /* see above regarding if the user lies to MPI. */
        int locked = multithreaded;
        myhbwmalloc_mspace = create_mspace_with_base( myhbwmalloc_slab, myhbwmalloc_slab_size, locked);
        if (myhbwmalloc_mspace == NULL) {
            fprintf(stderr, "hbwmalloc: create_mspace_with_base returned NULL\n");
            return;
        }
    }
}


/* PUBLIC API */

/*
 *
 *
 * Before any other calls in this library can be used numa_available() must be called.
 * */
int hbw_check_available(void)
{
    int rc = numa_available();
    if (rc != 0) {
        fprintf(stderr, "hbwmalloc: libnuma error = %d\n", rc);
        return ENODEV; /* ENODEV if high-bandwidth memory is unavailable. */
    }

#ifdef HAVE_PTHREAD
    /* this ensures that initializing within hbw_check_available()
     * can be thread-safe. */
    pthread_once( &myhbwmalloc_once_control, myhbwmalloc_init );
#else
    myhbwmalloc_init();
#endif

    /* FIXME need thread barrier here to be thread-safe */

    if (myhbwmalloc_mspace == NULL) {
        fprintf(stderr, "hbwmalloc: mspace creation failed\n");
        return ENODEV; /* ENODEV if high-bandwidth memory is unavailable. */
    }

    return 0;
}

void* hbw_malloc(size_t size)
{
    if (myhbwmalloc_mspace == NULL && !myhbwmalloc_hardfail) {
        fprintf(stderr, "hbwmalloc: mspace invalid - allocating from default heap\n");
        return malloc(size);
    }
    return mspace_malloc(myhbwmalloc_mspace, size);
}

void* hbw_calloc(size_t nmemb, size_t size)
{
    if (myhbwmalloc_mspace == NULL && !myhbwmalloc_hardfail) {
        fprintf(stderr, "hbwmalloc: mspace invalid - allocating from default heap\n");
        return calloc(nmemb, size);
    }
    return mspace_calloc(myhbwmalloc_mspace, nmemb, size);
}

void* hbw_realloc (void *ptr, size_t size)
{
    if (myhbwmalloc_mspace == NULL && !myhbwmalloc_hardfail) {
        fprintf(stderr, "hbwmalloc: mspace invalid - allocating from default heap\n");
        return realloc(ptr, size);
    }
    return mspace_realloc(myhbwmalloc_mspace, ptr, size);
}

void hbw_free(void *ptr)
{
    if (myhbwmalloc_mspace == NULL && !myhbwmalloc_hardfail) {
        fprintf(stderr, "hbwmalloc: mspace invalid - allocating from default heap\n");
        return free(ptr);
    }
    mspace_free(myhbwmalloc_mspace, ptr);
}

int hbw_posix_memalign(void **memptr, size_t alignment, size_t size)
{
    if (myhbwmalloc_mspace == NULL && !myhbwmalloc_hardfail) {
        fprintf(stderr, "hbwmalloc: mspace invalid - allocating from default heap\n");
        return posix_memalign(memptr, alignment, size);
    }
    *memptr = mspace_memalign(myhbwmalloc_mspace, alignment, size);
    return (*memptr == NULL) ? -1 : 0;
}

