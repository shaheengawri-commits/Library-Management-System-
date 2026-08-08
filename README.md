# Library Management System

A console-based **Library Management System** developed in **C**, built to manage books, users, and book issue/return records. The project uses parallel arrays for data storage and demonstrates core C programming concepts such as functions, string handling, searching, and sorting.

## Features

- Add Book
- Display All Books
- Search Book by Title
- Search Book by Author
- Delete Book by ID
- Edit Book Details
- List Books by Category
- Issue Book
- Return Book
- List Current Issues
- Search Issues by User
- Sort Books by Title
- Sort Books by Author
- Show Top Issued Books
- Show Library Statistics
- Load Sample Data
- Bulk Add Demo
- Help Menu
- Menu-driven console interface

## Technologies Used

- C
- Standard C libraries (`stdio.h`, `string.h`, `ctype.h`)
- Arrays
- Functions
- Strings
- Searching
- Sorting

## Concepts Demonstrated

This project demonstrates the following core programming concepts:

- Arrays
- Parallel arrays
- Functions
- Loops
- Conditional statements
- String handling
- Searching
- Bubble Sort
- Selection Sort
- Input validation
- Menu-driven programming

## Data Management

The program stores all data in memory using **parallel arrays** (not structures, not files, not a database):

- **Book information** — ID, title, author, category, total copies, available copies, and issue count are each stored in their own array, all indexed by the same position.
- **User information** — ID and name are stored in parallel arrays. A new user is registered automatically the first time an unknown User ID is used to issue a book.
- **Issue information** — Every issue record stores an issue ID, the associated Book ID, the associated User ID, and an active flag indicating whether the book is still issued or has been returned.

### Active / Inactive Flags

Instead of physically removing a record from its array (which would require shifting every other element), the program marks records as **inactive** using a flag (`bookActive`, `userActive`, `issueActive`). Deleted books and returned issues are simply skipped wherever the program searches, displays, or processes records. This keeps the array-based logic simple while still supporting "deletion" and "return" behavior.

## Sorting

- `sort_books_by_title()` — Sorts all books alphabetically by title using **Bubble Sort** (case-insensitive comparison).
- `sort_books_by_author()` — Sorts all books alphabetically by author using **Bubble Sort** (case-insensitive comparison).
- `show_top_issued()` — Ranks books by how many times they've been issued using a **Selection Sort** approach on a temporary index array, then displays the top results without altering the original book order.

## Main Functions

| Function | Description |
|---|---|
| `add_book_interactive()` | Prompts the user for details and adds a new book |
| `display_all_books()` | Displays all active books in a formatted table |
| `search_by_title()` | Case-insensitive search for books by title keyword |
| `search_by_author()` | Case-insensitive search for books by author keyword |
| `delete_book_by_id()` | Soft-deletes a book by marking it inactive |
| `edit_book_details()` | Edits an existing book's title, author, category, or copy count |
| `list_by_category()` | Lists all active books in a given category |
| `issue_book()` | Issues a book to a user (auto-registers new users) |
| `return_book()` | Returns a previously issued book |
| `list_current_issues()` | Lists all currently issued (not yet returned) books |
| `issues_by_user()` | Shows the full issue history for a given User ID |
| `sort_books_by_title()` | Sorts books by title using Bubble Sort |
| `sort_books_by_author()` | Sorts books by author using Bubble Sort |
| `show_top_issued()` | Shows the most frequently issued books using Selection Sort |
| `show_statistics()` | Displays overall library statistics |
| `main_menu()` | Displays the menu and reads the user's choice |
| `main()` | Program entry point; runs the menu loop |

## How to Run

### Compile with GCC

```bash
gcc library_management.c -o library_management
```

### Run

**Windows:**

```bash
library_management.exe
```

**Linux/macOS:**

```bash
./library_management
```

You can also open and run this project using **VS Code** (with the C/C++ extension and a configured GCC compiler) or any other standard C compiler/IDE.

## Program Flow

```text
Start Program
      ↓
Initialize Arrays
      ↓
Display Main Menu
      ↓
User Selects Option
      ↓
Execute Selected Function
      ↓
Return to Main Menu
      ↓
Exit
```

## Limitations

- Console-based interface only
- Uses arrays for storage (no structures)
- No database
- No file persistence — data is **not** saved between runs
- All data is lost when the program terminates
- Fixed maximum limits based on `MAX_BOOKS`, `MAX_USERS`, and `MAX_ISSUES`

## Future Improvements

The following are **not yet implemented** and are listed only as possible future work:

- File handling for persistent storage
- Database integration
- Graphical User Interface (GUI)
- Login/authentication system
- Better record management (e.g. using structures)
- Dynamic memory allocation

## Learning Outcomes

Working on this project helps reinforce the following fundamentals of C programming:

- Modular programming using functions
- Array-based data management with parallel arrays
- Implementing searching and sorting algorithms from scratch
- String manipulation and case-insensitive comparisons
- Designing a menu-driven console application
- General problem-solving skills in C

## Author

Developed as an academic/project implementation in C.
