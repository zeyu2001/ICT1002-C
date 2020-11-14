#include "chat1002.h"
#include <stdlib.h>

void *my_alloc(size_t s)
{
    int result = rand() % 5;
    if (result == 0)
    {
        return NULL;
    }
    else
    {
        return calloc(s, 1);
    }
    
}