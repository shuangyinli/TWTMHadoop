#!/bin/bash

echo $#

if [ $# -ne 8 ]

then
    echo "usage: <input file> <num_topics> <model dir> <tmp dir> <num round> <hdfs input dir> <hdfs output dir> <num reducer>"
    exit -1
fi

input_file=$1
num_topics=$2
model_dir=$3
tmp_dir=$4
num_round=$5
HADOOP_DIR=$6

HADOOP_HOME="/opt/hadoop/"

HADOOP_INPUT="$HADOOP_DIR/sslda.train.hadoop"
HADOOP_OUTPUT=$7
num_reducer=$8


awk '{print NR,$0}' $input_file > $tmp_dir/sslda.train.hadoop

#$HADOOP_HOME/bin/hadoop fs -rmr $HADOOP_DIR
$HADOOP_HOME/bin/hadoop fs -mkdir $HADOOP_DIR

$HADOOP_HOME/bin/hadoop fs -rm $HADOOP_INPUT 
$HADOOP_HOME/bin/hadoop fs -put $tmp_dir/sslda.train.hadoop $HADOOP_DIR

./init_model $input_file $num_topics $model_dir
cp /home/lijiefei/sslda-c-dist/label_prediction_version/setting.txt $model_dir

last_lik=0
lik_file=$tmp_dir"/likehoods.txt"
> $lik_file
for (( i=1; i <= $num_round; i++ ))
do
    echo -e "num round $i begin..."
	start_time=`date +%s.%N`
    (rm $model_dir/model.tar.gz; cd $model_dir; tar -zcvf model.tar.gz *;)
    $HADOOP_HOME/bin/hadoop fs -rm $HADOOP_DIR/model.tar.gz
    $HADOOP_HOME/bin/hadoop fs -put $model_dir/model.tar.gz $HADOOP_DIR

    $HADOOP_HOME/bin/hadoop fs -rmr $HADOOP_OUTPUT
    $HADOOP_HOME/bin/hadoop jar $HADOOP_HOME/contrib/streaming/*streaming*.jar \
                -D stream.map.output.field.separator=" "\
                -D map.output.key.field.separator=" "\
                -D stream.num.map.output.key.fields=3 \
                -D num.key.fields.for.partition=3 \
                -input $HADOOP_INPUT \
                -output $HADOOP_OUTPUT \
                -mapper "./mapper model/ init model/info.txt model/setting.txt" \
                -reducer "./reducer model/info.txt model/setting.txt" \
                -partitioner org.apache.hadoop.mapred.lib.KeyFieldBasedPartitioner \
                -cacheArchive hdfs://master:54310/user/cis/$HADOOP_DIR/model.tar.gz#model \
                -file "mapper" \
                -file "reducer" \
                -jobconf mapred.job.name="[TWTM-round-$i-hadoopv1]" \
		        -jobconf mapred.max.split.size="5000000" \
                -jobconf mapred.map.tasks="$num_reducer" \
                -jobconf mapred.reduce.tasks="$num_reducer" 

    lik=`hadoop fs -cat $HADOOP_OUTPUT/part-* | ./cal_para $model_dir init $model_dir/info.txt`
    if [ $i -eq 1 ]
    then
        converged=1000.0
    else
        converged=`python -c "print '%.7f' % ((float($last_lik) - float($lik))/float($last_lik))"`
    fi
	end_time=`date +%s.%N`
	cost_time=`python -c "print $end_time-$start_time"`
    echo -e "$i\t$lik\t$converged\t$cost_time" >> $lik_file
    echo -e "num round $i likehood: $lik converged:$converged cost_time:$cost_time"
    last_lik=$lik
    converged_flag=`python -c "print (float($converged) > 0.0001)"`
	end_time=`date +%s.%N`
    if [ "$converged_flag" == "False" ]
    then
        if [ $i -gt 10 ]
        then
            break;
        fi
    fi
done

(rm $model_dir/model.tar.gz; cd $model_dir; tar -zcvf model.tar.gz *;)
$HADOOP_HOME/bin/hadoop fs -rm $HADOOP_DIR/model.tar.gz
$HADOOP_HOME/bin/hadoop fs -put $model_dir/model.tar.gz $HADOOP_DIR

echo "all done."
