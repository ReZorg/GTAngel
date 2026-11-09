/* Types THNN.h names, sized for a one-neuron call. */
#ifndef THNN_HOST_H
#define THNN_HOST_H

#include <stdbool.h>
#include <stdint.h>

typedef double accreal;
typedef int64_t THIndex_t;

typedef struct THGenerator {
	unsigned long seed;
} THGenerator;

typedef struct THTensor {
	float *storage;
	long n;
} THTensor;

typedef THTensor THIndexTensor;
typedef THTensor THIntegerTensor;

typedef struct THNNState {
	int unused;
} THNNState;

#ifndef TH_API
#define TH_API
#endif

#define THNN_(NAME) THNN_float_ ## NAME

float *thnn_tensor_data(THTensor *t);

#endif
