#ifndef MODEL_H
#define MODEL_H

#include "stdio.h"
#include "utils.h"

struct Document {
    double* xi;
    double* log_gamma;
    int* labels_ptr;
    int* words_ptr;
    int* words_cnt_ptr;
    int num_labels;
    int num_words;
    int num_topics;
    int docid;
    double* topic;
    double lik;
    Document(int* labels_ptr_,int* words_ptr_,int* words_cnt_ptr_,int num_labels_,int num_words_,int num_topics_, int docid_) {
        num_topics = num_topics_;
        num_labels = num_labels_;
        num_words = num_words_;
        xi = new double[num_labels];
        log_gamma = new double[num_words * num_topics];
        topic = new double[num_topics];
        labels_ptr = labels_ptr_;
        words_ptr = words_ptr_;
        words_cnt_ptr = words_cnt_ptr_;
        lik = 100;
        docid = docid_;
        init();
    }
    Document(int* labels_ptr_, int* words_ptr_, int* words_cnt_ptr_, double* xi_, double* log_gamma_, int num_labels_, int num_words_, int num_topics_, int docid_) {
        num_topics = num_topics_;
        num_labels = num_labels_;
        num_words = num_words_;
        xi = xi_;
        log_gamma = log_gamma_;
        labels_ptr = labels_ptr_;
        words_ptr = words_ptr_;
        words_cnt_ptr = words_cnt_ptr_;
        docid = docid_;
    }
    ~Document() {
        if (xi)delete[] xi;
        if (log_gamma) delete[] log_gamma;
        if (labels_ptr) delete[] labels_ptr;
        if (words_ptr) delete[] words_ptr;
        if (words_cnt_ptr) delete[] words_cnt_ptr;
        if (topic) delete[] topic;
    }
    void init();
};

struct twtm_model {
    int num_words;
    int num_topics;
    int num_labels;
    int num_all_words;
    int num_docs;
    double* pi;
    double* log_theta;
    double* log_phi;
    twtm_model(char* model_root, char* prefix, char* infofile) {
        //read_model_info(model_root);
        read_data_info(infofile);
        char pi_file[1000];    
        sprintf(pi_file, "%s/%s.pi", model_root,prefix);
        char theta_file[1000];
        sprintf(theta_file,"%s/%s.theta",model_root,prefix);
        char phi_file[1000];
        sprintf(phi_file,"%s/%s.phi",model_root,prefix);
        pi = load_mat(pi_file, num_labels, 1);
        log_theta = load_mat(theta_file, num_labels, num_topics);
        log_phi = load_mat(phi_file, num_topics, num_words);
    }
    twtm_model(char* infofile, int num_docs_=0) {
        read_data_info(infofile);
        log_theta = log_phi = 0;
        pi = new double[num_labels];
        for (int i = 0; i < num_labels; i++) pi[i] = 0;
        num_docs = num_docs_;
    }
    ~twtm_model() {
        if (pi)delete[] pi;
        if (log_theta) delete[] log_theta;
        if (log_phi) delete[] log_phi;
    }
    double* load_mat(char* filename,int row,int col);
    void read_data_info(char* infofile);
    void print_mat(double* mat, int row, int col, char* filename);
};

struct Config {
    double pi_learn_rate;
    int max_pi_iter;
    double pi_min_eps;
    double xi_learn_rate;
    int max_xi_iter;
    double xi_min_eps;
    int max_em_iter;
    static bool print_debuginfo;
    int num_threads;
    int max_var_iter;
    double var_converence;
    double em_converence;
    Config(char* settingfile) {
        pi_learn_rate = 0.00001;
        max_pi_iter = 100;
        pi_min_eps = 1e-5;
        max_xi_iter = 100;
        xi_learn_rate = 10;
        xi_min_eps = 1e-5;
        max_em_iter = 30;
        num_threads = 1;
        var_converence = 1e-6;
        max_var_iter = 30;
        em_converence = 1e-4;
        if(settingfile) read_settingfile(settingfile);
    }
    void read_settingfile(char* settingfile);
};

#endif

