#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

// ==================== BASE CLASS: Person (Inheritance) ====================
class Person {
protected:
    string name;
    string id;
    
public:
    Person() : name(""), id("") {}
    Person(string n, string i) : name(n), id(i) {}
    
    // Virtual function for polymorphism
    virtual void display() const {
        cout << "ID: " << id << "\nName: " << name << endl;
    }
    
    // Getters
    string getName() const { return name; }
    string getID() const { return id; }
    
    // Setters
    void setName(string n) { name = n; }
    void setID(string i) { id = i; }
    
    virtual ~Person() {}
};

// ==================== CLASS: Course ====================
class Course {
private:
    string code;
    string name;
    int marks;
    int maxMarks;
    
public:
    Course() : code(""), name(""), marks(0), maxMarks(0) {}
    Course(string c, string n, int m, int mx) : code(c), name(n), marks(m), maxMarks(mx) {}
    
    void input() {
        cout << "Enter course code: ";
        cin >> code;
        cout << "Enter course name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter marks obtained: ";
        cin >> marks;
        cout << "Enter maximum marks: ";
        cin >> maxMarks;
    }
    
    void display() const {
        cout << left << setw(12) << code
             << setw(25) << name
             << setw(12) << marks
             << setw(12) << maxMarks << endl;
    }
    
    // Getters
    string getCode() const { return code; }
    string getName() const { return name; }
    int getMarks() const { return marks; }
    int getMaxMarks() const { return maxMarks; }
    
    // For file writing
    string toFileFormat() const {
        stringstream ss;
        ss << code << "|" << name << "|" << marks << "|" << maxMarks;
        return ss.str();
    }
};

// ==================== DERIVED CLASS: Student (Inheritance from Person) ====================
class Student : public Person {
private:
    vector<Course> courses;
    float percentage;
    char grade;
    
    // Private helper method - Encapsulation
    void calculatePercentage() {
        int total = 0, maxTotal = 0;
        for (const auto& c : courses) {
            total += c.getMarks();
            maxTotal += c.getMaxMarks();
        }
        percentage = (maxTotal > 0) ? (float)total / maxTotal * 100 : 0;
    }
    
    void calculateGrade() {
        if (percentage >= 90) grade = 'A';
        else if (percentage >= 75) grade = 'B';
        else if (percentage >= 60) grade = 'C';
        else if (percentage >= 50) grade = 'D';
        else if (percentage >= 40) grade = 'E';
        else grade = 'F';
    }
    
public:
    Student() : Person(), percentage(0), grade('F') {}
    Student(string n, string i) : Person(n, i), percentage(0), grade('F') {}
    
    void addCourse(const Course& c) {
        courses.push_back(c);
        calculatePercentage();
        calculateGrade();
    }
    
    void inputStudent() {
        cout << "\nEnter student PRN: ";
        cin >> id;
        cout << "Enter student name: ";
        cin.ignore();
        getline(cin, name);
        
        int n;
        cout << "How many courses? ";
        cin >> n;
        
        courses.clear();
        for (int i = 0; i < n; i++) {
            cout << "\n--- Course " << i + 1 << " ---\n";
            Course c;
            c.input();
            addCourse(c);
        }
        
        cout << "\nStudent result created successfully!\n";
    }
    
    // Override display - Polymorphism
    void display() const override {
        cout << "\n=====================================================\n";
        cout << "                  STUDENT REPORT CARD                \n";
        cout << "=====================================================\n";
        cout << "Student PRN: " << id << "\n";
        cout << "Student Name: " << name << "\n";
        
        cout << "\n-----------------------------------------------------\n";
        cout << left << setw(12) << "Code"
             << setw(25) << "Course Name"
             << setw(12) << "Marks"
             << setw(12) << "Max Marks" << endl;
        cout << "-----------------------------------------------------\n";
        
        for (const auto& c : courses)
            c.display();
        
        cout << "-----------------------------------------------------\n";
        cout << right << setw(35) << "Percentage: " 
             << fixed << setprecision(2) << percentage << "%\n";
        cout << right << setw(35) << "Grade: " << grade << "\n";
        cout << "=====================================================\n\n";
    }
    
    // Getters
    float getPercentage() const { return percentage; }
    char getGrade() const { return grade; }
    int getCourseCount() const { return courses.size(); }
    Course getCourse(int index) const { return courses[index]; }
    
    // File handling - Save to file (APPEND mode)
    void saveToFile(const string& filename) const {
        ofstream fout(filename, ios::app);  // Append mode - data preserved
        if (!fout) {
            cout << "Error opening file!\n";
            return;
        }
        
        fout << "---------------------------------------------\n";
        fout << "Student PRN: " << id << "\n";
        fout << "Student Name: " << name << "\n";
        fout << "Courses:\n";
        for (const auto& c : courses) {
            fout << "  " << c.getCode() << " - " << c.getName() 
                 << " : " << c.getMarks() << "/" << c.getMaxMarks() << "\n";
        }
        fout << "Percentage: " << fixed << setprecision(2) << percentage << "%\n";
        fout << "Grade: " << grade << "\n";
        fout << "---------------------------------------------\n\n";
        fout.close();
        
        cout << "\nReport card saved successfully to " << filename << "!\n";
    }
    
    // Export to JSON for web interface
    string toJSON() const {
        stringstream ss;
        ss << "{\"prn\":\"" << id << "\",\"name\":\"" << name 
           << "\",\"percentage\":" << fixed << setprecision(2) << percentage
           << ",\"grade\":\"" << grade << "\",\"courses\":[";
        
        for (size_t i = 0; i < courses.size(); i++) {
            if (i > 0) ss << ",";
            ss << "{\"code\":\"" << courses[i].getCode()
               << "\",\"name\":\"" << courses[i].getName()
               << "\",\"marks\":" << courses[i].getMarks()
               << ",\"maxMarks\":" << courses[i].getMaxMarks() << "}";
        }
        ss << "]}";
        return ss.str();
    }
};

// ==================== CLASS: ResultManager (File Handling & Map) ====================
class ResultManager {
private:
    map<string, Student> studentMap;  // STL Map for quick PRN-based search
    map<string, float> classPercentageMap;  // Map for classmate percentages from CSV
    string dataFile;
    string csvFile;
    
public:
    ResultManager(string df, string cf) : dataFile(df), csvFile(cf) {
        loadClassmateData();
    }
    
    // Load classmate data from CSV into Map (STL Map usage)
    void loadClassmateData() {
        ifstream fin(csvFile);
        if (!fin) {
            cout << "\nWarning: Classmate CSV file not found!\n";
            return;
        }
        
        string line;
        getline(fin, line);  // Skip header
        
        int count = 0;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            
            stringstream ss(line);
            string prn, name, percStr;
            
            getline(ss, prn, ',');
            getline(ss, name, ',');
            getline(ss, percStr, ',');
            
            try {
                float perc = stof(percStr);
                classPercentageMap[prn] = perc;
                count++;
            } catch (...) {
                continue;
            }
        }
        fin.close();
        cout << "\n   Loaded " << count << " classmate records from CSV!\n";
    }
    
    // Search classmate percentage by PRN using Map
    bool searchClassmatePercentage(const string& prn) {
        string searchPRN = prn;
        transform(searchPRN.begin(), searchPRN.end(), searchPRN.begin(), ::toupper);
        
        auto it = classPercentageMap.find(searchPRN);
        if (it != classPercentageMap.end()) {
            cout << "\n========= CLASSMATE RESULT FOUND =========\n";
            cout << "PRN: " << it->first << "\n";
            cout << "Percentage: " << fixed << setprecision(2) << it->second << "%\n";
            
            // Display grade
            char grade;
            if (it->second >= 90) grade = 'A';
            else if (it->second >= 75) grade = 'B';
            else if (it->second >= 60) grade = 'C';
            else if (it->second >= 50) grade = 'D';
            else if (it->second >= 40) grade = 'E';
            else grade = 'F';
            
            cout << "Grade: " << grade << "\n";
            cout << "==========================================\n\n";
            return true;
        }
        return false;
    }
    
    // Add student to map
    void addStudent(const Student& s) {
        studentMap[s.getID()] = s;
        s.saveToFile(dataFile);
    }
    
    // Search student by PRN from map
    Student* searchStudent(const string& prn) {
        string searchPRN = prn;
        transform(searchPRN.begin(), searchPRN.end(), searchPRN.begin(), ::toupper);
        
        auto it = studentMap.find(searchPRN);
        if (it != studentMap.end()) {
            return &(it->second);
        }
        return nullptr;
    }
    
    // Display all saved report cards from file
    void displayAllFromFile() {
        ifstream fin(dataFile);
        if (!fin) {
            cout << "\nNo saved report cards found!\n";
            return;
        }
        
        cout << "\n======= All Saved Report Cards =======\n\n";
        string line;
        while (getline(fin, line))
            cout << line << endl;
        
        fin.close();
    }
    
    // Search in file by PRN
    void searchInFile(const string& searchPRN) {
        ifstream fin(dataFile);
        if (!fin) {
            cout << "\nNo saved report cards found!\n";
            return;
        }
        
        string line;
        bool found = false, show = false;
        
        cout << "\nSearching for Student PRN: " << searchPRN << "...\n";
        
        while (getline(fin, line)) {
            if (line.find("Student PRN:") != string::npos) {
                if (line.find(searchPRN) != string::npos) {
                    found = true;
                    show = true;
                    cout << "\n========= REPORT CARD FOUND =========\n";
                } else {
                    show = false;
                }
            }
            
            if (show)
                cout << line << endl;
        }
        
        if (!found)
            cout << "\nNo record found for Student PRN: " << searchPRN << endl;
        
        fin.close();
    }
    
    // Get all classmate data as JSON for web
    string getClassmateJSON() {
        stringstream ss;
        ss << "[";
        bool first = true;
        for (auto& pair : classPercentageMap) {
            if (!first) ss << ",";
            first = false;
            ss << "{\"prn\":\"" << pair.first 
               << "\",\"percentage\":" << fixed << setprecision(2) << pair.second << "}";
        }
        ss << "]";
        return ss.str();
    }
};

// ==================== MAIN MENU ====================
int main() {
    ResultManager manager("reportcards.txt", "sample_se1.csv");
    Student currentStudent;
    int choice;
    
    do {
        cout << "\n\n=====================================================\n";
        cout << "        STUDENT RESULT MANAGEMENT SYSTEM (OOP)        \n";
        cout << "=====================================================\n";
        cout << "\n1. Create New Student Result\n";
        cout << "2. Display Current Report Card\n";
        cout << "3. Save Current Result to File\n";
        cout << "4. Show All Saved Report Cards\n";
        cout << "5. Search Report by PRN (from file)\n";
        cout << "6. Search Classmate Percentage by PRN (CSV)\n";
        cout << "7. Display All Classmate Data\n";
        cout << "0. Exit\n";
        cout << "\nEnter your choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                currentStudent.inputStudent();
                manager.addStudent(currentStudent);
                break;
                
            case 2:
                currentStudent.display();
                break;
                
            case 3:
                currentStudent.saveToFile("reportcards.txt");
                break;
                
            case 4:
                manager.displayAllFromFile();
                break;
                
            case 5: {
                string prn;
                cout << "\nEnter Student PRN to search: ";
                cin >> prn;
                manager.searchInFile(prn);
                break;
            }
            
            case 6: {
                string prn;
                cout << "\nEnter PRN to search classmate percentage: ";
                cin >> prn;
                if (!manager.searchClassmatePercentage(prn)) {
                    cout << "\n   PRN not found in classmate database!\n";
                }
                break;
            }
            
            case 7: {
                cout << "\n" << manager.getClassmateJSON() << endl;
                break;
            }
            
            case 0:
                cout << "\n   Exiting program. Goodbye!\n";
                break;
                
            default:
                cout << "\n   Invalid choice! Please try again.\n";
        }
    } while (choice != 0);
    
    return 0;
}
