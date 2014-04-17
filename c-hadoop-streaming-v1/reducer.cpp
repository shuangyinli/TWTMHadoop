#include "stdio.h"
#include "utils.h"
#include "twtm-learn.h"
#include "string.h"

struct node {
    Document* doc;
    node* next;
    node(Document* doc_, node* next_=NULL) {
        doc = doc_;
        next = next_;
    }
};

void do_pi(node* list_begin, int& num_docs, twtm_model*& model, Config* &config) {
    setbuf(stdout,NULL);
    Document** corpus = new Document*[num_docs];
    node* cur_ptr = list_begin;
    for (int d = 0; d < num_docs; d++) {
        node* temp = cur_ptr;
        corpus[d] = cur_ptr->doc; 
        cur_ptr = cur_ptr->next;
        delete temp;
    }
    int num_labels = model->num_labels;
    fprintf(stderr, "learn pi...\n");
    learn_pi(corpus, model,config);
    for (int i = 0; i < num_labels; i++) {
        printf("pi %d %lf\n", i, model->pi[i]);
    }
    fprintf(stderr, "learn pi done.\n");
    delete model;
    for (int d = 0; d < num_docs; d++) delete corpus[d];
    delete[] corpus;
    num_docs = 0;
}
void reducer(char* infofile, char* settingfile) {
    char prefix[100];
    node* list_begin = NULL;
    node* list_end = NULL;
    int num_docs = 0;
    char last_key=0,key;
    int last_row=-1, last_col=-1,row,col;
    double value;
    twtm_model* model=NULL;
    int num_topics;
    Config* config = NULL;
    int num_labels;
    while (scanf("%s", prefix)!=EOF) {
        if (strcmp(prefix, "pi") == 0) {
            if (model == NULL) {
                model = new twtm_model(infofile);
                config = new Config(settingfile);
                num_topics = model->num_topics;
                num_labels = model->num_labels;
            }
            int temp;
            scanf("%d",&temp);//read 0
            scanf("%d",&temp);//read 0
            int docid;
            int doc_num_labels;
            int doc_num_words;
            scanf("%d%d", &docid, &doc_num_labels);
            int* labels_ptr = new int[doc_num_labels];
            double* xi = new double[doc_num_labels];
            for (int i = 0; i < doc_num_labels; i++) {
                scanf("%d:%lf", &labels_ptr[i], &xi[i]);
            }
            scanf("%d", &doc_num_words);
            int* words_ptr = new int[doc_num_words];
            int* words_cnt_ptr = new int[doc_num_words];
            double* log_gamma = new double[doc_num_words * num_topics];
            for (int i = 0; i < doc_num_words; i++) {
                scanf("%d:%d", &words_ptr[i], &words_cnt_ptr[i]);
            }
            for (int i = 0; i < doc_num_words * num_topics; i++) {
                scanf("%lf", &log_gamma[i]);
            }
            double doc_lik;
            scanf("%lf", &doc_lik);
            printf("lik %d %lf\n", docid, doc_lik);
            Document* doc = new Document(labels_ptr, words_ptr, words_cnt_ptr, xi, log_gamma, doc_num_labels, doc_num_words, num_topics, docid);
            if (list_begin == NULL) {
                list_end = list_begin = new node(doc);
            }
            else {
                list_end->next = new node(doc);
                list_end = list_end->next;
            }
            num_docs ++;
            continue;
        }
        if (num_docs != 0) {
            model->num_docs = num_docs;
            do_pi(list_begin, num_docs, model, config);
        }
        key = prefix[0];
        double log_value = 0;
        scanf("%d%d%lf", &row, &col, &log_value);
        if (last_key == key && last_row == row && last_col == col) {
            value = util::log_sum(value, log_value);
        }
        else {
            if (last_key == 'p') {
                printf("phi %d %d %lf\n", last_row, last_col, value);
            }
            else if (last_key == 't') {
                printf("theta %d %d %lf\n", last_row, last_col, value);
            }
            value = log_value;
        }
        last_key = key;
        last_row = row;
        last_col = col;
    }
    if (last_key == 'p') {
        printf("phi %d %d %lf\n", last_row, last_col, value);
    }
    else if (last_key == 't') {
        printf("theta %d %d %lf\n", last_row, last_col, value);
    }
    else if (num_docs != 0) {
        do_pi(list_begin, num_docs, model, config);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("usage: <info file> <settingfile>\n");
        return -1;
    }
    reducer(argv[1], argv[2]);
    return 0;
}
