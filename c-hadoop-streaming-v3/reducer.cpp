#include "stdio.h"
#include "utils.h"
#include "twtm-learn.h"
#include "string.h"

void reducer() {
    char prefix[100];
    char last_key=0,key;
    int last_row=-1, last_col=-1,row,col;
    double value;
    int pi_cnt = 0;
    int pi_label = -1;
    int last_pi_label = -1;
    while (scanf("%s", prefix)!=EOF) {
        if (strcmp(prefix, "lik") == 0) {
            int docid;
            double lik;
            scanf("%d%lf", &docid, &lik);
            printf("lik %d %lf\n", docid, lik);
            continue;
        }
        if (strcmp(prefix, "pi") == 0) {
            key = prefix[1];
            int cnt;
            double v;
            scanf("%d", &pi_label);
            scanf("%d", &cnt);//read 0
            scanf("%d", &cnt);
            scanf("%lf", &v);
            if (last_key == key && last_pi_label == pi_label) {
                value += v;
                pi_cnt += cnt;
            }
            else {
                if (last_key == 'p') {
                    printf("phi %d %d %lf\n", last_row, last_col, value);
                }
                else if (last_key == 't') {
                    printf("theta %d %d %lf\n", last_row, last_col, value);
                }
                else if (last_key == 'i') {
                    printf("pi %d %lf\n", last_pi_label, value/pi_cnt);
                }
                value = v;
                pi_cnt = cnt;
            }
            last_pi_label = pi_label;
            last_key = key;
            continue;
        }
        else key = prefix[0];
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
            else if (last_key == 'i') {
                printf("pi %d %lf\n", last_pi_label, value/pi_cnt);
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
    else if (last_key == 'i') {
        printf("pi %d %lf\n", last_pi_label, value/pi_cnt);
    }
}

int main(int argc, char* argv[]) {
    reducer();
    return 0;
}
