#include "stdio.h"
#include "twtm-inference.h"


void print_doc(Document* doc, int num_learn_pi) {
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
    printf("pi 0 %d %d %d", util::randint(num_learn_pi), doc->docid, doc->num_labels);
    for (int i = 0; i < doc_num_labels; i++) {
        printf(" %d:%lf", doc->labels_ptr[i], doc->xi[i]);
    }
    printf(" %d", doc_num_words);
    for (int i = 0; i < doc_num_words; i++) {
        printf(" %d:%d", doc->words_ptr[i], doc->words_cnt_ptr[i]);
    }
    for (int i = 0; i < doc_num_words; i++) {
        for (int k = 0; k < num_topics; k++) {
            printf(" %lf", doc->log_gamma[i * num_topics + k]);
        }
    }
    printf(" %lf\n", doc->lik);
}

void mapper(char* model_root, char* prefix, char* infofile, char* settingfile, int num_learn_pi){
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
	srand(time(NULL));
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
        print_doc(doc, num_learn_pi);
        delete doc;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        printf("usage: <model_root> <prefix> <infofile> <settingfile> <num_learn_pi>\n");
        return -1;
    }
    mapper(argv[1], argv[2], argv[3], argv[4], atoi(argv[5]));
    return 0;
}

