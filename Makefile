BOOST = /export/home1/ykamatani/local/include

CPP       = g++
CPPFLAGS = -Wall -O4 -fopenmp
LIBS     = -lz
OBJS     = ParseFlatfile.o bed2.o ybase.o flatfile.o bGeno.o locus.o output.o
PROGRAM  = ParseFlatfile

all:	$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CPP) $(LDFLAGS) $(INCLUDES) $(OBJS) $(CPPFLAGS) $(LIBS) -o $(PROGRAM)

%.o: src/%.cpp
	$(CPP) $(LDFLAGS) $(INCLUDES) $(CPPFLAGS) $(LIBS) -c $<

clean:
	rm -f src/*.o *~ $(PROGRAM)
