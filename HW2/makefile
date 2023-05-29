obj = obj/operateArrays.o obj/main.o
binary = operateArrays
flags = -Wall -Werror -Wextra -g -c -lm -Iinclude

$(binary): $(obj)
	g++ -o $@ $^ -lm -pthread -Im

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(flags) -c -o $@ $< -lm -pthread 




valgrind: $(binary)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./$(binary)

.PHONY: clean
clean:
	rm -rf obj $(binary)
