CXX := g++
CXXFLAGS := -I/usr/include/mysql -I/usr/local/include/mysql++
LDFLAGS := -L/usr/local/lib -lmysqlpp -lmysqlclient -lnsl -lz -lm FeatureVector.cpp DataMatrix.cpp Node.cpp Tree.cpp Forest.cpp DataStore.cpp
EXECUTABLE := ToBeUsedWithRealData
 
all: ToBeUsedWithRealData
  
clean:
	rm -f $(EXECUTABLE) *.o

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
