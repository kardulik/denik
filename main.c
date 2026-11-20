#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

typedef struct ENTRY {
    char text[50];
} entry;

typedef struct DAYS {
    int day;
    int month;
    int year;

    entry *List;
    int size;
    int count;

    struct DAYS *previous;
    struct DAYS *next;

}days;

days *first;
days *last;
days *current;

void showAll() {
    if (current == NULL) {
        return;
    }
    current = first;
    while (current != NULL) {
        printf("%d %d %d, %d entries\n", current->day, current->month, current->year, current->count);
        current = current->next;
    }

    scanf("\n");
}

void memoryFlush() {
    if (current == NULL) {
        return;
    }
    current = first;
    while (1) {
        if (current->next == NULL) {
            free(current->List);
            free(current);
            return;
        }
        current = current->next;
        free(current->previous->List);
        free(current->previous);
    }
}



void showCurrent() {
    if (current == NULL) {
        return;
    }
    printf("\nDay %d %d %d has these notes:\n", current->day, current->month, current->year);
    for (int i = 0; i < current->count; i++) {
        printf("%d. %s\n", i+1, current->List[i].text);
    }
    printf("\n");
}
//0 means doesnt exist, 1 means exists
int findDay(int day, int month, int year) {
    if (current == NULL) {
        return 0;
    }
    current = first;
    while (1) {
        if (current->day == day && current->month == month && current->year == year) {
            return 1;
        }
        if (current->next == NULL) {
            return 0;
        }
        current = current->next;
    }
}

void insertDay(days *new) {
    if (current == NULL) {
        first = new;
        last = new;
        return;
    }
    current = first;
    while (1) {
        if (current->year > new->year ||
            (current->year == new->year && current->month > new->month) ||
            (current->year == new->year && current->month == new->month && current->day > new->day))
        {
            if (current == first) {
                new->next = current;
                current->previous = new;
                first = new;
                return;
            }
            new->next = current;
            new->previous = current->previous;
            new->previous->next = new;
            current->previous = new;
            return;
        }
        if (current->next == NULL) {
            current->next = new;
            new->previous = current;
            last = new;
            return;
        }
        current = current->next;
    }
}

//adds new entry, will create a new array if it doesn't exist, enlarge it if needed
void addEntry(char* text) {
    if (current->List == NULL) {
        current->size = 4;
        current->List = (entry *)malloc(sizeof(entry) * current->size);
    }
    else if (current->size == current->count) {
        current->size = current->size * 2;
        current->List = (entry *)realloc(current->List, sizeof(entry) * current->size);
    }

    strcpy(current->List[current->count].text, text);
    current->count++;
}

//removes an entry
void deleteEntry() {
    if (current == NULL) {
        return;
    }
    int x;
    printf("Select which entry you would like to remove?");
    scanf("%d", &x);

    if (x <= 0 || x > current->count) {
        return;
    }

    for (int i = x - 1; i < current->count; i++) {
        current->List[i] = current->List[i + 1];
    }
    current->count--;

    if (current->count == 0) {
        free(current->List);
        if (current == last && current == first) {
            current = NULL;
            first = NULL;
            last = NULL;
            return;
        }

        days* temp = current;

        if (current == first) {
            first = current->next;
            current = first;
            current->previous = NULL;
            free(temp);
            return;
        }
        if (current == last) {
            last = current->previous;
            current = last;
            current->next = NULL;
            free(temp);
            return;
        }
        current->next->previous = current->previous;
        current->previous->next = current->next;
        current = current->next;
        free(temp);
    }
}

//adds a new day, creates it if needed
void addDay(char *text, int dd, int mm, int yyyy) {
    if (findDay(dd, mm, yyyy) == 1) {
        addEntry(text);
        return;
    }

    days *new = (days *)malloc(sizeof(days));

    new->day = dd;
    new->month = mm;
    new->year = yyyy;

    new->List = NULL;
    new->count = 0;

    new->next = NULL;
    new->previous = NULL;

    insertDay(new);
    current = new;
    addEntry(text);
}

//gets user input about new day and creates it
void newDay() {
    printf("Enter a date [DD MM YYYY]: ");
    int dd, mm, yyyy;
    scanf("%d %d %d", &dd, &mm, &yyyy);

    printf("Enter your new entry\n(ends by typing '/'): ");
    char entry[255];
    char ch;
    int i = 0;
    while ((ch = getchar()) != '/') {
        entry[i++] = ch;
    }

    addDay(entry, dd, mm, yyyy);
}

//shows next day, if it exists
void nextDay() {
    if (current == NULL || current->next == NULL) {
        return;
    }
    current = current->next;
}

//shows previous day, if it exists
void previousDay() {
    if (current == NULL || current->previous == NULL) {
        return;
    }
    current = current->previous;
}

//shows first day, if it exists
void firstDay() {
    if (current == NULL || current == first) {
        return;
    }
    current = first;
}

//shows last day, if it exists
void lastDay() {
    if (current == NULL || current == last) {
        return;
    }
    current = last;
}

void saveFile() {

    char fileName[50];
    printf("Enter File Name: ");
    scanf("%s", &fileName);
    if (!fileName[0]) {
        fileName[0] = 'a';
    }
    strcat(fileName, ".hmm");
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        printf("File could not be opened\n");
    }

    if (current == NULL) {
        return;
    }
    current = first;
    while (1) {
        for (int i = 0; i < current->count; i++) {
            fprintf(file,"%s,%d/%d/%d\n", current->List[i].text, current->day, current->month, current->year);
        }
        if (current->next == NULL) {
            getchar();
            fclose(file);
            return;
        }
        current = current->next;
    }
}

void loadFile() {
    char fileName[50];
    printf("Enter File Name: ");
    scanf("%s", &fileName);
    if (!fileName[0]) {
        fileName[0] = 'a';
    }
    strcat(fileName, ".hmm");
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("File could not be opened\n");
        return;
    }

    memoryFlush();
    current = NULL;
    char line[100];
    while (fscanf(file, "%s,%d/%d/%d", line) == 1)
    {
        char *token = strtok(line, ",");
        int day = atoi(strtok(NULL, "/"));
        int month = atoi(strtok(NULL, "/"));
        int year = atoi(strtok(NULL, "/"));

        addDay(token, day, month, year);
    }
    fclose(file);
}

//main user loop
int main(void) {
    char input[25];
    int counter = 1;

    first = NULL;
    last = NULL;
    current = NULL;

    printf("Hi user!\nSelect a specific function by writing a specified number or writing required text\n");

    while(1) {
        printf("These are available operations:\n");


        printf("%i. Next\n", counter++);
        printf("%i. Previous\n", counter++);
        printf("%i. First\n", counter++);
        printf("%i. Last\n", counter++);
        printf("%i. New\n", counter++);
        printf("%i. Delete\n", counter++);
        printf("%i. Save\n", counter++);
        printf("%i. Load\n", counter++);
        printf("%i. Exit\n", counter);

        showCurrent();

        scanf("%s", &input);

        switch (input[0]) {
            case '0'://show all
                showAll();
                break;
            case '1'://next
                nextDay();
                break;
            case '2'://previous
                previousDay();
                break;
            case '3'://first
                firstDay();
                break;
            case '4'://last
                lastDay();
                break;
            case '5'://new
                newDay();
                break;
            case '6'://delete
                deleteEntry();
                break;
            case '7'://save
                saveFile();
                break;
            case '8'://load
                loadFile();
                break;
            case '9'://end
                return 0;
        }

        counter = 1;
        printf("\033c");
    }
    return 0;
}