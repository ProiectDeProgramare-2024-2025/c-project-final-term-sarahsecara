#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_MEETINGS 100
#define FILE_NAME "meetings.txt"

// ANSI color codes
#define COLOR_RESET    "\033[0m"
#define COLOR_RED      "\033[31m"
#define COLOR_GREEN    "\033[32m"
#define COLOR_YELLOW   "\033[33m"
#define COLOR_BLUE     "\033[34m"
#define COLOR_MAGENTA  "\033[35m"
#define COLOR_CYAN     "\033[36m"
#define COLOR_WHITE    "\033[37m"

typedef struct {
    char name[50];
    char date[20];
    char time[10];
    int duration;
} Meeting;

Meeting meetings[MAX_MEETINGS];
int meeting_count = 0;

int validate_date(const char *date) {
    if (strlen(date) != 10) return 0;
    if (!isdigit(date[0]) || !isdigit(date[1]) || date[2] != '/' ||
        !isdigit(date[3]) || !isdigit(date[4]) || date[5] != '/' ||
        !isdigit(date[6]) || !isdigit(date[7]) || !isdigit(date[8]) || !isdigit(date[9])) return 0;

    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');

    if (day < 1 || day > 31) return 0;
    if (month < 1 || month > 12) return 0;

    return 1;
}

int validate_time(const char *time) {
    if (strlen(time) != 5) return 0;
    if (!isdigit(time[0]) || !isdigit(time[1]) || time[2] != ':' ||
        !isdigit(time[3]) || !isdigit(time[4])) return 0;

    int hour = (time[0] - '0') * 10 + (time[1] - '0');
    int minute = (time[3] - '0') * 10 + (time[4] - '0');

    if (hour < 0 || hour > 23) return 0;
    if (minute < 0 || minute > 59) return 0;

    return 1;
}

int validate_duration(int duration) {
    return duration > 0;
}

void write_file() {
    FILE *file = fopen(FILE_NAME, "w");
    if (!file) {
        printf(COLOR_RED "Error saving meetings!\n" COLOR_RESET);
        return;
    }
    for (int i = 0; i < meeting_count; i++) {
        fprintf(file, "%s|%s|%s|%d\n", meetings[i].name, meetings[i].date, meetings[i].time, meetings[i].duration);
    }
    fclose(file);
}

void read_file() {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        file = fopen(FILE_NAME, "w");
        if (file) {
            fprintf(file, "Team Meeting|20/04/2025|14:00|60\n");
            fprintf(file, "Project Update|22/04/2025|10:30|45\n");
            fclose(file);
        }
        return;
    }
    meeting_count = 0;
    while (fscanf(file, " %49[^|]|%19[^|]|%9[^|]|%d\n",
                  meetings[meeting_count].name,
                  meetings[meeting_count].date,
                  meetings[meeting_count].time,
                  &meetings[meeting_count].duration) == 4) {
        meeting_count++;
        if (meeting_count >= MAX_MEETINGS) break;
    }
    fclose(file);
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void header() {
    printf("--------------\n");
    printf("1 - Add Meeting\n");
    printf("2 - Display Meetings\n");
    printf("3 - Delete Meeting\n");
    printf("0 - Exit\n");
    printf("--------------\n");
}

void add_meeting() {
    if (meeting_count >= MAX_MEETINGS) {
        printf(COLOR_RED "Meeting list is full! Press Enter to continue..." COLOR_RESET);
        getchar(); getchar();
        return;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF); // clear stdin

    do {
        printf("Enter meeting name: ");
        fgets(meetings[meeting_count].name, 50, stdin);
        strtok(meetings[meeting_count].name, "\n");
        if (strlen(meetings[meeting_count].name) == 0) {
            printf(COLOR_RED "Name cannot be empty! Please try again.\n" COLOR_RESET);
        }
    } while (strlen(meetings[meeting_count].name) == 0);

    do {
        printf("Enter date (dd/mm/yyyy): ");
        scanf(" %19s", meetings[meeting_count].date);
        if (!validate_date(meetings[meeting_count].date)) {
            printf(COLOR_RED "Invalid date format or range! Please try again.\n" COLOR_RESET);
        }
    } while (!validate_date(meetings[meeting_count].date));

    do {
        printf("Enter time (hh:mm): ");
        scanf(" %9s", meetings[meeting_count].time);
        if (!validate_time(meetings[meeting_count].time)) {
            printf(COLOR_RED "Invalid time format or range! Please try again.\n" COLOR_RESET);
        }
    } while (!validate_time(meetings[meeting_count].time));

    do {
        printf("Enter duration (minutes): ");
        scanf(" %d", &meetings[meeting_count].duration);
        if (!validate_duration(meetings[meeting_count].duration)) {
            printf(COLOR_RED "Invalid duration! Must be positive.\n" COLOR_RESET);
        }
    } while (!validate_duration(meetings[meeting_count].duration));

    meeting_count++;
    write_file();

    printf(COLOR_GREEN "Meeting added successfully! Press Enter to continue..." COLOR_RESET);
    getchar(); getchar();

    clear_screen();
}

void display_meetings() {
    read_file();
    if (meeting_count == 0) {
        printf(COLOR_RED "No meetings scheduled. Press Enter to continue..." COLOR_RESET);
        getchar(); getchar();
        return;
    }
    printf("Scheduled Meetings:\n");
    for (int i = 0; i < meeting_count; i++) {
        printf("%d. %s%s%s - %s%s%s %s%s%s - Duration: %s%d mins%s\n",
               i + 1,
               COLOR_CYAN, meetings[i].name, COLOR_RESET,
               COLOR_YELLOW, meetings[i].date, COLOR_RESET,
               COLOR_GREEN, meetings[i].time, COLOR_RESET,
               COLOR_MAGENTA, meetings[i].duration, COLOR_RESET);
    }
    printf("Press Enter to continue...");
    getchar(); getchar();
    clear_screen();
}

void delete_meeting() {
    if (meeting_count == 0) {
        printf(COLOR_RED "No meetings to delete! Press Enter to continue..." COLOR_RESET);
        getchar(); getchar();
        return;
    }
    printf("Enter meeting index to delete (1 to %d): ", meeting_count);
    int index;
    scanf(" %d", &index);
    if (index < 1 || index > meeting_count) {
        printf(COLOR_RED "Invalid index! Press Enter to continue..." COLOR_RESET);
        getchar(); getchar();
        return;
    }
    printf("Are you sure you want to delete '%s%s%s'? (y/n): ",
           COLOR_CYAN, meetings[index - 1].name, COLOR_RESET);
    char confirm;
    getchar();
    scanf("%c", &confirm);
    if (confirm == 'y' || confirm == 'Y') {
        for (int i = index - 1; i < meeting_count - 1; i++) {
            meetings[i] = meetings[i + 1];
        }
        meeting_count--;
        write_file();
        printf(COLOR_GREEN "Meeting deleted successfully! Press Enter to continue..." COLOR_RESET);
    } else {
        printf(COLOR_YELLOW "Deletion canceled! Press Enter to continue..." COLOR_RESET);
    }
    getchar(); getchar();
    clear_screen();
}

void menu(int op) {
    switch(op) {
    case 1:
        add_meeting();
        break;
    case 2:
        display_meetings();
        break;
    case 3:
        delete_meeting();
        break;
    default:
        printf("EXIT!\n");
    }
    clear_screen();
}

int main() {
    read_file();
    int option;
    do {
        header();
        printf("Enter option: ");
        scanf("%d", &option);
        menu(option);
        clear_screen();
    } while(option > 0 && option <= 3);
    return 0;
}
