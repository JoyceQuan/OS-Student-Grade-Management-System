# g++ -o Compile/User U.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
g++ -o Compile/UACT UserAndCourseTest.cpp FileSystem/inode.cpp FileSystem/txt.cpp -I/usr/include/mysql -L/usr/lib/x86_64-linux-gnu -lmysqlclient
Compile/UACT