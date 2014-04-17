


Mapper
-------------------


```
-mapper "./mapper model/ init model/info.txt model/setting.txt"
```


Reducer
-------------------

```
-reducer "./reducer model/info.txt model/setting.txt"
```

info.txt:
```
num_labels: 1000
num_words: 1000
num_topics: 1000
```
Set your input data's label number, word number and topic number in the info.txt <br/>


setting.txt is the same with the TWTM model. <br/>

init is a model prefix indicator, we will get the init.theta, init.phi and init.pi file and so on in the model/ path. <br/>

More details about streaming please see the my run-train.sh or run-test.sh <br/>
