#include "stdio.h"
#include "twtm-inference.h"


inline void print_doc(Document* doc) {
    printf("%d %lf\n", doc->docid, doc->lik);
}

void mapper(char* model_root, char* prefix, char* infofile, char* settingfile){
    setbuf(stdout,NULL);
    fprintf(stderr, "load init model..\n");
    twtm_model* model = new twtm_model(model_root, prefix, infofile);
    fprintf(stderr, "num_labels:%d\nnum_words:%d\nnum_topics:%d\n", model->num_labels, model->num_words, model->num_topics);
    int num_topics = model->num_topics;
    Config* config = new Config(settingfile);
    int doc_num_labels;
    int doc_num_words;
    int docid;
    char str[100];
    //FILE* fp = fopen(inputfile, "r");
    while (scanf("%d", &docid) != EOF) {
        scanf("%d", &doc_num_labels);
        int* labels_ptr = new int [doc_num_labels];
        for (int i = 0; i < doc_num_labels; i++) scanf("%d", &labels_ptr[i]);
        scanf("%s", str);
        scanf("%d", &doc_num_words);
        int* words_ptr = new int[doc_num_words];
        int* words_cnt_ptr = new int [doc_num_words];
        for (int i =0; i < doc_num_words; i++) {
            scanf("%d:%d", &words_ptr[i],&words_cnt_ptr[i]);
        }
        //printf("doc_num_labels:%d\ndoc_num_words:%d\n", doc_num_labels, doc_num_words);
        Document* doc = new Document(labels_ptr, words_ptr, words_cnt_ptr, doc_num_labels, doc_num_words, num_topics, docid);
        //printf("inference...\n");
        do_inference(doc, model, config);
        //printf("inference done\n");
        print_doc(doc);
        delete doc;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("usage: <model_root> <prefix> <infofile> <settingfile>\n");
        return -1;
    }
    mapper(argv[1], argv[2], argv[3], argv[4]);
    return 0;
}

