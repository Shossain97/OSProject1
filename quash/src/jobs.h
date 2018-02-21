#ifndef JOBS_H
#define JOBS_H

#include "processQueue.h"
typedef struct Job{
	processQueue* queue;
	int id;
	bool background;
}
