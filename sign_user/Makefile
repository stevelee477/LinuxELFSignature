EXE = sign user verify

CXX = g++

LIBS = -lssl -lcrypto -lpthread -lstdc++

CXXFLAGS = -O2 -std=c++11 -g

SIGNELF_OBJS = signelf.o readelf.o

SIGN_OBJS = sign.o $(SIGNELF_OBJS)

USER_OBJS = user.o $(SIGNELF_OBJS)

VERIFY_OBJS = verify.o $(SIGNELF_OBJS)

all: $(EXE)

sign: $(SIGN_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(SIGN_OBJS) $(LIBS)

user: $(USER_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(USER_OBJS) $(LIBS)

verify: $(VERIFY_OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(VERIFY_OBJS) $(LIBS)

.PHONY=clean

clean:
	rm -rf $(EXE) $(SIGN_OBJS) $(USER_OBJS) $(VERIFY_OBJS)