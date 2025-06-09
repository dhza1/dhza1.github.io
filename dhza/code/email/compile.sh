export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
g++ main.cpp -L/usr/local/lib -lsqlitecpp -lsqlite3 -o email && ./email
