TARGETS=c_example lua_example

all: $(TARGETS)

c_example: LDLIBS+=-lcutl

lua_example: LDLIBS+=-llua -lm -llutl

clean:
	rm -f $(TARGETS)

.PHONY: clean
