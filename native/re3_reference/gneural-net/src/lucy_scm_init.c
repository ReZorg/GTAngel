// libguile-lucy: the name (opencog lucy) loads; the primitives live in liblucy.
extern "C" void opencog_lucy_init(void);
extern "C" void opencog_lucy_guile_init(void) { opencog_lucy_init(); }
