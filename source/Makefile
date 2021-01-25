TARGET = ../search
OBJDIR = ./obj

OPTION = -O2

PATH_EIGEN = /usr/include/eigen3
PATH_OPENCV = /usr/local/include/opencv4 -lopencv_core -lopencv_flann -g
INCLUDE = -I ../include -I $(PATH_EIGEN) -I $(PATH_OPENCV)

SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))

$(TARGET):	$(OBJECTS)
	g++ -o $@ $^ $(INCLUDE)

$(OBJDIR)/%.o:	%.cpp
	@[ -d $(OBJDIR) ]
	g++ $(OPTION) -o $@ -c $^ $(INCLUDE)

clean:
	rm -f $(OBJDIR)/*.o
	rm $(TARGET)

