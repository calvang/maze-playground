# all: test

build-maze:
	@g++ maze_generator/main.cpp maze_generator/maze.cpp -std=c++11 -Wall -Werror -Wextra -pedantic -O3 -DNDEBUG -o maze

run-maze:
	@./maze

test-maze:
	@make -s build-maze
	@make -s run-maze

build-path:
	@g++ path_finder/main.cpp path_finder/path.cpp maze_generator/maze.cpp -std=c++11 -Wall -Werror -Wextra -pedantic -O3 -DNDEBUG -o path

run-path:
	@./path

test-path:
	@make -s build-path
	@make -s run-path

clean:
	@rm maze
	@rm path
