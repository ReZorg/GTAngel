#include <math.h>
#include <stddef.h>
#include <thnn_host.h>

#define TH_GENERIC_FILE "THNN.h"
#include <THNN.h>

float *thnn_tensor_data(THTensor *t)
{
	if (!t || !t->storage || t->n < 1)
		return NULL;
	return t->storage;
}

void THNN_(Tanh_updateOutput)(THNNState *state, THTensor *input, THTensor *output)
{
	float *x = thnn_tensor_data(input);
	float *y = thnn_tensor_data(output);
	(void)state;
	if (x && y)
		*y = tanhf(*x);
}

void THNN_(Sigmoid_updateOutput)(THNNState *state, THTensor *input, THTensor *output)
{
	float *x = thnn_tensor_data(input);
	float *y = thnn_tensor_data(output);
	(void)state;
	if (x && y)
		*y = 1.f / (1.f + expf(-*x));
}

void THNN_(ELU_updateOutput)(THNNState *state, THTensor *input, THTensor *output,
		accreal alpha, bool inplace)
{
	float *x = thnn_tensor_data(input);
	float *y = inplace ? x : thnn_tensor_data(output);
	(void)state;
	if (!x || !y)
		return;
	*y = (*x > 0.f) ? *x : (float)alpha * (expf(*x) - 1.f);
}
