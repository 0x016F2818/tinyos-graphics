#!/bin/bash
# Program:
# Create a database suitable for this project in one step!
# History:
# 2012/10/08    Arnold  First release

#配置参数
host=127.0.0.1                      #数据库服务器IP
user=root                           #数据库用户名
password=njjizyj0826                     #数据库用户密码
db_name=tinyos                      #数据库名称

RELA_PATH=/home/arnold/Documents/Git/tinyos-graphics/database/
BACKUP_DIR=$RELA_PATH/db_backup/             #备份文件存储路径
LOGFILE=$RELA_PATH/db_backup/data_backup.log #日记文件路径
DATE=`date '+%Y%m%d-%H%M%S'`        #日期格式（作为文件名）
DUMPFILE=$DATE.sql                  #备份文件名
ARCHIVE=$DATE.sql.tgz               #压缩文件名

#sh db_backup.sh

echo "Begin to resetting the database..."
#read -p "Please input the db_host: " host          # 提示输入数据库服务器地址
#read -p "Please input your db_user:  " user        # 输入数据库用户名
#read -p "Please input your password:  " password   # 输入用户密码
#read -p "Please input your db_name:  " db_name     # 输入数据库名
echo "Making sure the basic database exists:"
echo "create database if not exists $db_name" | mysql -u $user --password=$password -h $host
echo "Initializing the database..."
cat ./db_sql/table_creater.sql ./db_sql/view_creater.sql ./db_sql/delete_proc_creater.sql \
    ./db_sql/insert_proc_creater.sql ./db_sql/search_proc_creater.sql | mysql -u $user --password=$password -h $host $db_name
