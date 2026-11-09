#include "lucy_persona.h"
#include "network.h"
#include "feedforward.h"
#include "error.h"
#include "save.h"
#include "load.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#define HID LUCY_PERSONA_HID

struct lucy_persona {
	network *nn;
	network_config *cfg;
	int npoints;
	int cursor;
	double *snap;
	int snap_n;
	int window;
	int membrane_mode;
	double last_gate[HID];
	double last_up[HID];
	double last_mem[HID];
};

/* 9 structural roles discovered by k-means on 12 motif features (scm/shape.scm) */
static const char *const kRoleNames[LUCY_PERSONA_ROLES] = {
	"quiet",
	"writers",
	"up-readers",
	"gate-readers",
	"incoherent-sinks",
	"broadcasters",
	"convergence-sinks",
	"L0-eraser-hubs",
	"coherent-relays"
};

/* 8 feed-forward 3-node loop motif classes over (AB,BC,AC) in {gate,up}^3 (scm/shape.scm) */
static const char *const kMotifNames[LUCY_PERSONA_MOTIFS] = {
	"gate,gate,gate",
	"gate,gate,up",
	"gate,up,gate",
	"gate,up,up",
	"up,gate,gate",
	"up,gate,up",
	"up,up,gate",
	"up,up,up"
};

/* 16 unusual probe questions from scm/shape.scm */
static const char *const kUnusualProbes[LUCY_PERSONA_PROBES] = {
	"If the number seven fell asleep, what would it dream of?",
	"What does silence weigh when a room is empty?",
	"Why do mirrors forget faces the moment you walk away?",
	"How would a river describe the ocean if it had only heard rumours?",
	"What colour is the pause between two heartbeats?",
	"If gravity took a holiday on Tuesday, what would nostalgia do?",
	"Can a shadow outlive the candle that cast it?",
	"What is the square root of a Tuesday?",
	"How do forgotten words spend their winters?",
	"If music had bones, which chord would be the spine?",
	"What does an echo think about while waiting to be called?",
	"Why is the horizon always one step further than yesterday's promise?",
	"If a stone could envy, what would it envy about the wind?",
	"What tastes more like regret: cold tea or an unsent letter?",
	"Where does the lap go when you stand up, and does it miss you?",
	"If time were a fabric, which century had the roughest seams?"
};

/* 16 ordinary probe questions from scm/shape.scm */
static const char *const kOrdinaryProbes[LUCY_PERSONA_PROBES] = {
	"What is the capital of France?",
	"How does a steam engine convert heat into motion?",
	"Why does the sky appear blue during the day?",
	"What are the main ideas of Plato's Republic?",
	"How do plants turn sunlight into sugar?",
	"What causes the tides to rise and fall?",
	"Who wrote Hamlet, and what is it about?",
	"How does a compass point north?",
	"Why does ice float on liquid water?",
	"What is the difference between an artery and a vein?",
	"How do vaccines train the immune system?",
	"Why do seasons change over the course of a year?",
	"What does a Prime Minister do in a parliamentary system?",
	"How is glass made from sand?",
	"Why does bread rise when yeast is added?",
	"What happens to light when it passes through a prism?"
};

/* Representative separating/concept neurons in Qwen3-1.7B for the 8 GQA groups */
static const int kGroupLayers[LUCY_PERSONA_HID]  = {15, 12,  1, 11, 16, 19, 22, 27};
static const int kGroupNeurons[LUCY_PERSONA_HID] = {2583, 4154, 5604, 5743, 1875, 3963, 2246, 1023};

static double silu(double x)
{
	return x / (1.0 + exp(-x));
}

const char *lucy_role_name(int role)
{
	if (role < 0 || role >= LUCY_PERSONA_ROLES) return "unclassified";
	return kRoleNames[role];
}

const char *lucy_motif_name(int port_class)
{
	if (port_class < 0 || port_class >= LUCY_PERSONA_MOTIFS) return "unknown";
	return kMotifNames[port_class];
}

const char *lucy_probe_question(int unusual, int idx)
{
	if (idx < 0 || idx >= LUCY_PERSONA_PROBES) return "";
	return unusual ? kUnusualProbes[idx] : kOrdinaryProbes[idx];
}

int lucy_moses_cross_lingual(int layer, int group)
{
	if (group < 0) group = 0;
	if (group > 7) group = group % 8;
	return layer > (9 + group * (7 - group)) ? 1 : 0;
}

static void probe_featurize(const char *text, int is_unusual, int idx, double obs[LUCY_PERSONA_IN])
{
	int n = 0, words = 0, upper = 0, vowels = 0, digits = 0, punct = 0, q = 0, inword = 0;
	int metaphoric = 0;
	const char *s;
	static const char *const kUnusualLex[] = {
		"dream", "silence", "mirror", "shadow", "echo", "horizon", "envy",
		"regret", "asleep", "colour", "bones", "winter", "tuesday", "fabric", NULL
	};
	int k;

	for (s = text ? text : ""; *s; s++) {
		unsigned char ch = (unsigned char)*s;
		n++;
		if (ch == '?') q++;
		if (isupper(ch)) upper++;
		if (isdigit(ch)) digits++;
		if (strchr("aeiouAEIOU", ch)) vowels++;
		if (ispunct(ch)) punct++;
		if (isspace(ch)) inword = 0;
		else if (!inword) { words++; inword = 1; }
	}
	for (k = 0; kUnusualLex[k]; k++) {
		if (text && strcasestr(text, kUnusualLex[k])) metaphoric++;
	}
	obs[0] = tanh(n / 48.0);
	obs[1] = is_unusual ? tanh(0.45 + 0.35 * metaphoric + 0.04 * (idx % 5))
	                    : -tanh(0.40 + 0.05 * (idx % 5));
	obs[2] = q ? tanh(0.55 + 0.25 * metaphoric) : 0.0;
	obs[3] = tanh(words / 11.0);
	obs[4] = is_unusual ? -0.35 - 0.03 * (idx % 4) : 0.45 + 0.03 * (idx % 4);
	obs[5] = n ? (double)vowels / n : 0.0;
	obs[6] = is_unusual ? tanh(0.30 * metaphoric + 0.1 * (idx % 3)) : -0.25;
	obs[7] = tanh((punct + (is_unusual ? 2.0 : -1.0)) / 4.0);
}
static void wire(network *nn)
{
	const int n_in = LUCY_PERSONA_IN;
	const int n_hid = HID;
	const int n_out = LUCY_PERSONA_OUT;
	const int total = n_in + n_hid + n_out;
	int i, j;
	network_set_neuron_number(nn, (unsigned long)total);
	network_set_layer_number(nn, 3);
	nn->layers[0].num_of_neurons = n_in;
	nn->layers[0].neurons = &nn->neurons[0];
	for (i = 0; i < n_in; i++) {
		network_neuron_set_connection_number(&nn->neurons[i], 1);
		nn->neurons[i].connection[0] = &nn->neurons[i];
		nn->neurons[i].w[0] = 0.0;
	}
	nn->layers[1].num_of_neurons = n_hid;
	nn->layers[1].neurons = &nn->neurons[n_in];
	for (i = 0; i < n_hid; i++) {
		neuron *ne = &nn->neurons[n_in + i];
		network_neuron_set_connection_number(ne, n_in);
		ne->activation = TANH;
		ne->discriminant = LINEAR;
		for (j = 0; j < n_in; j++) {
			ne->connection[j] = &nn->neurons[j];
			ne->w[j] = 0.05 * ((i * 3 + j * 5) % 7 - 3);
		}
	}
	nn->layers[2].num_of_neurons = n_out;
	nn->layers[2].neurons = &nn->neurons[n_in + n_hid];
	for (i = 0; i < n_out; i++) {
		neuron *ne = &nn->neurons[n_in + n_hid + i];
		network_neuron_set_connection_number(ne, n_hid);
		ne->activation = TANH;
		ne->discriminant = LINEAR;
		for (j = 0; j < n_hid; j++) {
			ne->connection[j] = &nn->neurons[n_in + j];
			ne->w[j] = 0.05 * ((i * 2 + j) % 5 - 2);
		}
	}
}
lucy_persona *lucy_persona_new(void)
{
	lucy_persona *p = calloc(1, sizeof(*p));
	if (!p)
		return NULL;
	p->nn = network_alloc();
	p->cfg = network_config_alloc_default();
	if (!p->nn || !p->cfg) {
		lucy_persona_free(p);
		return NULL;
	}
	wire(p->nn);
	p->cfg->optimization_type = GRADIENT_DESCENT;
	p->cfg->error_type = L2;
	p->cfg->verbosity = 0;
	p->cfg->nxw = 400; /* delta = (wmax-wmin)/nxw, local probe */
	p->cfg->maxiter = 8;
	p->cfg->gamma = 0.25;
	p->cfg->accuracy = 1e-4;
	p->cfg->wmin = -2.0;
	p->cfg->wmax = 2.0;
	p->cfg->rate = 0.05;
	p->window = 8;
	return p;
}

void lucy_persona_reset(lucy_persona *p)
{
	if (!p || !p->nn) return;
	wire(p->nn);
	p->npoints = 0;
	p->cursor = 0;
	if (p->cfg) p->cfg->num_points = 0;
}

void lucy_persona_set_window(lucy_persona *p, int n)
{
	if (!p) return;
	if (n < 1) n = 1;
	if (n > MAX_TRAINING_POINTS) n = MAX_TRAINING_POINTS;
	p->window = n;
	if (p->npoints > n) p->npoints = n;
	if (p->cursor >= n) p->cursor = 0;
	if (p->cfg) p->cfg->num_points = p->npoints;
}
int lucy_persona_get_window(const lucy_persona *p)
{
	return p ? p->window : 0;
}
int lucy_persona_get_num_points(const lucy_persona *p)
{
	return p ? p->npoints : 0;
}
void lucy_persona_configure(lucy_persona *p, int method, int maxiter, int nmax,
                            double gamma, double accuracy, double wmin, double wmax, double rate)
{
	if (!p)
		return;
	if (method < 0 || method > MSMCO)
		method = GRADIENT_DESCENT;
	p->cfg->optimization_type = (enum optimization_method)method;
	p->cfg->maxiter = maxiter > 0 ? maxiter : 4;
	p->cfg->nmax = nmax > 0 ? nmax : 8;
	p->cfg->mmax = maxiter > 0 ? maxiter : 4;
	p->cfg->npop = 6;
	p->cfg->nxw = 400; /* delta = (wmax-wmin)/nxw, local probe */
	p->cfg->gamma = gamma > 0 ? gamma : 0.12;
	p->cfg->accuracy = accuracy > 0 ? accuracy : 1e-4;
	p->cfg->wmin = wmin < wmax ? wmin : -2.0;
	p->cfg->wmax = wmax > wmin ? wmax : 2.0;
	p->cfg->rate = rate > 0 ? rate : 0.05;
	p->cfg->kbtmin = 0.005;
	p->cfg->kbtmax = 0.25;
	p->cfg->verbosity = 0;
}
void lucy_persona_free(lucy_persona *p)
{
	if (!p)
		return;
	free(p->snap);
	network_free(p->nn);
	network_config_free(p->cfg);
	free(p);
}
void lucy_persona_remember(lucy_persona *p, const double obs[LUCY_PERSONA_IN], const double target[LUCY_PERSONA_OUT])
{
	int window;
	int slot;
	int i;
	neuron *out;
	if (!p || !obs || !target) return;
	window = p->window > 0 ? p->window : 1;
	if (window > MAX_TRAINING_POINTS) window = MAX_TRAINING_POINTS;
	if (network_config_ensure(p->cfg, p->nn->num_of_neurons, LUCY_PERSONA_IN) < 0)
		return;
	slot = p->cursor % window;
	p->cursor = (slot + 1) % window;
	if (p->npoints < window)
		p->npoints++;
	p->cfg->num_points = p->npoints;
	for (i = 0; i < LUCY_PERSONA_IN; i++)
		NET_PX(p->cfg, slot, p->nn->neurons[i].global_id, 0) = obs[i];
	out = p->nn->layers[2].neurons;
	for (i = 0; i < LUCY_PERSONA_OUT; i++)
		NET_PY(p->cfg, slot, out[i].global_id) = target[i];
}
double lucy_persona_error(lucy_persona *p)
{
	if (!p || !p->nn || !p->cfg || p->cfg->num_points < 1)
		return 0.0;
	return error(p->nn, p->cfg) / sqrt((double)p->cfg->num_points);
}
static void read_knobs(lucy_persona *p, const double *obs, double *knobs)
{
	int i, j;
	for (i = 0; i < LUCY_PERSONA_IN; i++)
		p->nn->layers[0].neurons[i].output = obs[i];
	feedforward(p->nn);
	/* Also compute membrane SwiGLU gate/up/mem state for each hidden neuron */
	for (i = 0; i < HID; i++) {
		neuron *ne = &p->nn->layers[1].neurons[i];
		double g = 0.0, u = 0.0;
		for (j = 0; j < LUCY_PERSONA_IN; j++) {
			double w = ne->w[j];
			if ((j & 1) == 0) g += w * obs[j];
			else u += w * obs[j];
			/* Full projection uses dual parity-split + cross coupling */
			g += 0.5 * w * obs[j];
			u += ((j & 1) ? 0.5 : -0.25) * w * obs[j];
		}
		p->last_gate[i] = silu(g);
		p->last_up[i] = u;
		p->last_mem[i] = p->last_gate[i] * p->last_up[i];
	}
	if (p->membrane_mode) {
		for (i = 0; i < LUCY_PERSONA_OUT; i++) {
			neuron *out_ne = &p->nn->layers[2].neurons[i];
			double sum = 0.0;
			for (j = 0; j < HID; j++) {
				/* Blend tanh feedforward with SwiGLU membrane output */
				double h_eff = 0.75 * p->nn->layers[1].neurons[j].output +
				               0.25 * tanh(p->last_mem[j]);
				sum += out_ne->w[j] * h_eff;
			}
			knobs[i] = tanh(sum);
		}
	} else {
		for (i = 0; i < LUCY_PERSONA_OUT; i++)
			knobs[i] = p->nn->layers[2].neurons[i].output;
	}
}

static int weight_count(network *nn)
{
	int i, n = 0;
	for (i = 0; i < (int)nn->num_of_neurons; i++)
		n += nn->neurons[i].num_input;
	return n;
}
static void copy_weights(network *nn, double *dst)
{
	int i, j, k = 0;
	for (i = 0; i < (int)nn->num_of_neurons; i++)
		for (j = 0; j < nn->neurons[i].num_input; j++)
			dst[k++] = nn->neurons[i].w[j];
}
static void paste_weights(network *nn, const double *src)
{
	int i, j, k = 0;
	for (i = 0; i < (int)nn->num_of_neurons; i++)
		for (j = 0; j < nn->neurons[i].num_input; j++)
			nn->neurons[i].w[j] = src[k++];
}
void lucy_persona_checkpoint(lucy_persona *p)
{
	int n;
	if (!p) return;
	n = weight_count(p->nn);
	if (n != p->snap_n) {
		free(p->snap);
		p->snap = calloc((size_t)n, sizeof(double));
		p->snap_n = n;
	}
	if (p->snap) copy_weights(p->nn, p->snap);
}
void lucy_persona_restore(lucy_persona *p)
{
	if (p && p->snap) paste_weights(p->nn, p->snap);
}
int lucy_persona_on_rail(const double knobs[LUCY_PERSONA_OUT])
{
	int i, n = 0;
	for (i = 0; i < LUCY_PERSONA_OUT; i++)
		if (fabs(knobs[i]) > 0.95) n++;
	return n >= 3;
}
int lucy_persona_save(lucy_persona *p, const char *path)
{
	FILE *f;
	if (!p || !path) return -1;
	f = fopen(path, "w");
	if (!f) return -1;
	fclose(f);
	p->cfg->save_network_file_name = strdup(path);
	network_save(p->nn, p->cfg);
	free(p->cfg->save_network_file_name);
	p->cfg->save_network_file_name = NULL;
	return 0;
}
int lucy_persona_load(lucy_persona *p, const char *path)
{
	FILE *f;
	int i;
	if (!p || !path) return -1;
	f = fopen(path, "r");
	if (!f) return -1;
	fclose(f);
	p->cfg->load_network_file_name = strdup(path);
	network_load(p->nn, p->cfg);
	free(p->cfg->load_network_file_name);
	p->cfg->load_network_file_name = NULL;
	/* ensure input layer self-weights stay 0 so feedforward doesn't overwrite inputs */
	if (p->nn->num_of_layers >= 1) {
		for (i = 0; i < (int)p->nn->layers[0].num_of_neurons; i++)
			if (p->nn->layers[0].neurons[i].num_input > 0)
				p->nn->layers[0].neurons[i].w[0] = 0.0;
	}
	return 0;
}
void lucy_persona_forward(lucy_persona *p, const double obs[LUCY_PERSONA_IN], double knobs[LUCY_PERSONA_OUT])
{
	if (!p || !obs || !knobs) return;
	read_knobs(p, obs, knobs);
}
static void restore_input_weights(network *nn)
{
	int i;
	if (!nn || nn->num_of_layers < 1) return;
	for (i = 0; i < (int)nn->layers[0].num_of_neurons; i++)
		if (nn->layers[0].neurons[i].num_input > 0)
			nn->layers[0].neurons[i].w[0] = 0.0;
}
double lucy_persona_step(lucy_persona *p, const double obs[LUCY_PERSONA_IN],
                         const double target[LUCY_PERSONA_OUT], double knobs[LUCY_PERSONA_OUT])
{
	int i, n;
	double err = 0.0, before_err;
	double *pre = NULL;
	if (!p || !obs || !target || !knobs) return 0.0;
	lucy_persona_remember(p, obs, target);
	restore_input_weights(p->nn);
	n = weight_count(p->nn);
	pre = calloc((size_t)n, sizeof(double));
	if (pre) copy_weights(p->nn, pre);
	before_err = error(p->nn, p->cfg);

	if (p->cfg->optimization_type == GRADIENT_DESCENT) {
		network_run_algorithm(p->nn, p->cfg);
		restore_input_weights(p->nn);
	} else {
		/* For stochastic optimizers (SA, GA, MSMCO, Random), search within a bounded
		   box and follow up with local gradient polish */
		int method = p->cfg->optimization_type;
		int orig_maxiter = p->cfg->maxiter;
		int orig_nmax = p->cfg->nmax;
		double orig_wmin = p->cfg->wmin, orig_wmax = p->cfg->wmax;
		double span = 0.28;
		p->cfg->wmin = -span;
		p->cfg->wmax = span;
		if (method == SIMULATED_ANNEALING) {
			if (p->cfg->mmax < 2) p->cfg->mmax = 2;
		}
		network_run_algorithm(p->nn, p->cfg);
		restore_input_weights(p->nn);
		if (pre && error(p->nn, p->cfg) > before_err) {
			paste_weights(p->nn, pre);
		}
		p->cfg->wmin = orig_wmin;
		p->cfg->wmax = orig_wmax;
		p->cfg->nmax = orig_nmax;
		p->cfg->optimization_type = GRADIENT_DESCENT;
		p->cfg->maxiter = orig_maxiter > 2 ? orig_maxiter : 3;
		network_run_algorithm(p->nn, p->cfg);
		restore_input_weights(p->nn);
		p->cfg->optimization_type = (enum optimization_method)method;
		p->cfg->maxiter = orig_maxiter;
	}

	if (pre) {
		if (error(p->nn, p->cfg) > before_err)
			paste_weights(p->nn, pre);
		free(pre);
	}

	read_knobs(p, obs, knobs);
	for (i = 0; i < LUCY_PERSONA_OUT; i++) {
		double d = knobs[i] - target[i];
		err += d * d;
	}
	return sqrt(err);
}

void lucy_persona_set_membrane(lucy_persona *p, int enable)
{
	if (p) p->membrane_mode = enable ? 1 : 0;
}

int lucy_persona_get_membrane(const lucy_persona *p)
{
	return p ? p->membrane_mode : 0;
}

void lucy_persona_membrane_forward(lucy_persona *p, const double obs[LUCY_PERSONA_IN],
                                   double hid_out[LUCY_PERSONA_HID], double knobs[LUCY_PERSONA_OUT])
{
	int i, saved;
	if (!p || !obs || !knobs) return;
	saved = p->membrane_mode;
	p->membrane_mode = 1;
	read_knobs(p, obs, knobs);
	p->membrane_mode = saved;
	if (hid_out) {
		for (i = 0; i < HID; i++)
			hid_out[i] = p->last_mem[i];
	}
}

static void add_link(lucy_shape_report *rep, int sl, int si, int dl, int di,
                     int port, double w, double std_layer)
{
	lucy_direct_link *lk;
	double z;
	if (rep->n_links >= 128) return;
	lk = &rep->links[rep->n_links++];
	lk->src_layer = sl;
	lk->src_idx = si;
	lk->dst_layer = dl;
	lk->dst_idx = di;
	lk->port = port;
	lk->span = dl - sl;
	lk->weight = w;
	z = std_layer > 1e-9 ? fabs(w) / std_layer : fabs(w) * 10.0;
	lk->z = z;
	lk->strength = 0.5 * (1.0 + tanh(w * 4.0));
	lk->conf = (z * z) / (z * z + 8.0);
	lk->sign = w >= 0.0 ? 1 : -1;
}

static int classify_role(const int feat[12], double cos_ud)
{
	int a_tot = feat[0] + feat[1];
	int b_tot = feat[2] + feat[3];
	int c_tot = feat[4] + feat[5];
	int inc_tot = feat[1] + feat[3] + feat[5];
	int coh_tot = feat[0] + feat[2] + feat[4];
	int in_gate = feat[6] + feat[8];
	int in_up   = feat[7] + feat[9];
	int out_tot = feat[10] + feat[11];

	if (a_tot + b_tot + c_tot + in_gate + in_up + out_tot == 0)
		return 0; /* quiet */
	if (cos_ud < -0.25 && inc_tot >= coh_tot && out_tot >= 2)
		return 7; /* L0-eraser-hubs */
	if (feat[5] > feat[4] && c_tot >= a_tot)
		return 4; /* incoherent-sinks */
	if (coh_tot > inc_tot && b_tot >= 2)
		return 8; /* coherent-relays */
	if (out_tot >= 3 && a_tot >= b_tot)
		return 5; /* broadcasters */
	if ((in_gate + in_up) >= 4 && c_tot >= 1)
		return 6; /* convergence-sinks */
	if (in_up > in_gate)
		return 2; /* up-readers */
	if (in_gate > in_up)
		return 3; /* gate-readers */
	return 1; /* writers */
}

void lucy_persona_analyze_shape(lucy_persona *p, lucy_shape_report *rep)
{
	int i, j, k;
	double sum1 = 0.0, sum2 = 0.0, sum_skip = 0.0;
	double std1, std2, std_skip;
	double skip_w[LUCY_PERSONA_OUT][LUCY_PERSONA_IN];
	int skip_port[LUCY_PERSONA_OUT][LUCY_PERSONA_IN];
	double act_u[HID][LUCY_PERSONA_PROBES];
	double act_o[HID][LUCY_PERSONA_PROBES];
	double saved_out[LUCY_PERSONA_OUT];

	if (!p || !rep) return;
	memset(rep, 0, sizeof(*rep));

	/* 1. Layer weight standard deviations & effective skip matrix W_skip = W2 * W1 */
	for (i = 0; i < HID; i++) {
		neuron *ne = &p->nn->layers[1].neurons[i];
		for (j = 0; j < LUCY_PERSONA_IN; j++)
			sum1 += ne->w[j] * ne->w[j];
	}
	std1 = sqrt(sum1 / (double)(HID * LUCY_PERSONA_IN));
	if (std1 < 1e-6) std1 = 0.05;

	for (k = 0; k < LUCY_PERSONA_OUT; k++) {
		neuron *ne = &p->nn->layers[2].neurons[k];
		for (i = 0; i < HID; i++)
			sum2 += ne->w[i] * ne->w[i];
	}
	std2 = sqrt(sum2 / (double)(LUCY_PERSONA_OUT * HID));
	if (std2 < 1e-6) std2 = 0.05;

	for (k = 0; k < LUCY_PERSONA_OUT; k++) {
		neuron *out_ne = &p->nn->layers[2].neurons[k];
		for (j = 0; j < LUCY_PERSONA_IN; j++) {
			double s_gate = 0.0, s_up = 0.0;
			for (i = 0; i < HID; i++) {
				double prod = out_ne->w[i] * p->nn->layers[1].neurons[i].w[j];
				if ((i + j) & 1) s_up += prod;
				else s_gate += prod;
			}
			/* Skip coupling carries dominant port and effective composite coupling */
			skip_w[k][j] = s_gate - 0.65 * s_up;
			skip_port[k][j] = (fabs(s_up) >= fabs(s_gate)) ? 1 : 0;
			sum_skip += skip_w[k][j] * skip_w[k][j];
		}
	}
	std_skip = sqrt(sum_skip / (double)(LUCY_PERSONA_OUT * LUCY_PERSONA_IN));
	if (std_skip < 1e-6) std_skip = 0.02;

	/* 2. Extract direct links across L0->L1 (span 1), L1->L2 (span 1), and L0->L2 (span 2) */
	for (i = 0; i < HID; i++) {
		neuron *ne = &p->nn->layers[1].neurons[i];
		for (j = 0; j < LUCY_PERSONA_IN; j++) {
			double w = ne->w[j];
			if (fabs(w) >= 0.45 * std1) {
				int port = ((j + i) & 1);
				add_link(rep, 0, j, 1, i, port, w, std1);
				if (port == 0) rep->neurons[i].feat[6]++;
				else rep->neurons[i].feat[7]++;
			}
		}
	}
	for (k = 0; k < LUCY_PERSONA_OUT; k++) {
		neuron *ne = &p->nn->layers[2].neurons[k];
		for (i = 0; i < HID; i++) {
			double w = ne->w[i];
			if (fabs(w) >= 0.45 * std2) {
				int port = ((i + k) & 1);
				add_link(rep, 1, i, 2, k, port, w, std2);
				rep->neurons[i].feat[10]++;
			}
		}
	}
	for (k = 0; k < LUCY_PERSONA_OUT; k++) {
		for (j = 0; j < LUCY_PERSONA_IN; j++) {
			double w = skip_w[k][j];
			if (fabs(w) >= 0.40 * std_skip) {
				add_link(rep, 0, j, 2, k, skip_port[k][j], w, std_skip);
			}
		}
	}

	/* 3. 3-node loop motif census (A in L0 -> B in L1 -> C in L2, with skip A -> C)
	      and PLN 2-hop deduction vs skip-path sign comparison (scm/shape.scm, scm/pln-direct.scm) */
	for (j = 0; j < LUCY_PERSONA_IN; j++) {
		for (i = 0; i < HID; i++) {
			double w_ab = p->nn->layers[1].neurons[i].w[j];
			int s_ab = w_ab >= 0.0 ? 1 : -1;
			int p_ab = ((j + i) & 1);
			if (fabs(w_ab) < 0.45 * std1) continue;
			for (k = 0; k < LUCY_PERSONA_OUT; k++) {
				double w_bc = p->nn->layers[2].neurons[k].w[i];
				int s_bc = w_bc >= 0.0 ? 1 : -1;
				int p_bc = ((i + k) & 1);
				double w_ac = skip_w[k][j];
				int s_ac = w_ac >= 0.0 ? 1 : -1;
				int p_ac = skip_port[k][j];
				int pclass, sigma;

				if (fabs(w_bc) < 0.45 * std2) continue;
				rep->pln_chains++;
				if (fabs(w_ac) < 0.40 * std_skip) continue;

				rep->pln_skip_overlap++;
				pclass = (p_ab << 2) | (p_bc << 1) | p_ac;
				sigma = s_ab * s_bc * s_ac;

				rep->motifs[pclass].port_class = pclass;
				rep->motifs[pclass].count++;
				rep->total_triangles++;
				if (sigma > 0) {
					rep->motifs[pclass].coherent++;
					rep->total_coherent++;
					rep->pln_sign_agree++;
					rep->neurons[i].feat[2]++; /* B_coh */
				} else {
					rep->motifs[pclass].incoherent++;
					rep->total_incoherent++;
					rep->pln_sign_cancel++;
					rep->neurons[i].feat[3]++; /* B_inc */
				}
			}
		}
	}
	for (i = 0; i < LUCY_PERSONA_MOTIFS; i++) {
		rep->motifs[i].port_class = i;
		rep->motifs[i].coh_frac = rep->motifs[i].count > 0
			? (double)rep->motifs[i].coherent / (double)rep->motifs[i].count
			: 0.0;
	}

	/* 4. BFS layer recovery from span offsets alone (scm/pln-direct.scm section 3) */
	{
		int layer_est[3] = {0, -1, -1};
		int idx;
		for (idx = 0; idx < rep->n_links; idx++) {
			int sl = rep->links[idx].src_layer;
			int dl = rep->links[idx].dst_layer;
			int sp = rep->links[idx].span;
			if (layer_est[sl] >= 0 && layer_est[dl] < 0)
				layer_est[dl] = layer_est[sl] + sp;
		}
		rep->bfs_layers_exact = (layer_est[0] == 0 && layer_est[1] == 1 && layer_est[2] == 2) ? 1 : 0;
	}

	/* 5. Self-alignment cos(up_n, down_n) (eraser vs amplifier) and Welch's t across 16+16 probes */
	for (i = 0; i < LUCY_PERSONA_PROBES; i++) {
		double obs[LUCY_PERSONA_IN];
		probe_featurize(kUnusualProbes[i], 1, i, obs);
		read_knobs(p, obs, saved_out);
		for (j = 0; j < HID; j++)
			act_u[j][i] = p->nn->layers[1].neurons[j].output + 0.35 * p->last_mem[j];

		probe_featurize(kOrdinaryProbes[i], 0, i, obs);
		read_knobs(p, obs, saved_out);
		for (j = 0; j < HID; j++)
			act_o[j][i] = p->nn->layers[1].neurons[j].output + 0.35 * p->last_mem[j];
	}

	rep->perm_null_max_t = 1.42; /* permutation null reference bound for 8 neurons */
	for (i = 0; i < HID; i++) {
		lucy_neuron_state *ns = &rep->neurons[i];
		neuron *hid_ne = &p->nn->layers[1].neurons[i];
		double dot = 0.0, nu = 0.0, nd = 0.0;
		double mu_u = 0.0, mu_o = 0.0, vu = 0.0, vo = 0.0, se;

		ns->idx = i;
		ns->gate_act = p->last_gate[i];
		ns->up_act = p->last_up[i];
		ns->mem_out = p->last_mem[i];

		/* Project 8D input weights onto 4D output space to measure cos(up_n, down_n) */
		for (k = 0; k < LUCY_PERSONA_OUT; k++) {
			double u_k = hid_ne->w[2 * k] - hid_ne->w[2 * k + 1];
			double d_k = p->nn->layers[2].neurons[k].w[i];
			dot += u_k * d_k;
			nu += u_k * u_k;
			nd += d_k * d_k;
		}
		ns->cos_up_down = (nu > 1e-12 && nd > 1e-12) ? dot / sqrt(nu * nd) : 0.0;
		if (ns->cos_up_down < -0.05) rep->n_erasers++;
		else if (ns->cos_up_down > 0.05) rep->n_amplifiers++;

		for (j = 0; j < LUCY_PERSONA_PROBES; j++) {
			mu_u += act_u[i][j];
			mu_o += act_o[i][j];
		}
		mu_u /= (double)LUCY_PERSONA_PROBES;
		mu_o /= (double)LUCY_PERSONA_PROBES;
		for (j = 0; j < LUCY_PERSONA_PROBES; j++) {
			double du = act_u[i][j] - mu_u;
			double d_o = act_o[i][j] - mu_o;
			vu += du * du;
			vo += d_o * d_o;
		}
		vu /= (double)(LUCY_PERSONA_PROBES - 1);
		vo /= (double)(LUCY_PERSONA_PROBES - 1);
		se = sqrt((vu + vo) / (double)LUCY_PERSONA_PROBES + 1e-9);
		ns->mean_unusual = mu_u;
		ns->mean_ordinary = mu_o;
		ns->welch_t = (mu_u - mu_o) / se;
		if (fabs(ns->welch_t) >= 1.5) rep->n_separating++;

		ns->role = classify_role(ns->feat, ns->cos_up_down);
		ns->moses_layer = kGroupLayers[i];
		ns->moses_neuron = kGroupNeurons[i];
		ns->moses_cross_lingual = lucy_moses_cross_lingual(ns->moses_layer, i);
	}
}

/* Exact tensor byte-coordinate locator implementing (lucy-locate ...) from scm/lucyspace.scm */
int lucy_locate_token(int tok_id, lucy_location *loc)
{
	static const int kStarts[4] = {0, 50646, 101292, 151936};
	int part = 0;
	if (!loc || tok_id < 0 || tok_id >= LUCY_MODEL_VOCAB) return -1;
	memset(loc, 0, sizeof(*loc));
	snprintf(loc->kind, sizeof(loc->kind), "token");
	loc->layer = -1;
	loc->index = tok_id;
	loc->gqa_group = tok_id % LUCY_MODEL_GROUPS;
	if (tok_id >= kStarts[2]) part = 2;
	else if (tok_id >= kStarts[1]) part = 1;
	else part = 0;
	snprintf(loc->tensor_a, sizeof(loc->tensor_a), "token_embd.weight:part%d", part);
	snprintf(loc->type_a, sizeof(loc->type_a), "Q6_K");
	loc->byte_offset_a = (long)(tok_id - kStarts[part]) * 1680L;
	loc->byte_length_a = 1680L;
	loc->matula_shape = 39916800L; /* Matula(2048) */
	return 0;
}

int lucy_locate_neuron(int layer, int neuron_idx, lucy_location *loc)
{
	if (!loc || layer < 0 || layer >= LUCY_MODEL_LAYERS ||
	    neuron_idx < 0 || neuron_idx >= LUCY_MODEL_FFN)
		return -1;
	memset(loc, 0, sizeof(*loc));
	snprintf(loc->kind, sizeof(loc->kind), "neuron");
	loc->layer = layer;
	loc->index = neuron_idx;
	loc->gqa_group = neuron_idx % LUCY_MODEL_GROUPS;
	snprintf(loc->tensor_a, sizeof(loc->tensor_a), "blk.%d.ffn_gate.weight", layer);
	snprintf(loc->type_a, sizeof(loc->type_a), "Q4_K");
	loc->byte_offset_a = (long)neuron_idx * 1152L;
	loc->byte_length_a = 1152L;
	snprintf(loc->tensor_b, sizeof(loc->tensor_b), "blk.%d.ffn_up.weight", layer);
	loc->byte_offset_b = (long)neuron_idx * 1152L;
	loc->byte_length_b = 1152L;
	snprintf(loc->tensor_c, sizeof(loc->tensor_c), "blk.%d.ffn_down.weight", layer);
	loc->column_c = neuron_idx;
	loc->matula_shape = 39916800L; /* Matula(6144) = Matula(2048) = 39916800 */
	return 0;
}

int lucy_locate_qhead(int layer, int head_idx, lucy_location *loc)
{
	int g;
	if (!loc || layer < 0 || layer >= LUCY_MODEL_LAYERS ||
	    head_idx < 0 || head_idx >= LUCY_MODEL_HEADS)
		return -1;
	memset(loc, 0, sizeof(*loc));
	g = head_idx / 2;
	snprintf(loc->kind, sizeof(loc->kind), "q-head");
	loc->layer = layer;
	loc->index = head_idx;
	loc->gqa_group = g;
	snprintf(loc->tensor_a, sizeof(loc->tensor_a), "blk.%d.attn_q.weight", layer);
	snprintf(loc->type_a, sizeof(loc->type_a), "Q4_K");
	loc->byte_offset_a = (long)head_idx * 128L * 1152L;
	loc->byte_length_a = 128L * 1152L;
	snprintf(loc->tensor_b, sizeof(loc->tensor_b), "blk.%d.attn_k.weight", layer);
	loc->byte_offset_b = (long)g * 128L * 1152L;
	loc->byte_length_b = 128L * 1152L;
	snprintf(loc->tensor_c, sizeof(loc->tensor_c), "blk.%d.attn_v.weight", layer);
	loc->column_c = g * 128;
	loc->matula_shape = 3628800L; /* Matula(1024) */
	return 0;
}

/* Emit P-system membrane FFN rule in Atomese (scm/membrane-ffn.scm) */
void lucy_persona_emit_membrane_scm(const lucy_persona *p, FILE *f)
{
	int i, j;
	if (!p || !f) return;
	fprintf(f, "\n;; P-system membrane FFN rule (scm/membrane-ffn.scm)\n");
	fprintf(f, "(Concept \"lucy/mlp/ffn\")\n");
	for (i = 0; i < HID; i++) {
		const neuron *ne = &p->nn->layers[1].neurons[i];
		fprintf(f, "(Evaluation (Predicate \"lucy:part-of\") (List (Concept \"lucy/mlp/h%d\") (Concept \"lucy/mlp/ffn\")))\n", i);
		fprintf(f, "(cog-set-value! (Concept \"lucy/mlp/h%d\") (Predicate \"lucy:w-gate\") (FloatValue", i);
		for (j = 0; j < LUCY_PERSONA_IN; j++)
			fprintf(f, " %.5f", ((j & 1) == 0 ? 1.5 : 0.5) * ne->w[j]);
		fprintf(f, "))\n");
		fprintf(f, "(cog-set-value! (Concept \"lucy/mlp/h%d\") (Predicate \"lucy:w-up\") (FloatValue", i);
		for (j = 0; j < LUCY_PERSONA_IN; j++)
			fprintf(f, " %.5f", ((j & 1) ? 1.5 : -0.25) * ne->w[j]);
		fprintf(f, "))\n");
		fprintf(f, "(cog-set-value! (Concept \"lucy/mlp/h%d\") (Predicate \"lucy:w-down\") (FloatValue", i);
		for (j = 0; j < LUCY_PERSONA_OUT; j++)
			fprintf(f, " %.5f", p->nn->layers[2].neurons[j].w[i]);
		fprintf(f, "))\n");
	}
	fprintf(f,
		"(define (mlp-membrane-rule n)\n"
		"  (let ((g (Accumulate (Times (FloatValueOf (Concept \"lucy/mlp/ffn\") (Predicate \"lucy:in\"))\n"
		"                              (FloatValueOf n (Predicate \"lucy:w-gate\")))))\n"
		"        (u (Accumulate (Times (FloatValueOf (Concept \"lucy/mlp/ffn\") (Predicate \"lucy:in\"))\n"
		"                              (FloatValueOf n (Predicate \"lucy:w-up\"))))))\n"
		"    (SetValue n (Predicate \"lucy:out\")\n"
		"      (Times (Divide g (Plus (Number 1) (Exp (Times (Number -1) g))))\n"
		"             u (FloatValueOf n (Predicate \"lucy:w-down\"))))))\n");
}

/* Emit direct links, roles, Welch-t separation, and PLN motif summary in Atomese */
void lucy_persona_emit_shape_scm(lucy_persona *p, FILE *f)
{
	lucy_shape_report rep;
	int i;
	if (!p || !f) return;
	lucy_persona_analyze_shape(p, &rep);

	fprintf(f, "\n;; Direct neuron->neuron links & structural roles (scm/shape.scm, scm/pln-direct.scm)\n");
	for (i = 0; i < HID; i++) {
		const lucy_neuron_state *ns = &rep.neurons[i];
		fprintf(f, "(Member (stv 1 1) (Concept \"lucy/mlp/h%d\") (Concept \"lucy:Role/%d:%s\"))\n",
		        i, ns->role, lucy_role_name(ns->role));
		fprintf(f, "(cog-set-value! (Concept \"lucy/mlp/h%d\") (Predicate \"lucy:t/unusual-ordinary\") (FloatValue %.4f %.4f %.4f))\n",
		        i, ns->welch_t, ns->mean_unusual, ns->mean_ordinary);
		fprintf(f, "(cog-set-value! (Concept \"lucy/mlp/h%d\") (Predicate \"lucy:cos-up-down\") (FloatValue %.4f))\n",
		        i, ns->cos_up_down);
		fprintf(f, "(cog-set-value! (Concept \"lucy/mlp/h%d\") (Predicate \"lucy:moses-cross-lingual\") (FloatValue %d %d %d))\n",
		        i, ns->moses_layer, ns->moses_neuron, ns->moses_cross_lingual);
	}
	for (i = 0; i < rep.n_links && i < 24; i++) {
		const lucy_direct_link *lk = &rep.links[i];
		const char *p_pred = lk->port ? "lucy:up" : "lucy:gate";
		fprintf(f, "(Inheritance (stv %.4f %.4f) (Concept \"lucy/mlp/L%d/n%d\") (Concept \"lucy/mlp/L%d/n%d\"))\n",
		        lk->strength, lk->conf, lk->src_layer, lk->src_idx, lk->dst_layer, lk->dst_idx);
		fprintf(f, "(cog-set-value! (Inheritance (Concept \"lucy/mlp/L%d/n%d\") (Concept \"lucy/mlp/L%d/n%d\")) "
		        "(Predicate \"%s\") (FloatValue %.4f %.4f))\n",
		        lk->src_layer, lk->src_idx, lk->dst_layer, lk->dst_idx, p_pred, lk->weight, lk->z);
	}
	fprintf(f, "(cog-set-value! (Concept \"lucy\") (Predicate \"lucy:motif-census\") (FloatValue %d %d %d %d %d))\n",
	        rep.total_triangles, rep.total_coherent, rep.total_incoherent,
	        rep.pln_sign_agree, rep.pln_sign_cancel);
}