#ifndef LUCY_PERSONA_H
#define LUCY_PERSONA_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define LUCY_PERSONA_IN    8
#define LUCY_PERSONA_HID   8
#define LUCY_PERSONA_OUT   4
#define LUCY_PERSONA_ROLES 9
#define LUCY_PERSONA_MOTIFS 8
#define LUCY_PERSONA_PROBES 16

/* lucyspace metagraph architecture constants (scm/lucyspace.scm) */
#define LUCY_MODEL_LAYERS  28
#define LUCY_MODEL_DIM     2048
#define LUCY_MODEL_FFN     6144
#define LUCY_MODEL_HEADS   16
#define LUCY_MODEL_GROUPS  8
#define LUCY_MODEL_HDIM    128
#define LUCY_MODEL_VOCAB   151936

typedef struct lucy_persona lucy_persona;

/* Direct neuron->neuron link (scm/shape.scm, scm/pln-direct.scm) */
typedef struct {
	int src_layer, src_idx;
	int dst_layer, dst_idx;
	int port;        /* 0 = gate, 1 = up */
	int span;        /* dst_layer - src_layer */
	double weight;   /* signed coupling */
	double z;        /* z-score above layer std */
	double strength; /* PLN strength in [0,1] */
	double conf;     /* PLN confidence z^2/(z^2+800) */
	int sign;        /* +1 or -1 */
} lucy_direct_link;

/* 3-node feed-forward motif A -> B -> C with skip A -> C (scm/shape.scm) */
typedef struct {
	int port_class;  /* 0..7: (port_ab<<2)|(port_bc<<1)|port_ac */
	int count;       /* total triangles in this port class */
	int coherent;    /* sigma = s(AB)*s(BC)*s(AC) = +1 */
	int incoherent;  /* sigma = -1 (eraser loop) */
	double coh_frac; /* coherent / count */
} lucy_motif_class;

/* Per-hidden-neuron structural & dynamical summary (scm/shape.scm, scm/membrane-ffn.scm) */
typedef struct {
	int idx;                 /* 0 .. LUCY_PERSONA_HID-1 (maps to GQAGroup g) */
	int role;                /* 0 .. 8 (lucy:Role/k) */
	int feat[12];            /* [A_coh,A_inc,B_coh,B_inc,C_coh,C_inc,ig1,iu1,ig2,iu2,o1,o2] */
	double gate_act;         /* last SiLU(w_gate . x) */
	double up_act;           /* last (w_up . x) */
	double mem_out;          /* last SwiGLU membrane activation gate_act * up_act */
	double cos_up_down;      /* self-alignment cos(up_n, down_n): <0 eraser, >0 amplifier */
	double welch_t;          /* Welch's t (unusual vs ordinary across 16+16 probes) */
	double mean_unusual;     /* mean activation on unusual probes */
	double mean_ordinary;    /* mean activation on ordinary probes */
	int moses_layer;         /* representative lucyspace layer L in 0..27 */
	int moses_neuron;        /* representative lucyspace FFN neuron in 0..6143 */
	int moses_cross_lingual; /* MOSES rule: layer > 9 + g*(7 - g) */
} lucy_neuron_state;

/* Full structural + dynamical report of the Lucy MLP (scm/shape.scm + scm/pln-direct.scm) */
typedef struct {
	int n_links;
	lucy_direct_link links[128];
	lucy_motif_class motifs[LUCY_PERSONA_MOTIFS];
	int total_triangles;
	int total_coherent;
	int total_incoherent;
	int n_erasers;           /* hidden neurons with cos(up,down) < -0.05 */
	int n_amplifiers;        /* hidden neurons with cos(up,down) > +0.05 */
	int n_separating;        /* hidden neurons with |welch_t| >= 1.5 */
	double perm_null_max_t;  /* max |t| under label permutation null */
	int pln_chains;          /* 2-hop chains A -> B -> C */
	int pln_skip_overlap;    /* chains that also have a direct skip A -> C */
	int pln_sign_agree;      /* skip sign agrees with s(AB)*s(BC) */
	int pln_sign_cancel;     /* skip sign opposes s(AB)*s(BC) (incoherent cancellation) */
	int bfs_layers_exact;    /* 1 if span-offset BFS recovers exact layer indices */
	lucy_neuron_state neurons[LUCY_PERSONA_HID];
} lucy_shape_report;

/* Tensor byte-coordinate locator result (lucy-locate in scm/lucyspace.scm) */
typedef struct {
	char kind[16];           /* "token", "neuron", or "q-head" */
	int layer;               /* 0..27 or -1 for token */
	int index;               /* token id, neuron id (0..6143), or head id (0..15) */
	int gqa_group;           /* 0..7 for q-head, or neuron%8 */
	char tensor_a[48];       /* primary tensor name */
	char type_a[12];         /* quantization type (Q6_K, Q4_K) */
	long byte_offset_a;      /* exact byte offset inside tensor_a */
	long byte_length_a;      /* byte span */
	char tensor_b[48];       /* secondary tensor (ffn_up or attn_k) */
	long byte_offset_b;
	long byte_length_b;
	char tensor_c[48];       /* tertiary tensor (ffn_down column) */
	int column_c;
	long matula_shape;       /* Matula prime-product encoding of tensor shape */
} lucy_location;

lucy_persona *lucy_persona_new(void);
void lucy_persona_free(lucy_persona *p);
void lucy_persona_reset(lucy_persona *p);
void lucy_persona_set_window(lucy_persona *p, int n);
int lucy_persona_get_window(const lucy_persona *p);
int lucy_persona_get_num_points(const lucy_persona *p);
void lucy_persona_configure(lucy_persona *p, int method, int maxiter, int nmax, double gamma, double accuracy, double wmin, double wmax, double rate);
int lucy_persona_save(lucy_persona *p, const char *path);
int lucy_persona_load(lucy_persona *p, const char *path);
void lucy_persona_checkpoint(lucy_persona *p);
void lucy_persona_restore(lucy_persona *p);
int lucy_persona_on_rail(const double knobs[LUCY_PERSONA_OUT]);
void lucy_persona_remember(lucy_persona *p, const double obs[LUCY_PERSONA_IN], const double target[LUCY_PERSONA_OUT]);
double lucy_persona_error(lucy_persona *p);
void lucy_persona_forward(lucy_persona *p, const double obs[LUCY_PERSONA_IN], double knobs[LUCY_PERSONA_OUT]);
double lucy_persona_step(lucy_persona *p, const double obs[LUCY_PERSONA_IN], const double target[LUCY_PERSONA_OUT], double knobs[LUCY_PERSONA_OUT]);

/* lucyspace Scheme model embodiment APIs */
void lucy_persona_set_membrane(lucy_persona *p, int enable);
int lucy_persona_get_membrane(const lucy_persona *p);
void lucy_persona_membrane_forward(lucy_persona *p, const double obs[LUCY_PERSONA_IN],
                                   double hid_out[LUCY_PERSONA_HID], double knobs[LUCY_PERSONA_OUT]);
void lucy_persona_analyze_shape(lucy_persona *p, lucy_shape_report *rep);
const char *lucy_role_name(int role);
const char *lucy_motif_name(int port_class);
const char *lucy_probe_question(int unusual, int idx);
int lucy_moses_cross_lingual(int layer, int group);
int lucy_locate_token(int tok_id, lucy_location *loc);
int lucy_locate_neuron(int layer, int neuron_idx, lucy_location *loc);
int lucy_locate_qhead(int layer, int head_idx, lucy_location *loc);
void lucy_persona_emit_membrane_scm(const lucy_persona *p, FILE *f);
void lucy_persona_emit_shape_scm(lucy_persona *p, FILE *f);

#ifdef __cplusplus
}
#endif
#endif
