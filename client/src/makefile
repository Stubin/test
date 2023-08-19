CC :=gcc
srcs :=$(wildcard *.c)
target :=main
objs := $(patsubst %.c,%.o,$(srcs))
$(target):$(objs)
	$(CC) -o $@ $(objs) -lpthread -g -lcrypt
%.o:%.c
	$(CC) -c $<  -lpthread -g
rebuild:clean all
clean:
	rm -rf $(target) 
