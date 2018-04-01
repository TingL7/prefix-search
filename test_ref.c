#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tst.h"

/** constants insert, delete, max word(s) & stack nodes */
enum { INS, DEL, WRDMAX = 256, STKMAX = 512, LMAX = 1024 };
#define REF INS
#define CPY DEL

/* timing helper function */
static double tvgetf(void)
{
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;

    return sec;
}

/* simple trim '\n' from end of buffer filled by fgets */
static void rmcrlf(char *s)
{
    size_t len = strlen(s);
    if (len && s[len - 1] == '\n')
        s[--len] = 0;
}


#define IN_FILE "cities.txt"
#define OUT_FILE "out_ref.txt"

int main(int argc, char **argv)
{
    char word[WRDMAX] = "", *str, str_ar[WRDMAX * 3];
    char (*word_tst)[WRDMAX] = malloc(260000 * sizeof(*word_tst));
    int i = 0;
    char *sgl[LMAX] = {NULL};
    tst_node *root = NULL, *res = NULL;
    int idx = 0, sidx = 0;
    FILE *fp = fopen(IN_FILE, "r");
    FILE *fp_out;
    double t1, t2;
    int bench_flag = (argc > 1 && !(strcmp(argv[1], "--bench")))?1:0;

    if (!fp) { /* prompt, open, validate file for reading */
        fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
        return 1;
    }

    t1 = tvgetf();
    while ((fgets(str_ar, WRDMAX * 3, fp)) != NULL) {
        str = str_ar;
        strcpy(word, strsep(&str, ",\n"));

        while (strcmp(word, "") != 0) {
            if(word[0] == ' ') {
                int j;
                for(j = 1; j < strlen(word); j++)
                    word_tst[i][j-1] = word[j];
                word_tst[i][j-1] = '\0';
            } else {
                int j;
                for(j = 0; j < strlen(word); j++)
                    word_tst[i][j] = word[j];
                word_tst[i][j] = '\0';
            }
            char *p = word_tst[i];
            /* FIXME: insert reference to each string */
            if (!tst_ins_del(&root, &p, INS, REF)) {
                fprintf(stderr, "error: memory exhausted, tst_insert.\n");
                fclose(fp);
                return 1;
            }
            idx++;
            i++;
            strcpy(word, strsep(&str, ",\n"));
        }
    }
    t2 = tvgetf();

    fclose(fp);
    printf("ternary_tree, loaded %d words in %.6f sec\n", idx, t2 - t1);
    if(bench_flag) {
        fp_out = fopen(OUT_FILE,"a");
        fprintf(fp_out, "tst_build %.6f ", t2 - t1);
    }

    for (;;) {
        char *p;
        if(bench_flag) {
            if(bench_flag > 1)
                strcpy(word, "q");
            else {
                strcpy(word, argv[2]);
                bench_flag++;
            }
        } else {
            printf(
                "\nCommands:\n"
                " a  add word to the tree\n"
                " f  find word in tree\n"
                " s  search words matching prefix\n"
                " d  delete word from the tree\n"
                " q  quit, freeing all data\n\n"
                "choice: ");
            fgets(word, sizeof word, stdin);
        }
        p = NULL;
        switch (*word) {
        case 'a':
            if(bench_flag)
                strcpy(word_tst[i], argv[3]);
            else {
                printf("enter word to add: ");
                if (!fgets(word_tst[i], sizeof word_tst[i], stdin)) {
                    fprintf(stderr, "error: insufficient input.\n");
                    break;
                }
            }
            rmcrlf(word_tst[i]);
            p = word_tst[i];
            t1 = tvgetf();
            /* FIXME: insert reference to each string */
            res = tst_ins_del(&root, &p, INS, REF);
            t2 = tvgetf();
            if (res) {
                i++;
                idx++;
                printf("  %s - inserted in %.6f sec. (%d words in tree)\n",
                       (char *) res, t2 - t1, idx);
            } else
                printf("  %s - already exists in list.\n", (char *) res);
            break;
        case 'f':
            if(bench_flag)
                strcpy(word, argv[3]);
            else {
                printf("find word in tree: ");
                if (!fgets(word, sizeof word, stdin)) {
                    fprintf(stderr, "error: insufficient input.\n");
                    break;
                }
            }
            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search(root, word);
            t2 = tvgetf();
            if (res)
                printf("  found %s in %.6f sec.\n", (char *) res, t2 - t1);
            else
                printf("  %s not found.\n", word);
            break;
        case 's':
            if(bench_flag)
                strcpy(word, argv[3]);
            else {
                printf("find words matching prefix (at least 1 char): ");
                if (!fgets(word, sizeof word, stdin)) {
                    fprintf(stderr, "error: insufficient input.\n");
                    break;
                }
            }
            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search_prefix(root, word, sgl, &sidx, LMAX);
            t2 = tvgetf();
            if (res) {
                printf("  %s - searched prefix in %.6f sec,%d suggests\n\n", word, t2 - t1, sidx);
                if(bench_flag == 0) {
                    for (int i = 0; i < sidx; i++)
                        printf("suggest[%d] : %s\n", i, sgl[i]);
                } else
                    fprintf(fp_out, "tst_search_prefix %.6f \n", t2 - t1);
            } else
                printf("  %s - not found\n", word);
            break;
        case 'd':
            if(bench_flag)
                printf("enter word to del: ");
            else {
                if (!fgets(word, sizeof word, stdin)) {
                    fprintf(stderr, "error: insufficient input.\n");
                    break;
                }
            }
            rmcrlf(word);
            p = word;
            printf("  deleting %s\n", word);
            t1 = tvgetf();
            /* FIXME: remove reference to each string */
            res = tst_ins_del(&root, &p, DEL, REF);
            t2 = tvgetf();
            if (res)
                printf("  delete failed.\n");
            else {
                printf("  deleted %s in %.6f sec\n", word, t2 - t1);
                idx--;
            }
            break;
        case 'q':
            tst_free(root);
            free(word_tst);
            if(bench_flag)
                fclose(fp_out);
            return 0;
            break;
        default:
            fprintf(stderr, "error: invalid selection.\n");
            break;
        }
    }

    return 0;
}
