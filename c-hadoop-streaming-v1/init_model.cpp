#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"


void read_data(char* filename, int& num_words, int& num_labels) {
    num_words = 0;
    num_labels = 0;
    FILE* fp = fopen(filename,"r");
    int doc_num_labels;
    int doc_num_words;
    char str[10];
    int num_docs = 0;
    int labelid, wordid, word_cnt;
    while(fscanf(fp,"%d",&doc_num_labels) != EOF) {
        for (int i = 0; i < doc_num_labels; i++) {
            fscanf(fp,"%d",&labelid);
            num_labels = num_labels > labelid?num_labels:labelid;
        }
        fscanf(fp,"%s",str); //read @
        fscanf(fp,"%d", &doc_num_words);
        for (int i =0; i < doc_num_words; i++) {
            fscanf(fp,"%d:%d", &wordid,&word_cnt);
            num_words = num_words < wordid?wordid:num_words;
        }
        num_docs ++;
    }
    fclose(fp);
    num_words ++;
    num_labels ++;
    printf("num_docs: %d\nnum_labels: %d\nnum_words:%d\n",num_docs,num_labels,num_words);
}

inline double myrandom(){
    return rand()/(RAND_MAX+1.0);
}

void print_mat(double* mat, int row, int col, char* filename) {
    FILE* fp = fopen(filename,"w");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            fprintf(fp,"%lf ",mat[i*col + j]);
        }   
        fprintf(fp,"\n");
    }   
    fclose(fp);
}
void init_model(char* output_dir, int num_words, int num_labels, int num_topics){
    char log_theta_file[1000];
    char log_phi_file[1000];
    char pi_file[1000];
    sprintf(log_theta_file, "%s/init.theta", output_dir);
    sprintf(log_phi_file, "%s/init.phi", output_dir);
    sprintf(pi_file, "%s/init.pi", output_dir);
    double* log_theta = (double*) calloc(num_labels * num_topics, sizeof(double));
    double* log_phi = (double*) calloc(num_topics * num_words, sizeof(double));
    double* pi = (double*) calloc(num_labels, sizeof(double));
    for (int i = 0; i < num_labels; i++) {
        pi[i] = myrandom() * 0.5 + 1;
        double temp = 0;
        for (int k = 0; k < num_topics; k++) {
            double v = myrandom();
            temp += v;
            log_theta[i * num_topics + k] = v;
        }
        for (int k = 0; k < num_topics; k++)log_theta[i*num_topics + k] = log(log_theta[i*num_topics + k] / temp);
    }
    for (int k = 0; k < num_topics; k++) {
        for (int i = 0; i < num_words; i++)log_phi[k*num_words + i] = log(1.0/num_words);
    }
    print_mat(log_theta, num_labels, num_topics, log_theta_file);
    print_mat(log_phi, num_topics, num_words, log_phi_file);
    print_mat(pi, num_labels, 1, pi_file);
    char info_file[1000];
    sprintf(info_file, "%s/info.txt", output_dir);
    FILE* info_fp = fopen(info_file,"w");
    fprintf(info_fp, "num_labels: %d\nnum_words: %d\nnum_topics: %d\n", num_labels, num_words, num_topics);
    fclose(info_fp);
    free(log_theta);
    free(pi);
    free(log_phi);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("usage: <twtm train file> <num topics> <output dir>\n");
        return -1;
    }
    srand(time(NULL));
    int num_words, num_labels;
    int num_topics = atoi(argv[2]);
    read_data(argv[1], num_words, num_labels);
    init_model(argv[3], num_words, num_labels, num_topics);
    return 0;
}
