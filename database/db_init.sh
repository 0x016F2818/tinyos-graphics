#!/bin/bash
# Program:
# Create a database suitable for this project in one step!
# History:
# 2012/10/08    Arnold  First release

#配置参数
host=127.0.0.1                      #数据库服务器IP
user=root                           #数据库用户名
password=arnold                     #数据库用户密码
db_name=tinyos                      #数据库名称
WEBMASTER=jq.arnold.li@gmail.com    #管理员邮箱地址，用以发送备份失败消息提醒

BACKUP_DIR=./db_backup/             #备份文件存储路径
LOGFILE=./db_backup/data_backup.log #日记文件路径
DATE=`date '+%Y%m%d-%H%M%S'`        #日期格式（作为文件名）
DUMPFILE=$DATE.sql                  #备份文件名
ARCHIVE=$DATE.sql.tgz               #压缩文件名
OPTIONS="-u$USER -p$PASSWORD –opt –extended-insert=false –triggers=false -R –hex-blob –flush-logs –delete-master-logs -B $DATABASE"  #mysqldump 参数 详情见帮助 mysqldump －help

#判断备份文件存储目录是否存在，否则创建该目录
if [ ! -d $BACKUP_DIR ] ;
then
mkdir -p "$BACKUP_DIR"
fi

#开始备份之前，将备份信息头写入日记文件
echo " " >> $LOGFILE
echo " " >> $LOGFILE
echo "———————————————–" >> $LOGFILE
echo "BACKUP DATE:" $(date +"%y-%m-%d %H:%M:%S") >> $LOGFILE
echo "———————————————– " >> $LOGFILE

#切换至备份目录
cd $BACKUP_DIR
#使用mysqldump 命令备份制定数据库，并以格式化的时间戳命名备份文件
#mysqldump $OPTIONS > $DUMPFILE
mysqldump -u $user --password=$password -h $host $db_name > $DUMPFILE
#判断数据库备份是否成功
if [ $? == 0 ] ; then
#创建备份文件的压缩包
    tar czvf $ARCHIVE $DUMPFILE >> $LOGFILE 2>&1
#输入备份成功的消息到日记文件
    echo "[$ARCHIVE] Backup Successful!" >> $LOGFILE
#删除原始备份文件，只需保 留数据库备份文件的压缩包即可
    rm -rf $DUMPFILE
    else
        echo "Database Backup Fail!" >> $LOGFILE

#备份失败后向网站管理者发送邮件提醒，需要mailutils或者类似终端下发送邮件工具的支持
#mail -s "Database:$DATABASE Daily Backup Fail" $WEBMASTER
fi
#输出备份过程结束的提醒消息
echo "Backup Process Done"

echo "Begin to resetting the database..."
#read -p "Please input the db_host: " host          # 提示输入数据库服务器地址
#read -p "Please input your db_user:  " user        # 输入数据库用户名
#read -p "Please input your password:  " password   # 输入用户密码
#read -p "Please input your db_name:  " db_name     # 输入数据库名
echo "Making sure the basic database exists:"
#echo "create database if not exists $db_name" | mysql -u $user --password=$password -h $host
#echo "Initializing the database..."
#cat ./db_sql/table_creater.sql ./db_sql/view_creater.sql ./db_sql/delete_proc_creater.sql \
    #./db_sql/insert_proc_creater.sql ./db_sql/search_proc_creater.sql | mysql -u $user --password=$password -h $host $db_name
