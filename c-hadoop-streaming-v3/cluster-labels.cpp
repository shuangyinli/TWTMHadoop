#include "stdio.h"
#include "string.h"

void read_data(char* filename, int** &corpus_labels_array, int& num_docs, int& num_labels) {
    FILE* fp = fopen(filename, "r");
    char c;
    while ((c=getc(fp))!=EOF) {
        if (c == '\n') num_docs ++;
    }
    fclose(fp);
    fp = fopen(filename, "r");
    corpus_labels_array = new int* [num_docs];
    num_labels = 0;
    char str[1000];
    int doc_num_labels, doc_num_words;
    num_docs = 0;
    while(fscanf(fp, "%d", &doc_num_labels) != EOF) {
        int* labels_ptr = new int [doc_num_labels + 1];
        labels_ptr[0] = doc_num_labels;
        for (int i = 1; i <= doc_num_labels; i++) {
            fscanf(fp, "%d", &labels_ptr[i]);
            num_labels = num_labels > labels_ptr[i]?num_labels:labels_ptr[i];
        }
        corpus_labels_array[num_docs++] = labels_ptr;
        fscanf(fp, "%s", str);
        fscanf(fp, "%d", &doc_num_words);
        for (int i = 0; i < doc_num_words; i++) fscanf(fp, "%s", str);
    }
    num_labels ++;
}

struct listnode {
    int docid;
    listnode* next;
    listnode(int docid_, listnode* next_=NULL) {
        docid = docid_;
        next = next_;
    }
};

listnode** get_index_doc_labels(int** corpus_labels_array, int num_docs, int num_labels) {
    listnode** index_doc_labels_begin = new  listnode* [num_labels];
    listnode** index_doc_labels_end = new listnode* [num_labels];
    for (int i = 0; i < num_labels; i++) index_doc_labels_begin[i] = index_doc_labels_end[i] = NULL;

    for (int d = 0; d < num_docs; d++) {
        int* doc_labels_ptr = corpus_labels_array[d];
        int doc_num_labels = doc_labels_ptr[0];
        for (int i = 1; i <= doc_num_labels; i++) {
            int labelid = corpus_labels_array[d][i];
            if (index_doc_labels_end[labelid] == NULL) index_doc_labels_end[labelid] = index_doc_labels_begin[labelid] = new listnode(d);
            else {
                index_doc_labels_end[labelid]->next = new listnode(d);
                index_doc_labels_end[labelid] = index_doc_labels_end[labelid]->next;
            }
        }
    }
    delete[] index_doc_labels_end;
    return index_doc_labels_begin;
}

void cluster(char* filename, char* output) {
    int** corpus_labels_array;
    int num_labels;
    int num_docs;
    read_data(filename, corpus_labels_array, num_docs, num_labels);
    listnode** index_doc_labels = get_index_doc_labels(corpus_labels_array, num_docs, num_labels);
    int* doc_cluster_id = new int [num_docs];
    memset(doc_cluster_id, -1, sizeof(int) * num_docs);
    int num_cluster = 0;
    listnode* queue_begin = NULL;
    listnode* queue_end = NULL;
    for (int i = 0; i < num_docs; i++) {
        if (doc_cluster_id[i] != -1) continue;
        queue_begin = queue_end = new listnode(i);
        doc_cluster_id[i] = num_cluster;
        while(queue_begin) {
            int docid = queue_begin->docid;
            int doc_num_labels = corpus_labels_array[docid][0];
            for (int l = 1; l <= doc_num_labels; l++) {
                int labelid = corpus_labels_array[docid][l];
                listnode* cur_ptr = index_doc_labels[labelid];
                while (cur_ptr) {
                    int near_docid = cur_ptr->docid;
                    if (doc_cluster_id[near_docid] == -1) {
                        doc_cluster_id[near_docid] = num_cluster;
                        queue_end->next = new listnode(near_docid);
                        queue_end = queue_end->next;
                    }
                    if (doc_cluster_id[near_docid] != num_cluster) {
                        fprintf(stderr, "algorithm error!\n");
                    }
                    cur_ptr = cur_ptr->next;
                }
            }
            listnode* temp = queue_begin;
            queue_begin = queue_begin->next;
            delete temp;
        }
        num_cluster ++;
    }
	printf("num_cluster: %d\n", num_cluster);
    FILE* fp = fopen(output, "w");
    for (int d = 0; d < num_docs; d++) {
        fprintf(fp, "%d %d\n", d, doc_cluster_id[d]);
    }
    fclose(fp);
    for (int i = 0; i < num_docs; i++) {
        delete[] corpus_labels_array[i];
    }
    delete[] corpus_labels_array;
    delete[] doc_cluster_id;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("usage: <input file> <outfile>\n");
        return -1;
    }
    cluster(argv[1], argv[2]);
    return 0;
}

