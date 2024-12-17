CC = gcc
CFLAGS = -g -Wall
LDFLAGS =
LDLIBS =
R = rm -f

PROG = http-server
HDRS = $(wildcard *.h)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TAGS = TAGS

VALG_FLAGS = --leak-check=yes -s
LOGS = ../README.txt

T_HTTP_PORT = 2077
T_WEB_ROOT = ~/html
T_MDB_HOST = 127.0.0.1
T_MDB_PORT = 23621
TEST_VARIABLES = $(T_HTTP_PORT) $(T_WEB_ROOT) $(T_MDB_HOST) $(T_MDB_PORT)


$(PROG) : $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS) $(LDLIBS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: all help clean run tags v-test valgrind


all: $(PROG)

clean:
	$(R) core $(OBJS) $(TAGS) $(PROG)

run: all
	$(R) core $(OBJS)
	./$(PROG) $(TEST_VARIABLES)

%:
	@:

v-test: clean $(PROG)
	$(call execute_logs)

valgrind: clean $(PROG)
	echo "\n\n" >> $(LOGS)
	valgrind $(VALG_FLAGS) ./$(PROG) \
		$(filter-out valgrind,$(MAKECMDGOALS)) \
		>> $(LOGS) 2>&1 | tee -a $(LOGS)
	echo "\n\n" >> $(LOGS)

help:
	@echo ""
	@echo " Usage:"
	@echo "  make <command>"
	@echo ""
	@echo " Available commands:"
	@echo "  help             shows available command list"
	@echo "  clean            removes all build files"
	@echo "  run              build and runs the program"
	@echo "  tags             creates tags for src & hdr files"
	@echo "  v-test           builds and runs exec w/ valgrind using str input"
	@echo "  valgrind <args>  cleans previous build files, builds program, runs valgrind"
	@echo ""

tags: $(SRCS) $(HDRS)
	etags -t $(SRCS) $(HDRS) -o $(TAGS)

# helpers
define execute_logs
	$(eval $@_START_TIME = $$(date "+%Y-%m-%d %H:%M:%S"))
	@echo "" >> $(LOGS)
	@echo "[START] VALGRIND FOR $(PROG) - $$($@_START_DATE)" >> $(LOGS)
	@echo "--------------------------------------------" >> $(LOGS)
	valgrind $(VALG_FLAGS) ./$(PROG) \
		$(if $(filter-out valgrind,$@), \
			$(filter-out valgrind,$(MAKECMDGOALS)), \
			$(TEST_VARIABLES)) \
		>> $(LOGS) 2>&1
	@echo "--------------------------------------------" >> $(LOGS)
	@echo "[END] VALGRIND FOR $(PROG) - $$(date "+%Y-%m-%d %H:%M:%S")" >> $(LOGS)
	@echo "" >> $(LOGS)
endef
