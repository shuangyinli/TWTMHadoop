
Preprocess
---------------
```
./cluster-label <train file> <docid mapid file>
```
We first cluster the documents by our cluster algorithm. Then we split the input file according the docid mapid file.
```
python split_trainfile.py <train file> <docid mapid file> <out_dir> <num mapper>
```
We can get the hdfs input file in the out_dir and put the out_dir in the hdfs. Then we use the file name in out_dirs as our hadoop's input.


Mapper
-------------------


```
-mapper "./run-mapper.sh <tmp_file>"
```
you can set any filename as tmp_file.


Reducer
-------------------

```
-reducer "./reducer"
```

More details about streaming please see the my run-train.sh or run-test.sh <br/>



