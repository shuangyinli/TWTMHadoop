#!/bin/bash

tmp_train_file=$1

HADOOP_HOME="/opt/hadoop/"

> $tmp_train_file

has_line=0
while read hdfs_file
do
    $HADOOP_HOME/bin/hadoop fs -cat $hdfs_file >> $tmp_train_file
	has_line=1
done

if [ $has_line -eq 1 ]
then
	cat $tmp_train_file | ./mapper model/ init model/info.txt model/setting.txt
fi



