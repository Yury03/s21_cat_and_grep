FLAGS=-Wall -Wextra -Werror -std=c11
TARGET=s21_cat
all: $(TARGET)
$(TARGET):
	gcc $(FLAGS) ./s21_cat.c -o $(TARGET)
rebuild: clean all
clean:
	rm $(TARGET) 