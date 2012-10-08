#!/bin/bash
# Program:
# Create a database suitable for this project quickly!
# History:
# 2012/10/08    Arnold  First release
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH

host=127.0.0.1
user=root
password=njjizyj0826
db_name=test3

#read -p "Please input the db_host: " host  # 提示输入数据库服务器地址
#read -p "Please input your db_user:  " user   # 输入数据库用户名
#read -p "Please input your password:  " password   # 输入用户密码
#read -p "Please input your db_name:  " db_name   # 输入数据库名

echo "Make sure the basic database exists:"
echo "create database if not exists $db_name" | mysql -u $user -h $host --password=$password
echo "Initialize the database..."
cat ./db_sql/table_creater.sql ./db_sql/view_creater.sql ./db_sql/insert_proc_creater.sql\
    ./db_sql/search_proc_creater.sql | mysql -u $user -h $host --password=$password $db_name
