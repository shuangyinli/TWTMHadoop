#!/bin/bash

if [ $# -ne 10 ]
then
    echo "usage: <input file> <num_topics> <model dir> <res dir> <part data dir> <num round> <num mapper> <hdfs input dir> <hdfs out dir> <hdfs part data dir>"
    exit -1
fi

input_file=$1
num_topics=$2
model_dir=$3
res_dir=$4
part_data_dir=$5
num_round=$6
num_mapper=$7
HADOOP_DIR=$8
HADOOP_INPUT="$8/wordpress-hdfs-filelist.txt"
HADOOP_OUTPUT="$9"
HADOOP_PART_INPUT=${10}


HADOOP_HOME="/opt/hadoop/"
#HADOOP_DIR=`dirname $HADOOP_INPUT`"/"


rm $part_data_dir/part-*

./split_trainfile.py $input_file docid-mapperid.txt $part_data_dir $num_mapper

$HADOOP_HOME/bin/hadoop fs -rmr $HADOOP_PART_INPUT
$HADOOP_HOME/bin/hadoop fs -mkdir $HADOOP_PART_INPUT
$HADOOP_HOME/bin/hadoop fs -put $part_data_dir/part-* $HADOOP_PART_INPUT

hdfs_filelist="$res_dir/wordpress-hdfs-filelist.txt"
>$hdfs_filelist
for ((i=0; i < $num_mapper; i++))
do
    echo "$HADOOP_PART_INPUT/part-$i" >> $hdfs_filelist
done

$HADOOP_HOME/bin/hadoop fs -rmr $HADOOP_DIR
$HADOOP_HOME/bin/hadoop fs -mkdir $HADOOP_DIR
$HADOOP_HOME/bin/hadoop fs -put $hdfs_filelist $HADOOP_DIR

./init_model $input_file $num_topics $model_dir

cp setting.txt $model_dir

last_lik=0
lik_file=$res_dir"/likehoods.txt"
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
                -mapper "./run-mapper.sh map-train.txt" \
                -reducer "./reducer" \
                -partitioner org.apache.hadoop.mapred.lib.KeyFieldBasedPartitioner \
                -cacheArchive hdfs://master:54310/user/cis/$HADOOP_DIR/model.tar.gz#model \
                -file "run-mapper.sh" \
                -file "mapper" \
                -file "reducer" \
                -jobconf mapred.job.name="[TWTM-round-$i-k$num_topics-num_mapper$num_mapper-hadoopv3]" \
		        -jobconf mapred.max.split.size="5000000" \
                -jobconf mapred.map.tasks="$num_mapper" \
                -jobconf mapred.reduce.tasks="$num_mapper" 

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
    echo -e "num round $i likehood: $lik converged:$converged"
    last_lik=$lik
    converged_flag=`python -c "print (float($converged) > 0.0001)"`
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
