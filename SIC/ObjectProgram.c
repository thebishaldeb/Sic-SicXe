#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

void headerRecord()
{
    FILE *objcode = fopen("assembly.txt", "r"),
         *objProg = fopen("ObjProg.txt", "w"),
         *assem = fopen("addMac.txt", "r");

    char buffer[50], name[10];

    int start, add;

    fscanf(objcode, "%s %*s %X", name, &start);
    while (fgets(buffer, 50, assem) != NULL)
    {
        sscanf(buffer, "%X", &add);
    }

    fprintf(objProg, "H.%-.6s.%.6X.%.6X\n", name, start, add + 3 - start);

    fclose(objcode);
    fclose(objProg);
    fclose(assem);
}

void textRecord()
{
    FILE *objProg = fopen("ObjProg.txt", "a"),
         *assem = fopen("addMac.txt", "r");

    char buffer[25], addressRecord[61], macCode[7];

    int add, byte, count;

    while (fgets(buffer, 25, assem) != NULL)
    {
        count = 0;
        byte = 0;

        if (strncmp(buffer, ".", 1) == 0)
        {
            continue;
        }

        strcpy(addressRecord, "");

        sscanf(buffer, "%X %s ", &add, macCode);
        strcat(addressRecord, macCode);

        fprintf(objProg, "T.%.6X", add);

        byte += 3;
        count += 1;

        while (fgets(buffer, 25, assem) != NULL)
        {
            if (strncmp(buffer, ".", 1) == 0 || count >= 10)
            {
                break;
            }

            sscanf(buffer, "%*s %s ", macCode);
            strcat(addressRecord, ".");
            strcat(addressRecord, macCode);

            byte += 3;
            count += 1;
        }

        fprintf(objProg, ".%.2X.%s\n", byte, addressRecord);
    }

    fclose(objProg);
    fclose(assem);
}

void endRecord()
{
    FILE *objProg = fopen("ObjProg.txt", "a"),
         *assem = fopen("addMac.txt", "r");

    char buffer[50];

    int start;

    fscanf(assem, "%X", &start);

    fprintf(objProg, "E.%.6X\n", start);

    fclose(objProg);
    fclose(assem);
}
int main()
{
    headerRecord();
    textRecord();
    endRecord();
}