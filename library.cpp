#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// Διαχωρίζει μια γραμμή στα tokens της βάσει του διαχωριστικού delim. (Δοσμένο από εκφώνηση)
std::vector<std::string> parseLine(const std::string& line, char delim = '|') {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

class CopyNode { 
private: 
    int copyID;           
    std::string status;
    CopyNode* next; 

public: 
    // Ο constructor παίρνει by default το status "available" αν δεν δοθεί κάτι άλλο
    CopyNode(int id, std::string st = "available") {
        this->copyID = id;
        this->status = st;
        this->next = nullptr;
    }

    // Getters / Setters
    int getCopyID() { return copyID; }
    std::string getStatus() { return status; }
    CopyNode* getNext() { return next; }
    
    void setNext(CopyNode* nextNode) { this->next = nextNode; }
    void setStatus(std::string newStatus) { this->status = newStatus; }
};

class BookNode {
private: 
    std::string title;
    std::string author;
    std::string ISBN;
    CopyNode* copiesHead;
    BookNode* next;

public: 
    // Constructor του βιβλίου
    BookNode(std::string title, std::string author, std::string ISBN) {
        this->title = title;
        this->author = author;
        this->ISBN = ISBN;
        this->copiesHead = nullptr;
        this->next = nullptr;
    }

    // Καταστροφέας: Όταν σβήνεται ένα βιβλίο, πρέπει να διαγράφονται και τα αντίτυπά του
    ~BookNode() {
        CopyNode* current = copiesHead;
        while (current != nullptr) {
            CopyNode* nextNode = current->getNext(); // Κρατάμε τον επόμενο
            delete current;                          // Διαγράφουμε τον τωρινό
            current = nextNode;                      // Προχωράμε
        }
    }

    // Getters / Setters
    void setNext(BookNode* nextBook) { this->next = nextBook; }
    std::string getTitle() { return title; }
    std::string getAuthor() { return author; }
    std::string getISBN() { return ISBN; }
    CopyNode* getCopiesHead() { return copiesHead; }
    BookNode* getNext() { return next; }

    // ΛΕΙΤΟΥΡΓΙΕΣ ΑΝΤΙΤΥΠΩΝ
    void addCopy(int copyID, std::string status = "available") {
        // Δημιουργούμε τον νέο κόμβο αντιτύπου
        CopyNode* newCopy = new CopyNode(copyID, status);
    
        // Αν η λίστα αντιτύπων είναι άδεια ή το νέο αντίτυπο έχει μικρότερο ID από το πρώτο
        if (copiesHead == nullptr || newCopy->getCopyID() < copiesHead->getCopyID()) {
            newCopy->setNext(copiesHead);
            copiesHead = newCopy; 
        } 
        // Αν το αντίτυπο μπαίνει κάπου στη μέση ή στο τέλος
        else {
            CopyNode* current = copiesHead;
            // Προχωράμε όσο υπάρχει επόμενο και το ID του επόμενου είναι μικρότερο από το νέο ID
            while (current->getNext() != nullptr && current->getNext()->getCopyID() < newCopy->getCopyID()) {
                current = current->getNext();
            }
            // Βρήκαμε τη θέση και κάνουμε τη σύνδεση
            newCopy->setNext(current->getNext());
            current->setNext(newCopy);
        }
    }

    bool removeCopy(int searchID) {
        if (copiesHead == nullptr) return false;

        // Βρίσκουμε το αντίτυπο για να κάνουμε τους ελέγχους που ζητάει η εκφώνηση
        CopyNode* target = findCopy(searchID);
        if (target == nullptr) {
            std::cout << "Το αντίτυπο δεν βρέθηκε.\n";
            return false;
        }
        
        // Απαγορεύεται η διαγραφή αν είναι δανεισμένο
        if (target->getStatus() == "borrowed") {
            std::cout << "Δεν επιτρέπεται η διαγραφή δανεισμένου αντιτύπου.\n";
            return false;
        }

        // Μετράμε πόσα αντίτυπα έχει το βιβλίο. Απαγορεύεται να σβήσουμε το μοναδικό.
        int totalCopies = 0;
        CopyNode* temp = copiesHead;
        while (temp != nullptr) {
            totalCopies++;
            temp = temp->getNext();
        }
        if (totalCopies == 1) {
            std::cout << "Δεν επιτρέπεται να διαγράψετε το μοναδικό αντίτυπο ενός βιβλίου.\n";
            return false;
        }

        // Αν περάσαμε τους ελέγχους, κάνουμε τη διαγραφή
        if (copiesHead == target) {
            copiesHead = copiesHead->getNext(); // Πηδάμε τον πρώτο κόμβο
            delete target; 
            return true;
        }

        CopyNode* current = copiesHead;
        while (current->getNext() != target) {
            current = current->getNext();
        }
        
        // Γεφυρώνουμε το κενό και διαγράφουμε
        current->setNext(target->getNext());
        delete target;
        return true;
    }

    // Δανεισμός Αντιτύπου
    bool borrowCopy(int searchID) {
        CopyNode* copy = findCopy(searchID);
        // Επιστρέφει false αν δεν υπάρχει ή δεν είναι available
        if (copy != nullptr && copy->getStatus() == "available") {
            copy->setStatus("borrowed");
            return true;
        }
        return false; 
    }

    // Επιστροφή Αντιτύπου
    bool returnCopy(int searchID, std::string newStatus) {
        CopyNode* copy = findCopy(searchID);
        // Πρέπει να είναι borrowed για να επιστραφεί
        if (copy != nullptr && copy->getStatus() == "borrowed") {
            copy->setStatus(newStatus); // Θα γίνει "available" ή "damaged"
            return true;
        }
        return false;
    }

    int countAvailable() {
        int count = 0;
        CopyNode* current = copiesHead; 
        while (current != nullptr) {
            if (current->getStatus() == "available") {
                count++;
            }
            current = current->getNext();
        }
        return count;
    }

    // Ψάχνει ένα αντίτυπο βάσει ID. Αν το βρει επιστρέφει τον δείκτη, αλλιώς nullptr.
    CopyNode* findCopy(int searchID) {
        CopyNode* current = copiesHead;
        while (current != nullptr) {
            if (current->getCopyID() == searchID) {
                return current; 
            }
            current = current->getNext();
        }
        return nullptr; 
    }
};    

class Library {
private:
    BookNode* booksHead;

public:
    Library() {
        this->booksHead = nullptr;
    }

    // Καταστροφέας της Βιβλιοθήκης (Απελευθερώνει όλη τη μνήμη)
    ~Library() {
        clear();
    }

    // Συνάρτηση που αδειάζει τη βιβλιοθήκη. Χρήσιμη για τον καταστροφέα και τη φόρτωση αρχείου.
    void clear() {
        BookNode* current = booksHead;
        while (current != nullptr) {
            BookNode* nextNode = current->getNext();
            delete current; // Καταστρέφει το βιβλίο και τα αντίτυπά του 
            current = nextNode;
        }
        booksHead = nullptr;
    }

    // Ελέγχει αν η βιβλιοθήκη είναι άδεια
    bool isEmpty() {
        return booksHead == nullptr;
    }

    // ΛΕΙΤΟΥΡΓΙΕΣ ΒΙΒΛΙΩΝ

    void addBook(std::string title, std::string author, std::string ISBN) {
        BookNode* newBook = new BookNode(title, author, ISBN);

        // Αν η λίστα είναι άδεια ή το νέο βιβλίο μπαίνει πρώτο αλφαβητικά
        if (booksHead == nullptr || newBook->getTitle() < booksHead->getTitle()) {
            newBook->setNext(booksHead);
            booksHead = newBook; 
        } 
        // Αν το βιβλίο μπαίνει κάπου στη μέση ή στο τέλος
        else {
            BookNode* current = booksHead;
            while (current->getNext() != nullptr && current->getNext()->getTitle() < newBook->getTitle()) {
                current = current->getNext();
            }
            newBook->setNext(current->getNext());
            current->setNext(newBook);
        }
    }

    bool removeBook(std::string searchISBN) {
        if (booksHead == nullptr) return false;

        BookNode* target = findBook(searchISBN);
        if (target == nullptr) return false;

        // Απαγορεύεται η διαγραφή αν έστω και ένα αντίτυπο είναι "borrowed"
        CopyNode* currCopy = target->getCopiesHead();
        while (currCopy != nullptr) {
            if (currCopy->getStatus() == "borrowed") {
                std::cout << "Το βιβλίο έχει δανεισμένα αντίτυπα. Δεν μπορεί να διαγραφεί!\n";
                return false; // Σταματάμε τη διαγραφή
            }
            currCopy = currCopy->getNext();
        }

        // Περίπτωση 1: Το βιβλίο είναι το πρώτο
        if (booksHead == target) {
            booksHead = booksHead->getNext();
            delete target; 
            return true;
        }

        // Περίπτωση 2: Το βιβλίο είναι στη μέση ή στο τέλος
        BookNode* current = booksHead;
        while (current->getNext() != target) {
            current = current->getNext();
        }

        current->setNext(target->getNext()); // Γεφυρώνουμε το κενό
        delete target;                       // Διαγράφουμε τον κόμβο
        return true;
    }

    BookNode* findBook(std::string searchISBN) {
        BookNode* current = booksHead;
        while (current != nullptr) {
            if (current->getISBN() == searchISBN) {
                return current;
            }
            current = current->getNext();
        }
        return nullptr;
    }

    void printAll() {
        BookNode* current = booksHead;
        if (current == nullptr) {
            std::cout << "Η βιβλιοθήκη είναι άδεια.\n";
            return;
        }

        while(current != nullptr) {
            std::cout << "Τίτλος: " << current->getTitle() 
                      << " | Συγγραφέας: " << current->getAuthor() 
                      << " | ISBN: " << current->getISBN() << std::endl;
            
            int totalCopies = 0;
            int availableCopies = 0;
            
            CopyNode* copyCurrent = current->getCopiesHead();
            while(copyCurrent != nullptr) {
                totalCopies++; 
                if (copyCurrent->getStatus() == "available") {
                    availableCopies++;
                }
                copyCurrent = copyCurrent->getNext();
            }
            
            std::cout << "Διαθέσιμα αντίτυπα: " << availableCopies << " / Συνολικά: " << totalCopies << "\n" << std::endl;
            current = current->getNext();
        }
    }

    // ΔΙΑΧΕΙΡΙΣΗ ΑΡΧΕΙΩΝ 

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Δεν ήταν δυνατό το άνοιγμα του αρχείου " << filename << "\n";
            return;
        }

        std::string line;
        BookNode* lastAddedBook = nullptr; 

        while (std::getline(file, line)) {
            if (line.empty()) continue; 
            
            std::vector<std::string> tokens = parseLine(line);
            
            if (tokens[0] == "BOOK") {
                addBook(tokens[2], tokens[3], tokens[1]);
                lastAddedBook = findBook(tokens[1]); 
                
            } else if (tokens[0] == "COPY") {
                int copyId = std::stoi(tokens[1]); 
                std::string status = tokens[2];
                
                if (lastAddedBook != nullptr) {
                    lastAddedBook->addCopy(copyId, status);
                }
            }
        }
        file.close();
        std::cout << "Η βιβλιοθήκη φορτώθηκε επιτυχώς από το αρχείο!\n";
    }

    void saveToFile(const std::string& filename) {
        std::ofstream file(filename); 
        if (!file.is_open()) {
            std::cout << "Δεν ήταν δυνατό το άνοιγμα του αρχείου για αποθήκευση.\n";
            return;
        }

        BookNode* currentBook = booksHead;
        while (currentBook != nullptr) {
            file << "BOOK|" << currentBook->getISBN() << "|" 
                 << currentBook->getTitle() << "|" 
                 << currentBook->getAuthor() << "\n";
            
            CopyNode* currentCopy = currentBook->getCopiesHead();
            while (currentCopy != nullptr) {
                file << "COPY|" << currentCopy->getCopyID() << "|" 
                     << currentCopy->getStatus() << "\n";
                currentCopy = currentCopy->getNext();
            }
            currentBook = currentBook->getNext();
        }
        
        file.close();
        std::cout << "Η βιβλιοθήκη αποθηκεύτηκε επιτυχώς στο αρχείο!\n";
    }
};

int main() {
    Library myLib;
    int choice = -1;

    std::string title, author, isbn;
    int copyId;

    do {
        std::cout << "====== ΒΙΒΛΙΟΘΗΚΗ - ΚΕΝΤΡΙΚΟ MENU ======" << std::endl;
        std::cout << "1. Φόρτωση βιβλιοθήκης από αρχείο" << std::endl;
        std::cout << "2. Αποθήκευση βιβλιοθήκης σε αρχείο" << std::endl;
        std::cout << "3. Εμφάνιση όλων των βιβλίων" << std::endl;
        std::cout << "4. Αναζήτηση βιβλίου (με ISBN)" << std::endl;
        std::cout << "5. Προσθήκη νέου βιβλίου" << std::endl;
        std::cout << "6. Διαγραφή βιβλίου" << std::endl;
        std::cout << "7. Προσθήκη αντιτύπου σε βιβλίο" << std::endl;
        std::cout << "8. Διαγραφή αντιτύπου από βιβλίο" << std::endl;
        std::cout << "9. Δανεισμός αντιτύπου" << std::endl;
        std::cout << "10. Επιστροφή αντιτύπου" << std::endl;
        std::cout << "11. Εμφάνιση διαθέσιμων αντιτύπων βιβλίου" << std::endl;
        std::cout << "0. Έξοδος" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "Επιλογή: ";
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                std::cout << "\n--- ΦΟΡΤΩΣΗ ΒΙΒΛΙΟΘΗΚΗΣ ---" << std::endl;
                // Έλεγχος αν υπάρχει ήδη φορτωμένη βιβλιοθήκη όπως ζητάει η εκφώνηση
                if (!myLib.isEmpty()) {
                    char ans;
                    std::cout << "Υπάρχει ήδη φορτωμένη βιβλιοθήκη. Θέλετε να αντικατασταθεί; (Y/N): ";
                    std::cin >> ans;
                    if (ans == 'y' || ans == 'Y') {
                        myLib.clear(); // Διαγράφει την παλιά μνήμη
                    } else {
                        std::cout << "Ακύρωση φόρτωσης.\n";
                        break;
                    }
                }
                myLib.loadFromFile("library.txt");
                break;
            }
            case 2:
                std::cout << "\n--- ΑΠΟΘΗΚΕΥΣΗ ΒΙΒΛΙΟΘΗΚΗΣ ---" << std::endl;
                myLib.saveToFile("library.txt");
                break;
            case 3:
                std::cout << "\n--- ΛΙΣΤΑ ΒΙΒΛΙΩΝ ---" << std::endl;
                myLib.printAll();
                break;
            case 4: {
                std::cout << "\n--- ΑΝΑΖΗΤΗΣΗ ΒΙΒΛΙΟΥ ---" << std::endl;
                std::cout << "Δώστε ISBN: ";
                std::cin >> isbn;
                
                BookNode* book = myLib.findBook(isbn);
                if (book != nullptr) {
                    std::cout << "\nΤο βιβλίο βρέθηκε:" << std::endl;
                    std::cout << "Τίτλος: " << book->getTitle() << std::endl;
                    std::cout << "Συγγραφέας: " << book->getAuthor() << std::endl;
                    std::cout << "ISBN: " << book->getISBN() << std::endl;
                    
                    std::cout << "Λίστα Αντιτύπων:" << std::endl;
                    CopyNode* currentCopy = book->getCopiesHead();
                    if (currentCopy == nullptr) {
                        std::cout << "  (Δεν υπάρχουν αντίτυπα για αυτό το βιβλίο)" << std::endl;
                    } else {
                        while (currentCopy != nullptr) {
                            std::cout << "  - ID Αντιτύπου: " << currentCopy->getCopyID() 
                                      << " | Κατάσταση: " << currentCopy->getStatus() << std::endl;
                            currentCopy = currentCopy->getNext();
                        }
                    }
                    std::cout << std::endl;
                } else {
                    std::cout << "Το βιβλίο με ISBN " << isbn << " δεν βρέθηκε.\n" << std::endl;
                }
                break;
            }
            case 5: {
                std::cout << "\n--- ΠΡΟΣΘΗΚΗ ΝΕΟΥ ΒΙΒΛΙΟΥ ---" << std::endl;
                std::cout << "Δώστε τίτλο: ";
                std::cin >> std::ws; 
                std::getline(std::cin, title);
                std::cout << "Δώστε συγγραφέα: ";
                std::getline(std::cin, author);
                std::cout << "Δώστε ISBN: ";
                std::cin >> isbn; 
                
                if (myLib.findBook(isbn) == nullptr) {
                    myLib.addBook(title, author, isbn);
                    std::cout << "Το βιβλίο προστέθηκε επιτυχώς!\n" << std::endl;
                } else {
                    std::cout << "Το βιβλίο δεν προστέθηκε! Το ISBN πρέπει να είναι μοναδικό.\n" << std::endl;
                }
                break;
            }
            case 6: {
                std::cout << "\n--- ΔΙΑΓΡΑΦΗ ΒΙΒΛΙΟΥ ---" << std::endl;
                std::cout << "Δώστε το ISBN του βιβλίου προς διαγραφή: ";
                std::cin >> isbn;
                    
                if (myLib.removeBook(isbn)) {
                    std::cout << "Το βιβλίο και όλα τα αντίτυπά του διαγράφηκαν επιτυχώς!" << std::endl;
                } else {
                    std::cout << "Η διαγραφή δεν ολοκληρώθηκε.\n" << std::endl;
                }
                break;
            }
            case 7: {
                std::cout << "\n--- ΠΡΟΣΘΗΚΗ ΑΝΤΙΤΥΠΟΥ ---" << std::endl;
                std::cout << "Δώστε ISBN: ";
                std::cin >> isbn;
                
                BookNode* book = myLib.findBook(isbn);
                if (book == nullptr) {
                    std::cout << "Το βιβλίο δεν βρέθηκε." << std::endl;
                } else {
                    std::cout << "Δώστε ID αντιτύπου: ";
                    std::cin >> copyId;
                    if (book->findCopy(copyId) == nullptr) {
                        book->addCopy(copyId);
                        std::cout << "Το αντίτυπο προστέθηκε επιτυχώς!\n" << std::endl;
                    } else {
                        std::cout << "Το αντίτυπο με αυτό το ID υπάρχει ήδη!\n" << std::endl;
                    }
                }
                break;
            }
            case 8: {
                std::cout << "\n--- ΔΙΑΓΡΑΦΗ ΑΝΤΙΤΥΠΟΥ ---" << std::endl;
                std::cout << "Δώστε το ISBN του βιβλίου: ";
                std::cin >> isbn;
                
                BookNode* book = myLib.findBook(isbn);
                if (book != nullptr) {
                    std::cout << "Δώστε το ID του αντιτύπου προς διαγραφή: ";
                    std::cin >> copyId;
                    if (book->removeCopy(copyId)) {
                        std::cout << "Το αντίτυπο διαγράφηκε επιτυχώς!" << std::endl;
                    }
                } else {
                    std::cout << "Το βιβλίο δεν βρέθηκε." << std::endl;
                }
                break;
            }
            case 9: { 
                std::cout << "\n--- ΔΑΝΕΙΣΜΟΣ ΑΝΤΙΤΥΠΟΥ ---" << std::endl;
                std::cout << "Δώστε το ISBN του βιβλίου: ";
                std::cin >> isbn;
                
                BookNode* book = myLib.findBook(isbn);
                if (book != nullptr) {
                    std::cout << "Δώστε το ID του αντιτύπου: ";
                    std::cin >> copyId;
                    
                    // Καλούμε την έτοιμη συνάρτηση από την κλάση
                    if (book->borrowCopy(copyId)) {
                        std::cout << "Επιτυχία: Το αντίτυπο δανείστηκε! Διαθέσιμα πλέον: " << book->countAvailable() << std::endl;
                    } else {
                        std::cout << "Το αντίτυπο δεν είναι διαθέσιμο (ή δεν υπάρχει).\n" << std::endl; 
                    }
                } else {
                    std::cout << "Το βιβλίο δεν βρέθηκε." << std::endl;
                }
                break; 
            } 
            case 10:  { 
                std::cout << "\n--- ΕΠΙΣΤΡΟΦΗ ΑΝΤΙΤΥΠΟΥ ---" << std::endl;
                std::cout << "Δώστε το ISBN του βιβλίου: ";
                std::cin >> isbn;
                
                BookNode* book = myLib.findBook(isbn);
                if (book != nullptr) {
                    std::cout << "Δώστε το ID του αντιτύπου: ";
                    std::cin >> copyId;
                    
                    char condition;
                    std::cout << "Επιστράφηκε σε καλή κατάσταση; (y/n): ";
                    std::cin >> condition;
                    
                    std::string finalStatus = (condition == 'y' || condition == 'Y') ? "available" : "damaged";
                    
                    // Καλούμε την έτοιμη συνάρτηση από την κλάση
                    if (book->returnCopy(copyId, finalStatus)) {
                        std::cout << "Επιτυχία: Το αντίτυπο επιστράφηκε!\n" << std::endl;
                    } else {
                        std::cout << "Το αντίτυπο δεν είναι δανεισμένο (ή δεν υπάρχει).\n" << std::endl; 
                    }
                } else {
                    std::cout << "Το βιβλίο δεν βρέθηκε." << std::endl;
                }
                break; 
            } 
            case 11: { 
                std::cout << "\n--- ΔΙΑΘΕΣΙΜΑ ΑΝΤΙΤΥΠΑ ---" << std::endl;
                std::cout << "Δώστε το ISBN του βιβλίου: ";
                std::cin >> isbn;
                
                BookNode* book = myLib.findBook(isbn);
                if (book != nullptr) {
                    std::cout << "Λίστα διαθέσιμων αντιτύπων:" << std::endl;
                    CopyNode* c = book->getCopiesHead();
                    while (c != nullptr) {
                        if (c->getStatus() == "available") {
                            std::cout << "  - ID: " << c->getCopyID() << std::endl;
                        }
                        c = c->getNext();
                    }
                    std::cout << "Συνολικά διαθέσιμα: " << book->countAvailable() << "\n" << std::endl; 
                } else {
                    std::cout << "Το βιβλίο δεν βρέθηκε.\n" << std::endl;
                }
                break; 
            }
            case 0: {
                char saveChoice;
                std::cout << "Θέλετε να αποθηκεύσετε τις αλλαγές πριν την έξοδο; (y/n): ";
                std::cin >> saveChoice;
                if (saveChoice == 'y' || saveChoice == 'Y') {
                    myLib.saveToFile("library.txt");
                }
                std::cout << "Τερματισμός προγράμματος..." << std::endl;
                break;
            }
            default:
                std::cout << "Λανθασμένη επιλογή. Παρακαλώ δοκιμάστε ξανά.\n" << std::endl;
                break;
        }
    } while (choice != 0);
    return 0;
}