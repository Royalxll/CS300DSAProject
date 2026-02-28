// ProjectTwo.cpp
// ABCU Advising Assistance Program (CS 300 Project Two)
// Tree-based implementation that matches the pseudocode structure.
// Single CPP file, no CSVParser.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cctype>

using namespace std;

// ---------------------------- Course Model ----------------------------

struct Course {
    string courseId;          // e.g., "CS200"
    string title;             // e.g., "Data Structures"
    vector<string> prereqs;   // e.g., ["CS100", "MATH201"]
};

// ---------------------------- Utility Helpers ----------------------------

static string Trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) start++;

    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) end--;

    return s.substr(start, end - start);
}

static string ToUpper(string s) {
    for (char& c : s) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
    return s;
}

// Simple CSV split (assumes no quoted commas)
static vector<string> Split(const string& line, char delimiter = ',') {
    vector<string> parts;
    string token;
    stringstream ss(line);

    while (getline(ss, token, delimiter)) {
        parts.push_back(Trim(token));
    }
    return parts;
}

// ---------------------------- Pseudocode-Matching Functions ----------------------------

// FUNCTION ValidateLineFormat(parts)
bool ValidateLineFormat(const vector<string>& parts) {
    if (parts.size() < 2) return false;              // must have courseId and title
    if (Trim(parts[0]).empty()) return false;
    if (Trim(parts[1]).empty()) return false;
    return true;
}

// FUNCTION BuildCourse(parts)
Course BuildCourse(const vector<string>& parts) {
    Course course;
    course.courseId = ToUpper(Trim(parts[0]));
    course.title = Trim(parts[1]);

    course.prereqs.clear();
    for (size_t i = 2; i < parts.size(); i++) {
        string prereq = ToUpper(Trim(parts[i]));
        if (!prereq.empty()) {
            course.prereqs.push_back(prereq);
        }
    }

    return course;
}

// FUNCTION PrintCourse(course)
void PrintCourse(const Course& course, const map<string, Course>& tree) {
    cout << course.courseId << ", " << course.title << "\n";

    if (course.prereqs.empty()) {
        cout << "Prerequisites: None\n";
        return;
    }

    cout << "Prerequisites:\n";
    for (const string& prereqId : course.prereqs) {
        auto it = tree.find(prereqId);
        if (it != tree.end()) {
            cout << "  - " << prereqId << ", " << it->second.title << "\n";
        }
        else {
            cout << "  - " << prereqId << " (title not found in file)\n";
        }
    }
}

// FUNCTION LoadCourses_Tree(fileName)
bool LoadCourses_Tree(const string& fileName, map<string, Course>& tree) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: cannot open file '" << fileName << "'.\n";
        return false;
    }

    tree.clear();

    string line;
    int lineNumber = 0;
    int loaded = 0;

    while (getline(file, line)) {
        lineNumber++;
        line = Trim(line);

        if (line.empty()) continue;

        vector<string> parts = Split(line, ',');

        if (!ValidateLineFormat(parts)) {
            cout << "Warning: bad line format on line " << lineNumber << ": " << line << "\n";
            continue;
        }

        Course course = BuildCourse(parts);

        // Insert into "tree" keyed by courseId (map is sorted by key)
        // Duplicate keys overwrite; warn to match best practices.
        if (tree.find(course.courseId) != tree.end()) {
            cout << "Warning: duplicate courseId '" << course.courseId
                << "' on line " << lineNumber << " (overwriting).\n";
        }

        tree[course.courseId] = course;
        loaded++;
    }

    file.close();

    if (loaded == 0) {
        cout << "Warning: No valid courses loaded.\n";
    }
    else {
        cout << "Courses loaded: " << loaded << "\n";
    }

    return true;
}

// FUNCTION PrintCourseList_Tree(tree)  (in-order print equivalent)
void PrintCourseList_Tree(const map<string, Course>& tree) {
    // map iterates in sorted (alphanumeric) key order, equivalent to in-order traversal
    for (const auto& pair : tree) {
        cout << pair.second.courseId << ", " << pair.second.title << "\n";
    }
}

// FUNCTION PrintOneCourse_Tree(tree, targetId)
void PrintOneCourse_Tree(const map<string, Course>& tree, const string& targetIdInput) {
    string targetId = ToUpper(Trim(targetIdInput));
    auto it = tree.find(targetId);

    if (it == tree.end()) {
        cout << "Course not found: " << targetId << "\n";
        return;
    }

    PrintCourse(it->second, tree);
}

// ---------------------------- Menu (Matches Pseudocode) ----------------------------

static void PrintMenu() {
    cout << "\n";
    cout << "1. Load Data Structure\n";
    cout << "2. Print Course List\n";
    cout << "3. Print Course\n";
    cout << "9. Exit\n";
    cout << "\n";
    cout << "Enter choice: ";
}

static int ReadIntegerSafe() {
    string input;
    getline(cin, input);
    input = Trim(input);

    if (input.empty()) return -1;

    // ensure numeric
    for (char c : input) {
        if (!isdigit(static_cast<unsigned char>(c))) return -1;
    }

    try {
        return stoi(input);
    }
    catch (...) {
        return -1;
    }
}

// ---------------------------- Main ----------------------------

int main() {
    map<string, Course> tree;          // Tree-based structure (ordered by key)
    bool coursesLoaded = false;

    cout << "Welcome to the ABCU Advising Assistance Program.\n";

    while (true) {
        PrintMenu();
        int choice = ReadIntegerSafe();

        if (choice == 1) {
            cout << "Enter file name: ";
            string fileName;
            getline(cin, fileName);
            fileName = Trim(fileName);

            if (fileName.empty()) {
                cout << "Error: file name cannot be empty.\n";
                continue;
            }

            coursesLoaded = LoadCourses_Tree(fileName, tree);

        }
        else if (choice == 2) {
            if (!coursesLoaded) {
                cout << "Please load courses first (Option 1).\n";
                continue;
            }

            PrintCourseList_Tree(tree);

        }
        else if (choice == 3) {
            if (!coursesLoaded) {
                cout << "Please load courses first (Option 1).\n";
                continue;
            }

            cout << "Enter course ID (e.g., CS200): ";
            string targetId;
            getline(cin, targetId);

            PrintOneCourse_Tree(tree, targetId);

        }
        else if (choice == 9) {
            cout << "Thank you for using the ABCU Advising Assistance Program.\n";
            break;

        }
        else {
            cout << "Error: " << choice << " is not a valid option.\n";
        }
    }

    return 0;
}