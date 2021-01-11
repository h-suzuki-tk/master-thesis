TARGET = ../search
OBJDIR = ./obj
INCLUDE = -I ~/eigen-3.3.7 -I ../include -I /usr/local/include/opencv4 -lopencv_core -lopencv_flann
SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))

$(TARGET):	$(OBJECTS)
	g++ -O2 -o $@ $^ -I /usr/local/include/opencv4 -lopencv_core -lopencv_flann -g

$(OBJDIR)/%.o:	%.cpp
	@[ -d $(OBJDIR) ]
	g++ -o $@ -c $^ $(INCLUDE)

clean:
	rm -f $(OBJDIR)/*.o
	rm $(TARGET)

