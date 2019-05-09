#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
char a[100][100];
int opcodes[100];
int tableSize;
int base;
void makeMTable()
{
    FILE *fp;
    fp = fopen("mnemonics.txt", "r");

    int i = 0;
    //FOR OPcode

    while (fscanf(fp, "%s %x", &a[i], &opcodes[i]) != EOF)
    {
        i++;
    }
    fclose(fp);
    tableSize = i;
}

int inTable(char str[])
{
    int i;
    for (i = 0; i < tableSize; i++)
    {
        if (strcmp(a[i], str) == 0)
        {
            return i;
        }
    }
    return -1;
}

void pass2()
{
    makeMTable();
    FILE *intT = fopen("intTable.txt", "r"),
         *symT = fopen("symTable.txt", "r"),
         *objCode = fopen("objCode.txt", "w");
    char str[10];
    int insLoc;
    while (fscanf(intT, "%x", &insLoc) != EOF)
    {

        // for BASE directive
        if (insLoc == 0xba)
        {
            fscanf(intT, "%*s %s", &str);
            fprintf(objCode, "\t\tBASE\t%s\n", str);
            char s[10];
            while (fscanf(symT, "%s %x ", &s, &base) != EOF)
            {
                if (strcmp(s, str) == 0)
                    break;
            }
            rewind(symT);
            continue;
        }
        // writing Location
        fprintf(objCode, "%.4x\t", insLoc);

        // reading Label or Mnemonic
        fscanf(intT, "%s", &str);
        if (strcmp(str, "RSUB") == 0)
        {
            fprintf(objCode, "\tRSUB\t\t4f0000\n");
            continue;
        }

        if (str[0] != '+')
        {
            if (inTable(str) == -1)
            {
                // writing Label
                fprintf(objCode, "%s\t", str);

                // reading mnemonic
                fscanf(intT, "%s", &str);
            }
            else
                fprintf(objCode, "\t");
        }
        else
            fprintf(objCode, "\t");

        //str holds Mnemonic
        // writing mnemonic
        fprintf(objCode, "%s\t", str);

        if (strcmp(str, "RESB") == 0 || strcmp(str, "RESW") == 0 || strcmp(str, "END") == 0)
        {
            // reading operand
            fscanf(intT, "%s", &str);
            // writing operand
            fprintf(objCode, "%s\t\n", str);
            // no machine code
            continue;
        }

        else if (strcmp(str, "WORD") == 0)
        {
            // reading operand
            int x;
            fscanf(intT, "%d", &x);

            fprintf(objCode, "%d\t%.6x\n", x, x);
            continue;
        }

        else if (strcmp(str, "BYTE") == 0)
        {
            // reading operand
            fscanf(intT, "%s", &str);
            fprintf(objCode, "%s\t", str);
            if (str[0] == 'x' || str[0] == 'X')
            {
                str[strlen(str) - 1] = '\0';
                strcpy(str, &str[2]);
                if (strlen(str) % 2 == 0)
                    fprintf(objCode, "%s\t\n", str);
                else
                    fprintf(objCode, "0%s\t\n", str);
            }
            else
            {
                str[strlen(str) - 1] = '\0';
                strcpy(str, &str[2]);
                int i = 0;
                while (i < strlen(str))
                {
                    fprintf(objCode, "%x", str[i++]);
                }
                fprintf(objCode, "\n");
            }
            continue;
        }
        int isExtended = 0;
        if (str[0] == '+')
        {
            strcpy(str, &str[1]);
            isExtended = 1;
        }

        int opcodeIndex = inTable(str);

        int loc = 0x0;

        //reading operand
        fscanf(intT, "%s", &str);

        //writing operand
        fprintf(objCode, "%s\t", str);

        // getting NI
        int NI = 0x3; // by default for N=1;I=1
        if (str[0] == '#')
        {
            NI = 0x1; // for N=0, I=1;
            strcpy(str, &str[1]);
        }
        else if (str[0] == '@')
        {
            NI = 0x2; // for N=1, I=0;
            strcpy(str, &str[1]);
        }

        // printing first two hexBytes
        fprintf(objCode, "%.2x", opcodes[opcodeIndex] + NI - (opcodes[opcodeIndex] % 16) % 4);

        int X = 0, B = 0, P = 0, E = 0; //flagBits
        // for X
        if (str[strlen(str) - 2] == ',')
        {
            X = 1;
            // removing ',x' from str to check operand
            str[strlen(str) - 2] = '\0';
        }

        // disp or address to be written in the end
        char s[10];
        int Immediate = 1;
        int add;
        while (fscanf(symT, "%s %x ", &s, &add) != EOF)
        {
            if (strcmp(s, str) == 0)
            {
                Immediate = 0;
                break;
            }
        }
        rewind(symT);

        //for extended 4byte instruction
        if (isExtended == 1)
        {
            E = 1;
            B = 0;
            P = 0;
        }

        else // non extended 3byte
        {
            E = 0;
            if (Immediate == 1)
            {
                B = 0;
                P = 0;
            }
            else
            {
                int disp = add - (insLoc + 0x3);
                // checking for PC relative
                if (disp >= -2048 && disp <= 2047)
                {
                    P = 1;
                    B = 0;
                }
                else
                {
                    disp = add - base;
                    P = 0;
                    B = 1;
                }
            }
        }

        fprintf(objCode, "%x", E * 1 + P * 2 + B * 4 + X * 8);

        // writing the disp or address field
        if (isExtended == 1 && Immediate == 0)
        {
            fprintf(objCode, "%.5x\n", add);
        }
        else if (isExtended == 1 && Immediate != 0)
        {
            int q = 5 - strlen(str);
            while (q > 0)
            {
                fprintf(objCode, "%x", 0);
                q--;
            }
            fprintf(objCode, "%s\n", str);
        }
        else if (isExtended == 0 && Immediate == 0)
        {
            // printing only the last 3 digits of hex address
            fprintf(objCode, "%.3x\n", add % 0x1000);
        }
        else if (isExtended == 0 && Immediate != 0)
        {
            int q = 3 - strlen(str);
            while (q > 0)
            {
                fprintf(objCode, "%x", 0);
                q--;
            }
            fprintf(objCode, "%s\n", str);
        }
    }
    fclose(intT);
    fclose(symT);
    fclose(objCode);
}

// Main Function
int main()
{
    pass2();
    return 0;
}