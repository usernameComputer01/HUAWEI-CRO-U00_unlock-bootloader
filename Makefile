TARGET := code-HWCRO

all:
	gcc $(TARGET).c -o $(TARGET) -Wall -O2 -s -static
clean:
	rm -fv $(TARGET)
