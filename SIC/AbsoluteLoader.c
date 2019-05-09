#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void absLoader()
{
    FILE *objPr = fopen("ObjProg.txt", "r"),
         *absL = fopen("AbsoluteLoader.txt", "w");

    char buffer[100];

    int start, add, length, prev, startFlag = 0, i, macCode;

    fscanf(objPr, "%*9s%X%*s ", &start);
    prev = start - 0x3;
    while (fgets(buffer, 100, objPr) != NULL)
    {
        if (strncmp(buffer, "E", 1) == 0)
            break;

        sscanf(buffer, "%*2s%X.%X", &add, &length);

        if (start > 0x0 && startFlag != 1)
        {
            fprintf(absL, "0000\t|\t.\t.\t.\n");
            fprintf(absL, ".\t\t|\t.\t.\t.\n");
            fprintf(absL, ".\t\t|\t.\t.\t.\n");
        }
        printf("%X %X %d\n", prev + 3, add, add - (prev + 3));
        if (prev + 3 < add)
        {
            if (add - (prev+3) > 0x9)
            {
                fprintf(absL, "%.4X\t|\t.\t.\t.\n", prev + 3);
                fprintf(absL, ".\t\t|\t.\t.\t.\n");
                fprintf(absL, ".\t\t|\t.\t.\t.\n");
                fprintf(absL, "%.4X\t|\t.\t.\t.\n", add - 3);
            }
            else
            {
                for (i = 1; i <= add - (prev+3) ; i += 3)
                {
                    fprintf(absL, "%.4X\t|\t.\t.\t.\n", prev + 3 * i);
                }
            }
        }
        if (start < add && startFlag != 1)
        {
            fprintf(absL, "%.4X\t|\t.\t.\t.\n", start);
        }
        startFlag = 1;

        for (i = 0; i < length / 3; i++)
        {
            sscanf(buffer + 12 + 7 * i, "%X", &macCode);
            fprintf(absL, "%.4X\t|\t%.2X\t%.2X\t%.2X\n", add + i * 3, macCode/0x10000, (macCode%0x10000)/0x100, macCode%0x100);
        }
        prev = add + i * 3 - 3;
    }
}

int main()
{
    absLoader();
}