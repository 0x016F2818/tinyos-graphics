#!/bin/bash
# Program:
# Create a database suitable for this project in one step!
# Backup the database if it already exists and then clear the database.
# History:
# 2012/10/12    Arnold  second release

#配置参数
host=127.0.0.1                      #数据库服务器IP
user=root                           #数据库用户名
password=arnold                     #数据库用户密码
db_name=tinyos                      #数据库名称

RELA_PATH=/home/arnold/Documents/Git/tinyos-graphics/database/
BACKUP_DIR=$RELA_PATH/db_backup/             #备份文件存储路径
LOGFILE=$RELA_PATH/db_backup/data_backup.log #日记文件路径
DATE=`date '+%Y%m%d-%H%M%S'`        #日期格式（作为文件名）
DUMPFILE=$DATE.sql                  #备份文件名
ARCHIVE=$DATE.sql.tgz               #压缩文件名

#如果tinyos数据库已存在则对其备份
#sh db_backup.sh

echo "Begin to resetting the database..."
#read -p "Please input the db_host: " host          # 提示输入数据库服务器地址
#read -p "Please input your db_user:  " user        # 输入数据库用户名
#read -p "Please input your password:  " password   # 输入用户密码
#read -p "Please input your db_name:  " db_name     # 输入数据库名

#如果$db_name数据库已存在，则删除
echo "Drop database if exists $db_name..."
echo "drop database if exists $db_name" | mysql -u $user --password=$password -h $host
#如果$db_name数据库不存在，则创建
echo "Createing database if not exists $db_name..."
echo "create database if not exists $db_name" | mysql -u $user --password=$password -h $host

#创建tinyos用户，并对其权限做设置
echo "createing the connecting user if not exists..."
echo "grant all privileges on tinyos.* to tinyos@'%' identified by 'njjizyj0826'" | mysql -u $user --password=$password -h $host

#初始化数据库
echo "Initializing the database..."
cat $RELA_PATH/db_sql/table_creater.sql $RELA_PATH/db_sql/view_creater.sql $RELA_PATH/db_sql/delete_proc_creater.sql \
    $RELA_PATH/db_sql/insert_proc_creater.sql $RELA_PATH/db_sql/search_proc_creater.sql | mysql -u $user --password=$password -h $host $db_name
