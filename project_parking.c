/*
GROUP 9
PROJECT QUESTION 3 PARKING SYSTEM
BIC10204:ALGORITMA DAN PENGATURCARAAN
PREPARED FOR: Dr. Rozanawati Binti Darman
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CARS 100

typedef struct {
    char numberPlate[20];
    char timeIn[6];
    char timeOut[6];
    char dateIn[11];  
    char dateOut[11]; 
    double hoursParked;
    double charge;
    int requiresOfficePayment;
} Car;


void calculateParkingCharges(Car *car);
int validateTimeFormat(const char *timeStr);
double calculateHours(const char *dateIn, const char *timeIn, const char *dateOut, const char *timeOut, double *fineAmount);
int isDuplicateNumberPlate(Car cars[], int count, const char *numberPlate);
void saveReceiptsToFile(const Car cars[], int totalCars, double totalHours, double totalCharges);
void logCarEntry(const Car *car);
void displayWelcomeMessage();

int main() {
    Car cars[MAX_CARS];
    int n, i, totalCars = 0;
    double totalHours = 0, totalCharges = 0;
    double fineAmount = 0.0;

    displayWelcomeMessage();

    printf("Enter the number of cars (Max %d): ", MAX_CARS);
    scanf("%d", &n);
    if (n > MAX_CARS) {
        printf("Error: Maximum car limit exceeded.\n");
        return 1;
    }

    for (i = 0; i < n; i++) {
        printf("\nCar %d:\n", totalCars + 1);

        char numberPlate[20];
        do {
            printf("Enter number plate: ");
            scanf("%s", numberPlate);

            //validate num plate using function
            if (isDuplicateNumberPlate(cars, totalCars, numberPlate)) {
                printf("Error: This number plate is already registered. Please enter a unique number plate.\n");
            } else {
                strcpy(cars[totalCars].numberPlate, numberPlate);
                break;
            }
        } while (1);

        do {
            printf("Enter Date In (YYYY-MM-DD): ");
            scanf("%s", cars[totalCars].dateIn);
        } while (strlen(cars[totalCars].dateIn) != 10 || cars[totalCars].dateIn[4] != '-' || cars[totalCars].dateIn[7] != '-');

        do {
            printf("Enter Time In (HH:MM): ");
            scanf("%s", cars[totalCars].timeIn);

            //validate time format using function
        } while (!validateTimeFormat(cars[totalCars].timeIn));

        do {
            printf("Enter Date Out (YYYY-MM-DD): ");
            scanf("%s", cars[totalCars].dateOut);
        } while (strlen(cars[totalCars].dateOut) != 10 || cars[totalCars].dateOut[4] != '-' || cars[totalCars].dateOut[7] != '-');
    
        do {
            printf("Enter Time Out (HH:MM): ");
            scanf("%s", cars[totalCars].timeOut);
        } while (!validateTimeFormat(cars[totalCars].timeOut));

        cars[totalCars].hoursParked = calculateHours(cars[totalCars].dateIn, cars[totalCars].timeIn, cars[totalCars].dateOut, cars[totalCars].timeOut, &fineAmount);

        if (cars[totalCars].hoursParked > 24) {
            cars[totalCars].charge = fineAmount;
            cars[totalCars].requiresOfficePayment = 1;
            printf("Fine has been applied for your parking fee as it exceeds 24 hours. Please settle your fine at the office.\n");
        } else {
            cars[totalCars].requiresOfficePayment = 0;
            calculateParkingCharges(&cars[totalCars]);
        }

        totalHours += cars[totalCars].hoursParked;
        totalCharges += cars[totalCars].charge;
        totalCars++;

        logCarEntry(&cars[totalCars - 1]);
    }

    printf("\n-------------------------------------------------------------\n");
    printf("Car   Number Plate   Hours   Charge (RM)   Office Payment\n");
    printf("---------------------------------------------------------------\n");
    for (i = 0; i < totalCars; i++) {
        printf("%-5d %-15s %-7.2f RM%-8.2f %s\n", i + 1, cars[i].numberPlate, cars[i].hoursParked, 
               cars[i].charge, cars[i].requiresOfficePayment ? "Yes" : "No");
    }
    printf("---------------------------------------------------------------\n");
    printf("TOTAL Hours   : %.2f\n", totalHours);
    printf("TOTAL Charges : RM%.2f\n", totalCharges);
    printf("----------------------------------------------------------------\n");

    char choice;
    printf("Do you want to save the receipts to a file? (y/n): ");
    getchar(); 
    scanf("%c", &choice);

    if (choice == 'y' || choice == 'Y') {
        saveReceiptsToFile(cars, totalCars, totalHours, totalCharges);
    } else {
        printf("Receipts were not saved to a file.\n");
    }

    printf("\nThank you for using our car park system, safe ride!");

    return 0;
}


void displayWelcomeMessage() {
    printf("===============================================================\n");
    printf("            Welcome to UTHM Badminton Court\n");
    printf("                 Parking Center\n");
    printf("===============================================================\n");

    printf("\n");

    printf("INITIAL CHARGES\n");
    printf("===============================================================\n");
    printf("| %-30s | %-10s |\n", "Initial 3 hours", "RM 5.00");
    printf("| %-30s | %-10s |\n", "Extra charges per hour", "RM 0.50");
    printf("| %-30s | %-10s |\n", "Max charge for 24 hours", "RM 15.00");
    printf("---------------------------------------------------------------\n");

    printf("\n");

    printf("FINE CHARGES (Pay at office)\n");
    printf("===============================================================\n");
    printf("| %-30s | %-10s |\n", "1 to 7 days", "RM 50");
    printf("| %-30s | %-10s |\n", "8 to 30 days", "RM 150");
    printf("| %-30s | %-10s |\n", "31 to 365 days", "RM 500");
    printf("| %-30s | %-10s |\n", "More than 365 days", "RM 1000");
    printf("---------------------------------------------------------------\n");

    

    printf("\n");
    printf("INPUT FORMAT INSTRUCTIONS\n");
    printf("================================================================\n");
    printf("| %-30s | %-10s |\n", "Date format", "YYYY-MM-DD");
    printf("| %-30s | %-10s |\n", "Time format", "HH:MM (24-hour)");
    printf("---------------------------------------------------------------\n");

    printf("\n");
    printf(" Example: If the parking started on December 28, 2024, at 2:30 PM,\n");
    printf("          you should enter the following:\n");
    printf("          Date: 2024-12-28\n");
    printf("          Time: 14:30\n");

    printf("\n");
    printf("===============================================================\n");
    printf("             Please fill in the form below to proceed.\n");
    printf("===============================================================\n");
}



void calculateParkingCharges(Car *car) {
    double hours = car->hoursParked;
    
    //include 24 hours logic
    if (hours <= 3) {
        car->charge = 5.00;
    } else if (hours <= 24) {
        car->charge = 5.00 + (hours - 3) * 0.50;
        if (car->charge > 15.00) {
            car->charge = 15.00;
        }
    }
}

int validateTimeFormat(const char *timeStr) {
    
    int hours, minutes;
    
    //validate time format
    if (sscanf(timeStr, "%d:%d", &hours, &minutes) != 2) {
        printf("Invalid time format. Please use HH:MM (24-hour format).\n");
        return 0;
    }
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        printf("Invalid time. Hours (0-23) and minutes (0-59) required.\n");
        return 0;
    }
    return 1;
}

double calculateHours(const char *dateIn, const char *timeIn, const char *dateOut, const char *timeOut, double *fineAmount) {
    
    struct tm tmIn = {0};
    struct tm tmOut = {0};

    sscanf(dateIn, "%4d-%2d-%2d", &tmIn.tm_year, &tmIn.tm_mon, &tmIn.tm_mday);
    sscanf(timeIn, "%2d:%2d", &tmIn.tm_hour, &tmIn.tm_min);
    sscanf(dateOut, "%4d-%2d-%2d", &tmOut.tm_year, &tmOut.tm_mon, &tmOut.tm_mday);
    sscanf(timeOut, "%2d:%2d", &tmOut.tm_hour, &tmOut.tm_min);

    tmIn.tm_year -= 1900;
    tmIn.tm_mon -= 1;
    tmOut.tm_year -= 1900;
    tmOut.tm_mon -= 1;

    time_t timeInSec = mktime(&tmIn);
    time_t timeOutSec = mktime(&tmOut);

    if (timeOutSec < timeInSec) {
        timeOutSec += 24 * 60 * 60; 
    }

    double hours = difftime(timeOutSec, timeInSec) / 3600.0;
    double fine = 0.0;
    double daysParked = hours / 24.0;

    // Apply fine and use pointer to fineAmount
    if (daysParked > 1) {
        if (daysParked <= 7) {
            printf("Fine Applied: RM 50 (Parked for %.0f days)\n", daysParked);
            fine = 50.00;  
        } else if (daysParked <= 30) {
            printf("Fine Applied: RM 150 (Parked for %.0f days)\n", daysParked);
            fine = 150.00;
        } else if (daysParked <= 365) {
            printf("Fine Applied: RM 500 (Parked for %.0f days)\n", daysParked);
            fine = 500.00;
        } else {
            printf("Fine Applied: RM 1000 (Parked for %.0f days)\n", daysParked);
            fine = 1000.00;
        }
    }

    if (fineAmount) {
        *fineAmount = fine; 
    }

    return hours;
}


void saveReceiptsToFile(const Car cars[], int totalCars, double totalHours, double totalCharges) {
    char filename[50];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
   
    //new file for receipts with unique filename
    strftime(filename, sizeof(filename), "receipts_%Y%m%d_%H%M%S.txt", t);

    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Cannot open file.\n");
        return;
    }

    fprintf(file, "---------------------------------------------------------\n");
    fprintf(file, "Car   Number Plate   Hours   Charge (RM)   Office Payment\n");
    fprintf(file, "-----------------------------------------------------------\n");
    for (int i = 0; i < totalCars; i++) {
        fprintf(file, "%-5d %-15s %-7.2f RM%-8.2f %s\n",
                i + 1, cars[i].numberPlate, cars[i].hoursParked,
                cars[i].charge, cars[i].requiresOfficePayment ? "Yes" : "No");
    }
    fprintf(file, "TOTAL Hours   : %.2f\n", totalHours);
    fprintf(file, "TOTAL Charges : RM%.2f\n", totalCharges);
    fclose(file);

    printf("Receipts saved to '%s'.\n", filename);
}

void logCarEntry(const Car *car) {
    FILE *file = fopen("car_log.txt", "a");
    if (!file) {
        printf("Error: Cannot open log file.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
    
    //print and store in file
    fprintf(file, "Timestamp: %s\n", timestamp);
    fprintf(file, "Number Plate: %s\n", car->numberPlate);
    fprintf(file, "Date In: %s Time In: %s\n", car->dateIn, car->timeIn);
    fprintf(file, "Date Out: %s Time Out: %s\n", car->dateOut, car->timeOut);
    fprintf(file, "Hours Parked: %.2f\n", car->hoursParked);
    fprintf(file, "Charge: RM%.2f\n", car->charge);
    fprintf(file, "Office Payment: %s\n", car->requiresOfficePayment ? "Yes" : "No");
    fprintf(file, "------------------------------------------------------\n");

    fclose(file);
}

int isDuplicateNumberPlate(Car cars[], int count, const char *numberPlate) {
    for (int i = 0; i < count; i++) {
        
        //not case sensitive for number plate
        if (strcasecmp(cars[i].numberPlate, numberPlate) == 0) {
            return 1;
        }
    }
    return 0;
}
