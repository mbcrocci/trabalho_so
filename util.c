#include "util.h"

void init_random_generator (void)
{
    srand ((unsigned) time (NULL));
}

int random_number (int min, int max)
{
    return min + rand () % (max-min+1);
}