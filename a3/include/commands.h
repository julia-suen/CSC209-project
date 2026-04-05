#ifndef COMMANDS_H
#define COMMANDS_H

#include "protocol.h"
#include "common.h"

typedef enum {
    CMD_TEXT,
    CMD_NICK,
    CMD_JOIN,
    CMD_LEAVE,
    CMD_MSG,
    CMD_WHO,
    CMD_LIST,
    CMD_QUIT,
    CMD_HELP,
    CMD_INVALID
} CommandType;

typedef struct {
    CommandType type;
    char arg1[MAX_DEST]; /* USERNAME; ROOM; TARGET USER */
    char arg2[MAX_MSG]; /* MESSAGE BODY; EXTRA TEXT*/
} Command;

/* Initialize a Command struct to default values.*/
void init_command(Command *cmd);

/* Parse one line of user input into a Command.
 *  Returns:
 *    - 0 on success
 *    - (-1) on invalid command / parse error
 */
int parse_command(const char *input, Command *cmd);

/* Optional debugging helper */
void print_command(const Command *cmd);

int command_to_packet(const Command *cmd, Packet *pkt,
                      const char *usrid, const char *current_room);

#endif