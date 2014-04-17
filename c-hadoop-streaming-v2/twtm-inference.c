#include "twtm-inference.h"

void inference_gamma(Document* doc, twtm_model* model) {
    double* log_theta = model->log_theta;
    double* log_phi = model->log_phi;
    int num_topics = model->num_topics;
    int num_words = model->num_words;
    int doc_num_words = doc->num_words;
    double* log_gamma = doc->log_gamma;
    double* theta_xi = new double[num_topics];
    double sigma_xi = 0;
    for (int i = 0; i < doc->num_labels; i++){
        sigma_xi += doc->xi[i];
    }
    for (int k = 0; k < num_topics; k++) {
        double temp = 0;
        for (int i = 0; i < doc->num_labels; i++) {
            temp += doc->xi[i]/sigma_xi * log_theta[doc->labels_ptr[i]*num_topics + k];
            //temp *= pow(theta[doc->labels_ptr[i] * num_topics + k],doc->xi[i]/sigma_xi);
        }
        theta_xi[k] = temp;
        //printf("%lf\n",temp);
        //exp_theta_xi[k] = temp;
        if (isnan(temp)) {
            printf("temp nan sigma_xi:%lf\n",sigma_xi);
        }
    }
    for (int i = 0; i < doc_num_words; i++) {
        int wordid = doc->words_ptr[i];
        double sum_log_gamma = 0;
        for (int k = 0; k < num_topics; k++) {
            double temp = log_phi[k * num_words + wordid] + theta_xi[k];
            log_gamma[ i * num_topics + k] = temp;
            if (k == 0) sum_log_gamma = temp;
            else sum_log_gamma = util::log_sum(sum_log_gamma, temp);
        }
        for (int k = 0; k < num_topics; k++)log_gamma[i*num_topics + k] -= sum_log_gamma;
    }
    delete[] theta_xi;
}

void get_descent_xi(Document* doc, twtm_model* model,double* descent_xi) {
    double sigma_xi = 0.0;
    double sigma_pi = 0.0;
    int num_labels = doc->num_labels;
    for (int i = 0; i < num_labels; i++) {
        sigma_xi += doc->xi[i];
        sigma_pi += model->pi[doc->labels_ptr[i]];
    }
    for (int i = 0; i < num_labels; i++) {
        descent_xi[i] = util::trigamma(doc->xi[i]) * ( model->pi[doc->labels_ptr[i]] - doc->xi[i]);
        descent_xi[i] -= util::trigamma(sigma_xi) * (sigma_pi - sigma_xi);
    }
    int doc_num_words = doc->num_words;
    int num_topics = model->num_topics;
    double* log_theta = model->log_theta;
    double* sum_log_theta = new double[num_topics];
    memset(sum_log_theta, 0, sizeof(double) * num_topics);
    for (int k = 0; k < num_topics; k++) {
        sum_log_theta[k] = 0;
        for (int i = 0; i < num_labels; i++) {
            int label_id = doc->labels_ptr[i];
            sum_log_theta[k] +=log_theta[label_id * num_topics + k] * doc->xi[i];
        }
    }
    double* sum_gamma_array = new double[num_topics];
    //memset(sum_gamma_array, 0, sizeof(double) * num_topics);
    for (int k = 0; k < num_topics; k++) {
        sum_gamma_array[k] = 0;
        for (int i = 0; i < doc_num_words; i++) {
            sum_gamma_array[k] += exp(doc->log_gamma[i * num_topics + k]) * doc->words_cnt_ptr[i];
        }
    }
    for (int j = 0; j < num_labels; j++) {
        for (int k = 0; k < num_topics; k++) {
            double temp = 0;
            double sum_gamma = 0.0;
            temp += log_theta[doc->labels_ptr[j] * num_topics + k] * sigma_xi;
            /*for (int i = 0; i < doc_num_words; i++) {
                sum_gamma += doc->gamma[i * num_topics + k] * doc->words_cnt_ptr[i];
            }*/
            sum_gamma = sum_gamma_array[k];
            /*for (int i = 0; i < num_labels; i++) {
                int label_id = doc->labels_ptr[i];
                temp -= log(theta[label_id * num_topics + k] ) * doc->xi[i];
                //printf("theta %f xi %f temp %f\n", theta[label_id * num_topics + k],doc->xi[i],temp);
            }*/
            temp -= sum_log_theta[k];
            temp = sum_gamma * (temp/(sigma_xi * sigma_xi));
            //printf("sum_gamma:%lf temp:%lf descent_xi:%lf\n",sum_gamma,temp,descent_xi[j]);
            if (isnan(temp)) {
                printf("sum_gamma:%lf temp:%lf descent_xi:%lf\n",sum_gamma,temp,descent_xi[j]);
            }
            descent_xi[j] += temp;
        }
        if (isnan(descent_xi[j])) {
            printf("descent_xi nan\n");
        }
    }
    delete[] sum_log_theta;
    delete[] sum_gamma_array;
}

double get_xi_function(Document* doc, twtm_model* model) {
    double xi_function_value = 0.0;
    int num_labels = doc->num_labels;
    double sigma_xi = 0.0;
    double* pi = model->pi;
    double* log_theta = model->log_theta;
    for (int i = 0; i < num_labels; i++) sigma_xi += doc->xi[i];
    for (int i = 0; i < num_labels; i++) {
        xi_function_value += (pi[doc->labels_ptr[i]] - doc->xi[i] )* (util::digamma(doc->xi[i]) - util::digamma(sigma_xi)) + util::log_gamma(doc->xi[i]);
    }
    xi_function_value -= util::log_gamma(sigma_xi);

    int doc_num_words = doc->num_words;
    int num_topics = model->num_topics;

    double* sum_log_theta = new double[num_topics];
    for (int k = 0; k < num_topics; k++) {
        double temp = 0;
        for (int j = 0; j < num_labels; j++) temp += log_theta[doc->labels_ptr[j] * num_topics + k] * doc->xi[j]/sigma_xi;
        sum_log_theta[k] = temp;
    }

    for (int i = 0; i < doc_num_words; i++) {
        for (int k = 0; k < num_topics; k++) {
            double temp = sum_log_theta[k];
            xi_function_value += temp * exp(doc->log_gamma[i * num_topics + k]) * doc->words_cnt_ptr[i];
        }
    }
    delete[] sum_log_theta;
    return xi_function_value;
}


inline void init_xi(double* xi,int num_labels) {
    for (int i = 0; i < num_labels; i++) xi[i] = util::random();//init 100?!
}


void inference_xi(Document* doc, twtm_model* model,Config* config) {
    int num_labels = doc->num_labels;
    double* descent_xi = new double[num_labels];
    init_xi(doc->xi,num_labels);
    double z = get_xi_function(doc,model);
    double learn_rate = config->xi_learn_rate;
    //printf("%lf\n",learn_rate);
    double eps = 10000;
    int num_round = 0;
    int max_xi_iter = config->max_xi_iter;
    double xi_min_eps = config->xi_min_eps;
    double last_z;
    double* last_xi = new double[num_labels];
    do {
        last_z = z;
        memcpy(last_xi,doc->xi,sizeof(double)*num_labels);
        //for (int i = 0; i < num_labels; i++)last_xi[i] = doc->xi[i];
        get_descent_xi(doc,model,descent_xi);
        
        bool has_neg_value_flag = false;
        for (int i = 0; !has_neg_value_flag && i < num_labels; i++) {
            //printf("doc->last_xi: %lf ",doc->xi[i]);
            doc->xi[i] += learn_rate * descent_xi[i];
            if (doc->xi[i] < 0)has_neg_value_flag = true;
            if (isnan(-doc->xi[i])) printf("doc->xi[i] nan\n");
            //printf("doc->xi: %lf learn_rate: %lf, descent_xi: %lf last_xi: %lf\n",doc->xi[i],learn_rate,descent_xi[i],last_xi[i]);
        }
        if ( has_neg_value_flag || last_z > (z = get_xi_function(doc,model))) {
            learn_rate *= 0.1;
            z = last_z;
            eps = 10000;
            memcpy(doc->xi,last_xi,sizeof(double)*num_labels);
            //for (int i = 0; i < num_labels; i++)doc->xi[i] = last_xi[i];
        }
        else eps = util::norm2(last_xi,doc->xi,num_labels);
        num_round ++;
        //printf("xi round %d: ",num_round);
    }
    while (num_round < max_xi_iter && eps > xi_min_eps);
    delete[] last_xi;
    delete[] descent_xi;
}


void do_inference(Document* doc, twtm_model* model, Config* config) {
    int var_iter = 0;
    double lik_old = 0.0;
    double converged = 1;
    double lik;
    while ((converged > config->var_converence) && ((var_iter < config->max_var_iter || config->max_var_iter == -1))) {
//        printf("e-step round: %d\n",var_iter);
        var_iter ++;
        inference_xi(doc, model, config);
        inference_gamma(doc, model);
        lik = compute_doc_likehood(doc,model);
//        printf("E-Step Round %d: %lf\n", var_iter,lik);
        converged = (lik_old -lik) / lik_old;
        lik_old = lik;
    }
    return;
}

