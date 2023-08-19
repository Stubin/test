CC :=gcc
srcs :=$(wildcard *.c)
target :=main
objs := $(patsubst %.c,%.o,$(srcs))
$(target):$(objs)
	$(CC) -o $@ $(objs) -lpthread -I /usr/include/mysql/  -lmysqlclient -lcrypt -g
%.o:%.c
	$(CC) -o %@ $<  -lpthread  -I /usr/include/mysql/   -lmysqlclient -g;
rebuild:clean all
clean:
	rm -rf $(target) 
