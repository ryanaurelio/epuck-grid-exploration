
#This is a template to build your own project with the e-puck2_main-processor folder as a library.
#Simply adapt the lines below to be able to compile

# Define project name here
PROJECT = Project_template

#Define path to the e-puck2_main-processor folder
GLOBAL_PATH = ../e-puck2_main-processor

#Source files to include
CSRC += ./main.c \
		./map.c	\
		./move.c \
		./robot.c \
		./pathfinder.c \
		./types.c \
#		./e_bluetooth.c \


#Source files to include
CSRC += #./stack.c \
		#./main.c \
		
#CPPSRC += 	$(CHCPPSRC) \
#			./robot-side-development/map.cpp \
#			./robot-side-development/main.cpp \
#			./robot-side-development/pathfinder.cpp \

#Header folders to include
INCDIR += 

#USE_OPT += --specs=nosys.specs
#
#USE_OPT += -lstdc++
#
#USE_CPPOPT += -std=c++11

##Header folders to include
#INCDIR += 

#Jump to the main Makefile
include $(GLOBAL_PATH)/Makefile
