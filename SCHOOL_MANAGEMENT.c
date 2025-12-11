#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STUDENT_FILE "students.dat"
#define TEMP_STUDENT_FILE "temp_students.dat"
#define ADMIN_PASS "admin123"
#define MAX_NAME 100
#define MAX_GRADE 8
typedef struct{
    int roll;
    char name[MAX_NAME];
    int age;
    char grade[MAX_GRADE];
    float fees;
    int fees_paid;
} Student;
void clear_input_buffer(void);
void press_enter_to_continue(void);
int admin_login(void);
int find_student_by_roll(int roll, Student *out);
void add_student(void);
void remove_student(void);
void view_student_by_roll(void);
void list_all_students(void);
void update_fees_paid(void);
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
void press_enter_to_continue(void) {
    printf("\nPress Enter to continue...");
    clear_input_buffer();
}
int admin_login(void) {
    char pass[100];
    int attempts = 3;
    while (attempts--) {
        printf("Enter admin password: ");
        if (fgets(pass, sizeof(pass), stdin) == NULL) return 0;
        pass[strcspn(pass, "\n")] = 0;
        if (strcmp(pass, ADMIN_PASS) == 0) return 1;
        printf("Wrong password. %d attempt(s) left.\n", attempts);
    }
    return 0;
}
int find_student_by_roll(int roll, Student *out) {
    FILE *fp = fopen(STUDENT_FILE, "rb");
    if (!fp) return 0;
    Student s;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            if (out) *out = s;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}
void add_student(void) {
    Student s; FILE *fp;
    printf("\n---- Admission: Add New Student ----\n");
    printf("Enter roll number (integer): ");
    if (scanf("%d", &s.roll) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();

    if (find_student_by_roll(s.roll, NULL)) {
        printf("Student with roll %d already exists.\n", s.roll); return;
    }
    printf("Enter full name: ");
    if (fgets(s.name, sizeof(s.name), stdin) == NULL) return;
    s.name[strcspn(s.name, "\n")] = 0;
    printf("Enter age: ");
    if (scanf("%d", &s.age) != 1) { clear_input_buffer(); printf("Invalid age.\n"); return; }
    clear_input_buffer();
    printf("Enter grade/class (e.g. 10A): ");
    if (fgets(s.grade, sizeof(s.grade), stdin) == NULL) return;
    s.grade[strcspn(s.grade, "\n")] = 0;
    printf("Enter fees amount (e.g. 5000.00): ");
    if (scanf("%f", &s.fees) != 1) { clear_input_buffer(); printf("Invalid amount.\n"); return; }
    clear_input_buffer();
    s.fees_paid = 0;
    fp = fopen(STUDENT_FILE, "ab");
    if (!fp) { perror("Error opening student file"); return; }
    if (fwrite(&s, sizeof(Student), 1, fp) != 1) { perror("Error writing student"); }
    fclose(fp);
    printf("\nStudent added successfully.\n");
}
void remove_student(void) {
    int roll;
    printf("\n---- Remove Student ----\n");
    printf("Enter roll number to remove: ");
    if (scanf("%d", &roll) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();
    Student s;
    if (!find_student_by_roll(roll, &s)) {
        printf("Student with roll %d not found.\n", roll); return;
    }
    printf("CONFIRM: Remove student %s (Roll %d)? (y/n): ", s.name, roll);
    char confirmation;
    if (scanf(" %c", &confirmation) != 1 || (confirmation != 'y' && confirmation != 'Y')) {
        printf("Removal cancelled.\n"); clear_input_buffer(); return;
    }
    clear_input_buffer();
    FILE *original_s = fopen(STUDENT_FILE, "rb");
    FILE *temp_s = fopen(TEMP_STUDENT_FILE, "wb");
    if (!original_s || !temp_s) { perror("Error opening files"); return; }
    Student current_s; int removed = 0;
    while (fread(&current_s, sizeof(Student), 1, original_s) == 1) {
        if (current_s.roll != roll) {
            fwrite(&current_s, sizeof(Student), 1, temp_s);
        } else {
            removed = 1;
        }
    }
    fclose(original_s); fclose(temp_s);
    remove(STUDENT_FILE);
    rename(TEMP_STUDENT_FILE, STUDENT_FILE);
    if (removed) {
        printf("Student record for Roll %d successfully removed.\n", roll);
    } else {
        printf("Error: Student record was not found during removal process.\n");
    }
}
void view_student_by_roll(void) {
    int roll;
    printf("Enter roll number to view: ");
    if (scanf("%d", &roll) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();
    Student s;
    if (find_student_by_roll(roll, &s)) {
        printf("\n--- Student Details ---\n");
        printf("Roll: %d | Name: %s\n", s.roll, s.name);
        printf("Age: %d | Grade: %s\n", s.age, s.grade);
        printf("Fees: $%.2f | Paid: %s\n", s.fees, s.fees_paid ? "Yes" : "No");
    } else {
        printf("Student with roll %d not found.\n", roll);
    }
}
void list_all_students(void) {
    FILE *fp = fopen(STUDENT_FILE, "rb");
    if (!fp) { printf("No student records.\n"); return; }
    Student s;
    printf("----- All Students -----\n");
    printf("%-6s | %-30s | %-8s | %-8s\n", "ROLL", "NAME", "GRADE", "FEES PAID");
    printf("------------------------------------------------------------------\n");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        printf("%-6d | %-30s | %-8s | %-8s\n",
               s.roll, s.name, s.grade, s.fees_paid ? "Yes" : "No");
    }
    fclose(fp);
}
void update_fees_paid(void) {
    int roll;
    printf("Enter roll number to update fees status: ");
    if (scanf("%d", &roll) != 1) { clear_input_buffer(); printf("Invalid input.\n"); return; }
    clear_input_buffer();
    FILE *fp = fopen(STUDENT_FILE, "rb+");
    if (!fp) { perror("Unable to open student file"); return; }
    Student s; long pos; int found = 0;
    while ((pos = ftell(fp)), fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            found = 1;
            printf("Found student: %s (Fees: %.2f) | Paid status: %s\n", s.name, s.fees, s.fees_paid ? "Yes" : "No");
            printf("Set fees status (1 for PAID, 0 for NOT PAID): ");
            int val;
            if (scanf("%d", &val) != 1 || (val != 0 && val != 1)) {
                clear_input_buffer(); printf("Invalid status input.\n"); break;
            }
            clear_input_buffer();
            s.fees_paid = val;
            fseek(fp, pos, SEEK_SET);
            if (fwrite(&s, sizeof(Student), 1, fp) != 1) {
                perror("Error updating record");
            } else {
                printf("Fees status updated to: %s\n", s.fees_paid ? "PAID" : "NOT PAID");
            }
            break;
        }
    }
    if (!found) printf("Student roll %d not found.\n", roll);
    fclose(fp);
}
int main(void) {
    printf("==== SCHOOL MANAGEMENT SYSTEM ====\n\n");
    FILE *t = fopen(STUDENT_FILE, "ab+"); if (t) fclose(t);
    if (!admin_login()) {
        printf("Access denied. Exiting.\n"); return 0;
    }
    int choice;
    while (1) {
        printf("\n--- Admin Menu ---\n");
        printf("1. Admission (Add Student)\n");
        printf("2. Remove Student\n");
        printf("3. View Student Details (by Roll)\n");
        printf("4. List All Students\n");
        printf("5. Update Fees Paid Status\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer(); printf("Invalid input.\n"); continue;
        }
        clear_input_buffer();
        switch (choice) {
            case 1: add_student(); break;
            case 2: remove_student(); break;
            case 3: view_student_by_roll(); break;
            case 4: list_all_students(); break;
            case 5: update_fees_paid(); break;
            case 6:
                printf("System shutting down. Goodbye.\n"); return 0;
            default:
                printf("Invalid option. Please try again.\n");
        }
        press_enter_to_continue();
    }
    return 0;
}
