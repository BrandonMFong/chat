/**
 * author: brando
 * date: 1/26/24
 */

#ifndef IO_HPP
#define IO_HPP

#include <typechatconfig.h>

typedef struct {
	int cd; // client socket descriptor
	ChatConfig * config;
} IOTools;

void IOIn(void * in);
void IOOut(void * in);

#endif // IO_HPP

