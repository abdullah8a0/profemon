#ifndef pairing_h
#define pairing_h

#define HOSTING_TIMEOUT_MS 5000
bool connect_wifi();
// TODO: work on using double mode for listen and broadcsat
bool broadcast(char *my_id);

bool listen(char *other_id);

void post_ids(char *my_id, char *other_id);

void sync_ids(char *my_id, char *game_id);

#endif