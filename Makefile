TARGET_FILE=call-cntr

LLVM_CONFIG=llvm-config

CXX=`$(LLVM_CONFIG) --bindir`/clang
CXXFLAGS=`$(LLVM_CONFIG) --cppflags` -fPIC -fno-rtti
LDFLAGS=`$(LLVM_CONFIG) --ldflags`

all: $(TARGET_FILE).so

$(TARGET_FILE).so: $(TARGET_FILE).o
	$(CXX) -shared $(TARGET_FILE).o -o $(TARGET_FILE).so $(LDFLAGS) -fPIC

$(TARGET_FILE).o: $(TARGET_FILE).cpp
	$(CXX) -c $(TARGET_FILE).cpp -o $(TARGET_FILE).o $(CXXFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(TARGET_FILE).so
