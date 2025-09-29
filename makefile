CXX = g++

CPPFLAGS = -Wfatal-errors -Werror \
-Wall -Wextra -Wpedantic \
-Wfloat-equal -Wsign-conversion -Wfloat-conversion \
-Wno-error=unused-but-set-parameter \
-Wno-error=unused-but-set-variable \
-Wno-error=unused-function \
-Wno-error=unused-label \
-Wno-error=unused-local-typedefs \
-Wno-error=unused-parameter \
-Wno-error=unused-result \
-Wno-error=unused-variable \
-Wno-error=unused-value

bin/libjamerror.a: obj/jamerror.o | bin
	ar rcs $@ $<

bin:
	mkdir -p bin

obj/jamerror.o: src/jamerror.cpp | obj
	$(CXX) $(CPPFLAGS) -I include -c $< -o $@

obj:
	mkdir -p obj

test/test: test/test.cpp include/jamerror.hpp bin/libjamerror.a
	$(CXX) $(CPPFLAGS) -I include $< bin/libjamerror.a -o $@

.PHONY: test clean

clean:
	rm -rf obj bin test/test

test: test/test
	test/test

install: bin/libjamerror.a include/jamerror.hpp
	cp bin/libjamerror.a /usr/local/lib/
	chmod a=r,u=rw /usr/local/lib/libjamerror.a
	cp include/jamerror.hpp /usr/local/include/
	chmod a=r,u=rw /usr/local/include/jamerror.hpp

uninstall:
	rm /usr/local/lib/libjamerror.a
	rm /usr/local/include/jamerror.hpp
