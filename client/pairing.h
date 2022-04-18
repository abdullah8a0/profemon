#ifndef pairing_h
#define pairing_h
#include "util.h"

bool connect_wifi();

bool broadcast(char *my_id);

bool listen(char *other_id);

void post_ids(char *my_id, char *other_id)
#endif