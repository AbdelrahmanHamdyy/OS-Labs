#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

struct student {
    int midtermGrade;
    int finalGrade;
};

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Not enough arguments! Exiting..\n");
        return 1;
    }
    int N = atoi(argv[2]); // Number of TAs
    int P = atoi(argv[3]); // Passing Grade

    FILE *filePtr = fopen(argv[1], "r"); // Opening the File
    if (filePtr == NULL) {
        printf("Error! File not opened.\n");
        return 2;
    }
    int S = 0;
    fscanf(filePtr, "%4d", &S);// Number of Students
    struct student *students = malloc(sizeof(struct student) * S); // Total Students array
    
    int num = 0; 
    int i = 0; // Index
    bool even = 0; // Mid or Final 
    while (i != S) {
        fscanf(filePtr, "%4d", &num); // Read 
        if (!even) 
            students[i].midtermGrade = num;
        else
        {
            students[i].finalGrade = num;
            i++; // Next Student
        }
        even ^= 1; // Toggle boolean
    }
    fclose(filePtr); // CLose File

    int pid, last, stat_loc;
    int* sorted = malloc(sizeof(int) * N); // Maintain order of TAs

    for (int i = 0; i < N; i++) {
        pid = fork();
        last = i; // TA
        if (pid == 0)
            break;
        sorted[i] = pid; // Insert into sorted pids
    }

    int n = S / N; // Quotient
    int m = n;
    int pass = 0; // Counter
    if (pid > 0) {
        for (int i = 0; i < N; i++)
        {
            waitpid(sorted[i], &stat_loc, 0); // wait for all TAs to finish grading
            if(WIFEXITED(stat_loc))
  	            printf("%d ", WEXITSTATUS(stat_loc)); // print passing students
        }
    }
    else if (pid == 0) {
        if (last == N - 1) { // Last TA will take the rest
            n = S - (n * (last));
        }
        int index = last * m; // Starting Index
        for (int i = index; i < index + n; i++) {
            int grade = students[i].midtermGrade + students[i].finalGrade;
            if (grade >= P)
                pass++;
        }
        return pass; // exit
    }
    else 
        printf("Forking Error.\n");
        
    printf("\n");
    return 0;
}