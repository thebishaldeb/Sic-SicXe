#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// Pass 1
void pass1()
{
	FILE *assm = fopen("sicxe.txt", "r");
	FILE *intT = fopen("intTable.txt", "w");
	FILE *symT = fopen("symTable.txt", "w");

	char buffer[100], label[10], mnemonic[10], operand[10];
	int addr, nullOperand;

	fscanf(assm, "%*s %*s %X ", &addr);

	while (fgets(buffer, 1000, assm) != NULL)
	{
		if (buffer[0] != '\t')
		{
			nullOperand = sscanf(buffer, "%s %s %s ", label, mnemonic, operand);
			operand[0] = nullOperand == 2 ? '\0' : operand[0];
			fprintf(symT, "%s\t%X\n", label, addr);
		}
		else
		{
			label[0] = '\0';
			nullOperand = sscanf(buffer, " %s %s ", mnemonic, operand);
			operand[0] = nullOperand == 1 ? '\0' : operand[0];
		}

		if (strcmp(mnemonic, "BASE") == 0)
			fprintf(intT, "\t%s\t%s\t%s\n", label, mnemonic, operand);
		else
			fprintf(intT, "%.4X\t%s\t%s\t%s\n", addr, label, mnemonic, operand);

		addr += 3;
		addr += strcmp(mnemonic, "RESW") == 0 ? (atoi(operand) * 3) - 3 : 0;
		addr += strcmp(mnemonic, "RESB") == 0 ? atoi(operand) - 3 : 0;
		addr += strcmp(mnemonic, "WORD") == 0 ? 3 - 3 : 0;
		addr -= strcmp(mnemonic, "BASE") == 0 ? 3 : 0;
		if (strncmp(mnemonic, "+", 1) == 0)
			addr += 4 - 3;
		if (strcmp(mnemonic, "BYTE") == 0)
		{
			addr += operand[0] == 'X' ? ceil((strlen(operand) - 3) / 2.0) - 3 : strlen(operand) - 3 - 3;
		}
	}
	fclose(intT);
	fclose(symT);
	fclose(assm);
}

// Main Function
int main()
{
	pass1();
	return 0;
}