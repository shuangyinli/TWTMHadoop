#include "stdio.h"
#include "twtm-inference.h"
#include "twtm-learn.h"

struct node {
    Document* doc;
    node* next;
    node(Document* doc_, node* next_=NULL) {
        doc = doc_;
        next = next_;
    }
};

void print_doc(Document* doc) {
    int doc_num_labels = doc->num_labels;
    int doc_num_words = doc->num_words;
    int num_topics = doc->num_topics;
    double sigma_xi = 0;
    for (int i = 0; i < doc_num_labels; i++) sigma_xi += doc->xi[i];
    for (int i = 0; i < doc_num_labels; i++) {
        int labelid = doc->labels_ptr[i];
        for (int k = 0; k < num_topics; k++) {
            int j = 0;
            double temp = log(doc->words_cnt_ptr[j]) + doc->log_gamma[j * num_topics + k] + log(doc->xi[i]) - log(sigma_xi);
            for (j = 1; j < doc_num_words; j++) {
                temp = util::log_sum(temp, log(doc->words_cnt_ptr[j]) + doc->log_gamma[j * num_topics + k] + log(doc->xi[i]) - log(sigma_xi));
            }
            printf("theta %d %d %lf\n", labelid, k, temp);
        }
    }
    for (int k = 0; k < num_topics; k++) {
        for (int i = 0; i < doc_num_words; i++) {
            int wordid = doc->words_ptr[i];
            printf("phi %d %d %lf\n", k, wordid, log(doc->words_cnt_ptr[i]) + doc->log_gamma[i*num_topics + k]);
        }
    }
    printf("lik %d %lf\n", doc->docid, doc->lik);
}

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
        printf("pi %d 0 1 %lf\n", i, model->pi[i]);
    }
    fprintf(stderr, "learn pi done.\n");
    delete model;
    for (int d = 0; d < num_docs; d++) delete corpus[d];
    delete[] corpus;
    num_docs = 0;
}

void mapper(char* model_root, char* prefix, char* infofile, char* settingfile){
    srand(time(NULL));
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
    node* list_begin = NULL;
    node* list_end = NULL;
    int num_docs = 0;
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
        Document* doc = new Document(labels_ptr, words_ptr, words_cnt_ptr, doc_num_labels, doc_num_words, num_topics, docid);
        do_inference(doc, model, config);
        print_doc(doc);
        if (list_begin == NULL) {
            list_end = list_begin = new node(doc);
        }
        else {
            list_end->next = new node(doc);
            list_end = list_end->next;
        }
        num_docs ++;
        if (num_docs % 1000 == 0) fprintf(stderr, "inference %d docs done.\n", num_docs);
    }
	if (num_docs != 0) {
	    model->num_docs = num_docs;
		do_pi(list_begin, num_docs, model, config);
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

