/* activation.c -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016 Jean Michel Sellier
   <jeanmichel.sellier@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// returns the specified activation function given the input x

#include <activation.h>
#include <thnn_host.h>
#define TH_GENERIC_FILE "THNN.h"
#include <THNN.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static double thnn_unary(void (*op)(THNNState *, THTensor *, THTensor *), double x)
{
	float in = (float)x;
	float out = 0.f;
	THTensor input = { &in, 1 };
	THTensor output = { &out, 1 };
	op(NULL, &input, &output);
	return out;
}

inline double activation(enum activation_function type,double x){
 switch (type) {
  case TANH:
   return thnn_unary(THNN_(Tanh_updateOutput), x);
   break;
  case EXP:
   return thnn_unary(THNN_(Sigmoid_updateOutput), x);
   break;
  case ID:
   return x;
   break;
  case POL1:
   return 1. + x;
   break;
  case POL2:
   return 1. + x + x * x ;
   break;
  case ELU: {
   float in = (float)x;
   float out = 0.f;
   THTensor input = { &in, 1 };
   THTensor output = { &out, 1 };
   THNN_(ELU_updateOutput)(NULL, &input, &output, 1.0, false);
   return out;
  }
   break;
  default:
   printf("unknown activation function!\n");
   exit(-1);
 }
}

