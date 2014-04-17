#!/bin/bash

if [ $# -ne 5 ]
then
    echo "usage: <input file> <hdfs model.tar.gz> <out dir> <hdfs input dir> <hdfs output dir>"
    exit -1
fi

input_file=$1
hdfs_model_file=$2
out_dir=$3


HADOOP_HOME="/opt/hadoop/"
HADOOP_INPUT="$4/sslda.test.hadoop"
HADOOP_OUTPUT="$5/"

HADOOP_DIR=`dirname $HADOOP_INPUT`

$HADOOP_HOME/bin/hadoop fs -mkdir $HADOOP_DIR

awk '{print NR,$0}' $input_file > $out_dir/sslda.test.hadoop

$HADOOP_HOME/bin/hadoop fs -rm $HADOOP_INPUT 

$HADOOP_HOME/bin/hadoop fs -put $out_dir/sslda.test.hadoop $HADOOP_DIR

last_lik=0
lik_file=$out_dir"/test-likehoods.txt"
$HADOOP_HOME/bin/hadoop fs -rmr $HADOOP_OUTPUT
$HADOOP_HOME/bin/hadoop jar $HADOOP_HOME/contrib/streaming/*streaming*.jar \
                -D stream.map.output.field.separator=" "\
                -D map.output.key.field.separator=" "\
                -D stream.num.map.output.key.fields=3 \
                -D num.key.fields.for.partition=3 \
                -input $HADOOP_INPUT \
                -output $HADOOP_OUTPUT \
                -mapper "./test-mapper model/ init model/info.txt model/setting.txt" \
                -partitioner org.apache.hadoop.mapred.lib.KeyFieldBasedPartitioner \
                -cacheArchive hdfs://master:54310/user/cis/$hdfs_model_file#model \
                -file "test-mapper" \
                -jobconf mapred.job.name="[TWTM-inference]" \
		        -jobconf mapred.max.split.size="5000000" \
                -jobconf mapred.map.tasks="40" \
                -jobconf mapred.reduce.tasks="0" 

$HADOOP_HOME/bin/hadoop fs -cat $HADOOP_OUTPUT/part-* > $lik_file

lik=`hadoop fs -cat $HADOOP_OUTPUT/part-* | awk 'BEGIN{cnt=0}{cnt+=$2}END{printf("%.5f\n",cnt);}'`

echo "test likehood: $lik"
echo "test done."
