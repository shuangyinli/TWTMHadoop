#include "time.h"
#include "utils.h"
#include "string.h"
#include "model.h"
#include "twtm-learn.h"


void read_data_info(char* infofile, int& num_labels, int& num_words, int& num_topics) {
    FILE* fp = fopen(infofile,"r");
    char str[100];
    int value;
    while (fscanf(fp,"%s%d",str,&value)!=EOF) {
        if (strcmp(str,"num_labels:") == 0)num_labels = value;
        if (strcmp(str, "num_words:") == 0)num_words = value;
        if (strcmp(str, "num_topics:") == 0)num_topics = value;
    }
    fprintf(stderr,"num_labels: %d\nnum_words: %d\nnum_topics: %d\n",num_labels,num_words, num_topics);
    fclose(fp);
}

void print_para(twtm_model* model, char* model_root, char* prefix) {
    char pi_file[1000];    
    sprintf(pi_file, "%s/%s.pi", model_root,prefix);
    char theta_file[1000];
    sprintf(theta_file,"%s/%s.theta",model_root,prefix);
    char phi_file[1000];
    sprintf(phi_file,"%s/%s.phi",model_root,prefix);
    model->print_mat(model->pi, model->num_labels, 1, pi_file);
    model->print_mat(model->log_theta, model->num_labels, model->num_topics, theta_file);
    model->print_mat(model->log_phi, model->num_topics, model->num_words, phi_file);
}

void run(char* model_root, char* prefix, char* infofile) {
    twtm_model* model = new twtm_model(model_root, prefix, infofile);
    char pre[100];
    srand(time(NULL));
    double sum_lik = 0.0;
    int docid, labelid, topicid, wordid;
    double lik;
    int num_words = model->num_words;
    int num_topics = model->num_topics;
    double temp;

    while (scanf("%s", pre) != EOF) {
        if (strcmp(pre, "lik") == 0) {
            scanf("%d%lf", &docid, &lik);
            sum_lik += lik;
        }
        else if (strcmp(pre, "pi") == 0) {
            scanf("%d%lf", &labelid, &temp);
            model->pi[labelid] = temp;
        }
        else if (strcmp(pre,"phi") == 0) {
            scanf("%d%d%lf", &topicid, &wordid, &temp);
            model->log_phi[topicid * num_words + wordid] = temp;
        }
        else if (strcmp(pre,"theta") == 0) {
            scanf("%d%d%lf", &labelid, &topicid, &temp);
            model->log_theta[labelid * num_topics + topicid] = temp;
        }
    }
    normalize_log_matrix_rows(model->log_theta, model->num_labels, num_topics);
    normalize_log_matrix_rows(model->log_phi, num_topics, model->num_words);
    print_para(model, model_root, prefix);
    printf("%lf\n", sum_lik);
    delete model;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("usage: <model_root_dir> <prefix> <info file>\n");
        return -1;
    }
    run(argv[1], argv[2], argv[3]);
    return 0;
}
