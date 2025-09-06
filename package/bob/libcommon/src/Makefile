TARGETS = libcommon.so

CFLAGS  += -Wall -fno-strict-aliasing
LDFLAGS += -L. -lpthread -lcjson -luci -lm -lz -lubus -lblobmsg_json -lubox -ldatconf -lkvcutil

SRCS := $(wildcard ./*.c)
OBJS := $(subst .c,.o,$(SRCS))

all: $(TARGETS)

%.o : %.c
	$(CC) $(CFLAGS) -Wp,-MD,$@.d -c -fpic -o $@ $^
	
$(TARGETS): $(OBJS)
	$(CC) -shared -lc -o  $@ $^ $(LDFLAGS)
	
clean:
	rm -f *.o $(TARGETS) *.d 