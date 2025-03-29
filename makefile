# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -O3

# Target executable
TARGET = graphColoring.out
BENCHMARK_TARGET = benchmark.out
SOLUTION_DIR = solutions

# Source files
SRCS = main.c dsatur.c graph_constructor.c imp_color.c other_algorithms/rlf.c other_algorithms/wp.c other_algorithms/ldo.c
BENCHMARK_SRCS = benchmark.c

# Object files directory
OBJ_DIR = obj

# Object files
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

# Build rules
$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(BENCHMARK_TARGET): $(addprefix $(OBJ_DIR)/, $(BENCHMARK_SRCS:.c=.o))
		$(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/, $(BENCHMARK_SRCS:.c=.o)) -o $(BENCHMARK_TARGET)

$(OBJ_DIR)/%.o: %.c
		mkdir -p $(OBJ_DIR)
		$(CC) $(CFLAGS) -c $< -o $@


# Clean rule
clean:
		rm -rf $(OBJ_DIR) $(SOLUTION_DIR) $(TARGET) $(BENCHMARK_TARGET)

# Phony targets
.PHONY: clean benchmark
benchmark: $(BENCHMARK_TARGET)