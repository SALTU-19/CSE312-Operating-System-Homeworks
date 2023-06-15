obj = obj/makeFileSystem.o obj/main.o
binary = makeFileSystem
flags = -Wall -Werror -Wextra -g -c -lm -Iinclude

$(binary): $(obj)
	gcc -o $@ $^ 

obj/%.o: src/%.c
	mkdir -p $(@D)
	gcc $(flags) -c -o $@ $< 

.PHONY: clean
clean:
	rm -rf obj $(binary)
