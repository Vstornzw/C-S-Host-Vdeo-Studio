20200403 开工第一天！！

20200405 完成用户注册
 如果想本程序运行起来，注意创建数据库过程：
 1 cmd->mysql -u root -p
 2 show databases;
 3 drop database (数据库名)；
 4 create database stu2020； 
 5 use stu2020；
 6 create table tb_user(Id varchar(20),user_name varchar(20),user_pwd varchar(20),online varchar(20),money varchar(20),score varchar(20)); （可复制）
 7 desc tb_user;
 8 select * from tb_user;
 9 delete from tb_user where user_name = 1;
10 drop table table_name;
11 delete from tb_user where user_name = 2;(清除数据表中某一项)
12 delete from tb_user;(清除整个数据表里面的内容)