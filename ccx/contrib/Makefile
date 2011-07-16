LIBEVENT_PATH     = libevent-1.4.13-stable
LIBEVENT_LIB     ?= $(LIBEVENT_PATH)/.libs/libevent.a

PTYPES_PATH       = ptypes-2.1.1
PTYPES_LIB       ?= $(PTYPES_PATH)/lib/libptypes.a


all: $(LIBEVENT_LIB) $(PTYPES_LIB)
	mkdir -p ./lib
	cp $? ./lib

$(LIBEVENT_LIB):
	-cd $(LIBEVENT_PATH); test ! -f config.status && sh ./configure --disable-shared
	$(MAKE) -C $(LIBEVENT_PATH)

$(PTYPES_LIB):
	$(MAKE) -C $(PTYPES_PATH)

clean:
	-$(MAKE) -C $(LIBEVENT_PATH) distclean
	-$(MAKE) -C $(PTYPES_PATH) clean
	-rm -r ./lib