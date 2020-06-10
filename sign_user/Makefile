EXE = sign user

CXX = g++

LIBS = -lssl -lcrypto -lpthread -lstdc++

CXXFLAGS = -O2 -std=c++11

SIGN_OBJS = sign.o signelf.o readelf.o

USER_OBJS = user.o signelf.o readelf.o

all: $(EXE)

sign: $(SIGN_OBJS)
	$(CXX) -o sign $(CXXFLAGS) $(SIGN_OBJS) $(LIBS)

user: $(USER_OBJS)
	$(CXX) -o user $(CXXFLAGS) $(USER_OBJS) $(LIBS)

.PHONY=clean

clean:
	rm -rf $(EXE) $(SIGN_OBJS) $(USER_OBJS)