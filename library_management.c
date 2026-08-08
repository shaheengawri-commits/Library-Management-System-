/* =====================================================================
 * LIBRARY MANAGEMENT SYSTEM
 * ---------------------------------------------------------------------
 * A console-based Library Management System written in C.
 *
 * This program uses parallel arrays (NOT structures, NOT files, NOT a
 * database) to store information about books, users, and book issues.
 * Records are never physically removed from the arrays; instead an
 * "active" flag is used to mark a record as deleted/inactive.
 *
 * Concepts demonstrated:
 *   - Arrays and parallel arrays
 *   - Functions
 *   - Loops and conditional statements
 *   - String handling (strcmp, strcpy, strstr)
 *   - Case-insensitive searching
 *   - Bubble Sort (sort books by title / author)
 *   - Selection Sort (find top issued books)
 *   - Basic input validation
 * ===================================================================== */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ---------------------------------------------------------------------
 * CONSTANTS - fixed maximum sizes for our parallel arrays
 * --------------------------------------------------------------------- */
#define MAX_BOOKS   100
#define MAX_USERS   50
#define MAX_ISSUES  200
#define NAME_LEN    100
#define CAT_LEN     50

/* ---------------------------------------------------------------------
 * BOOK DATA (parallel arrays)
 * --------------------------------------------------------------------- */
int  bookIds[MAX_BOOKS];
char bookTitles[MAX_BOOKS][NAME_LEN];
char bookAuthors[MAX_BOOKS][NAME_LEN];
char bookCategories[MAX_BOOKS][CAT_LEN];
int  bookTotalCopies[MAX_BOOKS];
int  bookAvailableCopies[MAX_BOOKS];
int  bookIssueCount[MAX_BOOKS];   /* how many times a book has been issued (for stats) */
int  bookActive[MAX_BOOKS];       /* 1 = active, 0 = deleted */
int  bookCount = 0;
int  nextBookId = 1;

/* ---------------------------------------------------------------------
 * USER DATA (parallel arrays)
 * --------------------------------------------------------------------- */
int  userIds[MAX_USERS];
char userNames[MAX_USERS][NAME_LEN];
int  userActive[MAX_USERS];
int  userCount = 0;
int  nextUserId = 1;

/* ---------------------------------------------------------------------
 * ISSUE DATA (parallel arrays)
 * --------------------------------------------------------------------- */
int  issueIds[MAX_ISSUES];
int  issueBookId[MAX_ISSUES];
int  issueUserId[MAX_ISSUES];
int  issueActive[MAX_ISSUES];     /* 1 = currently issued, 0 = returned */
int  issueCount = 0;
int  nextIssueId = 1;

/* =====================================================================
 * UTILITY FUNCTIONS
 * ===================================================================== */

/* Clears the input buffer after scanf() to avoid leftover newline
 * characters interfering with the next input. */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* discard */
    }
}

/* Copies src into dest, converting every character to lowercase.
 * Used to perform case-insensitive searching. */
void to_lower_copy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = (char) tolower((unsigned char) src[i]);
        i++;
    }
    dest[i] = '\0';
}

/* Reads a full line of text into buffer, safely, and strips the
 * trailing newline character if present. */
void read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        int len = (int) strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        buffer[0] = '\0';
    }
}

/* =====================================================================
 * LOOKUP HELPERS - find array index from an ID
 * ===================================================================== */

/* Returns the array index of an active book with the given ID,
 * or -1 if not found. */
int find_book_index_by_id(int id) {
    int i;
    for (i = 0; i < bookCount; i++) {
        if (bookIds[i] == id && bookActive[i] == 1) {
            return i;
        }
    }
    return -1;
}

/* Returns the array index of an active user with the given ID,
 * or -1 if not found. */
int find_user_index_by_id(int id) {
    int i;
    for (i = 0; i < userCount; i++) {
        if (userIds[i] == id && userActive[i] == 1) {
            return i;
        }
    }
    return -1;
}

/* =====================================================================
 * USER HELPER - used internally when issuing a book to a new user
 * ===================================================================== */

/* Registers a new user record and returns its array index.
 * Called automatically from issue_book() when an unknown user ID
 * is supplied, so that a book can be issued to that user. */
int register_user(int id, const char *name) {
    if (userCount >= MAX_USERS) {
        printf("User storage is full. Cannot register more users.\n");
        return -1;
    }
    userIds[userCount] = id;
    strcpy(userNames[userCount], name);
    userActive[userCount] = 1;
    userCount++;
    return userCount - 1;
}

/* =====================================================================
 * BOOK MANAGEMENT FUNCTIONS
 * ===================================================================== */

/* Prompts the user for book details and adds a new book record. */
void add_book_interactive(void) {
    if (bookCount >= MAX_BOOKS) {
        printf("Book storage is full. Cannot add more books.\n");
        return;
    }

    char title[NAME_LEN], author[NAME_LEN], category[CAT_LEN];
    int copies;

    printf("Enter Book Title: ");
    read_line(title, NAME_LEN);

    printf("Enter Author Name: ");
    read_line(author, NAME_LEN);

    printf("Enter Category: ");
    read_line(category, CAT_LEN);

    printf("Enter Number of Copies: ");
    while (scanf("%d", &copies) != 1 || copies < 1) {
        printf("Invalid input. Please enter a positive number of copies: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    bookIds[bookCount] = nextBookId++;
    strcpy(bookTitles[bookCount], title);
    strcpy(bookAuthors[bookCount], author);
    strcpy(bookCategories[bookCount], category);
    bookTotalCopies[bookCount] = copies;
    bookAvailableCopies[bookCount] = copies;
    bookIssueCount[bookCount] = 0;
    bookActive[bookCount] = 1;

    printf("Book added successfully with ID: %d\n", bookIds[bookCount]);
    bookCount++;
}

/* Displays all active books in a formatted table. */
void display_all_books(void) {
    int i, found = 0;

    printf("\n%-5s %-25s %-20s %-15s %-8s %-10s\n",
           "ID", "Title", "Author", "Category", "Total", "Available");
    printf("--------------------------------------------------------------------------\n");

    for (i = 0; i < bookCount; i++) {
        if (bookActive[i] == 1) {
            printf("%-5d %-25s %-20s %-15s %-8d %-10d\n",
                   bookIds[i], bookTitles[i], bookAuthors[i],
                   bookCategories[i], bookTotalCopies[i], bookAvailableCopies[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No books found in the library.\n");
    }
}

/* Searches for books whose title contains the given keyword.
 * The search is case-insensitive and uses strstr(). */
void search_by_title(void) {
    char keyword[NAME_LEN], lowerKeyword[NAME_LEN], lowerTitle[NAME_LEN];
    int i, found = 0;

    printf("Enter title keyword to search: ");
    read_line(keyword, NAME_LEN);
    to_lower_copy(lowerKeyword, keyword);

    printf("\nSearch Results:\n");
    for (i = 0; i < bookCount; i++) {
        if (bookActive[i] == 1) {
            to_lower_copy(lowerTitle, bookTitles[i]);
            if (strstr(lowerTitle, lowerKeyword) != NULL) {
                printf("ID: %d | Title: %s | Author: %s | Category: %s | Available: %d\n",
                       bookIds[i], bookTitles[i], bookAuthors[i],
                       bookCategories[i], bookAvailableCopies[i]);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No books matched your search.\n");
    }
}

/* Searches for books whose author name contains the given keyword.
 * The search is case-insensitive and uses strstr(). */
void search_by_author(void) {
    char keyword[NAME_LEN], lowerKeyword[NAME_LEN], lowerAuthor[NAME_LEN];
    int i, found = 0;

    printf("Enter author keyword to search: ");
    read_line(keyword, NAME_LEN);
    to_lower_copy(lowerKeyword, keyword);

    printf("\nSearch Results:\n");
    for (i = 0; i < bookCount; i++) {
        if (bookActive[i] == 1) {
            to_lower_copy(lowerAuthor, bookAuthors[i]);
            if (strstr(lowerAuthor, lowerKeyword) != NULL) {
                printf("ID: %d | Title: %s | Author: %s | Category: %s | Available: %d\n",
                       bookIds[i], bookTitles[i], bookAuthors[i],
                       bookCategories[i], bookAvailableCopies[i]);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No books matched your search.\n");
    }
}

/* Marks a book as inactive (soft delete) based on its ID.
 * The record stays in the array but is skipped everywhere else. */
void delete_book_by_id(void) {
    int id, index;

    printf("Enter Book ID to delete: ");
    while (scanf("%d", &id) != 1) {
        printf("Invalid input. Enter a numeric Book ID: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    index = find_book_index_by_id(id);
    if (index == -1) {
        printf("Book with ID %d not found.\n", id);
        return;
    }

    bookActive[index] = 0;
    printf("Book '%s' deleted successfully.\n", bookTitles[index]);
}

/* Allows editing of an existing book's title, author, category and
 * total copy count. Pressing Enter on a field keeps its old value. */
void edit_book_details(void) {
    int id, index;
    char input[NAME_LEN];

    printf("Enter Book ID to edit: ");
    while (scanf("%d", &id) != 1) {
        printf("Invalid input. Enter a numeric Book ID: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    index = find_book_index_by_id(id);
    if (index == -1) {
        printf("Book with ID %d not found.\n", id);
        return;
    }

    printf("Leave a field blank and press Enter to keep the current value.\n");

    printf("Current Title: %s\nNew Title: ", bookTitles[index]);
    read_line(input, NAME_LEN);
    if (strlen(input) > 0) strcpy(bookTitles[index], input);

    printf("Current Author: %s\nNew Author: ", bookAuthors[index]);
    read_line(input, NAME_LEN);
    if (strlen(input) > 0) strcpy(bookAuthors[index], input);

    printf("Current Category: %s\nNew Category: ", bookCategories[index]);
    read_line(input, CAT_LEN);
    if (strlen(input) > 0) strcpy(bookCategories[index], input);

    printf("Current Total Copies: %d\nNew Total Copies (0 to keep current): ", bookTotalCopies[index]);
    {
        int newCopies;
        while (scanf("%d", &newCopies) != 1) {
            printf("Invalid input. Enter a numeric value: ");
            clear_input_buffer();
        }
        clear_input_buffer();
        if (newCopies > 0) {
            int issuedOut = bookTotalCopies[index] - bookAvailableCopies[index];
            bookTotalCopies[index] = newCopies;
            bookAvailableCopies[index] = newCopies - issuedOut;
            if (bookAvailableCopies[index] < 0) bookAvailableCopies[index] = 0;
        }
    }

    printf("Book details updated successfully.\n");
}

/* Lists all active books belonging to a given category
 * (case-insensitive exact match). */
void list_by_category(void) {
    char category[CAT_LEN], lowerInput[CAT_LEN], lowerStored[CAT_LEN];
    int i, found = 0;

    printf("Enter Category: ");
    read_line(category, CAT_LEN);
    to_lower_copy(lowerInput, category);

    printf("\nBooks in category '%s':\n", category);
    for (i = 0; i < bookCount; i++) {
        if (bookActive[i] == 1) {
            to_lower_copy(lowerStored, bookCategories[i]);
            if (strcmp(lowerStored, lowerInput) == 0) {
                printf("ID: %d | Title: %s | Author: %s | Available: %d\n",
                       bookIds[i], bookTitles[i], bookAuthors[i], bookAvailableCopies[i]);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No books found in this category.\n");
    }
}

/* =====================================================================
 * ISSUE / RETURN LOGIC
 * ===================================================================== */

/* Issues a book to a user. If the given User ID does not already
 * exist, the user is registered automatically. Decreases the book's
 * available copy count and creates a new active issue record. */
void issue_book(void) {
    int bookId, userId, bookIndex, userIndex;
    char userName[NAME_LEN];

    if (issueCount >= MAX_ISSUES) {
        printf("Issue records are full. Cannot issue more books.\n");
        return;
    }

    printf("Enter Book ID to issue: ");
    while (scanf("%d", &bookId) != 1) {
        printf("Invalid input. Enter a numeric Book ID: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    bookIndex = find_book_index_by_id(bookId);
    if (bookIndex == -1) {
        printf("Book with ID %d not found.\n", bookId);
        return;
    }
    if (bookAvailableCopies[bookIndex] <= 0) {
        printf("No available copies of '%s' at the moment.\n", bookTitles[bookIndex]);
        return;
    }

    printf("Enter User ID: ");
    while (scanf("%d", &userId) != 1) {
        printf("Invalid input. Enter a numeric User ID: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    userIndex = find_user_index_by_id(userId);
    if (userIndex == -1) {
        printf("User ID not found. Registering a new user.\n");
        printf("Enter User Name: ");
        read_line(userName, NAME_LEN);
        userIndex = register_user(userId, userName);
        if (userIndex == -1) {
            return; /* user storage full */
        }
    }

    /* Create the issue record */
    issueIds[issueCount] = nextIssueId++;
    issueBookId[issueCount] = bookId;
    issueUserId[issueCount] = userId;
    issueActive[issueCount] = 1;
    issueCount++;

    bookAvailableCopies[bookIndex]--;
    bookIssueCount[bookIndex]++;

    printf("Book '%s' issued successfully to %s (User ID: %d).\n",
           bookTitles[bookIndex], userNames[userIndex], userId);
}

/* Returns a book based on its active issue record. Increases the
 * book's available copy count and marks the issue record inactive. */
void return_book(void) {
    int bookId, userId, i, bookIndex, recordIndex = -1;

    printf("Enter Book ID to return: ");
    while (scanf("%d", &bookId) != 1) {
        printf("Invalid input. Enter a numeric Book ID: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    printf("Enter User ID: ");
    while (scanf("%d", &userId) != 1) {
        printf("Invalid input. Enter a numeric User ID: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    /* Find the matching active issue record */
    for (i = 0; i < issueCount; i++) {
        if (issueBookId[i] == bookId && issueUserId[i] == userId && issueActive[i] == 1) {
            recordIndex = i;
            break;
        }
    }

    if (recordIndex == -1) {
        printf("No active issue record found for this Book ID and User ID.\n");
        return;
    }

    issueActive[recordIndex] = 0;

    bookIndex = find_book_index_by_id(bookId);
    if (bookIndex != -1) {
        bookAvailableCopies[bookIndex]++;
    }

    printf("Book returned successfully.\n");
}

/* Lists every issue record that is currently active (not yet returned). */
void list_current_issues(void) {
    int i, bookIndex, userIndex, found = 0;

    printf("\n%-8s %-8s %-25s %-8s %-20s\n",
           "IssueID", "BookID", "Title", "UserID", "User Name");
    printf("--------------------------------------------------------------------------\n");

    for (i = 0; i < issueCount; i++) {
        if (issueActive[i] == 1) {
            bookIndex = find_book_index_by_id(issueBookId[i]);
            userIndex = find_user_index_by_id(issueUserId[i]);

            printf("%-8d %-8d %-25s %-8d %-20s\n",
                   issueIds[i],
                   issueBookId[i],
                   (bookIndex != -1) ? bookTitles[bookIndex] : "(unknown)",
                   issueUserId[i],
                   (userIndex != -1) ? userNames[userIndex] : "(unknown)");
            found = 1;
        }
    }

    if (!found) {
        printf("There are no books currently issued.\n");
    }
}

/* Displays every issue (active and returned) associated with a
 * given User ID. */
void issues_by_user(void) {
    int userId, i, bookIndex, found = 0;

    printf("Enter User ID: ");
    while (scanf("%d", &userId) != 1) {
        printf("Invalid input. Enter a numeric User ID: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    printf("\nIssue history for User ID %d:\n", userId);
    for (i = 0; i < issueCount; i++) {
        if (issueUserId[i] == userId) {
            bookIndex = find_book_index_by_id(issueBookId[i]);
            printf("IssueID: %d | Book: %s | Status: %s\n",
                   issueIds[i],
                   (bookIndex != -1) ? bookTitles[bookIndex] : "(unknown)",
                   issueActive[i] == 1 ? "Currently Issued" : "Returned");
            found = 1;
        }
    }

    if (!found) {
        printf("No issue history found for this User ID.\n");
    }
}

/* =====================================================================
 * SORTING ALGORITHMS
 * ===================================================================== */

/* Swaps every parallel-array field belonging to book index a and b.
 * Used by both bubble sort routines below. */
void swap_books(int a, int b) {
    int tmpInt;
    char tmpStr[NAME_LEN];

    tmpInt = bookIds[a];           bookIds[a] = bookIds[b];                   bookIds[b] = tmpInt;
    tmpInt = bookTotalCopies[a];   bookTotalCopies[a] = bookTotalCopies[b];   bookTotalCopies[b] = tmpInt;
    tmpInt = bookAvailableCopies[a]; bookAvailableCopies[a] = bookAvailableCopies[b]; bookAvailableCopies[b] = tmpInt;
    tmpInt = bookIssueCount[a];    bookIssueCount[a] = bookIssueCount[b];     bookIssueCount[b] = tmpInt;
    tmpInt = bookActive[a];        bookActive[a] = bookActive[b];             bookActive[b] = tmpInt;

    strcpy(tmpStr, bookTitles[a]);     strcpy(bookTitles[a], bookTitles[b]);       strcpy(bookTitles[b], tmpStr);
    strcpy(tmpStr, bookAuthors[a]);    strcpy(bookAuthors[a], bookAuthors[b]);     strcpy(bookAuthors[b], tmpStr);
    strcpy(tmpStr, bookCategories[a]); strcpy(bookCategories[a], bookCategories[b]); strcpy(bookCategories[b], tmpStr);
}

/* Sorts the book array by title (A-Z) using Bubble Sort.
 * Comparison is case-insensitive. */
void sort_books_by_title(void) {
    int i, j;
    char lowerA[NAME_LEN], lowerB[NAME_LEN];

    for (i = 0; i < bookCount - 1; i++) {
        for (j = 0; j < bookCount - 1 - i; j++) {
            to_lower_copy(lowerA, bookTitles[j]);
            to_lower_copy(lowerB, bookTitles[j + 1]);
            if (strcmp(lowerA, lowerB) > 0) {
                swap_books(j, j + 1);
            }
        }
    }
    printf("Books sorted by Title (A-Z).\n");
    display_all_books();
}

/* Sorts the book array by author name (A-Z) using Bubble Sort.
 * Comparison is case-insensitive. */
void sort_books_by_author(void) {
    int i, j;
    char lowerA[NAME_LEN], lowerB[NAME_LEN];

    for (i = 0; i < bookCount - 1; i++) {
        for (j = 0; j < bookCount - 1 - i; j++) {
            to_lower_copy(lowerA, bookAuthors[j]);
            to_lower_copy(lowerB, bookAuthors[j + 1]);
            if (strcmp(lowerA, lowerB) > 0) {
                swap_books(j, j + 1);
            }
        }
    }
    printf("Books sorted by Author (A-Z).\n");
    display_all_books();
}

/* Finds and displays the top N most-issued books using a
 * Selection Sort approach: repeatedly select the book with the
 * highest remaining issue count. The original book array order is
 * not modified; a temporary index array is sorted instead. */
void show_top_issued(void) {
    int indexOrder[MAX_BOOKS];
    int i, j, maxIndex, tmp, limit, shown = 0;

    if (bookCount == 0) {
        printf("No books available.\n");
        return;
    }

    for (i = 0; i < bookCount; i++) {
        indexOrder[i] = i;
    }

    /* Selection Sort on indexOrder[], ranked by bookIssueCount descending */
    for (i = 0; i < bookCount - 1; i++) {
        maxIndex = i;
        for (j = i + 1; j < bookCount; j++) {
            if (bookIssueCount[indexOrder[j]] > bookIssueCount[indexOrder[maxIndex]]) {
                maxIndex = j;
            }
        }
        if (maxIndex != i) {
            tmp = indexOrder[i];
            indexOrder[i] = indexOrder[maxIndex];
            indexOrder[maxIndex] = tmp;
        }
    }

    limit = (bookCount < 5) ? bookCount : 5;

    printf("\nTop Issued Books:\n");
    for (i = 0; i < bookCount && shown < limit; i++) {
        int idx = indexOrder[i];
        if (bookActive[idx] == 1) {
            printf("%d. %s by %s - Issued %d time(s)\n",
                   shown + 1, bookTitles[idx], bookAuthors[idx], bookIssueCount[idx]);
            shown++;
        }
    }

    if (shown == 0) {
        printf("No issue activity recorded yet.\n");
    }
}

/* =====================================================================
 * STATISTICS
 * ===================================================================== */

/* Displays overall library statistics: total/active books, total
 * copies, currently issued copies, registered users, and active
 * issue records. */
void show_statistics(void) {
    int i;
    int activeBooks = 0, totalCopies = 0, availableCopies = 0;
    int activeUsers = 0, activeIssues = 0;

    for (i = 0; i < bookCount; i++) {
        if (bookActive[i] == 1) {
            activeBooks++;
            totalCopies += bookTotalCopies[i];
            availableCopies += bookAvailableCopies[i];
        }
    }

    for (i = 0; i < userCount; i++) {
        if (userActive[i] == 1) {
            activeUsers++;
        }
    }

    for (i = 0; i < issueCount; i++) {
        if (issueActive[i] == 1) {
            activeIssues++;
        }
    }

    printf("\n--------- Library Statistics ---------\n");
    printf("Total Active Books (titles): %d\n", activeBooks);
    printf("Total Copies in Library:     %d\n", totalCopies);
    printf("Copies Currently Available:  %d\n", availableCopies);
    printf("Copies Currently Issued:     %d\n", totalCopies - availableCopies);
    printf("Registered Users:            %d\n", activeUsers);
    printf("Active Issue Records:        %d\n", activeIssues);
    printf("---------------------------------------\n");
}

/* =====================================================================
 * SAMPLE / DEMO DATA
 * ===================================================================== */

/* Loads a small, fixed set of sample books and users so the menu
 * options can be tested immediately without manual data entry. */
void load_sample_data(void) {
    struct { const char *title; const char *author; const char *category; int copies; } sample[] = {
        {"The C Programming Language", "Dennis Ritchie", "Programming", 3},
        {"Introduction to Algorithms", "Thomas Cormen",  "Computer Science", 2},
        {"Clean Code", "Robert Martin", "Programming", 4},
        {"A Brief History of Time", "Stephen Hawking", "Science", 2},
        {"The Alchemist", "Paulo Coelho", "Fiction", 5}
    };
    int n = (int)(sizeof(sample) / sizeof(sample[0]));
    int i;

    for (i = 0; i < n && bookCount < MAX_BOOKS; i++) {
        bookIds[bookCount] = nextBookId++;
        strcpy(bookTitles[bookCount], sample[i].title);
        strcpy(bookAuthors[bookCount], sample[i].author);
        strcpy(bookCategories[bookCount], sample[i].category);
        bookTotalCopies[bookCount] = sample[i].copies;
        bookAvailableCopies[bookCount] = sample[i].copies;
        bookIssueCount[bookCount] = 0;
        bookActive[bookCount] = 1;
        bookCount++;
    }

    if (userCount < MAX_USERS) {
        register_user(nextUserId++, "Aarav Sharma");
    }
    if (userCount < MAX_USERS) {
        register_user(nextUserId++, "Priya Verma");
    }

    printf("Sample data loaded: %d books, %d users.\n", n, userCount);
}

/* Quickly adds several generic demo books in a loop, used to test
 * the system with a larger dataset (e.g. for sorting demonstrations). */
void bulk_add_demo(void) {
    int count, i;

    printf("How many demo books do you want to add? ");
    while (scanf("%d", &count) != 1 || count < 1) {
        printf("Invalid input. Enter a positive number: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    for (i = 0; i < count && bookCount < MAX_BOOKS; i++) {
        bookIds[bookCount] = nextBookId++;
        sprintf(bookTitles[bookCount], "Demo Book %d", bookIds[bookCount]);
        sprintf(bookAuthors[bookCount], "Demo Author %d", bookIds[bookCount]);
        strcpy(bookCategories[bookCount], "Demo");
        bookTotalCopies[bookCount] = 1;
        bookAvailableCopies[bookCount] = 1;
        bookIssueCount[bookCount] = 0;
        bookActive[bookCount] = 1;
        bookCount++;
    }

    printf("%d demo book(s) added.\n", i);
}

/* =====================================================================
 * HELP MENU
 * ===================================================================== */

/* Prints a short explanation of every menu option for new users. */
void show_help(void) {
    printf("\n--------------------- HELP ---------------------\n");
    printf("1.  Add Book            - Add a new book to the library\n");
    printf("2.  Display All Books   - View all active books\n");
    printf("3.  Search by Title     - Find books by title keyword\n");
    printf("4.  Search by Author    - Find books by author keyword\n");
    printf("5.  Delete Book by ID   - Remove a book (soft delete)\n");
    printf("6.  Edit Book Details   - Update an existing book's info\n");
    printf("7.  List by Category    - View books in a category\n");
    printf("8.  Issue Book          - Issue a book to a user\n");
    printf("9.  Return Book         - Return a previously issued book\n");
    printf("10. List Current Issues - View all currently issued books\n");
    printf("11. Search Issues by User - View a user's issue history\n");
    printf("12. Sort by Title       - Sort books alphabetically by title\n");
    printf("13. Sort by Author      - Sort books alphabetically by author\n");
    printf("14. Top Issued Books    - View most frequently issued books\n");
    printf("15. Library Statistics  - View overall library statistics\n");
    printf("16. Load Sample Data    - Load ready-made sample records\n");
    printf("17. Bulk Add Demo       - Quickly add multiple demo books\n");
    printf("18. Help                - Show this help menu\n");
    printf("0.  Exit                - Close the program\n");
    printf("--------------------------------------------------\n");
}

/* =====================================================================
 * MAIN MENU / PROGRAM ENTRY POINT
 * ===================================================================== */

/* Displays the main menu and reads the user's numeric choice.
 * Repeats until a valid integer is entered. */
int main_menu(void) {
    int choice;

    printf("\n===================================================\n");
    printf("           LIBRARY MANAGEMENT SYSTEM\n");
    printf("===================================================\n");
    printf(" 1. Add Book\n");
    printf(" 2. Display All Books\n");
    printf(" 3. Search Book by Title\n");
    printf(" 4. Search Book by Author\n");
    printf(" 5. Delete Book by ID\n");
    printf(" 6. Edit Book Details\n");
    printf(" 7. List Books by Category\n");
    printf(" 8. Issue Book\n");
    printf(" 9. Return Book\n");
    printf("10. List Current Issues\n");
    printf("11. Search Issues by User\n");
    printf("12. Sort Books by Title\n");
    printf("13. Sort Books by Author\n");
    printf("14. Show Top Issued Books\n");
    printf("15. Show Library Statistics\n");
    printf("16. Load Sample Data\n");
    printf("17. Bulk Add Demo\n");
    printf("18. Help Menu\n");
    printf(" 0. Exit\n");
    printf("===================================================\n");
    printf("Enter your choice: ");

    while (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number: ");
        clear_input_buffer();
    }
    clear_input_buffer();

    return choice;
}

int main(void) {
    int choice;

    printf("Welcome to the Library Management System!\n");

    do {
        choice = main_menu();

        switch (choice) {
            case 1:  add_book_interactive(); break;
            case 2:  display_all_books(); break;
            case 3:  search_by_title(); break;
            case 4:  search_by_author(); break;
            case 5:  delete_book_by_id(); break;
            case 6:  edit_book_details(); break;
            case 7:  list_by_category(); break;
            case 8:  issue_book(); break;
            case 9:  return_book(); break;
            case 10: list_current_issues(); break;
            case 11: issues_by_user(); break;
            case 12: sort_books_by_title(); break;
            case 13: sort_books_by_author(); break;
            case 14: show_top_issued(); break;
            case 15: show_statistics(); break;
            case 16: load_sample_data(); break;
            case 17: bulk_add_demo(); break;
            case 18: show_help(); break;
            case 0:  printf("Thank you for using the Library Management System. Goodbye!\n"); break;
            default: printf("Invalid choice. Please select a valid menu option.\n"); break;
        }

    } while (choice != 0);

    return 0;
}
