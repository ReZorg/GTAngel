# Lucy is C++ (AtomSpace, namespaces) stored as .c. gneural-net stays a C
# program; this extra target compiles those sources with g++ against the
# OpenCog tree that contains this package (tools/lucyspace/gneural-net).
# Persona controller: gneural-net C objects + optional Lucy FFN signature.
srcdir ?= .
OC_ROOT ?= $(abspath $(srcdir)/../../../..)
LUCY_CXX ?= g++
LUCY_OBJDIR = .lucy
LUCY_INCLUDES = \
	-I$(srcdir)/../include \
	-I$(OC_ROOT)/components/atomspace \
	-I$(OC_ROOT)/build/components/atomspace \
	-I$(OC_ROOT)/components/cogutil \
	-I$(OC_ROOT)/build/components/cogutil \
	-I$(OC_ROOT)/components/atomspace-storage \
	-I/usr/include/guile/3.0
LUCY_CXXFLAGS = -O3 -std=gnu++20 -fPIC -fopenmp -DHAVE_GUILE=1 $(LUCY_INCLUDES)
LUCY_RPATH = -Wl,-rpath,$(abspath .):$(abspath $(LUCY_OBJDIR)):$(OC_ROOT)/build/components/atomspace/opencog/atomspace:$(OC_ROOT)/build/components/atomspace/opencog/guile:$(OC_ROOT)/build/components/cogutil/opencog/util:$(OC_ROOT)/build/components/atomspace-storage/opencog/persist/file
LUCY_LIBDIRS = \
	-L$(OC_ROOT)/build/components/atomspace/opencog/guile \
	-L$(OC_ROOT)/build/components/atomspace/opencog/atomspace \
	-L$(OC_ROOT)/build/components/cogutil/opencog/util \
	-L$(OC_ROOT)/build/components/atomspace-storage/opencog/persist/file

LUCY_CORE_OBJS = $(LUCY_OBJDIR)/lucy_infer.o $(LUCY_OBJDIR)/lucy_atoms.o $(LUCY_OBJDIR)/lucy_scm.o $(LUCY_OBJDIR)/lucy_shape.o

.PHONY: lucy
lucy: lucy-generate libguile-lucy.so

$(LUCY_OBJDIR)/%.o: $(srcdir)/%.c
	@mkdir -p $(LUCY_OBJDIR)
	$(LUCY_CXX) $(LUCY_CXXFLAGS) -c $< -o $@

liblucy.so: $(LUCY_CORE_OBJS)
	$(LUCY_CXX) -shared -fPIC -fopenmp -o $@ $(LUCY_CORE_OBJS) $(LUCY_RPATH) $(LUCY_LIBDIRS) -lsmob -latomspace -lcogutil -lguile-3.0 -lgc -lpthread -ldl -lgomp

libguile-lucy.so: $(LUCY_OBJDIR)/lucy_scm_init.o liblucy.so
	$(LUCY_CXX) -shared -fPIC -o $@ $(LUCY_OBJDIR)/lucy_scm_init.o -L. -llucy $(LUCY_RPATH)

lucy-generate: $(LUCY_OBJDIR)/lucy_generate_main.o liblucy.so
	$(LUCY_CXX) -fopenmp -o $@ $(LUCY_OBJDIR)/lucy_generate_main.o -Wl,--copy-dt-needed-entries -L. -llucy $(LUCY_LIBDIRS) -lfast_load_scm $(LUCY_RPATH)

clean-lucy:
	rm -rf $(LUCY_OBJDIR) liblucy.so libguile-lucy.so lucy-generate lucy-persona gneuralucy
# Persona controller: gneural-net C objects + optional Lucy FFN signature.
GN_SRC = network.c feedforward.c error.c activation.c thnn_host.c binom.c fact.c gradient_descent.c random_search.c simulated_annealing.c genetic_algorithm.c msmco.c rnd.c randomize.c save.c load.c
GN_OBJS = $(patsubst %.c,$(LUCY_OBJDIR)/gn_%.o,$(GN_SRC))

$(LUCY_OBJDIR)/gn_%.o: $(srcdir)/%.c
	@mkdir -p $(LUCY_OBJDIR)
	$(CC) -O2 -Wall -std=gnu11 -include math.h -I$(srcdir)/../include -c $< -o $@

$(LUCY_OBJDIR)/lucy_persona.o: $(srcdir)/lucy_persona.c
	@mkdir -p $(LUCY_OBJDIR)
	$(CC) -O2 -Wall -std=gnu11 -include math.h -I$(srcdir)/../include -c $< -o $@

$(LUCY_OBJDIR)/lucy_persona_main.o: $(srcdir)/lucy_persona_main.c liblucy.so
	@mkdir -p $(LUCY_OBJDIR)
	$(LUCY_CXX) $(LUCY_CXXFLAGS) -c $< -o $@

.PHONY: lucy-persona gneuralucy
lucy-persona: $(LUCY_OBJDIR)/lucy_persona_main.o $(LUCY_OBJDIR)/lucy_persona.o $(GN_OBJS) liblucy.so
	$(LUCY_CXX) -fopenmp -o $@ $(LUCY_OBJDIR)/lucy_persona_main.o $(LUCY_OBJDIR)/lucy_persona.o $(GN_OBJS) -Wl,--copy-dt-needed-entries -L. -llucy $(LUCY_LIBDIRS) -lfast_load_scm -lm $(LUCY_RPATH)
$(LUCY_OBJDIR)/gneuralucy.o: $(srcdir)/gneuralucy.c $(srcdir)/lucy_persona.c
	@mkdir -p $(LUCY_OBJDIR)
	$(CC) -O2 -Wall -std=gnu11 -include math.h -I$(srcdir)/../include -c $(srcdir)/gneuralucy.c -o $@

.PHONY: gneuralucy
gneuralucy: $(LUCY_OBJDIR)/gneuralucy.o $(LUCY_OBJDIR)/lucy_persona.o $(GN_OBJS)
	$(CC) -O2 -o $@ $(LUCY_OBJDIR)/gneuralucy.o $(LUCY_OBJDIR)/lucy_persona.o $(GN_OBJS) -lm
