#include <stdio.h>
#include <stdlib.h>

#define WRDMAX 256

int main(void)
{
    FILE *fp = fopen("out_cpy.txt", "r");
    FILE *output = fopen("output.txt", "w");
    if (!fp) {
        printf("ERROR opening input file out_cpy.txt\n");
        exit(0);
    }
    int i = 0;
    char build[50], search[50];
    double cpy_sum_b = 0.0, cpy_sum_s = 0.0, cpy_b, cpy_s;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %lf %s %lf\n", build, &cpy_b, search, &cpy_s);
        cpy_sum_b += cpy_b;
        cpy_sum_s += cpy_s;
    }
    fclose(fp);

    fp = fopen("out_ref.txt", "r");
    if (!fp) {
        fp = fopen("out_cpy.txt", "r");
        if (!fp) {
            printf("ERROR opening input file out_ref.txt\n");
            exit(0);
        }
    }
    double ref_sum_b = 0.0, ref_sum_s = 0.0, ref_b, ref_s;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %lf %s %lf\n", build, &ref_b, search, &ref_s);
        ref_sum_b += ref_b;
        ref_sum_s += ref_s;
    }

    fprintf(output, "tst-build %lf %lf\n",cpy_sum_b / 100.0, ref_sum_b / 100.0);
    fprintf(output, "tst-prefix-search() %lf %lf", cpy_sum_s / 100.0, ref_sum_s / 100.0);
    fclose(output);
    fclose(fp);
    return 0;
}
