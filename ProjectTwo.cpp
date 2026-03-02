// ProjectTwo.cpp : This file contains the 'main' function at line 278. Program execution begins and ends there.
// Ryan Kishi
// CS 300 Analysis and Design
// 2/22/26


// preprocessors
#include <algorithm>
#include <cctype>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// call standard library components
using namespace std;


// data structure
struct Course {
    string number; // example, "CS300"
    string title; // example, "Analysis and Design"
    vector<string> prereqNumbers; // example, ["CS255", "MAT225"]
};

// BST for Course storage and sorting
class CourseBST {
private:
    struct Node {
        Course data; // course data stored
        std::unique_ptr<Node> left; // left subtree
        std::unique_ptr<Node> right; // right subtree

        explicit Node(const Course& c) : data(c) {}
    };

    std::unique_ptr<Node> root; // BST root

    static void insertNode(std::unique_ptr<Node>& node, const Course& course) {
        if (!node) { // if the node is null create a node
            node = std::make_unique<Node>(course);
            return;
        }
        if (course.number < node->data.number) { // if the course.number < node recurse to the left
            insertNode(node->left, course);
        }
        else if (course.number > node->data.number) { // if the course number > node recurse to the right
            insertNode(node->right, course);
        }
        else {
            // necessary for preventing duplicate keys
        }
    }

    // print function which starts from the leftmost(smallest node) continuing to the right(largest node)
    static void inOrderPrint(const Node* node) { 
        if (!node) return;
        inOrderPrint(node->left.get());
        cout << node->data.number << ", " << node->data.title << endl;
        inOrderPrint(node->right.get());
    }
    // search function to find course and return data (==) within tree or null if not able to be found
    static const Course* searchNode(const Node* node, const string& courseNumber) {
        if (!node) 
            return nullptr; // not found returns null
        if (courseNumber == node->data.number) 
            return &node->data; // if found returns the node data
        if (courseNumber < node->data.number) //traversal
            return searchNode(node->left.get(), courseNumber); //traversal 
        return searchNode(node->right.get(), courseNumber); //traversal
    }

public:
    // for clearing all nodes from the tree
    void clear() { 
        root.reset(); 
    }

    // for recursion helper to make sure the course.number is in the correct form
    void insert(const Course& course) { 
        insertNode(root, course); 
    }

    // for printing the entire tree in sorted order rather than called individually from menu
    void printSortedList() const { 
        inOrderPrint(root.get()); 
    }

    // for calling course by number or returning null 
    const Course* find(const string& courseNumber) const { 
        return searchNode(root.get(), courseNumber); 
    }

    // for checking if tree is empty
    bool empty() const { return root == nullptr; }
};


// utility helpers (input normalization, tokenization)

static inline string trim(const string& s) { // remove leading and trailing whitespace in data
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) 
        start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) 
        end--;
    return s.substr(start, end - start);
}

static inline string toUpper(string s) { // normalize string to uppercase ex. in case of lowercase input
    for (char& ch : s) 
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return s;
}

// splitCSV data cell function using vector
// entire string is treated as a field, while each indiviual string is a cell
// function trims the whitespace from each cell
// then stores each and pushes each cleaned cell
// the function returns the entire field
static vector<string> splitCSV(const string& line) {
    vector<string> fields;
    string cell;

    for (char ch : line) {
        if (ch == ',') {
            fields.push_back(trim(cell));
            cell.clear();
        }
        else {
            cell.push_back(ch);
        }
    }
    fields.push_back(trim(cell));
    return fields;
}

// loading + validation
struct LoadResult {
    bool success = false; // success variable
    string errorMessage; // contains error
};

// reads file -> builds:
//  1. registry map for validation + lookup
//  2. BST for sorted printing
static LoadResult loadCoursesFromFile(const string& filePath,
    CourseBST& tree,
    std::map<string, Course>& registry) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return { false, "Could not open file: " + filePath }; // returns message if file cannot be opened
    }

    tree.clear();
    registry.clear();
    // clearing allows the load and sort to start from scratch if the file loads wrong or improperly

    string line;
    int lineNumber = 0;

    // pass 1. parse + build registry + insert into BST
    // # changed variable to cooked to differ from previous data variables (field, cell) for clarity
    while (std::getline(file, line)) {
        lineNumber++;
        string cooked = trim(line);

        //skip empty lines or lines beginning with #
        if (cooked.empty()) continue; 
        if (!cooked.empty() && cooked[0] == '#') continue;

        // convert to 'tokens'
        vector<string> tokens = splitCSV(cooked); 

        // format rule: at least 2 tokens course number + title
        if (tokens.size() < 2) {
            return { false, "Format error on line " + std::to_string(lineNumber) +
                               ": expected at least 2 fields (course number and title)." };
        }

        Course c;
        c.number = toUpper(trim(tokens[0])); // normalize tokens to uppercase
        c.title = trim(tokens[1]);

        if (c.number.empty() || c.title.empty()) { // redundancy check if format error
            return { false, "Format error on line " + std::to_string(lineNumber) +
                               ": course number/title cannot be empty." };
        }

        // collect prereqs (if any exist)
        for (size_t i = 2; i < tokens.size(); i++) {
            string prereq = toUpper(trim(tokens[i]));
            if (!prereq.empty()) c.prereqNumbers.push_back(prereq);
        }

        // duplicate detection
        if (registry.find(c.number) != registry.end()) { // returns an error message if duplicate string data exists
            return { false, "Format error on line " + std::to_string(lineNumber) +
                               ": duplicate course number '" + c.number + "'." };
        }

        registry[c.number] = c;
        tree.insert(c);
        // saves to registry and to BST for search and print functions
    }

    // pass 2. validate prereq existence 
    for (const auto& kv : registry) {
        const Course& c = kv.second;
        for (const string& prereqNum : c.prereqNumbers) {
            if (prereqNum == c.number) { // if the prereq number is the same as the course number return false with error
                return { false, "Validation error: course '" + c.number +
                                   "' lists itself as a prerequisite." };
            }
            if (registry.find(prereqNum) == registry.end()) { // if the prereq number is the same as thre registry number return false with error
                return { false, "Validation error: course '" + c.number +
                                   "' has missing prerequisite '" + prereqNum + "'." };
            }
        }
    }

    return { true, "" };
}

// printing course detail (option 3)
static void printCourseInfo(const string& queryNumber,
    const CourseBST& tree,
    const std::map<string, Course>& registry) {
    string key = toUpper(trim(queryNumber)); // normalize input query to uppercase using toUpper function. input format must match stored keys
    const Course* course = tree.find(key);

    if (!course) { // if not found return error message
        cout << "Course not found: " << key << endl;
        return;
    }

    cout << course->number << ", " << course->title << endl; // prints course header

    if (course->prereqNumbers.empty()) { // prints prereqs none if prereqNumbers is empty
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites:" << endl;
    for (const string& prereqNum : course->prereqNumbers) {
        auto it = registry.find(prereqNum);
        if (it != registry.end()) { // prints prereqs from registry if they exist
            cout << "  " << it->second.number << ", " << it->second.title << endl;
        }
        else {
            // defensive redundancy incase loader validates incorrectly to avoid loop/crash
            cout << "  " << prereqNum << " (title unavailable)" << endl;
        }
    }
}

// menu + input handling
// 
// menu display
static void printMenu() {
    cout << endl;
    cout << "****************************************" << endl;
    cout << "1. Load Data Structure" << endl;
    cout << "2. Print Course List" << endl;
    cout << "3. Print Course" << endl;
    cout << "9. Exit" << endl;
    cout << "****************************************" << endl;
    cout << "Enter choice (1, 2, 3, or 9): ";
}

// MAIN
// contains input functionality
int main() {
    CourseBST tree;
    std::map<string, Course> registry;

    bool loaded = false;

    cout << "ABCU Course Advising Tool" << endl;

    int choice = 0;
    while (choice != 9) { // while not equal to 9 (exit) print the menu
        printMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // necessary to avoid cin output leftover stacking
            // ** changed from defined numeric_limits 10000
            // experienced a frustrating bug where option 3 would not function after first time input and printing
            // likely due to std::cin leftover or blank space inputs stacking

            cout << "Invalid input. Please enter a number from the menu." << endl;
            continue;
        }

        // ** also changed from defined numeric_limits 10000
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear rest of line

        if (choice == 1) {
            const std::string defaultFile = "CS 300 ABCU_Advising_Program_Input.csv"; // default data file "(name)".csv

            // option to press enter with no input to select the default file name or enter in a (filename).csv
            // defined default file, but left option to enter in another
            std::cout << "Enter the filename (press Enter for " << defaultFile << " if contained in source folder)\n ";
            std::cout << "Or type a path: ";

            std::string filePath;
            std::getline(std::cin, filePath);

            if (filePath.empty()) {
                filePath = defaultFile; // user accepted the default .csv
            }

            LoadResult result = loadCoursesFromFile(filePath, tree, registry);
            if (!result.success) {
                std::cout << result.errorMessage << "\n"; //print error message if load fails
            }
            else {
                std::cout << "Courses loaded successfully.\n"; // print success if load succeeds
                loaded = true;
            }
        }
        else if (choice == 2) {
            if (!loaded) {
                cout << "Please load data first (Option 1)." << endl; //print error if file data is not loaded
                continue;
            }
            cout << endl;
            tree.printSortedList(); // outputs printed list of cleaned and sorted class list data

        }
        else if (choice == 3) {
            if (!loaded) {
                cout << "Please load data first (Option 1)." << endl; //print error if file data is not loaded
                continue;
            }
            string courseNum;
            cout << "Enter the course number: ";
            std::getline(cin, courseNum); // gets user input for course number

            if (courseNum.empty()) { // blank input check
                cout << "Course number cannot be blank. Enter the course number: "; 
                std::getline(cin, courseNum);
            }

            printCourseInfo(courseNum, tree, registry); // outputs print list of course and prereqs

        }
        else if (choice == 9) { // initiates program end with goodbye message
            cout << "Goodbye." << endl;

        }
        else {
            cout << "Invalid option. Please choose 1, 2, 3, or 9." << endl; // invalid entry redundancy message
        }
    }

    return 0;
}