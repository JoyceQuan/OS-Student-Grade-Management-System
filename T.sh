# g++ -o Compile/Terminal T.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
# g++ -o Compile/Terminal T.cpp QUERY.cpp PAGE_CHANGE.cpp PRIVILEDGE.cpp CODES.cpp USER.cpp COURSE.cpp FileSystem/inode.cpp FileSystem/txt.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
g++ -o Compile/Terminal T.cpp QUERY.cpp PAGE_CHANGE.cpp PRIVILEDGE.cpp CODES.cpp USER.cpp COURSE.cpp inode.cpp txt.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
Compile/Terminal