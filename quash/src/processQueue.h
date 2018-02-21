#ifndef PROCESSQUEUE_H
#define PROCESSQUEUE_H

#include "deque.h"
#include "command.h"
#include <stdio.h>
#include <unistd.h>

typedef struct process{
	char* cmd;
	pid_t pid;
}

typedef struct processQueue{
	process*