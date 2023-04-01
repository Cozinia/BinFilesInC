#include <stdio.h>
#include <string.h>
#include <stdlib.h>//for exit function
#include <ctype.h>
#include <stdarg.h>
#pragma warning(disable:4996) //pentru a dezactiva _CRT_SECURE_NO_WARNINGS.

void startMenu();

struct Angajat {
	int ID;
	int esteSetat;
	char Nume[100];
	int Varsta;
	double Salariu;
	char Telefon[30];
};

void LOG(const char* fmt, ...)
{
	FILE* file;
	file = fopen("employees.log", "a"); // open file in append mode
	char buffer[4096];
	va_list args;
	va_start(args, fmt);
	int rc = vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	printf("%s", buffer);
	fprintf(file, "%s", buffer); // write the formatted buffer to file
	
	return;	
}

int NofScannedArguments = 0;//check for errors in scanf

void checkScanfErrors(int NofScannedArguments) {
	if (NofScannedArguments != 1) /* should be one number */
	{
		exit(EXIT_FAILURE); /* failure, assumptions of program are not met */
	}
	return;
}

void printOptions_StartMenu() {
	printf("\n1.Add employee \t\t2.View employees\n");
	printf("3.Modify emplyees \t4.Delete emplyee\n");
	printf("5.Close program\n");
	return;
}

void printOptions_ModifyEmployee() {
	printf("\n1.First and last name \t2.Age\n");
	printf("3.Salary \t\t4.Phone number\n");
	printf("5.Save and close menu\t6.esteSetat\n");
	return;
}

void saveEmployee(Angajat employee) {
	FILE* writeEmployee = fopen("employees.bin", "ab");
	//write in file an entire struct 
	fwrite(&employee, sizeof(struct Angajat), 1, writeEmployee);
	fclose(writeEmployee);
	return;
}

Angajat AddEmplyee(Angajat employee) {
	int NofScannedArguments = 0;
	employee.ID = employee.ID + 1;
	printf("Add emplyee:\n");
	printf("First and last name: ");
	NofScannedArguments = scanf("%s", employee.Nume);
	printf("Age: ");
	NofScannedArguments = scanf("%d", &employee.Varsta);
	printf("Salary: ");
	NofScannedArguments = scanf("%lf", &employee.Salariu);
	printf("Phone number: ");
	NofScannedArguments = scanf("%s", employee.Telefon);
	employee.esteSetat = 1;
	saveEmployee(employee);
	checkScanfErrors(NofScannedArguments);
	return employee;
}

void ViewEmployees(Angajat employee) {
	FILE* readEmployees = fopen("employees.bin", "rb");
	if (readEmployees != NULL)
	{
		while (fread(&employee, sizeof(struct Angajat), 1, readEmployees))
		{
			
			if (employee.esteSetat != 0)
			{
				printf("ID: %d\n", employee.ID);
				printf("esteSetat: %d\n", employee.esteSetat);
				printf("First and last name: %s\n", employee.Nume);
				printf("Age: %d\n", employee.Varsta);
				printf("Salary: %lf\n", employee.Salariu);
				printf("Phone number: %s\n\n", employee.Telefon);
			}
			
		}
		fclose(readEmployees);
	}
	else {
		printf("Cannot open employee.bin\n");
	}
	return;
}

Angajat modifyMenu_ModifyEmployee(Angajat employee) {
	int userChoice;
	printf("Please choose an option:\n");
	printOptions_ModifyEmployee();
	scanf_s("%d", &userChoice);
	while (userChoice != 5)
	{
		switch (userChoice) {
		case 1:
			printf("First and last name: ");
			fgets(employee.Nume, sizeof(employee.Nume), stdin);
			break;
		case 2:
			printf("Age: ");
			NofScannedArguments = scanf("%d", &employee.Varsta);
			break;
		case 3:
			printf("Salary: ");
			NofScannedArguments = scanf("%lf", &employee.Salariu);
			break;
		case 4:
			printf("Phone number: ");
			NofScannedArguments = scanf("%s", employee.Telefon);
			break;
		case 6:
			printf("esteSetat: ");
			NofScannedArguments = scanf("%d", &employee.esteSetat);
			break;
		default:
			printf("Invalid option, please enter another option.\n");
			break;
		}
		printf("Please choose an option:\n");
		printOptions_ModifyEmployee();
		scanf_s("%d", &userChoice);
	}
	return employee;
}

void ModifyEmployee(Angajat employee) {
	FILE* readEmployees = fopen("employees.bin", "rb+");
	long int offset = -1;
	int userInputID, wasInFunction = 0;
	printf("Employee's ID:\n");
	scanf_s("%d", &userInputID);
	Angajat modifEmpl;
	while (fread(&employee, sizeof(struct Angajat), 1, readEmployees))
	{
		if ((employee.ID == userInputID) && (wasInFunction == 0))
		{
			offset = ftell(readEmployees) - sizeof(struct Angajat); // ma pozitionez la inceputul structurii pentru a o putea modifica 
			employee = modifyMenu_ModifyEmployee(employee);
			fseek(readEmployees, offset, SEEK_SET);
			fwrite(&employee, sizeof(struct Angajat), 1, readEmployees);
			printf("Employee's info saved!\n");
			break;
		}
	}
	fclose(readEmployees);

	return;
}

void DeleteEmployee(Angajat employee) {

	FILE* file = fopen("employees.bin", "rb+");
	int userInputID, recordFound = 0;
	printf("Employee's ID:\n");
	scanf_s("%d", &userInputID);

	while (fread(&employee, sizeof(struct Angajat), 1, file)) {
		if (employee.ID == userInputID) {
			recordFound = 1;
			break;
		}
	}

	if (recordFound) {
		// We found the record, so let's delete it by moving the rest of the
		// file over the deleted record
		while (fread(&employee, sizeof(struct Angajat), 1, file)) {
			fseek(file, -2 * sizeof(struct Angajat), SEEK_CUR);
			fwrite(&employee, sizeof(struct Angajat), 1, file);
			fseek(file, sizeof(struct Angajat), SEEK_CUR);
		}
		employee.esteSetat = 0;
		fseek(file, -1*sizeof(struct Angajat), SEEK_CUR);
		fwrite(&employee, sizeof(struct Angajat), 1, file);
	}
		fclose(file);
	return;
}

void startMenu() {
	int userInput;
	Angajat employee;
	employee.ID = 0;
	employee.esteSetat = 0;
	printf("Please choose an option:\n");
	printOptions_StartMenu();
	scanf_s("%d", &userInput);
	while (userInput != 5)
	{
		switch (userInput) {
			//Add emplyee
		case 1:
			employee = AddEmplyee(employee);
			//saveEmployee(employee);
			break;
			//View emplyees
		case 2:
			ViewEmployees(employee);
			//printf("Not implemented yet.\n");
			break;
			//Modify employee
		case 3:
			ModifyEmployee(employee);
			//printf("Not implemented yet.\n");
			break;
			//Delete employee
		case 4:
			DeleteEmployee(employee);
			//printf("Not implemented yet.\n");
			break;
		case 5:
			exit(1);
			break;
		default:
			printf("Invalid option, please enter another option.\n");
			break;
		}
		printf("Please choose an option:\n");
		printOptions_StartMenu();
		scanf_s("%d", &userInput);
	}

	return;
}

int main() {
	LOG("test 5");
	startMenu();

	return 0;
}


