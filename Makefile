BOOST = /export/home1/ykamatani/local/include

CPP       = g++
CPPFLAGS = -Wall -O4 -fopenmp
LIBS     = -lz
OBJS     = src/ParseFlatfile.o src/bed2.o src/ybase.o src/flatfile.o src/bGeno.o src/locus.o src/output.o
PROGRAM  = ParseFlatfile

all:	$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CPP) $(LDFLAGS) $(INCLUDES) $(OBJS) $(CPPFLAGS) $(LIBS) -o $(PROGRAM)

%.o: %.cpp
	$(CPP) $(LDFLAGS) $(INCLUDES) $(CPPFLAGS) $(LIBS) -c $<

clean:
	rm -f src/*.o *~ $(PROGRAM)
