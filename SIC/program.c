#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
int mnemonicSearch(char req_mnemo[100])
{
	FILE *mnemonic = fopen("mnemonic_asm.txt", "r");
	char mnemo[100];
	int opcode;
	while (fscanf(mnemonic, "%s %X ", mnemo, &opcode) == 2)
	{
		if (strcmp(req_mnemo, mnemo) == 0)
			return opcode;
	}
}
int labelSearch(char req_label[100])
{
	FILE *sym_table = fopen("Symbol_Table.txt", "r");
	char label[100];
	int address;
	while (fscanf(sym_table, "%X %s %*s ", &address, label) == 2)
	{
		if (strcmp(req_label, label) == 0)
		{
			fclose(sym_table);
			return address;
		}
	}
}
void pass2()
{
	int X = 0;
	FILE *intermediate = fopen("Intermediate.txt", "r"),
		 *objCode = fopen("Object_Code.txt", "w"),
		 *addMc = fopen("addMac.txt", "w");

	char buffer[1000], label[100], mnemonic[100], operand[100];
	int add, flag = 0;
	fscanf(intermediate, "%X", &add);
	while (fgets(buffer, 1000, intermediate) != NULL)
	{
		buffer[strlen(buffer) - 1] = '\0';
		fprintf(objCode, "%X", add);
		fprintf(objCode, "%s\t", buffer);
		int set_operand_null;
		if (buffer[1] != '\t')
		{
			set_operand_null = sscanf(buffer, "%s %s %s ", label, mnemonic, operand);
			operand[0] = set_operand_null == 2 ? '\0' : operand[0];
		}
		else
		{
			label[0] = '\0';
			set_operand_null = sscanf(buffer, " %s %s ", mnemonic, operand);
			operand[0] = set_operand_null == 1 ? '\0' : operand[0];
		}
		if (strcmp(mnemonic, "LDX") == 0)
		{
			FILE *sym_tab = fopen("Symbol_Table.txt", "r");
			char operand_name[100], operand_value[100];
			while (fscanf(sym_tab, "%*s %s %s ", operand_name, operand_value) == 2)
			{
				if (strcmp(operand_name, operand) == 0)
				{
					X = atoi(operand_value);
					break;
				}
			}
			fclose(sym_tab);
		}
		if (strcmp(mnemonic, "RESW") == 0 || strcmp(mnemonic, "RESB") == 0)
		{
			fscanf(intermediate, "%X", &add);
			fputc('\n', objCode);
			if (flag == 0)
				fprintf(addMc, ".\n");
			flag = 1;
			continue;
		}
		else if (strcmp(mnemonic, "WORD") == 0)
		{
			fprintf(objCode, "%.6X\n", atoi(operand));
			fprintf(addMc, "%X\t%.6X\n",add, atoi(operand));
				flag = 0;
		}
		else if (strcmp(mnemonic, "BYTE") == 0)
		{
			if (operand[0] == 'C' || operand[0] == 'c')
			{
				int i = 2;
				fprintf(addMc, "%.4X\t", add);
				flag = 0;
				while (i < strlen(operand) - 1)
				{
					fprintf(objCode, "%X", (int)operand[i]);
					fprintf(addMc, "%X", (int)operand[i]);
					i++;
				}
				fputc('\n', objCode);
				fputc('\n', addMc);
			}
			if (operand[0] == 'X' || operand[0] == 'x')
			{
				char print_val[100];
				sscanf(operand, "%*[^\']%*c%[^\']", print_val);
				if (strlen(print_val) % 2)
				{
					fprintf(objCode, "0%s\n", print_val);
					fprintf(addMc, "%.4X\t0%s\n", add, print_val);
					flag = 0;
				}
				else
				{
					fprintf(objCode, "%s\n", print_val);
					fprintf(addMc, "%.4X\t0%s\n", add, print_val);
					flag = 0;
				}
			}
		}
		else
		{
			int opcode = mnemonicSearch(mnemonic);
			;
			int address;
			if ((operand[strlen(operand) - 2] == ',') && (operand[strlen(operand) - 1] == 'X' || operand[strlen(operand) - 1] == 'x'))
			{
				operand[strlen(operand) - 2] = '\0';
				address = labelSearch(operand);
				address -= X;
				address += 0x8000;
			}
			else
			{
				address = labelSearch(operand);
			}
			fprintf(objCode, "%.2X%.4X\n", opcode, address);
			fprintf(addMc, "%.4X\t%.2X%.4X\n", add, opcode, address);
			flag = 0;
		}
		fscanf(intermediate, "%X", &add);
	}
	printf("%X", X);
	fclose(intermediate);
	fclose(objCode);
	fclose(addMc);
}
void pass1()
{
	FILE *fp = fopen("assembly.txt", "r");
	int start_loc, i = 0;
	char buffer[1000], label[100], mnemonic[100], operand[100];
	FILE *intermediate = fopen("Intermediate.txt", "w");
	FILE *sym_table = fopen("Symbol_Table.txt", "w");
	fscanf(fp, "%*s %*s %X ", &start_loc);
	while (fgets(buffer, 1000, fp) != NULL)
	{
		int set_operand_null;
		if (buffer[0] != '\t')
		{
			set_operand_null = sscanf(buffer, "%s %s %s ", label, mnemonic, operand);
			operand[0] = set_operand_null == 2 ? '\0' : operand[0];
			fprintf(sym_table, "%X\t%s\t%s\n", start_loc, label, operand);
		}
		else
		{
			label[0] = '\0';
			set_operand_null = sscanf(buffer, " %s %s ", mnemonic, operand);
			operand[0] = set_operand_null == 1 ? '\0' : operand[0];
		}
		fprintf(intermediate, "%X\t%s\t%s\t%s\n", start_loc, label, mnemonic, operand);
		start_loc += 3;
		start_loc += strcmp(mnemonic, "RESW") == 0 ? (atoi(operand) * 3) - 3 : 0;
		start_loc += strcmp(mnemonic, "RESB") == 0 ? atoi(operand) - 3 : 0;
		start_loc += strcmp(mnemonic, "WORD") == 0 ? 3 - 3 : 0;
		if (strcmp(mnemonic, "BYTE") == 0)
		{
			if (operand[0] == 'X' || operand[0] == 'x')
				start_loc += ceil((strlen(operand) - 3) / 2.0) - 3;
			else
				start_loc += (strlen(operand) - 3 - 3);
		}
	}
	fclose(intermediate);
	fclose(fp);
	fclose(sym_table);
}
int main()
{
	pass1();
	pass2();
}