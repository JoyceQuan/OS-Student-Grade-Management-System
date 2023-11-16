# OS-Student-Grade-Management-System
基于linux的C++编程实现的OS大作业：学生成绩管理系统

## **0 Assumption**
1. 暂时假设作业是字符串流，而非文件。
2. 一门课一个老师

## **1 Overview**
### **1.1 思路总览**
    系统构成=网络通信+登录等数据库操作+文件系统操作+备份与恢复（快照）
- **网络通信**：自定义协议实现本机多终端通信（实现使用socket）
- **数据库**：本项目仅将用户信息(id,account,name)和课程信息(id,name,credit)放入数据库中。数据库内数据不存在修改（备份恢复与注册除外）。
- **文件系统**：
  ![privilege](./readme_pic/privilege.png)
  - 用一个txt文件模拟磁盘，使用inode做index管理。（具体见1.3）
  - 内置函数:createDir，deleteDir，ls，createFile，deleteFile，readFile，writeFile，changePrivilege。
  - 权限设置：权限设置是针对文件来设置的。功能函数均可访问，但访问后可能会有权限的阻挡，返回不可访问。（对于还没有文件的操作也是一样，要先输入路径，因此判断该路径下用户有没有权限做创建和查看的任务。）
  - 功能函数：功能函数相当于对内置函数的包装。例如：创建课程（创建文件夹），删除课程（删除文件夹），查看当前所选全部课程（ls），写作业（创建文件+写文件），查看作业（读文件），删除上传的文件（删除文件）......
  
- **备份与恢复**：
  - 磁盘上的文件：暂时用copy的方式备份。
  - 数据库的更改：以日志的方式做记录。
  
### **1.2 代码架构**


### **1.3 File System**
由于只考虑字符串的读写来模拟文件读写，所以客户端是不需要文件夹做数据记录的，因此只需要考虑终端的文件结构。
- **File Structure**
  ![FileStructure](./readme_pic/FileStructure.png)
- **txt模拟磁盘**
  ![备份结构](./readme_pic/备份结构.png)
- **MemExpansion**  
  当整个结构分配的系统空间不足的时候，从上向下，从外层到里层，找到最低级inodeLevel（inode属性inodeLevel：该inode连接的是几级结构）的inode,将对应的数据block的位置改为一个DirBlock，然后把该数据block置于改成目录的首位。
  ![MemExpansion](./readme_pic/MemExpansion.png)
- **FileExpansion**  
  当文件大小大于1blcok时，需要fileExpansion。  
  暂时还没细想，实现过程书中有，目前推测就是在相关的block间像Dirblock那样加指针。



## 2 Preparation
### Prepare MySQL
* [Install MySQL in Ubuntu](https://blog.csdn.net/fanrongwoaini/article/details/107518693)

* Create 2 tables: course、users
```sql
create TABLE course (
ID varchar(30) NOT NULL,
Course_name varchar(30),
Credit INT,
PRIMARY KEY(ID)) ;

create TABLE users(
ID varchar(30) NOT NULL,
Account  varchar(30),
Password  varchar(30),
Identity INT NOT NULL,
 PRIMARY KEY(ID));
```
* Insect course
```sql
insert into course values('CS101','Computer Science',2);
insert into course values('CS201','Data Structure',4);
insert into course values('CS505','C++ Programming',3);
insert into course values('CS809','Python',2);
insert into course values('B0103','Organic Chemistry',3);
insert into course values('B0511','Chemical Experiment',2);
insert into course values('01109','College Physics',4);
insert into course values('01201','Quantum Mechanics',4);
insert into course values('LA101','Chinese Language',2);
insert into course values('LA255','English Writing',2);
```
* Insect users:0:学生；1:老师；2：管理员
```sql
insert into users values('1','1','1',1);
insert into users values('2','2','2',2);
insert into users values('3','3','3',3);
```

## Code
* Terminal:T.cpp ;  Client:U.cpp