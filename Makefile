BOOST = /export/home1/ykamatani/local/include

CC       = g++
# CPPFLAGS = -Wall -O4 -I/home/ykamatani/Documents/C++/tests/ParseFlatfile2/src -I/home/ykamatani/local/include
CPPFLAGS = -Wall -O4 -I./src -I/usr/include -I/usr/local/include -I$(BOOST) -fopenmp
LIBS     = -lz -L/usr/lib -L/usr/local/lib
OBJS     = src/ParseFlatfile.o src/bed2.o src/ybase.o src/flatfile.o src/bGeno.o src/locus.o src/output.o
PROGRAM  = ParseFlatfile

all:	$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(CC) $(OBJS) $(CPPFLAGS) $(LIBS) -o $(PROGRAM)

bed2.o:	src/bed2.cpp
	 $(CC) $(OBJS) $(CPPFLAGS) $(LIBS) -c src/bed2.cpp

ybase.o:	src/ybase.cpp
	 $(CC) $(OBJS) $(CPPFLAGS) $(LIBS) -c src/ybase.cpp

flatfile.o:	src/flatfile.cpp
	 $(CC) $(OBJS) $(CPPFLAGS) $(LIBS) -c src/flatfile.cpp

locus.o:	src/locus.cpp
	 $(CC) $(OBJS) $(CPPFLAGS) $(LIBS) -c src/locus.cpp

bGeno.o:	src/bGeno.cpp
	 $(CC) $(OBJS) $(CPPFLAGS) $(LIBS) -c src/bGeno.cpp

output.o:	src/output.cpp
	 $(CC) $(OBJS) $(CPPFLAGS) $(LIBS) -c src/output.cpp
clean:
	rm -f src/*.o *~ $(PROGRAM)
