include ../common.mk

SRCS += syscall.c

OBJS := $(SRCS)
OBJS := $(OBJS:.c=.o)
OBJS := $(OBJS:.cc=.o)
OBJS := $(OBJS:.cpp=.o)
OBJS := $(OBJS:.S=.o)
OBJS := $(OBJS:.s=.o)

all: $(OBJS)

-include $(OBJS:.o=.d)

.PHONY : clean
clean:
	-rm -f $(OBJS) $(OBJS:.o=.d) $(OBJS:.o=.su)

.SECONDARY: $(OBJS)