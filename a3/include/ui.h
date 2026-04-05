#ifndef UI_H
#define UI_H

#include "protocol.h"

void print_system_message(const char *msg);
void print_error_message(const char *msg);
void print_chat_message(const char *user, const char *msg);
void print_dm_message(const char *user, const char *msg);
void print_help_menu(void);
void print_prompt(void);
void display_packet(const Packet *pkt);

#endif