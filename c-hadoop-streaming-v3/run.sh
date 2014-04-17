#!/bin/bash

num_fold=1
num_mapper=10

twtm_data_dir="/home/lijiefei/Data/IMDB/5fold/$num_fold-fold/"

twtm_train_file=$twtm_data_dir"/twtm.train"
twtm_test_file=$twtm_data_dir"/twtm.test"


#topics=(10 20 50 100 150 200)
topics=(10)
l=${#topics[@]}

for ((i=0;i<$l;i++))
do
num_topics=${topics[$i]}

model_dir="imdb-model/$num_fold-fold/$num_topics-$num_mapper/"
tmp_dir="imdb-tmp/$num_fold-fold/$num_topics-$num_mapper/"
log_dir="imdb-log/$num_fold-fold/$num_topics-$num_mapper/"

hdfs_input_dir="lijiefei/twtm/c-hadoop-v2-lessnetworkcost/$model_dir/"
hdfs_output_dir="lijiefei/twtm/c-hadoop-v2-lessnetworkcost/out/imdb-model/$num_fold-fold/$num_topics-$num_mapper/"
test_hdfs_output_dir="lijiefei/twtm/c-hadoop-v2-lessnetworkcost/out/imdb-test-likehood/$num_fold-fold/$num_topics-$num_mapper/"

if [ ! -d $model_dir ]
then
    mkdir -p $model_dir
fi

if [ ! -d $tmp_dir ]
then
    mkdir -p $tmp_dir
fi

if [ ! -d $log_dir ]
then
    mkdir -p $log_dir
fi


./run-train.sh $twtm_train_file $num_topics $model_dir/ $tmp_dir/ 100 $num_mapper $hdfs_input_dir $hdfs_output_dir 1> $log_dir/train.stdout 2> $log_dir/train.stderr

./run-test.sh $twtm_test_file $hdfs_input_dir/model.tar.gz $tmp_dir/ $hdfs_input_dir $test_hdfs_output_dir 1> $log_dir/test.stdout 2> $log_dir/test.stderr

done

