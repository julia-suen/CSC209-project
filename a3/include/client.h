#ifndef CLIENT_H
#define CLIENT_H

#include "../include/protocol.h"

// Prompts user for username and sends it to server
// Returns 0 on success, -1 on failure or quit
int get_username(char *username, int soc);

// Handles user input from stdin, parses commands,
// and sends corresponding packets to server.
// Returns 1 if user quits, 0 to continue, -1 on error
int handle_user_input(int soc, const char *buf);

#endif
