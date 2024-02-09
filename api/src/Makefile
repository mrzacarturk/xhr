CC = gcc
SRCS = ./main.c
OUT = ../out/capi
CFLAGS = -Wall -Wextra -Werror

.PHONY: all make re debug redebug run

all: $(OUT)
$(OUT): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUT)

debug:

	@echo "\e[33m[ BUILD_INFO ]\e[0m Building debug..."
	@if [ ! -d "../out" ]; then \
		mkdir ../out; \
	fi
	$(CC) $(SRCS) -o $(OUT)

run:
# # # # # # # # # # # # # # # # # # # # # # #
# For debug development, will be gone soon...
	@clear
	@./$(OUT)
# # # # # # # # # # # # # # # # # # # # # # #

clean:
	@echo "\e[33m[ BUILD_INFO ]\e[0m Cleaning out directory..."
	@rm -rf $(OUT)

re: clean all

redebug: clean debug run
