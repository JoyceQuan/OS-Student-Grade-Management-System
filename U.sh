# g++ -o Compile/User U.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
# g++ -o Compile/User U.cpp CODES.cpp QUERY.cpp USER.cpp FileSystem/inode.cpp FileSystem/txt.cpp  -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
g++ -o Compile/User U.cpp CODES.cpp QUERY.cpp USER.cpp inode.cpp txt.cpp  -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
Compile/User