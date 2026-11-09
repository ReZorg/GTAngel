/* simulated_annealing.c -- This belongs to gneural_network

   gneural_network is the GNU package which implements a programmable neural network.

   Copyright (C) 2016 Jean Michel Sellier
   <jeanmichel.sellier@gmail.com>
   Copyright (C) 2016 Francesco Maria Virlinzi
   <francesco.virlinzi@gmail.com>

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

// train the neural network by means of simulated annealing to optimize the weights

#include <simulated_annealing.h>
#include <randomize.h>
#include <rnd.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void simulated_annealing(network *nn, network_config *config) {
 int output = config->verbosity;	/* screen output - on/off */
 int mmax = config->mmax > 1 ? config->mmax : 2;	/* outer loop - number of effective temperature steps*/
 int nmax = config->nmax > 0 ? config->nmax : 1;	/* inner loop - number of test configurations */
 double kbtmin = config->kbtmin;	/* effective temperature minimum */
 double kbtmax = config->kbtmax;	/* effective temperature maximum */
 double eps = config->accuracy;
 int m, n, i, j, k;
 double err, e0, de, kbt, e_best;
 unsigned int nweights = network_weight_slots(nn);
 double *wbackup;
 double *wbest;

 wbackup = malloc(nweights * sizeof(*wbackup));
 wbest = malloc(nweights * sizeof(*wbest));
 if (!wbackup || !wbest) {
  free(wbackup);
  free(wbest);
  printf("SA: Not enough memory to allocate weight backup arrays\n");
  exit(-1);
 }

 e0 = error(nn, config);
 e_best = e0;
 for (k = 0, i = 0; i < nn->num_of_neurons; i++)
  for (j = 0; j < nn->neurons[i].num_input; j++, k++)
   wbest[k] = nn->neurons[i].w[j];

 for (m = 0; (m < mmax) && (e0 > eps); m++) {
  kbt = kbtmax - m * (kbtmax - kbtmin) / (mmax - 1);
  if (kbt <= 0.0) kbt = 1e-6;

  for (n = 0; (n < nmax) && (e0 > eps); n++) {
   for (k = 0, i = 0; i < nn->num_of_neurons; i++)
    for (j = 0; j < nn->neurons[i].num_input; j++, k++)
     wbackup[k] = nn->neurons[i].w[j];

   randomize(nn, config);
   err = error(nn, config);
   de = err - e0;
   if (de > 0.) {
    double p = exp(-de / kbt);
    if (rnd() < p) {
     e0 = err;
    } else {
     for (k = 0, i = 0; i < nn->num_of_neurons; i++)
      for (j = 0; j < nn->neurons[i].num_input; j++, k++)
       nn->neurons[i].w[j] = wbackup[k];
    }
   } else {
    e0 = err;
   }

   if (e_best > e0) {
    for (k = 0, i = 0; i < nn->num_of_neurons; i++)
     for (j = 0; j < nn->neurons[i].num_input; j++, k++)
      wbest[k] = nn->neurons[i].w[j];
    e_best = e0;
   }
  }
  if (output == ON)
   printf("SA: %d %g %g\n", m, kbt, e_best);
 }

 for (k = 0, i = 0; i < nn->num_of_neurons; i++)
  for (j = 0; j < nn->neurons[i].num_input; j++, k++)
   nn->neurons[i].w[j] = wbest[k];

 if (output == ON)
  printf("\n");

 free(wbackup);
 free(wbest);
}
