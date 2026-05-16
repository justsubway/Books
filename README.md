# Library Management System (Δομές Δεδομένων)

Console application in C++ that manages a library catalog: books, physical copies, borrowing, returns, and persistence to a text file. Built with **linked lists** (no STL containers for the core data structures).

## Features

- Load and save the catalog from `library.txt`
- Add, search (by ISBN), list, and delete books
- Manage copies per book (`available`, `borrowed`, `damaged`)
- Borrow and return copies with condition tracking on return
- List available copies for a given book

Books are kept in **alphabetical order by title**. Each book owns a linked list of copies.

## Data structures

| Class       | Role                                      |
|------------|-------------------------------------------|
| `CopyNode` | One physical copy (ID, status)            |
| `BookNode` | Title, author, ISBN, list of copies       |
| `Library`  | Linked list of books (head: `booksHead`)  |

## Build & run

Requires a C++ compiler with C++11 or later (e.g. `g++`).

```bash
g++ -std=c++11 -o library library.cpp
./library
```

On startup, choose **1** to load `library.txt` from the same directory.

## File format (`library.txt`)

Pipe-separated lines:

```
BOOK|<ISBN>|<title>|<author>
COPY|<copy_id>|<status>
```

`status` is one of: `available`, `borrowed`, `damaged`.

## Documentation

Full design explanation, complexity analysis, and walkthrough (Greek): **[explanation.docx](explanation.docx)** — download from the repository root.

## Menu (summary)

| # | Action                          |
|---|---------------------------------|
| 1 | Load from file                  |
| 2 | Save to file                    |
| 3 | List all books                  |
| 4 | Search by ISBN                  |
| 5 | Add book                        |
| 6 | Delete book                     |
| 7 | Add copy                        |
| 8 | Remove copy                     |
| 9 | Borrow copy                     |
| 10| Return copy                     |
| 11| List available copies           |
| 0 | Exit (optional save)            |

## License

Academic coursework — see course materials for usage terms.
