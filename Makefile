all: test

build:
	@g++ maze_generator/main.cpp maze_generator/maze.cpp -std=c++11 -Wall -Werror -Wextra -pedantic -O3 -DNDEBUG -o maze

run:
	@./maze

test:
	@make -s build
	@make -s run

clean:
	@rm maze
