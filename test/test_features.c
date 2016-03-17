#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hbwmalloc.h"

int main(int argc, char * argv[])
{
    size_t bytes = (argc>1) ? atol(argv[1]) : 1024*1024;

    int avail = hbw_check_available();
    printf("%s returned %s\n", "hbw_check_available", avail==0 ? "SUCCESS" : "FAILURE");
    assert(avail == 0);

    void * a = hbw_malloc(bytes);
    assert(a != NULL);
    hbw_free(a);

    void * b = hbw_calloc(bytes, 1);
    assert(b != NULL);
    hbw_free(b);

    void * c = hbw_realloc(NULL, bytes);
    printf("hbw_realloc c=%p\n", c);
    assert(c != NULL);

    void * d = hbw_realloc(c, 0);
    printf("hbw_realloc c=%p d=%p\n", c, d);

    void * e = NULL;
    int rc = hbw_posix_memalign(&e, 4096, bytes);
    printf("hbw_posix_memalign rc=%d e=%p\n", rc, e);
    assert(rc == 0 && e != NULL);

    return 0;
}
