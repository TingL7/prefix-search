TESTS = \
    test_cpy \
    test_ref

CFLAGS = -Wall -Werror -g
DATA = s Tain

# Control the build verbosity                                                   
ifeq ("$(VERBOSE)","1")
    Q :=
    VECHO = @true
else
    Q := @
    VECHO = @printf
endif

GIT_HOOKS := .git/hooks/applied

.PHONY: all clean

all: $(GIT_HOOKS) $(TESTS)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

OBJS_LIB = \
    tst.o

OBJS := \
    $(OBJS_LIB) \
    test_cpy.o \
    test_ref.o

deps := $(OBJS:%.o=.%.o.d)

test_%: test_%.o $(OBJS_LIB)
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(VECHO) "  CC\t$@\n"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

bench: $(TESTS) calculate.c
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./test_cpy --bench $(DATA)
	
	perf stat --repeat 100 \
		-e cache-misses,cache-references,instructions,cycles \
		./test_ref --bench $(DATA)
	
	$(CC) $(CFLAGS) calculate.c -o calculate
	./calculate	
	gnuplot scripts/runtime.gp

clean:
	$(RM) $(TESTS) $(OBJS)
	$(RM) $(deps) out_cpy.txt out_ref.txt output.txt calculate runtime.png

-include $(deps)
