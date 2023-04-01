#include <stdio.h>
#include <string.h>
#include <stdlib.h>//for exit function
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
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

char* TellTime() {
	time_t rawtime;
	struct tm* timeinfo;
	static char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "Date: %d.%m.%Y Time: %H:%M:%S: ", timeinfo);//format the date and time as a string
	return buffer;
}


void LOG(const char* fmt, ...)
{
	FILE* file;
	file = fopen("log.txt", "a+");
	if (file == NULL)
	{
		printf("cannot open log.txt");
	}
	else {
		char buffer[4096];
		va_list args;
		va_start(args, fmt);
		int rc = vsnprintf(buffer, sizeof(buffer), fmt, args);
		fprintf(file, TellTime());
		fprintf(file, buffer);
		va_end(args);
		fclose(file);
	}
	
	return;
}

int NotScannedArguments = 0;//check for errors in scanf

void checkScanfErrors(int NotScannedArguments) {
	if (NotScannedArguments != 1) 
	{
		exit(EXIT_FAILURE); /* failure, assumptions of program are not met */
	}
	return;
}

void printOptions_StartMenu() {
	printf("\n1.Add employee \t\t2.View employees\n");
	printf("3.Modify employees \t4.Delete employee\n");
	printf("5.Close program\n");
	return;
}

void printOptions_ModifyEmployee() {
	printf("\n1.First and last name \t2.Age\n");
	printf("3.Salary \t\t4.Phone number\n");
	printf("5.Save and close menu\n");
	return;
}

void saveEmployee(Angajat employee) {
	FILE* writeEmployee = fopen("employees.bin", "ab");
	fwrite(&employee, sizeof(struct Angajat), 1, writeEmployee);//write in file an entire struct 
	fclose(writeEmployee);
	return;
}

Angajat AddEmployee(Angajat employee) {
	employee.ID = employee.ID + 1;
	while (getchar() != '\n');//clears the buffer
	printf("Add employee:\n");
	printf("First and last name: ");
	fgets(employee.Nume, sizeof(employee.Nume), stdin);
	employee.Nume[strcspn(employee.Nume, "\n")] = '\0';//delete the \n from the end of the string
	printf("Age: ");
	NotScannedArguments = scanf("%d", &employee.Varsta);
	printf("Salary: ");
	NotScannedArguments = scanf("%lf", &employee.Salariu);
	printf("Phone number: ");
	NotScannedArguments = scanf("%s", employee.Telefon);
	employee.esteSetat = 1;
	saveEmployee(employee);
	checkScanfErrors(NotScannedArguments);
	
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
	char modification[32];
	printf("Please choose an option:\n");
	printOptions_ModifyEmployee();
	scanf_s("%d", &userChoice);
	while (userChoice != 5)
	{
		switch (userChoice) {
		case 1:
			printf("First and last name: ");
			fgets(employee.Nume, sizeof(employee.Nume), stdin);
			sprintf(modification, "New name: %s\n", employee.Nume);
			break;
		case 2:
			printf("Age: ");
			NotScannedArguments = scanf("%d", &employee.Varsta);
			sprintf(modification, "New age: %d\n", employee.Varsta);
			break;
		case 3:
			printf("Salary: ");
			NotScannedArguments = scanf("%lf", &employee.Salariu);
			sprintf(modification, "New salary: %lf\n", employee.Salariu);
			break;
		case 4:
			printf("Phone number: ");
			NotScannedArguments = scanf("%s", employee.Telefon);
			sprintf(modification, "New phone number: %s\n", employee.Telefon);
			break;
		default:
			printf("Invalid option, please enter another option.\n");
			break;
		}
		LOG(modification);
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

Angajat DeleteEmployee(Angajat employee) {

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
		while (fread(&employee, sizeof(struct Angajat), 1, file)) {
			fseek(file, -2 * sizeof(struct Angajat), SEEK_CUR);
			fwrite(&employee, sizeof(struct Angajat), 1, file);
			fseek(file, sizeof(struct Angajat), SEEK_CUR);
		}
		employee.esteSetat = 0;
		fseek(file, - 1* sizeof(struct Angajat), SEEK_CUR);
		fwrite(&employee, sizeof(struct Angajat), 1, file);
		
	}
		fclose(file);
		
	return employee;
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
			//Add employee
		case 1:
			employee = AddEmployee(employee);
			LOG("User chose option 1-add employee with name: %s\n", employee.Nume);
			break;
			//View employees
		case 2:
			ViewEmployees(employee);
			LOG("User chose option 2-view employees\n");
			break;
			//Modify employee
		case 3:
			LOG("User chose option 3-modify employee\t");
			ModifyEmployee(employee);
			
			break;
			//Delete employee
		case 4:
			employee = DeleteEmployee(employee);
			LOG("User chose option 4-delete employee with name: %s\n", employee.Nume);
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

int main() 
{
	startMenu();

	return 0;
}


