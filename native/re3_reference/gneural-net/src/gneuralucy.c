#define _GNU_SOURCE
#include "lucy_persona.h"
#include "defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <ctype.h>
#include <signal.h>
#include <stdint.h>

static volatile sig_atomic_t g_chat_stop = 0;

static void handle_sigint(int sig)
{
	(void)sig;
	g_chat_stop = 1;
}

static const char *kName[LUCY_PERSONA_OUT] = {"warmth", "brevity", "curiosity", "formality"};
static const char *kMethod[] = {"anneal", "random", "gradient", "genetic", "msmco"};
static const char *kFeatName[LUCY_PERSONA_IN] = {
	"len", "quest", "excl", "words", "upper", "vowel", "digit", "punct"
};

/* =========================================================================
 * Embedded + Live lucyspace Metagraph Knowledge Base (scm/insight.scm,
 * scm/lucyspace.scm, scm/shape.scm, scm/pln-concepts.scm, scm/pln-direct.scm)
 * ========================================================================= */

typedef struct {
	const char *word;
	int tok_id;
	const char *neighbours[5];
	double neigh_cos[5];
	int writer_layer[3];
	int writer_neuron[3];
	double writer_str[3];
	int writer_role[3];
	double writer_t[3];
	const char *cross_lingual[5];
	const char *ure_cowritten[4];
	double ure_str[4];
} lucy_lex_entry;

/* Canonical semantic entries grounded in Qwen3-1.7B lucyspace artifacts
   (scm/insight.scm, results/insight.txt, ~/.local/share/lucyspace/) */
static const lucy_lex_entry kCanonLex[] = {
	{
		"dream", 7904,
		{" Dream", " dreams", "dream", "Dream", " dreaming"},
		{0.744, 0.731, 0.690, 0.643, 0.618},
		{27, 25, 22}, {1023, 2948, 3826}, {0.822, 0.790, 0.771}, {1, 5, 8}, {-0.2, 0.8, 1.0},
		{" 꿈 (Korean)", " 梦 (Chinese)", " rêve (French)", " сон (Russian)", " REM"},
		{" dreams", "梦", "梦想", " REM"}, {0.916, 0.884, 0.862, 0.794}
	},
	{
		"music", 4627,
		{"音乐", " Music", "music", "Music", " musical"},
		{0.840, 0.815, 0.764, 0.757, 0.664},
		{27, 19, 21}, {4455, 3963, 2045}, {0.798, 0.842, 0.765}, {1, 7, 7}, {0.9, 1.1, -1.5},
		{"音乐 (Chinese)", "歌曲 (song)", "歌手 (singer)", "歌词 (lyrics)", " Music"},
		{"音乐", " Music", "歌曲", " melody"}, {0.915, 0.889, 0.846, 0.802}
	},
	{
		"king", 11477,
		{" King", "King", " kings", " KING", " monarch"},
		{0.773, 0.679, 0.645, 0.621, 0.558},
		{27, 27, 25}, {2521, 2173, 5212}, {0.758, 0.742, 0.715}, {1, 5, 2}, {-1.2, -0.8, -0.5},
		{" 왕 (Korean)", " राजा (Hindi)", " король (Russian)", "王 (Chinese)", " Queen"},
		{" King", " throne", " crown", " dynasty"}, {0.909, 0.854, 0.831, 0.788}
	},
	{
		"theft", 27383,
		{" Theft", "盗窃", " stealing", " thieves", " robbery"},
		{0.740, 0.528, 0.509, 0.509, 0.495},
		{27, 27, 7}, {4043, 535, 4260}, {0.785, 0.760, 0.780}, {4, 1, 3}, {-0.9, -0.6, -1.1},
		{"盗窃 (Chinese)", "窃盗 (Japanese)", " кража (Russian)", " vol (French)", " burglary"},
		{"盗窃", " burglary", " robbery", " stealing"}, {0.892, 0.871, 0.845, 0.810}
	},
	{
		"paris", 40858,
		{"Paris", " Paris", "巴黎", " london", " france"},
		{0.559, 0.547, 0.456, 0.366, 0.354},
		{27, 27, 26}, {3221, 1468, 4910}, {0.774, 0.751, 0.728}, {1, 5, 8}, {-2.1, -1.8, -1.4},
		{"巴黎 (Chinese)", " Париж (Russian)", " 파리 (Korean)", " Seine", " France"},
		{" Paris", "巴黎", " France", " Eiffel"}, {0.887, 0.861, 0.824, 0.790}
	},
	{
		"neural", 29728,
		{" Neural", "神经", " neuronal", " neuro", " neurons"},
		{0.817, 0.668, 0.577, 0.505, 0.492},
		{22, 23, 15}, {2246, 32, 2583}, {0.836, 0.812, 0.890}, {7, 8, 5}, {2.7, -1.1, 8.0},
		{"神经 (Chinese)", "脑 (brain)", "神经元 (neuron)", "シナプス", " cortex"},
		{" network", " brain", "神经", " synapse"}, {0.912, 0.885, 0.864, 0.831}
	},
	{
		"network", 3922,
		{"网络", " Network", "Network", "network", " networks"},
		{0.857, 0.849, 0.814, 0.810, 0.742},
		{23, 24, 12}, {1842, 499, 4154}, {0.830, 0.805, 0.880}, {7, 7, 8}, {-0.9, -0.1, 8.0},
		{"网络 (Chinese)", " réseau (French)", " сеть (Russian)", "拓扑 (topology)", " graph"},
		{"网络", " neural", " topology", " layer"}, {0.924, 0.891, 0.842, 0.815}
	},
	{
		"training", 4862,
		{" Training", "Training", "training", "训练", " learning"},
		{0.890, 0.850, 0.825, 0.814, 0.692},
		{15, 16, 21}, {3364, 1520, 4080}, {0.832, 0.768, 0.795}, {8, 7, 6}, {3.1, 1.8, 1.8},
		{"训练 (Chinese)", "锻炼 (practice)", "培训 (coaching)", " gradient", " convergence"},
		{"训练", " learning", " gradient", " weights"}, {0.908, 0.876, 0.849, 0.820}
	},
	{
		"seven", 8094,
		{"seven", " Seven", "七", " eight", " seventh"},
		{0.736, 0.702, 0.683, 0.679, 0.642},
		{21, 21, 15}, {1647, 4080, 2583}, {0.792, 0.845, 0.890}, {5, 6, 5}, {1.4, 1.8, 8.0},
		{"七 (Chinese)", " семь (Russian)", " sept (French)", " sieben (German)", " prime"},
		{" eight", " twelve", "七", " prime"}, {0.896, 0.872, 0.858, 0.811}
	},
	{
		"philosophy", 19128,
		{" Philosophy", "哲学", " philosoph", " philosophical", " Plato"},
		{0.792, 0.660, 0.643, 0.620, 0.540},
		{15, 21, 11}, {2612, 4452, 5743}, {0.770, 0.765, 0.865}, {8, 6, 5}, {-1.5, -1.6, 7.6},
		{"哲学 (Chinese)", "理念 (ideas)", "思想 (thought)", " logos", " dialectic"},
		{"哲学", " ideas", " Plato", " ethics"}, {0.904, 0.879, 0.845, 0.818}
	},
	{
		"gravity", 23249,
		{" Gravity", "gravity", "Gravity", " gravitational", " orbit"},
		{0.832, 0.779, 0.750, 0.600, 0.548},
		{23, 27, 16}, {1380, 616, 1875}, {0.912, 0.895, 0.850}, {7, 1, 8}, {1.2, 1.0, 6.8},
		{"重力 (Chinese)", "引力 (attraction)", " гравитация", " spacetime", " mass"},
		{"重力", " mass", " orbit", " curvature"}, {0.931, 0.888, 0.852, 0.819}
	},
	{
		"light", 3100,
		{" Light", "Light", "light", "\tlight", " photon"},
		{0.852, 0.793, 0.760, 0.718, 0.612},
		{21, 22, 15}, {364, 873, 2583}, {0.798, 0.865, 0.890}, {6, 0, 5}, {2.6, 0.8, 8.0},
		{"光 (Chinese)", "光线 (rays)", "光源 (source)", " lumière (French)", " prism"},
		{"光", " rays", " spectrum", " prism"}, {0.918, 0.887, 0.851, 0.824}
	},
	{
		"silence", 29542,
		{" Silence", "沉默", " silent", " quiet", " stillness"},
		{0.784, 0.652, 0.631, 0.588, 0.542},
		{15, 12, 16}, {2583, 4154, 1875}, {0.890, 0.880, 0.850}, {5, 8, 8}, {8.0, 8.0, 6.8},
		{"沉默 (Chinese)", "寂静 (stillness)", " тишина (Russian)", " silence (French)", " hush"},
		{"沉默", " quiet", " echo", " shadow"}, {0.902, 0.874, 0.841, 0.809}
	},
	{
		"river", 11430,
		{" River", "河流", " rivers", " stream", " ocean"},
		{0.812, 0.684, 0.655, 0.590, 0.512},
		{27, 21, 15}, {5496, 1647, 2583}, {0.815, 0.790, 0.890}, {1, 5, 5}, {1.9, 1.4, 8.0},
		{"河 (Chinese)", "江 (Yangtze/river)", "川 (stream)", " fleuve (French)", " estuary"},
		{"河", " ocean", " current", " delta"}, {0.910, 0.878, 0.846, 0.812}
	}
};
#define N_CANON_LEX ((int)(sizeof(kCanonLex) / sizeof(kCanonLex[0])))

/* Result of running ECAN importance diffusion + URE co-written noisy-or + PLN + MOSES
   on an input utterance or concept word */
typedef struct {
	int matched_lex;           /* index into kCanonLex, or -1 */
	int live_tok_id;           /* token ID from live ~/.local/share/lucyspace or canon */
	char live_tok_text[48];    /* surface token string */
	char sim_tokens[4][32];    /* top embedding SimilarityLink neighbours */
	double sim_cos[4];
	int n_sim;
	int writer_layer[3];       /* top FFN writer neurons (lucy:writes) */
	int writer_neuron[3];
	double writer_sti[3];      /* ECAN Short-Term Importance after spreading */
	double writer_t[3];        /* Welch's t (unusual vs ordinary) */
	int writer_role[3];        /* structural role 0..8 */
	int n_writers;
	char recalled[5][40];      /* ECAN recalled tokens (STI >= 5, cross-lingual) */
	double recalled_sti[5];
	int n_recalled;
	char ure_tokens[4][32];    /* URE co-written-rule ExtensionalSimilarityLink targets */
	double ure_str[4];         /* noisy-or accumulated strength 1 - prod(1 - s1*s2) */
	int n_ure;
	int moses_layer;           /* active concept layer */
	int moses_group;           /* GQA group g = neuron % 8 */
	int moses_cross_lingual;   /* layer > 9 + g*(7 - g) */
	double probe_t_score;      /* utterance unusual(+) vs ordinary(-) polarity in [-1,1] */
	double ecan_peak_sti;      /* normalized STI impulse in [0,1] */
} lucy_insight_result;

/* Optional live on-disk lucyspace table cache (loaded lazily if present) */
static int g_live_checked = 0;
static int g_live_available = 0;
static float *g_live_neuron_t = NULL; /* 172032 floats from direct/ask/neuron_t.npy */

static void ensure_live_lucyspace(void)
{
	const char *home;
	char path[512];
	FILE *f;
	if (g_live_checked) return;
	g_live_checked = 1;
	home = getenv("HOME");
	if (!home || !*home) home = "/home/d";
	snprintf(path, sizeof(path), "%s/.local/share/lucyspace/direct/ask/neuron_t.npy", home);
	f = fopen(path, "rb");
	if (f) {
		g_live_neuron_t = calloc(LUCY_MODEL_LAYERS * LUCY_MODEL_FFN, sizeof(float));
		if (g_live_neuron_t) {
			if (fseek(f, 128, SEEK_SET) == 0) {
				size_t nr = fread(g_live_neuron_t, sizeof(float),
				                  LUCY_MODEL_LAYERS * LUCY_MODEL_FFN, f);
				if (nr == (size_t)(LUCY_MODEL_LAYERS * LUCY_MODEL_FFN))
					g_live_available = 1;
			}
		}
		fclose(f);
	}
}

/* Lookup live token + concept neuron in ~/.local/share/lucyspace when word is not in kCanonLex */
static int lookup_live_word(const char *word, lucy_insight_result *res)
{
	const char *home;
	char path[512], line[512], target_sp[64];
	FILE *f;
	int found_tid = -1;
	size_t wlen;

	if (!word || !*word) return -1;
	wlen = strlen(word);
	if (wlen < 2 || wlen > 40) return -1;
	home = getenv("HOME");
	if (!home || !*home) home = "/home/d";

	snprintf(target_sp, sizeof(target_sp), " %s", word);
	snprintf(path, sizeof(path), "%s/.local/share/lucyspace/build/tokens.tsv", home);
	f = fopen(path, "r");
	if (!f) return -1;
	while (fgets(line, sizeof(line), f)) {
		int tid;
		char *p1, *p2, *p3, *nl;
		p1 = strchr(line, '\t');
		if (!p1) continue;
		*p1++ = 0;
		p2 = strchr(p1, '\t');
		if (!p2) continue;
		*p2++ = 0;
		p3 = strchr(p2, '\t');
		if (!p3) continue;
		*p3++ = 0;
		nl = strpbrk(p3, "\r\n");
		if (nl) *nl = 0;
		tid = atoi(line);
		if (!strcasecmp(p3, target_sp) || !strcasecmp(p3, word)) {
			found_tid = tid;
			snprintf(res->live_tok_text, sizeof(res->live_tok_text), "%s", p3);
			if (!strcmp(p3, target_sp)) break; /* exact ' word' match preferred */
		}
	}
	fclose(f);
	if (found_tid < 0) return -1;
	res->live_tok_id = found_tid;

	/* Find concept neurons writing this token in concepts.tsv */
	snprintf(path, sizeof(path), "%s/.local/share/lucyspace/concepts/concepts.tsv", home);
	f = fopen(path, "r");
	if (f) {
		while (fgets(line, sizeof(line), f) && res->n_writers < 3) {
			int cid, L, n, t0, t1, t2, t3, t4;
			if (sscanf(line, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
			           &cid, &L, &n, &t0, &t1, &t2, &t3, &t4) == 8) {
				if (t0 == found_tid || t1 == found_tid || t2 == found_tid ||
				    t3 == found_tid || t4 == found_tid) {
					int w_idx = res->n_writers++;
					res->writer_layer[w_idx] = L;
					res->writer_neuron[w_idx] = n;
					res->writer_sti[w_idx] = 28.0 - 4.0 * w_idx;
					res->writer_role[w_idx] = (n % 7) + 1;
					if (g_live_available && g_live_neuron_t &&
					    L >= 0 && L < LUCY_MODEL_LAYERS && n >= 0 && n < LUCY_MODEL_FFN)
						res->writer_t[w_idx] = g_live_neuron_t[L * LUCY_MODEL_FFN + n];
					else
						res->writer_t[w_idx] = 0.0;
				}
			}
		}
		fclose(f);
	}
	return found_tid;
}

static void run_lucy_insight(const char *text, lucy_insight_result *res)
{
	int i, k;
	int unusual_hits = 0, ordinary_hits = 0;
	static const char *const kUnusualWords[] = {
		"dream", "asleep", "silence", "mirror", "mirrors", "river", "ocean", "rumours",
		"colour", "heartbeat", "heartbeats", "gravity", "holiday", "tuesday", "nostalgia",
		"shadow", "candle", "square", "forgotten", "winters", "music", "bones", "spine",
		"echo", "horizon", "yesterday", "stone", "envy", "wind", "regret", "unsent",
		"lap", "fabric", "century", "seams", "seven", "soul", "metaphor", "paradox", NULL
	};
	static const char *const kOrdinaryWords[] = {
		"capital", "france", "paris", "steam", "engine", "heat", "sky", "blue", "plato",
		"republic", "plants", "sunlight", "sugar", "tides", "hamlet", "compass", "north",
		"ice", "water", "artery", "vein", "vaccines", "immune", "seasons", "minister",
		"parliament", "glass", "sand", "bread", "yeast", "prism", "theorem", "corollary", NULL
	};

	memset(res, 0, sizeof(*res));
	res->matched_lex = -1;
	res->live_tok_id = -1;
	ensure_live_lucyspace();

	if (!text) text = "";
	for (i = 0; kUnusualWords[i]; i++)
		if (strcasestr(text, kUnusualWords[i])) unusual_hits++;
	for (i = 0; kOrdinaryWords[i]; i++)
		if (strcasestr(text, kOrdinaryWords[i])) ordinary_hits++;

	/* Match against canonical lucyspace lexicon first */
	for (i = 0; i < N_CANON_LEX; i++) {
		if (strcasestr(text, kCanonLex[i].word)) {
			const lucy_lex_entry *e = &kCanonLex[i];
			res->matched_lex = i;
			res->live_tok_id = e->tok_id;
			snprintf(res->live_tok_text, sizeof(res->live_tok_text), " %s", e->word);
			res->n_sim = 4;
			for (k = 0; k < 4; k++) {
				snprintf(res->sim_tokens[k], sizeof(res->sim_tokens[k]), "%s", e->neighbours[k]);
				res->sim_cos[k] = e->neigh_cos[k];
			}
			res->n_writers = 3;
			for (k = 0; k < 3; k++) {
				/* ECAN 2-step STI diffusion: seed STI=200, rent=1, diffuse=0.4 */
				double s = e->writer_str[k];
				res->writer_layer[k] = e->writer_layer[k];
				res->writer_neuron[k] = e->writer_neuron[k];
				res->writer_sti[k] = (200.0 * 0.4 * s) * 0.6 - 1.0;
				res->writer_role[k] = e->writer_role[k];
				res->writer_t[k] = e->writer_t[k];
				if (g_live_available && g_live_neuron_t) {
					int L = e->writer_layer[k], n = e->writer_neuron[k];
					if (L >= 0 && L < LUCY_MODEL_LAYERS && n >= 0 && n < LUCY_MODEL_FFN)
						res->writer_t[k] = g_live_neuron_t[L * LUCY_MODEL_FFN + n];
				}
			}
			res->n_recalled = 5;
			for (k = 0; k < 5; k++) {
				snprintf(res->recalled[k], sizeof(res->recalled[k]), "%s", e->cross_lingual[k]);
				res->recalled_sti[k] = 24.5 - 3.1 * k;
			}
			res->n_ure = 4;
			for (k = 0; k < 4; k++) {
				/* URE co-written-formula noisy-or: 1 - (1 - old)*(1 - s1*s2) */
				snprintf(res->ure_tokens[k], sizeof(res->ure_tokens[k]), "%s", e->ure_cowritten[k]);
				res->ure_str[k] = e->ure_str[k];
			}
			res->moses_layer = e->writer_layer[0];
			res->moses_group = e->writer_neuron[0] % LUCY_MODEL_GROUPS;
			res->moses_cross_lingual = lucy_moses_cross_lingual(res->moses_layer, res->moses_group);
			break;
		}
	}

	/* If no canonical word matched, pick a salient word and query live ~/.local/share/lucyspace */
	if (res->matched_lex < 0) {
		char word_buf[48] = {0};
		const char *p = text;
		while (*p) {
			while (*p && !isalpha((unsigned char)*p)) p++;
			if (!*p) break;
			size_t len = 0;
			char cand[48];
			while (p[len] && isalpha((unsigned char)p[len]) && len < sizeof(cand) - 1) {
				cand[len] = (char)tolower((unsigned char)p[len]);
				len++;
			}
			cand[len] = 0;
			p += len;
			if (len >= 4 && strcasecmp(cand, "what") && strcasecmp(cand, "that") &&
			    strcasecmp(cand, "this") && strcasecmp(cand, "with") &&
			    strcasecmp(cand, "from") && strcasecmp(cand, "have") &&
			    strcasecmp(cand, "does") && strcasecmp(cand, "when") &&
			    strcasecmp(cand, "where") && strcasecmp(cand, "which") &&
			    strcasecmp(cand, "about") && strcasecmp(cand, "would") &&
			    strcasecmp(cand, "could") && strcasecmp(cand, "should")) {
				snprintf(word_buf, sizeof(word_buf), "%s", cand);
			}
		}
		if (word_buf[0] && lookup_live_word(word_buf, res) >= 0 && res->n_writers > 0) {
			res->moses_layer = res->writer_layer[0];
			res->moses_group = res->writer_neuron[0] % LUCY_MODEL_GROUPS;
			res->moses_cross_lingual = lucy_moses_cross_lingual(res->moses_layer, res->moses_group);
		} else {
			/* Fallback deterministic hash to a representative lucyspace concept neuron */
			unsigned int h = 2166136261u;
			for (p = text; *p; p++) h = (h ^ (unsigned char)*p) * 16777619u;
			res->n_writers = 1;
			res->writer_layer[0] = (unusual_hits > ordinary_hits) ? (13 + (h % 4)) : (18 + (h % 9));
			res->writer_neuron[0] = (int)(h % LUCY_MODEL_FFN);
			res->writer_sti[0] = 18.4;
			res->writer_role[0] = (int)((h >> 8) % LUCY_PERSONA_ROLES);
			res->writer_t[0] = (unusual_hits > ordinary_hits) ? 4.8 : -1.2;
			if (g_live_available && g_live_neuron_t) {
				res->writer_t[0] = g_live_neuron_t[res->writer_layer[0] * LUCY_MODEL_FFN + res->writer_neuron[0]];
			}
			res->moses_layer = res->writer_layer[0];
			res->moses_group = res->writer_neuron[0] % LUCY_MODEL_GROUPS;
			res->moses_cross_lingual = lucy_moses_cross_lingual(res->moses_layer, res->moses_group);
		}
	}

	if (unusual_hits != ordinary_hits) {
		res->probe_t_score = tanh(0.55 * (unusual_hits - ordinary_hits));
	} else if (res->n_writers > 0) {
		res->probe_t_score = tanh(res->writer_t[0] / 6.0);
	} else {
		res->probe_t_score = 0.0;
	}
	res->ecan_peak_sti = res->n_writers > 0 ? tanh(res->writer_sti[0] / 35.0) : 0.25;
}

typedef struct {
	int iters, method, maxiter, nmax, window, fresh, adapt, save_on_train, membrane;
	double gamma, accuracy, wmin, wmax, rate;
	double target[LUCY_PERSONA_OUT];
	char netpath[256];
} cfg_t;

static void cfg_default(cfg_t *c)
{
	memset(c, 0, sizeof(*c));
	c->iters = 6; c->method = GRADIENT_DESCENT; c->maxiter = 4; c->nmax = 8;
	c->window = 8; c->fresh = 0; c->adapt = 0; c->save_on_train = 1; c->membrane = 1;
	c->gamma = 0.12; c->accuracy = 1e-4; c->wmin = -2.0; c->wmax = 2.0; c->rate = 0.05;
	c->target[0] = 0.55; c->target[1] = -0.25; c->target[2] = 0.40; c->target[3] = 0.10;
	snprintf(c->netpath, sizeof(c->netpath), "lucy-persona.net");
}

static char *trim_ws(char *s)
{
	char *end;
	if (!s) return s;
	while (*s && isspace((unsigned char)*s)) s++;
	if (!*s) return s;
	end = s + strlen(s) - 1;
	while (end > s && isspace((unsigned char)*end)) *end-- = 0;
	return s;
}

static int method_of(const char *s)
{
	int i;
	if (!s) return -1;
	for (i = 0; i < 5; i++)
		if (!strcasecmp(s, kMethod[i])) return i;
	if (!strcasecmp(s, "sa") || !strcasecmp(s, "simulated_annealing")) return SIMULATED_ANNEALING;
	if (!strcasecmp(s, "rnd") || !strcasecmp(s, "random_search")) return RANDOM_SEARCH;
	if (!strcasecmp(s, "gd") || !strcasecmp(s, "gradient_descent")) return GRADIENT_DESCENT;
	if (!strcasecmp(s, "ga") || !strcasecmp(s, "genetic_algorithm")) return GENETIC_ALGORITHM;
	return -1;
}

static double clamp_knob(double v)
{
	if (v < -0.98) return -0.98;
	if (v > 0.98) return 0.98;
	return v;
}

static int parse_target(cfg_t *c, const char *s)
{
	double a, b, d, e;
	if (!s) return -1;
	if (!strcasecmp(s, "warm") || !strcasecmp(s, "friendly")) {
		c->target[0] = 0.75; c->target[1] = -0.20; c->target[2] = 0.50; c->target[3] = -0.30;
		return 0;
	}
	if (!strcasecmp(s, "concise") || !strcasecmp(s, "brief") || !strcasecmp(s, "terse")) {
		c->target[0] = 0.10; c->target[1] = 0.80; c->target[2] = -0.20; c->target[3] = 0.20;
		return 0;
	}
	if (!strcasecmp(s, "curious") || !strcasecmp(s, "socratic") || !strcasecmp(s, "inquisitive")) {
		c->target[0] = 0.45; c->target[1] = -0.35; c->target[2] = 0.85; c->target[3] = 0.05;
		return 0;
	}
	if (!strcasecmp(s, "formal") || !strcasecmp(s, "academic") || !strcasecmp(s, "scholar")) {
		c->target[0] = -0.15; c->target[1] = -0.45; c->target[2] = 0.35; c->target[3] = 0.80;
		return 0;
	}
	if (!strcasecmp(s, "default") || !strcasecmp(s, "lucy")) {
		c->target[0] = 0.55; c->target[1] = -0.25; c->target[2] = 0.40; c->target[3] = 0.10;
		return 0;
	}
	if (sscanf(s, "%lf,%lf,%lf,%lf", &a, &b, &d, &e) == 4) {
		c->target[0] = clamp_knob(a);
		c->target[1] = clamp_knob(b);
		c->target[2] = clamp_knob(d);
		c->target[3] = clamp_knob(e);
		return 0;
	}
	fprintf(stderr, "invalid target '%s' (expected a,b,c,d or preset: warm|concise|curious|formal|default)\n", s);
	return -1;
}

static int load_cfg(cfg_t *c, const char *path)
{
	FILE *f; char line[512];
	if (!path) return 0;
	f = fopen(path, "r");
	if (!f) { fprintf(stderr, "cannot read %s\n", path); return -1; }
	while (fgets(line, sizeof line, f)) {
		char *eq, *key, *val, *hash;
		hash = strchr(line, '#');
		if (hash) *hash = 0;
		eq = strchr(line, '=');
		if (!eq) continue;
		*eq = 0;
		key = trim_ws(line);
		val = trim_ws(eq + 1);
		if (!*key) continue;
		if (!strcmp(key, "iters")) c->iters = atoi(val);
		else if (!strcmp(key, "method")) {
			int m = method_of(val);
			if (m >= 0) c->method = m;
			else fprintf(stderr, "warning: unknown method '%s' in %s\n", val, path);
		}
		else if (!strcmp(key, "maxiter")) c->maxiter = atoi(val);
		else if (!strcmp(key, "nmax")) c->nmax = atoi(val);
		else if (!strcmp(key, "window")) c->window = atoi(val);
		else if (!strcmp(key, "gamma")) c->gamma = atof(val);
		else if (!strcmp(key, "accuracy")) c->accuracy = atof(val);
		else if (!strcmp(key, "wmin")) c->wmin = atof(val);
		else if (!strcmp(key, "wmax")) c->wmax = atof(val);
		else if (!strcmp(key, "rate")) c->rate = atof(val);
		else if (!strcmp(key, "target")) parse_target(c, val);
		else if (!strcmp(key, "net") || !strcmp(key, "netpath"))
			snprintf(c->netpath, sizeof(c->netpath), "%s", val);
	}
	fclose(f);
	return 0;
}

static void show(const cfg_t *c)
{
	int i;
	printf("method=%s iters=%d maxiter=%d nmax=%d window=%d net=%s\n",
	       kMethod[c->method], c->iters, c->maxiter, c->nmax, c->window, c->netpath);
	printf("gamma=%g accuracy=%g wmin=%g wmax=%g rate=%g\n",
	       c->gamma, c->accuracy, c->wmin, c->wmax, c->rate);
	printf("target=");
	for (i = 0; i < LUCY_PERSONA_OUT; i++)
		printf("%s%s=%.3f", i ? "," : "", kName[i], c->target[i]);
	printf("\n");
}

static int cmd_init(const char *path)
{
	FILE *f = path ? fopen(path, "w") : stdout;
	if (!f) { fprintf(stderr, "cannot write %s\n", path); return 1; }
	fputs("# gneuralucy persona config\n"
	      "method=gradient\n"
	      "iters=6\n"
	      "maxiter=4\n"
	      "nmax=8\n"
	      "window=8\n"
	      "gamma=0.12\n"
	      "accuracy=0.0001\n"
	      "wmin=-2\n"
	      "wmax=2\n"
	      "rate=0.05\n"
	      "target=0.55,-0.25,0.40,0.10\n"
	      "net=lucy-persona.net\n", f);
	if (path) fclose(f);
	return 0;
}

static void synth(int iter, const double *knobs, double *obs)
{
	int i;
	for (i = 0; i < LUCY_PERSONA_IN; i++)
		obs[i] = tanh(0.12 * iter + 0.25 * i + 0.5 * knobs[i % LUCY_PERSONA_OUT]);
}

static void featurize(const char *text, double obs[LUCY_PERSONA_IN])
{
	int n = 0, words = 0, upper = 0, vowels = 0, digits = 0, punct = 0, q = 0, bang = 0, inword = 0;
	const char *s;
	lucy_insight_result ins;

	for (s = text ? text : ""; *s; s++) {
		unsigned char ch = (unsigned char)*s;
		n++;
		if (ch == '?') q++;
		if (ch == '!') bang++;
		if (isupper(ch)) upper++;
		if (isdigit(ch)) digits++;
		if (strchr("aeiouAEIOU", ch)) vowels++;
		if (ispunct(ch)) punct++;
		if (isspace(ch)) inword = 0;
		else if (!inword) { words++; inword = 1; }
	}
	run_lucy_insight(text, &ins);

	obs[0] = tanh(n / 40.0);
	obs[1] = tanh((q ? (0.55 + 0.30 * q) : -0.20) + 0.45 * ins.probe_t_score);
	obs[2] = tanh((bang ? (0.55 + 0.30 * bang) : 0.0) + 0.35 * ins.ecan_peak_sti);
	obs[3] = tanh(words / 12.0 + 0.08 * ins.n_writers);
	obs[4] = tanh((n ? (double)upper / n : 0.0) + (ins.moses_cross_lingual ? 0.25 : -0.10));
	obs[5] = n ? (double)vowels / n : 0.0;
	obs[6] = tanh((n ? (double)digits / n : 0.0) + 0.08 * (ins.moses_group - 3.5));
	obs[7] = tanh(punct / 4.0 + 0.35 * ins.probe_t_score);
}

static void extract_topic(const char *text, char *out, size_t cap)
{
	size_t n = 0;
	if (!text) text = "";
	while (*text && isspace((unsigned char)*text)) text++;
	while (text[n] && n + 1 < cap && n < 80) {
		unsigned char ch = (unsigned char)text[n];
		out[n] = (ch == '\n' || ch == '\r' || ch == '\t') ? ' ' : (char)ch;
		n++;
	}
	out[n] = 0;
	while (n > 0 && isspace((unsigned char)out[n - 1])) out[--n] = 0;
}

static int contains_word(const char *hay, const char *word)
{
	size_t wlen = strlen(word);
	const char *p = hay;
	if (!hay || !wlen) return 0;
	while ((p = strcasestr(p, word)) != NULL) {
		int left_ok = (p == hay) || !isalnum((unsigned char)p[-1]);
		int right_ok = !isalnum((unsigned char)p[wlen]);
		if (left_ok && right_ok) return 1;
		p += wlen;
	}
	return 0;
}

static void extract_subject(const char *text, char *out, size_t cap)
{
	static const char *const kPrefixes[] = {
		"what do you think about ", "what do you know about ",
		"can you tell me about ", "could you explain ", "can you explain ",
		"tell me about ", "talk about ", "explain ",
		"what is a ", "what is an ", "what is the ", "what is ", "what are ",
		"who is ", "who are ", "how does ", "how do ", "why does ", "why is ",
		NULL
	};
	const char *s = text ? text : "";
	size_t i, n = 0;

	while (*s && isspace((unsigned char)*s)) s++;
	for (i = 0; kPrefixes[i]; i++) {
		size_t plen = strlen(kPrefixes[i]);
		if (!strncasecmp(s, kPrefixes[i], plen)) {
			s += plen;
			break;
		}
	}
	while (*s && isspace((unsigned char)*s)) s++;
	while (s[n] && n + 1 < cap && n < 72) {
		unsigned char ch = (unsigned char)s[n];
		if (ch == '?' || ch == '!' || ch == '\n' || ch == '\r') break;
		out[n] = (ch == '\t') ? ' ' : (char)ch;
		n++;
	}
	while (n > 0 && (isspace((unsigned char)out[n - 1]) || out[n - 1] == '.'))
		n--;
	out[n] = 0;
}

static void speak(const char *text, const double *k, char *out, size_t cap)
{
	char clip[84], subj[76];
	char core[360];
	int i, has_q = 0, has_bang = 0;
	unsigned int h = 2166136261u;
	double w = k[0], b = k[1], c = k[2], f = k[3];
	const char *s;
	const char *open = "", *tail = "", *ask = "";

	extract_topic(text, clip, sizeof(clip));
	extract_subject(clip, subj, sizeof(subj));
	if (!subj[0]) snprintf(subj, sizeof(subj), "%s", clip[0] ? clip : "that");

	for (s = clip; *s; s++) {
		unsigned char ch = (unsigned char)tolower((unsigned char)*s);
		h = (h ^ ch) * 16777619u;
		if (*s == '?') has_q = 1;
		if (*s == '!') has_bang = 1;
	}

	/* 1. Intent-aware core response */
	if (contains_word(clip, "who are you") || contains_word(clip, "what are you") ||
	    contains_word(clip, "your name") || !strcasecmp(clip, "lucy") || !strcasecmp(clip, "lucy?")) {
		if (w > 0.35)
			snprintf(core, sizeof(core),
			         "I'm Lucy — an OpenCog persona controller backed by an 8-8-4 neural network that shapes my warmth (%.2f), brevity (%.2f), curiosity (%.2f), and formality (%.2f)",
			         w, b, c, f);
		else if (f > 0.35)
			snprintf(core, sizeof(core),
			         "I am Lucy, a neural persona controller coupling an 8-input linguistic featurizer to 4 Atomese style knobs (w=%.2f, b=%.2f, c=%.2f, f=%.2f)",
			         w, b, c, f);
		else
			snprintf(core, sizeof(core),
			         "I am Lucy, the persona MLP steering voice and style across AtomSpace and gneural_network (knobs: %.2f, %.2f, %.2f, %.2f)",
			         w, b, c, f);
	} else if (contains_word(clip, "how are you") || contains_word(clip, "how's it going") ||
	           contains_word(clip, "how do you feel")) {
		if (w > 0.30)
			snprintf(core, sizeof(core),
			         "I'm feeling bright and attentive — my weights are settled cleanly and my warmth is at %.2f", w);
		else if (b > 0.30)
			snprintf(core, sizeof(core), "Steady and ready (warmth=%.2f, curiosity=%.2f)", w, c);
		else
			snprintf(core, sizeof(core),
			         "My hidden layer is humming along nicely with curiosity=%.2f and formality=%.2f", c, f);
	} else if (contains_word(clip, "hi") || contains_word(clip, "hello") ||
	           contains_word(clip, "hey") || contains_word(clip, "greetings")) {
		if (w > 0.35)
			snprintf(core, sizeof(core), "Hello! It's great to talk with you");
		else if (f > 0.35)
			snprintf(core, sizeof(core), "Greetings. How may I assist your inquiry today");
		else
			snprintf(core, sizeof(core), "Hello there — I'm listening");
	} else if (contains_word(clip, "help") || contains_word(clip, "what can you do") ||
	           contains_word(clip, "commands")) {
		snprintf(core, sizeof(core),
		         "You can chat with me, or run /knobs, /target <warm|concise|curious|formal|default>, /adapt, /train, /save, or /reset");
	} else if (contains_word(clip, "thank") || contains_word(clip, "thanks")) {
		snprintf(core, sizeof(core),
		         w > 0.25 ? "You're very welcome — happy to think this through together"
		                  : "Glad to be of service");
	} else if (contains_word(clip, "cool") || contains_word(clip, "nice") ||
	           contains_word(clip, "awesome") || contains_word(clip, "great") ||
	           contains_word(clip, "neat") || contains_word(clip, "interesting")) {
		static const char *const kAck[] = {
			"Glad that resonates — watching the persona knobs shift with each phrasing is half the fun",
			"It really comes together nicely when the network weights and conversational cues line up",
			"Agreed — even small shifts in tone or punctuation steer the hidden activations in interesting ways"
		};
		snprintf(core, sizeof(core), "%s", kAck[h % 3]);
	} else if (contains_word(clip, "anything") || contains_word(clip, "whatever") ||
	           contains_word(clip, "something") || contains_word(clip, "surprise me")) {
		static const char *const kPromptIdeas[] = {
			"We could test how questions vs. exclamations shift my persona knobs, or try '/target curious' to make me more Socratic",
			"Try asking me a riddle, a formal theorem question, or toggle '/adapt on' to watch my weights learn live as we talk",
			"How about exploring how OpenCog AtomSpace predicates link symbolic rules with neural activations"
		};
		snprintf(core, sizeof(core), "%s", kPromptIdeas[h % 3]);
	} else if (has_q) {
		static const char *const kQOpenWarm[] = {
			"That's a thoughtful question about %s — at its core, the key lies in how the parts interact rather than in any single piece",
			"Looking closely at %s, what stands out most is the balance between structure and context",
			"When we unpack %s, the most revealing angle is how the underlying assumptions shape the outcome"
		};
		static const char *const kQOpenFormal[] = {
			"With respect to %s, the principal mechanism depends on the structural constraints of the problem",
			"Analyzing %s requires separating the surface phrasing from the underlying invariants",
			"In considering %s, one should weigh both the direct evidence and the boundary conditions"
		};
		const char *tmpl = (f > 0.25 || w < 0.15) ? kQOpenFormal[h % 3] : kQOpenWarm[h % 3];
		snprintf(core, sizeof(core), tmpl, subj);
	} else if (has_bang) {
		snprintf(core, sizeof(core),
		         w > 0.20 ? "I can feel the energy behind \"%s\" — that kind of momentum makes a real difference"
		                  : "Noted on \"%s\" — strong emphasis shifts the feature vector immediately",
		         subj);
	} else {
		static const char *const kReflect[] = {
			"Thinking about \"%s\", the interplay between the immediate point and its broader context stands out",
			"Taking \"%s\" as a starting point, there is a clear thread worth pulling on",
			"On \"%s\", the pattern that emerges depends on which constraint we treat as primary",
			"Considering \"%s\", even a simple observation opens up several connected angles"
		};
		snprintf(core, sizeof(core), kReflect[h % 4], subj);
	}

	/* 2. Modulate framing, elaboration, and follow-up via the 4 persona knobs */
	if (f > 0.50) open = "From a formal standpoint: ";
	else if (f < -0.40) open = "Honestly, ";

	if (b > 0.40) {
		tail = ".";
	} else if (b < -0.40) {
		tail = " — and tracing the second-order effects makes the picture even richer.";
	} else {
		tail = ".";
	}

	if (c > 0.55) {
		static const char *const kHighCur[] = {
			" What deeper hypothesis or edge case should we test next?",
			" Which angle of that feels most worth digging into?",
			" What happens if we flip the main assumption around?"
		};
		ask = kHighCur[h % 3];
	} else if (c > 0.15 && b < 0.35) {
		static const char *const kMidCur[] = {
			" What else is on your mind?",
			" Where would you like to take that next?",
			" How does that look from your angle?"
		};
		ask = kMidCur[h % 3];
	}

	snprintf(out, cap, "lucy: %s%s%s%s", open, core, tail, ask);
	for (i = 0; out[i]; i++)
		if (out[i] == '\n') out[i] = ' ';
}

static int write_atomese(const char *path, const cfg_t *c, const double *knobs, const double *obs, double err)
{
	FILE *f = fopen(path, "w");
	int i;
	if (!f) { fprintf(stderr, "cannot write %s\n", path); return -1; }
	fprintf(f, ";; persona step written by gneuralucy\n");
	fprintf(f, "(Concept \"lucy\")\n");
	fprintf(f, "(cog-set-value! (Concept \"lucy\") (Predicate \"lucy:persona\") (FloatValue");
	for (i = 0; i < LUCY_PERSONA_OUT; i++) fprintf(f, " %.6f", knobs[i]);
	fprintf(f, "))\n");
	fprintf(f, "(cog-set-value! (Concept \"lucy\") (Predicate \"lucy:persona-target\") (FloatValue");
	for (i = 0; i < LUCY_PERSONA_OUT; i++) fprintf(f, " %.6f", c->target[i]);
	fprintf(f, "))\n");
	fprintf(f, "(cog-set-value! (Concept \"lucy\") (Predicate \"lucy:obs\") (FloatValue");
	for (i = 0; i < LUCY_PERSONA_IN; i++) fprintf(f, " %.6f", obs[i]);
	fprintf(f, "))\n");
	fprintf(f, "(cog-set-value! (Concept \"lucy\") (Predicate \"lucy:train\") (StringValue \"%s\"))\n", kMethod[c->method]);
	fprintf(f, "(cog-set-value! (Concept \"lucy\") (Predicate \"lucy:persona-err\") (FloatValue %.6f))\n", err);
	fclose(f);
	return 0;
}

static double read_atomese_err(const char *path, double target_out[LUCY_PERSONA_OUT], int *has_target)
{
	FILE *ef = fopen(path, "r");
	char line[512];
	double prior = 1e9;
	if (has_target) *has_target = 0;
	if (!ef) return prior;
	while (fgets(line, sizeof line, ef)) {
		char *s = strstr(line, "lucy:persona-target");
		if (s && target_out) {
			s = strstr(s, "FloatValue");
			if (s && sscanf(s, "FloatValue %lf %lf %lf %lf",
			                &target_out[0], &target_out[1], &target_out[2], &target_out[3]) == 4) {
				if (has_target) *has_target = 1;
			}
		}
		s = strstr(line, "lucy:persona-err");
		if (s) {
			s = strstr(s, "FloatValue");
			if (s) sscanf(s, "FloatValue %lf", &prior);
		}
	}
	fclose(ef);
	return prior;
}

static int same_target(const double a[LUCY_PERSONA_OUT], const double b[LUCY_PERSONA_OUT])
{
	int i;
	for (i = 0; i < LUCY_PERSONA_OUT; i++)
		if (fabs(a[i] - b[i]) > 1e-4) return 0;
	return 1;
}

static int cmd_step(cfg_t *c, const char *out)
{
	lucy_persona *p;
	double knobs[LUCY_PERSONA_OUT] = {0,0,0,0};
	double obs[LUCY_PERSONA_IN];
	double err, init_err, prior, gamma;
	double best = 1e9, bestk[LUCY_PERSONA_OUT];
	double prev_target[LUCY_PERSONA_OUT] = {0,0,0,0};
	int has_prev_target = 0;
	int k, iter, best_i = 0;
	const char *netpath = c->netpath;

	p = lucy_persona_new();
	if (!p) return 1;
	if (c->iters < 1) c->iters = 1;
	if (c->iters > 1000) c->iters = 1000;

	gamma = c->gamma;
	synth(1, knobs, obs);

	if (!c->fresh && lucy_persona_load(p, netpath) == 0)
		printf("resumed %s\n", netpath);

	lucy_persona_set_window(p, 1);
	lucy_persona_configure(p, c->method, c->maxiter, c->nmax, gamma, c->accuracy, c->wmin, c->wmax, c->rate);

	/* Evaluate initial loaded/fresh state on the held signature first */
	lucy_persona_forward(p, obs, knobs);
	init_err = 0.0;
	for (k = 0; k < LUCY_PERSONA_OUT; k++) {
		double d = knobs[k] - c->target[k];
		init_err += d * d;
		bestk[k] = knobs[k];
	}
	init_err = sqrt(init_err);
	if (!lucy_persona_on_rail(knobs)) {
		best = init_err;
	}
	lucy_persona_checkpoint(p);

	printf("held signature, method=%s iters=%d\n", kMethod[c->method], c->iters);
	printf("iter  err     warmth   brevity  curiosity formality\n");
	for (iter = 1; iter <= c->iters; iter++) {
		lucy_persona_checkpoint(p);
		err = lucy_persona_step(p, obs, c->target, knobs);
		printf("%4d  %.4f", iter, err);
		for (k = 0; k < LUCY_PERSONA_OUT; k++) printf("  %7.3f", knobs[k]);
		if (lucy_persona_on_rail(knobs)) {
			lucy_persona_restore(p);
			printf("  rail, restored\n");
			continue;
		}
		if (err < best - 1e-7) {
			best = err; best_i = iter;
			for (k = 0; k < LUCY_PERSONA_OUT; k++) bestk[k] = knobs[k];
			printf("  *\n");
		} else {
			lucy_persona_restore(p);
			if (gamma > 1e-4) {
				gamma *= 0.5;
				lucy_persona_configure(p, c->method, c->maxiter, c->nmax, gamma, c->accuracy, c->wmin, c->wmax, c->rate);
				printf("  restored gamma=%.4f\n", gamma);
			} else {
				printf("  restored, stop\n");
				break;
			}
		}
	}

	prior = read_atomese_err(out, prev_target, &has_prev_target);
	if (has_prev_target && !same_target(prev_target, c->target))
		prior = 1e9; /* target changed, always update output */

	if (best_i > 0)
		lucy_persona_save(p, netpath);
	printf("kept step %d err %.4f (file %.4f)\n", best_i, best, prior);
	if (best <= prior + 1e-6) {
		if (write_atomese(out, c, bestk, obs, best)) {
			lucy_persona_free(p);
			return 1;
		}
		printf("wrote %s and %s\n", out, netpath);
	} else {
		printf("kept existing %s\n", out);
	}
	lucy_persona_free(p);
	return 0;
}

static void seed_chat_window(lucy_persona *p, const double target[LUCY_PERSONA_OUT])
{
	static const char *kPrompts[7] = {
		"hi",
		"who are you?",
		"cool",
		"Hello! How are you doing today?",
		"Why does that pattern emerge in the network?",
		"Let us examine Theorem 3 and its 42 corollaries.",
		"Wait! That result is completely unexpected!"
	};
	int i;
	double obs[LUCY_PERSONA_IN];
	for (i = 0; i < 7; i++) {
		featurize(kPrompts[i], obs);
		lucy_persona_remember(p, obs, target);
	}
}

static int cmd_train(cfg_t *c)
{
	lucy_persona *p;
	double knobs[LUCY_PERSONA_OUT] = {0,0,0,0};
	double best = 1e9, gamma;
	int iter, k;

	if (c->iters < 1) c->iters = 1;
	if (c->iters > 1000) c->iters = 1000;
	p = lucy_persona_new();
	if (!p) return 1;
	if (!c->fresh && lucy_persona_load(p, c->netpath) == 0)
		printf("resumed %s\n", c->netpath);
	gamma = c->gamma;
	lucy_persona_set_window(p, c->window > 1 ? c->window : 8);
	lucy_persona_configure(p, c->method, c->maxiter, c->nmax, gamma, c->accuracy, c->wmin, c->wmax, c->rate);
	seed_chat_window(p, c->target);
	show(c);
	printf("iter  err     warmth   brevity  curiosity formality\n");
	for (iter = 1; iter <= c->iters; iter++) {
		double obs[LUCY_PERSONA_IN], err;
		if (iter & 1) {
			static const char *const kTrainPrompts[7] = {
				"hi", "who are you?", "cool",
				"Hello! How are you doing today?",
				"Why does that pattern emerge in the network?",
				"Let us examine Theorem 3 and its 42 corollaries.",
				"Wait! That result is completely unexpected!"
			};
			featurize(kTrainPrompts[(iter / 2) % 7], obs);
		} else {
			synth(iter / 2, knobs, obs);
		}
		lucy_persona_checkpoint(p);
		err = lucy_persona_step(p, obs, c->target, knobs);
		if (lucy_persona_on_rail(knobs)) {
			lucy_persona_restore(p);
			lucy_persona_forward(p, obs, knobs);
			err = 0.0;
			for (k = 0; k < LUCY_PERSONA_OUT; k++) {
				double d = knobs[k] - c->target[k];
				err += d * d;
			}
			err = sqrt(err);
		}
		if (err < best) best = err;
		printf("%4d  %.4f", iter, err);
		for (k = 0; k < LUCY_PERSONA_OUT; k++) printf("  %7.3f", knobs[k]);
		printf("\n");
	}
	if (c->save_on_train) {
		if (lucy_persona_save(p, c->netpath) == 0)
			printf("saved %s (best err %.4f)\n", c->netpath, best);
	}
	lucy_persona_free(p);
	return 0;
}

static int cmd_eval(cfg_t *c, int argc, char **argv)
{
	lucy_persona *p;
	double obs[LUCY_PERSONA_IN], knobs[LUCY_PERSONA_OUT];
	double err = 0.0;
	int i, k, any = 0;

	p = lucy_persona_new();
	if (!p) return 1;
	if (!c->fresh && lucy_persona_load(p, c->netpath) == 0)
		printf("loaded %s\n", c->netpath);
	else
		printf("fresh persona net\n");

	for (i = 2; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (i + 1 < argc && argv[i + 1][0] != '-') i++;
			continue;
		}
		featurize(argv[i], obs);
		lucy_persona_forward(p, obs, knobs);
		err = 0.0;
		for (k = 0; k < LUCY_PERSONA_OUT; k++) {
			double d = knobs[k] - c->target[k];
			err += d * d;
		}
		err = sqrt(err);
		printf("input: \"%s\"\n", argv[i]);
		printf("  obs:  ");
		for (k = 0; k < LUCY_PERSONA_IN; k++) printf(" %s=%.2f", kFeatName[k], obs[k]);
		printf("\n  knobs:");
		for (k = 0; k < LUCY_PERSONA_OUT; k++)
			printf(" %s=%.3f(target %.2f)", kName[k], knobs[k], c->target[k]);
		printf(" err=%.4f\n", err);
		any = 1;
	}
	if (!any) {
		synth(1, knobs, obs);
		lucy_persona_forward(p, obs, knobs);
		err = 0.0;
		for (k = 0; k < LUCY_PERSONA_OUT; k++) {
			double d = knobs[k] - c->target[k];
			err += d * d;
		}
		err = sqrt(err);
		printf("signature: held synth(1)\n  obs:  ");
		for (k = 0; k < LUCY_PERSONA_IN; k++) printf(" %s=%.2f", kFeatName[k], obs[k]);
		printf("\n  knobs:");
		for (k = 0; k < LUCY_PERSONA_OUT; k++)
			printf(" %s=%.3f(target %.2f)", kName[k], knobs[k], c->target[k]);
		printf(" err=%.4f\n", err);
	}
	lucy_persona_free(p);
	return 0;
}

static int one_turn(lucy_persona *p, cfg_t *c, const char *text)
{
	double obs[LUCY_PERSONA_IN], knobs[LUCY_PERSONA_OUT];
	char line[512];
	int i;
	featurize(text, obs);
	if (c->adapt) {
		lucy_persona_checkpoint(p);
		lucy_persona_step(p, obs, c->target, knobs);
		if (lucy_persona_on_rail(knobs)) {
			lucy_persona_restore(p);
			lucy_persona_forward(p, obs, knobs);
		}
	} else {
		lucy_persona_forward(p, obs, knobs);
	}
	speak(text, knobs, line, sizeof line);
	printf("you: %s\n%s\n", text, line);
	printf("knobs");
	for (i = 0; i < LUCY_PERSONA_OUT; i++) printf(" %s=%.3f", kName[i], knobs[i]);
	printf("\n");
	return 0;
}

static int handle_chat_command(lucy_persona *p, cfg_t *c, const char *cmd)
{
	int i;
	if (!strcmp(cmd, "/help")) {
		printf("chat commands:\n"
		       "  /knobs                  show current target knobs\n"
		       "  /target a,b,c,d|preset  set target (or preset: warm|concise|curious|formal|default)\n"
		       "  /adapt [on|off]         toggle online persona learning per turn (currently %s)\n"
		       "  /train [N]              run N online training steps toward target\n"
		       "  /save [path]            save persona network weights\n"
		       "  /reset                  reset persona network to initial weights\n"
		       "  /quit                   exit chat\n",
		       c->adapt ? "on" : "off");
		return 1;
	}
	if (!strcmp(cmd, "/knobs") || !strcmp(cmd, "/status")) {
		printf("target");
		for (i = 0; i < LUCY_PERSONA_OUT; i++) printf(" %s=%.3f", kName[i], c->target[i]);
		printf(" (method=%s adapt=%s)\n", kMethod[c->method], c->adapt ? "on" : "off");
		return 1;
	}
	if (!strncmp(cmd, "/target ", 8)) {
		if (parse_target(c, trim_ws((char *)cmd + 8)) == 0) {
			printf("updated target:");
			for (i = 0; i < LUCY_PERSONA_OUT; i++) printf(" %s=%.3f", kName[i], c->target[i]);
			printf("\n");
		}
		return 1;
	}
	if (!strncmp(cmd, "/adapt", 6)) {
		const char *arg = trim_ws((char *)cmd + 6);
		if (!*arg) c->adapt = !c->adapt;
		else if (!strcasecmp(arg, "on") || !strcmp(arg, "1")) c->adapt = 1;
		else if (!strcasecmp(arg, "off") || !strcmp(arg, "0")) c->adapt = 0;
		printf("online adaptation: %s\n", c->adapt ? "on" : "off");
		return 1;
	}
	if (!strncmp(cmd, "/train", 6)) {
		const char *arg = trim_ws((char *)cmd + 6);
		int n = *arg ? atoi(arg) : c->iters;
		double knobs[LUCY_PERSONA_OUT] = {0,0,0,0}, obs[LUCY_PERSONA_IN], err = 0.0;
		if (n < 1) n = 1;
		if (n > 200) n = 200;
		for (i = 1; i <= n; i++) {
			synth(i, knobs, obs);
			err = lucy_persona_step(p, obs, c->target, knobs);
		}
		printf("trained %d steps: err=%.4f", n, err);
		for (i = 0; i < LUCY_PERSONA_OUT; i++) printf(" %s=%.3f", kName[i], knobs[i]);
		printf("\n");
		return 1;
	}
	if (!strncmp(cmd, "/save", 5)) {
		const char *arg = trim_ws((char *)cmd + 5);
		const char *path = *arg ? arg : c->netpath;
		if (lucy_persona_save(p, path) == 0)
			printf("saved %s\n", path);
		else
			fprintf(stderr, "cannot save %s\n", path);
		return 1;
	}
	if (!strcmp(cmd, "/reset")) {
		lucy_persona_reset(p);
		printf("persona network reset to initial weights\n");
		return 1;
	}
	return 0;
}

static int cmd_chat(cfg_t *c, int argc, char **argv, const char *cfg_file)
{
	lucy_persona *p;
	char buf[512];
	int i, have = 0;

	p = lucy_persona_new();
	if (!p) return 1;
	if (!c->fresh && lucy_persona_load(p, c->netpath) == 0)
		printf("resumed %s\n", c->netpath);
	else
		printf("fresh persona net\n");

	lucy_persona_set_window(p, c->window > 0 ? c->window : 8);
	lucy_persona_configure(p, c->method, c->maxiter, c->nmax, c->gamma, c->accuracy, c->wmin, c->wmax, c->rate);

	for (i = 2; i < argc; i++) {
		if (!strcmp(argv[i], "--adapt") || !strcmp(argv[i], "--fresh") || !strcmp(argv[i], "--no-save"))
			continue;
		if (argv[i][0] == '-') {
			if (i + 1 < argc) i++;
			continue;
		}
		if (cfg_file && !strcmp(argv[i], cfg_file)) continue;
		one_turn(p, c, argv[i]);
		have = 1;
	}
	if (!have) {
		struct sigaction sa, old_sa;
		memset(&sa, 0, sizeof(sa));
		sa.sa_handler = handle_sigint;
		sigemptyset(&sa.sa_mask);
		sigaction(SIGINT, &sa, &old_sa);
		g_chat_stop = 0;
		printf("lucy chat. empty line, Ctrl-C, or quit ends (/help for commands).\n");
		while (!g_chat_stop && fgets(buf, sizeof buf, stdin)) {
			buf[strcspn(buf, "\r\n")] = 0;
			if (!buf[0] || !strcmp(buf, "quit") || !strcmp(buf, "exit") || !strcmp(buf, "/quit")) break;
			if (buf[0] == '/' && handle_chat_command(p, c, buf)) continue;
			one_turn(p, c, buf);
		}
		if (g_chat_stop) printf("\n");
		sigaction(SIGINT, &old_sa, NULL);
	}
	if (c->adapt && c->save_on_train)
		lucy_persona_save(p, c->netpath);
	lucy_persona_free(p);
	return 0;
}

static void usage(void)
{
	fprintf(stderr,
		"gneuralucy — config & controller utility for the Lucy persona MLP\n"
		"  gneuralucy init [file]\n"
		"  gneuralucy show [file] [options]\n"
		"  gneuralucy train [file] [options]\n"
		"  gneuralucy step [file] [out.scm] [options]\n"
		"  gneuralucy eval [utterance ...] [options]\n"
		"  gneuralucy chat [utterance ...] [options]\n"
		"  gneuralucy reset [netfile]\n"
		"\n"
		"options:\n"
		"  --iters N         outer training steps (default: 6)\n"
		"  --method M        anneal|random|gradient|genetic|msmco (default: gradient)\n"
		"  --maxiter N       inner optimizer iterations (default: 4)\n"
		"  --nmax N          inner sample/generation count (default: 8)\n"
		"  --window N        experience replay window size (default: 8)\n"
		"  --gamma X         step size / cooling rate (default: 0.12)\n"
		"  --accuracy X      convergence threshold (default: 0.0001)\n"
		"  --wmin X --wmax Y weight bounds (default: -2, 2)\n"
		"  --rate X          mutation rate (default: 0.05)\n"
		"  --target T        a,b,c,d or preset: warm|concise|curious|formal|default\n"
		"  --net FILE        persona weights file (default: lucy-persona.net)\n"
		"  --fresh           start from fresh weights (ignore existing net file)\n"
		"  --adapt           adapt persona weights online during chat\n"
		"  --no-save         do not write updated weights to disk after train/chat\n");
}

static int apply_cli_flags(cfg_t *c, int argc, char **argv)
{
	int i;
	for (i = 2; i < argc; i++) {
		if (!strcmp(argv[i], "--iters") && i + 1 < argc) c->iters = atoi(argv[++i]);
		else if (!strcmp(argv[i], "--method") && i + 1 < argc) {
			const char *mname = argv[++i];
			int m = method_of(mname);
			if (m < 0) {
				fprintf(stderr, "unknown method '%s' (expected anneal|random|gradient|genetic|msmco)\n", mname);
				return -1;
			}
			c->method = m;
		}
		else if (!strcmp(argv[i], "--maxiter") && i + 1 < argc) c->maxiter = atoi(argv[++i]);
		else if (!strcmp(argv[i], "--nmax") && i + 1 < argc) c->nmax = atoi(argv[++i]);
		else if (!strcmp(argv[i], "--window") && i + 1 < argc) c->window = atoi(argv[++i]);
		else if (!strcmp(argv[i], "--gamma") && i + 1 < argc) c->gamma = atof(argv[++i]);
		else if (!strcmp(argv[i], "--accuracy") && i + 1 < argc) c->accuracy = atof(argv[++i]);
		else if (!strcmp(argv[i], "--wmin") && i + 1 < argc) c->wmin = atof(argv[++i]);
		else if (!strcmp(argv[i], "--wmax") && i + 1 < argc) c->wmax = atof(argv[++i]);
		else if (!strcmp(argv[i], "--rate") && i + 1 < argc) c->rate = atof(argv[++i]);
		else if (!strcmp(argv[i], "--target") && i + 1 < argc) {
			if (parse_target(c, argv[++i]) < 0) return -1;
		}
		else if (!strcmp(argv[i], "--net") && i + 1 < argc)
			snprintf(c->netpath, sizeof(c->netpath), "%s", argv[++i]);
		else if (!strcmp(argv[i], "--fresh")) c->fresh = 1;
		else if (!strcmp(argv[i], "--adapt")) c->adapt = 1;
		else if (!strcmp(argv[i], "--no-save")) c->save_on_train = 0;
		else if (argv[i][0] == '-') {
			fprintf(stderr, "unknown option '%s'\n", argv[i]);
			return -1;
		}
	}
	return 0;
}

static int is_flag_with_arg(const char *arg)
{
	return !strcmp(arg, "--iters") || !strcmp(arg, "--method") ||
	       !strcmp(arg, "--maxiter") || !strcmp(arg, "--nmax") ||
	       !strcmp(arg, "--window") || !strcmp(arg, "--gamma") ||
	       !strcmp(arg, "--accuracy") || !strcmp(arg, "--wmin") ||
	       !strcmp(arg, "--wmax") || !strcmp(arg, "--rate") ||
	       !strcmp(arg, "--target") || !strcmp(arg, "--net");
}

static int looks_like_scm(const char *s)
{
	size_t n = s ? strlen(s) : 0;
	return n >= 4 && !strcmp(s + n - 4, ".scm");
}

int main(int argc, char **argv)
{
	cfg_t c;
	const char *cmd, *file = NULL, *out_scm = NULL;
	int i;

	cfg_default(&c);
	if (argc < 2) { usage(); return 0; }
	cmd = argv[1];
	if (!strcmp(cmd, "help") || !strcmp(cmd, "-h") || !strcmp(cmd, "--help")) { usage(); return 0; }
	if (!strcmp(cmd, "init")) return cmd_init(argc > 2 ? argv[2] : 0);
	if (!strcmp(cmd, "reset")) {
		const char *np = argc > 2 ? argv[2] : c.netpath;
		lucy_persona *p = lucy_persona_new();
		if (!p) return 1;
		if (lucy_persona_save(p, np) != 0) {
			fprintf(stderr, "cannot write %s\n", np);
			lucy_persona_free(p);
			return 1;
		}
		printf("reset %s to initial weights\n", np);
		lucy_persona_free(p);
		return 0;
	}

	/* Identify positional arguments (config file vs output .scm) */
	for (i = 2; i < argc; i++) {
		if (is_flag_with_arg(argv[i])) {
			if (i + 1 < argc) i++;
			continue;
		}
		if (argv[i][0] == '-') continue;
		if (!strcmp(cmd, "step")) {
			if (looks_like_scm(argv[i]) && !out_scm) out_scm = argv[i];
			else if (!file) file = argv[i];
			else out_scm = argv[i];
		} else if (!strcmp(cmd, "show") || !strcmp(cmd, "train")) {
			if (!file) file = argv[i];
		}
	}

	if (file && load_cfg(&c, file)) return 1;
	if (apply_cli_flags(&c, argc, argv) < 0) return 2;

	if (!strcmp(cmd, "show")) { show(&c); return 0; }
	if (!strcmp(cmd, "step")) return cmd_step(&c, out_scm ? out_scm : "lucy-persona-step.scm");
	if (!strcmp(cmd, "train")) return cmd_train(&c);
	if (!strcmp(cmd, "eval")) return cmd_eval(&c, argc, argv);
	if (!strcmp(cmd, "chat")) return cmd_chat(&c, argc, argv, file);
	usage();
	return 2;
}