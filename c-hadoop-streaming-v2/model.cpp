#include "model.h"

void Document::init() {
    for (int i = 0; i < num_labels; i++) {
        xi[i] = util::random(); //100?!
    }
    for (int i = 0; i < num_words; i++) {
        for (int k = 0; k < num_topics; k++) log_gamma[i*num_topics + k] = log(1.0/num_topics);
    }
}

void twtm_model::read_data_info(char* infofile) {
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


double* twtm_model::load_mat(char* filename, int row, int col) {
    FILE* fp = fopen(filename,"r");
    double* mat = new double[row * col];
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            fscanf(fp, "%lf", &mat[i*col+j]);
        }
    }
    fclose(fp);
    return mat;
}
void twtm_model::print_mat(double* mat, int row, int col, char* filename) {
    FILE* fp = fopen(filename,"w");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            fprintf(fp,"%lf ",mat[i*col + j]);
        }   
        fprintf(fp,"\n");
    }   
    fclose(fp);
}
void Config::read_settingfile(char* settingfile) {
    FILE* fp = fopen(settingfile,"r");
    char key[100];
    while (fscanf(fp,"%s",key)!=EOF){
        if (strcmp(key,"pi_learn_rate")==0) {
            fscanf(fp,"%lf",&pi_learn_rate);
            continue;
        }
        if (strcmp(key,"max_pi_iter") == 0) {
            fscanf(fp,"%d",&max_pi_iter);
            continue;
        }
        if (strcmp(key,"pi_min_eps") == 0) {
            fscanf(fp,"%lf",&pi_min_eps);
            continue;
        }
        if (strcmp(key,"xi_learn_rate") == 0) {
            fscanf(fp,"%lf",&xi_learn_rate);
            continue;
        }
        if (strcmp(key,"max_xi_iter") == 0) {
            fscanf(fp,"%d",&max_xi_iter);
            continue;
        }
        if (strcmp(key,"xi_min_eps") == 0) {
            fscanf(fp,"%lf",&xi_min_eps);
            continue;
        }
        if (strcmp(key,"max_em_iter") == 0) {
            fscanf(fp,"%d",&max_em_iter);
            continue;
        }
        if (strcmp(key,"num_threads") == 0) {
            fscanf(fp, "%d", &num_threads);
        }
        if (strcmp(key, "var_converence") == 0) {
            fscanf(fp, "%lf", &var_converence);
        }
        if (strcmp(key, "max_var_iter") == 0) {
            fscanf(fp, "%d", &max_var_iter);
        }
        if (strcmp(key, "em_converence") == 0) {
            fscanf(fp, "%lf", &em_converence);
        }
    }
}

