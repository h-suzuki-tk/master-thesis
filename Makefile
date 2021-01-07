OBJDIR = ./obj
INCLUDE = -I ~/eigen-3.3.7 -I ./include -I /usr/local/include/opencv4 -lopencv_core -lopencv_flann

main:	$(OBJDIR)/main.o $(OBJDIR)/calc.o $(OBJDIR)/data.o $(OBJDIR)/xmeans.o
	g++ -O3 -o main $(OBJDIR)/main.o $(OBJDIR)/calc.o $(OBJDIR)/data.o $(OBJDIR)/xmeans.o -I /usr/local/include/opencv4 -lopencv_core -lopencv_flann -g

$(OBJDIR)/main.o:	main.cpp
	@[ -d $(OBJDIR) ]
	g++ -O3 -o $@ -c main.cpp $(INCLUDE)
$(OBJDIR)/calc.o:	calc.cpp
	@[ -d $(OBJDIR) ]
	g++ -O3 -o $@ -c calc.cpp $(INCLUDE)
$(OBJDIR)/data.o:	data.cpp
	@[ -d $(OBJDIR) ]
	g++ -O3 -o $@ -c data.cpp $(INCLUDE)
$(OBJDIR)/xmeans.o:	xmeans.cpp
	@[ -d $(OBJDIR) ]
	g++ -O3 -o $@ -c xmeans.cpp $(INCLUDE)

clean:
	rm -f *.o
