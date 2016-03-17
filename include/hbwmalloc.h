/* definitions copied from hwmalloc.3 man page of memkind */
int hbw_check_available(void);
void* hbw_malloc(size_t size);
void* hbw_calloc(size_t nmemb, size_t size);
void* hbw_realloc (void *ptr, size_t size);
void hbw_free(void *ptr);
int hbw_posix_memalign(void **memptr, size_t alignment, size_t size);
/*
int hbw_posix_memalign_psize(void **memptr, size_t alignment, size_t size, hbw_pagesize_t pagesize);
hbw_policy_t hbw_get_policy(void);
int hbw_set_policy(hbw_policy_t mode);
*/
