.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

# Keep intermediate files
#.PRECIOUS: %.o

CC = gcc
CFLAGS = -Wall -Werror -g
LDFLAGS =

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
#LIBS += -lsocket -lnsl -lrt
LIBS += -lpthread

INCLUDE = csapp.h
OBJS = csapp.o utils.o cJSON.o
INCLDIR = -I.

PROGS = ftpclient ftpserver ftpmaster


all: $(PROGS)
	scripts/init.sh

%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) $(INCLDIR) -c -o $@ $<
	
%: %.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)
#	$(CC) -o $@ $(LDFLAGS) $(LIBS) $^
	
clean:
	scripts/delete_cache.sh
	@echo "Lauched a client!"

start_server:
	scripts/launch_server.sh
	@echo "Master and clusters initialized!"

end_server:
	scripts/terminate_server.sh
	@echo "Master and cluster terminated!"

client:
	@echo "Lauched a client!"
	scripts/open_client.sh
	


