#include "util.h"

void init_random_generator (void)
{
    srand ((unsigned) time (NULL));
}

int random_number (int min, int max)
{
    return min + rand () % (max-min+1);
}
void clearScreen (void)
{
  const char* CLEAR_SCREE_ANSI = "\e[1;1H\e[2J";
  write (STDOUT_FILENO, CLEAR_SCREE_ANSI, 12);
}
