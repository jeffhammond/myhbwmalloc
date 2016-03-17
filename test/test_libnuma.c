#include <stdio.h>
#include <stdlib.h>

#include <numa.h>

int main(void)
{
    /* is libnuma available? */
    {
        int rc = numa_available();
        printf("%s returned %d\n", "numa_available", rc);
        if (rc == -1) abort();
    }

    /* see what is available */
    {
        int num = numa_num_configured_nodes();
        printf("%s returned %d\n", "numa_num_configured_nodes", num);

        int pref = numa_preferred();
        printf("%s returned %d\n", "numa_preferred", pref);
    }

    return 0;
}
