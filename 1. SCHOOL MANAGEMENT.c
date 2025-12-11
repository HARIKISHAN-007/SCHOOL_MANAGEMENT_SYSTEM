#include <stdio.h>
#include <string.h>

#define MAX 50

char getGrade(float p) {
    if(p>=90) return 'O';
    else if(p>=80) return 'A';
    else if(p>=70) return 'B';
    else if(p>=60) return 'C';
    else if(p>=50) return 'D';
    else return 'F';
}

const char* getClass(float p) {
    if(p>=75) return "Distinction";
    else if(p>=60) return "First Class";
    else if(p>=50) return "Second Class";
    else if(p>=40) return "Pass";
    else return "Fail";
}

const char* getStatus(int marks[], int M, float p) {
    for(int i=0;i<M;i++) if(marks[i]<35) return "Fail";
    return (p<40)?"Fail":"Pass";
}

int main() {
    int N,M;
    printf("Enter number of students and subjects: ");
    scanf("%d %d",&N,&M);

    int roll[MAX], marks[MAX][MAX], total[MAX];
    float percent[MAX];
    char name[MAX][50], grade[MAX], status[MAX][10], classification[MAX][20];

    for(int i=0;i<N;i++) {
        printf("\nRoll No: "); scanf("%d",&roll[i]);
        printf("Name: "); scanf("%s",name[i]);
        total[i]=0;
        for(int j=0;j<M;j++) {
            do { printf("Marks for subject %d: ",j+1); scanf("%d",&marks[i][j]);
                 if(marks[i][j]<0||marks[i][j]>100) printf("Invalid!\n");
            } while(marks[i][j]<0||marks[i][j]>100);
            total[i]+=marks[i][j];
        }
        percent[i]= (float)total[i]/M;
        grade[i]=getGrade(percent[i]);
        strcpy(classification[i], getClass(percent[i]));
        strcpy(status[i], getStatus(marks[i],M,percent[i]));
    }

    printf("\nRoll\tName\tTotal\t%%\tGrade\tClass\t\tStatus\n");
    for(int i=0;i<N;i++)
        printf("%d\t%s\t%d\t%.2f\t%c\t%-12s\t%s\n", roll[i], name[i], total[i], percent[i], grade[i], classification[i], status[i]);

    float avg=0; int top=0;
    for(int i=0;i<N;i++) { avg+=percent[i]; if(total[i]>total[top]) top=i; }
    printf("\nClass Average: %.2f%%\nTopper: Roll %d, Name %s, Total %d, %% %.2f\n", avg/N, roll[top], name[top], total[top], percent[top]);

    return 0;
}
