/* Name: main.c++
 * Description: C++ program that keeps track of a busy CEO's email.
 * Inputs: file
 * Output: terminal output between user and program
 * Collaborators: N/A
 * Sources: Gemini
 * Author: Wyatt Sullivan
 * Creation date: 10/1/2026
 * Revision date: 10/1/2026
 */
#include <iostream>  // Includes the standard input-output stream library for printing output to terminal
#include <fstream>   // Includes the file stream library to read commands from test.txt
#include <sstream>   // Includes the string stream library for parsing comma-delimited string fields
#include <string>    // Includes the std::string class for string manipulation operations
#include <algorithm> // Includes algorithmic helpers like std::swap
#include <cctype>    // Includes character testing utilities like std::isdigit

// Helper function to remove leading and trailing whitespace/newlines from a string
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n"); // Finds the index of the first non-whitespace character
    if (start == std::string::npos) return "";      // Returns an empty string if the line contains only whitespace
    size_t end = str.find_last_not_of(" \t\r\n");    // Finds the index of the last non-whitespace character
    return str.substr(start, end - start + 1);       // Extracts and returns the trimmed substring
}

// Class representing an individual Email record
class Email {
private:
    std::string senderCategory; // Holds sender priority tier (e.g., "Boss", "Subordinate")
    std::string subject;        // Holds the subject text of the email
    std::string date;           // Holds the date string formatted as MM-DD-YYYY

    // Converts date string (MM-DD-YYYY) to integer (YYYYMMDD) for numerical comparison
    int getFormattedDate() const {
        if (date.length() != 10) return 0;            // Guards against invalid string lengths
        int month = std::stoi(date.substr(0, 2));     // Extracts two-digit month and converts to integer
        int day = std::stoi(date.substr(3, 2));       // Extracts two-digit day and converts to integer
        int year = std::stoi(date.substr(6, 4));      // Extracts four-digit year and converts to integer
        return year * 10000 + month * 100 + day;      // Computes single composite integer value YYYYMMDD
    }

    // Assigns integer priority score to sender category (Higher value = higher priority)
    int getCategoryPriority() const {
        if (senderCategory == "Boss") return 5;            // Highest priority category
        if (senderCategory == "Subordinate") return 4;     // Second highest priority category
        if (senderCategory == "Peer") return 3;            // Third highest priority category
        if (senderCategory == "ImportantPerson") return 2; // Fourth priority category
        if (senderCategory == "OtherPerson") return 1;     // Lowest priority category
        return 0;                                          // Fallback priority for unknown categories
    }

public:
    // Default constructor initializing empty email fields
    Email() : senderCategory(""), subject(""), date("") {}

    // Parameterized constructor populating email fields
    Email(std::string s, std::string sub, std::string d)
        : senderCategory(s), subject(sub), date(d) {}

    std::string getSender() const { return senderCategory; } // Getter for sender category
    std::string getSubject() const { return subject; }       // Getter for email subject
    std::string getDate() const { return date; }             // Getter for email date

    // Overloaded strictly-less-than operator to evaluate heap priority order
    bool operator<(const Email& other) const {
        // If categories differ, compare category priorities directly
        if (getCategoryPriority() != other.getCategoryPriority()) {
            return getCategoryPriority() < other.getCategoryPriority(); // Returns true if this email has lower category rank
        }
        // If categories are equal, Sprint rule dictates newest date has higher priority
        return getFormattedDate() < other.getFormattedDate(); // Returns true if this email is older than the other
    }

    // Overloaded greater-than operator defined in terms of the less-than operator
    bool operator>(const Email& other) const {
        return other < *this; // Delegates logic to operator<
    }
};

// HeapNode class representing an element in a doubly-linked list MaxHeap implementation
class HeapNode {
public:
    Email data;      // Holds the Email object payload
    HeapNode* next;  // Pointer to the next node in the list
    HeapNode* prev;  // Pointer to the previous node in the list

    // Node constructor initializing payload and setting pointers to null
    HeapNode(Email e) : data(e), next(nullptr), prev(nullptr) {}
};

// MaxHeap class implemented via a list-based data structure
class ListMaxHeap {
private:
    HeapNode* head; // Pointer to the head node (index 0 / root of heap)
    int size;       // Current count of nodes in the heap

    // Traverses linked list to fetch pointer to node at target index
    HeapNode* getNodeAt(int index) const {
        HeapNode* curr = head;                           // Starts traversal at head pointer
        for (int i = 0; i < index && curr != nullptr; ++i) { // Iterates until target index is reached
            curr = curr->next;                           // Advances node pointer forward
        }
        return curr;                                     // Returns node pointer at requested index
    }

    // Restores MaxHeap invariant upwards from inserted element index
    void heapifyUp(int index) {
        while (index > 0) {                                    // Loop until root node index is reached
            int parentIndex = (index - 1) / 2;                 // Computes parent node index in complete binary tree
            HeapNode* childNode = getNodeAt(index);            // Obtains pointer to current child node
            HeapNode* parentNode = getNodeAt(parentIndex);     // Obtains pointer to parent node

            // If parent email priority is less than child email priority, swap payload
            if (parentNode->data < childNode->data) {
                std::swap(parentNode->data, childNode->data);  // Swaps email contents between nodes
                index = parentIndex;                           // Updates current working index to parent index
            } else {
                break;                                         // Heap property satisfied; terminate loop
            }
        }
    }

    // Restores MaxHeap invariant downwards from root node index
    void heapifyDown(int index) {
        while (true) {                                         // Loop until heap order is restored
            int leftIndex = 2 * index + 1;                     // Formula for left child index
            int rightIndex = 2 * index + 2;                    // Formula for right child index
            int largest = index;                               // Tracking index for node with max priority

            HeapNode* largestNode = getNodeAt(largest);        // Fetch node corresponding to current largest
            HeapNode* leftNode = getNodeAt(leftIndex);          // Fetch left child node pointer
            HeapNode* rightNode = getNodeAt(rightIndex);        // Fetch right child node pointer

            // Check if left child exists and has higher priority than current largest
            if (leftNode != nullptr && largestNode->data < leftNode->data) {
                largest = leftIndex;                           // Update index of largest priority element
                largestNode = leftNode;                        // Update node pointer of largest element
            }
            // Check if right child exists and has higher priority than current largest
            if (rightNode != nullptr && largestNode->data < rightNode->data) {
                largest = rightIndex;                          // Update index of largest priority element
            }

            // If a child node had higher priority, perform payload swap and continue down
            if (largest != index) {
                HeapNode* currNode = getNodeAt(index);         // Fetch current node pointer
                HeapNode* targetNode = getNodeAt(largest);     // Fetch target child node pointer to swap with
                std::swap(currNode->data, targetNode->data);   // Swap email payloads
                index = largest;                               // Continue heapifyDown at child's index
            } else {
                break;                                         // Heap property satisfied; terminate loop
            }
        }
    }

public:
    // Heap constructor initializing empty list state
    ListMaxHeap() : head(nullptr), size(0) {}

    // Destructor to prevent memory leaks by freeing all dynamically allocated nodes
    ~ListMaxHeap() {
        while (head != nullptr) {     // Iterate while elements remain in list
            HeapNode* temp = head;    // Save reference to current head
            head = head->next;        // Advance head to next node
            delete temp;              // Free memory of old head node
        }
    }

    // Inserts a new email into the list-based MaxHeap
    void insert(Email email) {
        HeapNode* newNode = new HeapNode(email); // Dynamically allocates new node with email payload
        if (!head) {                             // If heap is currently empty
            head = newNode;                      // Set head to new node
        } else {
            HeapNode* tail = getNodeAt(size - 1); // Retrieve tail node pointer
            tail->next = newNode;                // Attach new node to tail's next pointer
            newNode->prev = tail;                // Set back-link pointer to previous tail
        }
        size++;                                  // Increment element counter
        heapifyUp(size - 1);                     // Restore heap order starting from inserted tail index
    }

    // Returns top email element with highest priority without removing it
    Email getMax() const {
        if (isEmpty()) return Email(); // Return blank Email object if heap is empty
        return head->data;             // Root node (head) always holds maximum priority email
    }

    // Removes root email element with highest priority from heap
    void removeMax() {
        if (isEmpty()) return; // Exit early if heap is empty

        HeapNode* lastNode = getNodeAt(size - 1); // Get pointer to last node in complete tree
        head->data = lastNode->data;             // Copy last node payload into root node

        if (lastNode->prev) {                    // If last node has a predecessor
            lastNode->prev->next = nullptr;      // Sever forward pointer from predecessor
        } else {
            head = nullptr;                      // Heap became empty; set head pointer to null
        }

        delete lastNode;                         // Free dynamic memory of extracted node
        size--;                                  // Decrement size count

        if (size > 0) {
            heapifyDown(0);                      // Restore heap order starting from root index 0
        }
    }

    int getSize() const { return size; }  // Returns count of active unread emails
    bool isEmpty() const { return size == 0; } // Returns true if heap contains no emails
};

// Manager class handling CEO command execution and input validation logic
class CEOEmailManager {
private:
    ListMaxHeap priorityQueue; // Internal MaxHeap priority queue instance

    // Validates if date string conforms strictly to MM-DD-YYYY structure
    bool isValidDate(const std::string& date) const {
        if (date.length() != 10) return false;             // Returns false if length is not exactly 10
        if (date[2] != '-' || date[5] != '-') return false; // Returns false if hyphen delimiters are missing
        for (int i = 0; i < 10; ++i) {                     // Loop over characters
            if (i == 2 || i == 5) continue;                // Skip checking hyphen positions
            if (!std::isdigit(date[i])) return false;      // Return false if non-numeric character found
        }
        return true;                                       // Date string valid
    }

public:
    // Processes raw text input line representing CEO execution commands
    void processCommand(const std::string& rawLine) {
        std::string line = trim(rawLine); // Remove whitespace/carriage returns

        // Guardrail 1: Ignore blank or whitespace-only lines
        if (line.empty()) {
            return;
        }

        // Guardrail 2: Process EMAIL command string
        if (line.rfind("EMAIL", 0) == 0) {
            // Ensure proper delimiter space follows command identifier
            if (line.length() <= 6 || line[5] != ' ') {
                std::cerr << "[Warning] Skipping malformed command syntax: " << line << "\n";
                return;
            }

            std::string payload = line.substr(6); // Extract fields after "EMAIL " prefix
            std::stringstream ss(payload);        // Wrap payload in stringstream for split operations
            std::string category, subject, date;  // Declare variables for parsed fields

            // Extract comma-delimited tokens
            bool hasCategory = static_cast<bool>(std::getline(ss, category, ','));
            bool hasSubject  = static_cast<bool>(std::getline(ss, subject, ','));
            bool hasDate     = static_cast<bool>(std::getline(ss, date, ','));

            category = trim(category); // Trim whitespace from extracted category string
            subject  = trim(subject);  // Trim whitespace from extracted subject string
            date     = trim(date);     // Trim whitespace from extracted date string

            // Guardrail 3: Check for missing required fields
            if (!hasCategory || !hasSubject || !hasDate || category.empty() || subject.empty() || date.empty()) {
                std::cerr << "[Warning] Skipping EMAIL with missing fields: " << line << "\n";
                return;
            }

            // Guardrail 4: Verify category matches allowed CEO priority tiers
            if (category != "Boss" && category != "Subordinate" && category != "Peer" &&
                category != "ImportantPerson" && category != "OtherPerson") {
                std::cerr << "[Warning] Skipping EMAIL with unknown category '" << category << "'\n";
                return;
            }

            // Guardrail 5: Verify date formatting
            if (!isValidDate(date)) {
                std::cerr << "[Warning] Skipping EMAIL with invalid date format '" << date << "'\n";
                return;
            }

            // Construct email object and insert into Priority Queue
            Email email(category, subject, date);
            priorityQueue.insert(email);
        }
        // Process COUNT command string
        else if (line == "COUNT") {
            std::cout << "There are " << priorityQueue.getSize() << " emails to read.\n"; // Print unread count
        }
        // Process NEXT command string
        else if (line == "NEXT") {
            if (!priorityQueue.isEmpty()) {              // Guardrail against empty queue reads
                Email top = priorityQueue.getMax();       // Fetch top priority email object
                std::cout << "Next email:\n";             // Terminal output header
                std::cout << "Sender: " << top.getSender() << "\n";  // Print sender category
                std::cout << "Subject: " << top.getSubject() << "\n"; // Print subject line
                std::cout << "Date: " << top.getDate() << "\n";       // Print email date
            }
        }
        // Process READ command string
        else if (line == "READ") {
            if (!priorityQueue.isEmpty()) {              // Guardrail against popping from empty queue
                priorityQueue.removeMax();                // Dequeue highest priority email from heap
            }
        }
        // Guardrail 6: Catch unrecognized or corrupted command inputs
        else {
            std::cerr << "[Warning] Unrecognized command encountered: " << line << "\n";
        }
    }
};

// Main execution entry point
int main() {
    CEOEmailManager manager;             // Instantiate controller manager object
    std::ifstream inputFile("test.txt"); // Open input file handle for test.txt

    // Ensure file exists and opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open test.txt\n"; // Print error log
        return 1;                                        // Exit program with failure code
    }

    std::string line; // Declare line buffer variable
    
    // Read test.txt line by line until end-of-file
    while (std::getline(inputFile, line)) {
        manager.processCommand(line); // Execute command processing logic for each line
    }

    inputFile.close(); // Close file input stream handle
    return 0;          // Return success code 0
}
