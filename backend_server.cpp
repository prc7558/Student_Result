#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cstdlib>
using namespace std;

// ==================== BASE CLASS: Person (Inheritance) ====================
class Person {
protected:
    string name;
    string id;
    
public:
    Person() : name(""), id("") {}
    Person(string n, string i) : name(n), id(i) {}
    
    virtual void display() const {
        cout << "ID: " << id << "\nName: " << name << endl;
    }
    
    string getName() const { return name; }
    string getID() const { return id; }
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
    
    string getCode() const { return code; }
    string getName() const { return name; }
    int getMarks() const { return marks; }
    int getMaxMarks() const { return maxMarks; }
    
    string toJSON() const {
        stringstream ss;
        ss << "{\"code\":\"" << code << "\",\"name\":\"" << name 
           << "\",\"marks\":" << marks << ",\"maxMarks\":" << maxMarks << "}";
        return ss.str();
    }
};

// ==================== DERIVED CLASS: Student (Inheritance) ====================
class Student : public Person {
private:
    vector<Course> courses;
    float percentage;
    char grade;
    
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
    
    float getPercentage() const { return percentage; }
    char getGrade() const { return grade; }
    const vector<Course>& getCourses() const { return courses; }
    
    void saveToFile(const string& filename) const {
        ofstream fout(filename, ios::app);
        if (!fout) return;
        
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
    }
    
    string toJSON() const {
        stringstream ss;
        ss << "{\"prn\":\"" << id << "\",\"name\":\"" << name 
           << "\",\"percentage\":" << fixed << setprecision(2) << percentage
           << ",\"grade\":\"" << grade << "\",\"courses\":[";
        
        for (size_t i = 0; i < courses.size(); i++) {
            if (i > 0) ss << ",";
            ss << courses[i].toJSON();
        }
        ss << "]}";
        return ss.str();
    }
};

// ==================== CLASS: ResultManager ====================
class ResultManager {
private:
    map<string, Student> studentMap;
    map<string, float> classPercentageMap;
    string dataFile;
    string csvFile;
    
public:
    ResultManager(string df, string cf) : dataFile(df), csvFile(cf) {
        loadClassmateData();
        loadExistingStudents();
    }
    
    void loadClassmateData() {
        ifstream fin(csvFile);
        if (!fin) return;
        
        string line;
        getline(fin, line);  // Skip header
        
        while (getline(fin, line)) {
            if (line.empty()) continue;
            
            // Remove BOM if present
            if (line.size() >= 3 && line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF') {
                line = line.substr(3);
            }
            
            stringstream ss(line);
            string prn, name, percStr;
            
            getline(ss, prn, ',');
            getline(ss, name, ',');
            getline(ss, percStr, ',');
            
            try {
                float perc = stof(percStr);
                classPercentageMap[prn] = perc;
            } catch (...) {
                continue;
            }
        }
        fin.close();
    }
    
    void loadExistingStudents() {
        ifstream fin(dataFile);
        if (!fin) return;

        string line;
        string prn = "", studentName = "";
        vector<Course> courses;
        
        while (getline(fin, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty()) continue;
            
            if (line.find("Student PRN:") != string::npos) {
                prn = line.substr(line.find(":") + 2);
                // Trim spaces
                prn.erase(0, prn.find_first_not_of(" \t"));
            } 
            else if (line.find("Student ID:") != string::npos) {
                prn = line.substr(line.find(":") + 2);
                prn.erase(0, prn.find_first_not_of(" \t"));
            } 
            else if (line.find("Student Name:") != string::npos) {
                studentName = line.substr(line.find(":") + 2);
                studentName.erase(0, studentName.find_first_not_of(" \t"));
            } 
            else if (line.find("Courses:") != string::npos) {
                courses.clear();
            } 
            else if (line.find(" - ") != string::npos && line.find(" : ") != string::npos) {
                // Parse course line: "  CSE101 - Programming : 85/100"
                try {
                    size_t dashPos = line.find(" - ");
                    size_t colonPos = line.find(" : ");
                    size_t slashPos = line.find("/");
                    
                    if (dashPos != string::npos && colonPos != string::npos && slashPos != string::npos) {
                        // Extract parts
                        string code = line.substr(0, dashPos);
                        string courseName = line.substr(dashPos + 3, colonPos - dashPos - 3);
                        string marksStr = line.substr(colonPos + 3, slashPos - colonPos - 3);
                        string maxMarksStr = line.substr(slashPos + 1);
                        
                        // Trim spaces
                        code.erase(0, code.find_first_not_of(" \t"));
                        code.erase(code.find_last_not_of(" \t") + 1);
                        courseName.erase(0, courseName.find_first_not_of(" \t"));
                        courseName.erase(courseName.find_last_not_of(" \t") + 1);
                        marksStr.erase(0, marksStr.find_first_not_of(" \t"));
                        marksStr.erase(marksStr.find_last_not_of(" \t") + 1);
                        maxMarksStr.erase(0, maxMarksStr.find_first_not_of(" \t"));
                        maxMarksStr.erase(maxMarksStr.find_last_not_of(" \t") + 1);
                        
                        // Convert to integers
                        int marks = stoi(marksStr);
                        int maxMarks = stoi(maxMarksStr);
                        
                        courses.push_back(Course(code, courseName, marks, maxMarks));
                    }
                } catch (...) {
                    // Skip malformed course lines
                    continue;
                }
            } 
            else if (line.find("---------------------------------------------") != string::npos) {
                // End of student record
                if (!prn.empty() && !studentName.empty() && !courses.empty()) {
                    Student s(studentName, prn);
                    for (const auto& c : courses) {
                        s.addCourse(c);
                    }
                    
                    // Store PRN in uppercase for consistency
                    string prnUpper = prn;
                    transform(prnUpper.begin(), prnUpper.end(), prnUpper.begin(), ::toupper);
                    studentMap[prnUpper] = s;
                }
                
                // Reset for next student
                prn = studentName = "";
                courses.clear();
            }
        }
        
        // Handle last student if file doesn't end with dashes
        if (!prn.empty() && !studentName.empty() && !courses.empty()) {
            Student s(studentName, prn);
            for (const auto& c : courses) {
                s.addCourse(c);
            }
            string prnUpper = prn;
            transform(prnUpper.begin(), prnUpper.end(), prnUpper.begin(), ::toupper);
            studentMap[prnUpper] = s;
        }
        
        fin.close();
    }
    
    void addStudent(const Student& s) {
        string prnUpper = s.getID();
        transform(prnUpper.begin(), prnUpper.end(), prnUpper.begin(), ::toupper);
        studentMap[prnUpper] = s;
        s.saveToFile(dataFile);
    }
    
    Student* searchStudent(const string& prn) {
        string searchPRN = prn;
        transform(searchPRN.begin(), searchPRN.end(), searchPRN.begin(), ::toupper);
        
        auto it = studentMap.find(searchPRN);
        if (it != studentMap.end()) {
            return &(it->second);
        }
        return nullptr;
    }
    
    bool searchClassmate(const string& prn, float& percentage) {
        string searchPRN = prn;
        transform(searchPRN.begin(), searchPRN.end(), searchPRN.begin(), ::toupper);
        
        auto it = classPercentageMap.find(searchPRN);
        if (it != classPercentageMap.end()) {
            percentage = it->second;
            return true;
        }
        return false;
    }
    
    string getAllStudentsJSON() {
        stringstream ss;
        ss << "[";
        bool first = true;
        for (auto& pair : studentMap) {
            if (!first) ss << ",";
            first = false;
            ss << pair.second.toJSON();
        }
        ss << "]";
        return ss.str();
    }
    
    string getClassmateJSON(const string& prn) {
        float perc;
        if (searchClassmate(prn, perc)) {
            char grade;
            if (perc >= 90) grade = 'A';
            else if (perc >= 75) grade = 'B';
            else if (perc >= 60) grade = 'C';
            else if (perc >= 50) grade = 'D';
            else if (perc >= 40) grade = 'E';
            else grade = 'F';
            
            stringstream ss;
            ss << "{\"success\":true,\"prn\":\"" << prn 
               << "\",\"percentage\":" << fixed << setprecision(2) << perc
               << ",\"grade\":\"" << grade << "\"}";
            return ss.str();
        }
        return "{\"success\":false,\"error\":\"PRN not found\"}";
    }
};

// ==================== WEB BRIDGE ====================
void handleWebRequest(ResultManager& manager) {
    string command;
    getline(cin, command);
    
    try {
        if (command.substr(0, 3) == "ADD") {
            stringstream ss(command);
            string token;
            vector<string> parts;
            
            // Split command into parts
            while (getline(ss, token, '|')) {
                parts.push_back(token);
            }
            
            // Validate minimum required parts
            if (parts.size() < 4) {
                cout << "{\"error\":\"Invalid command format\"}" << endl;
                return;
            }
            
            string prn = parts[1];
            string studentName = parts[2];
            
            // Validate course count
            int courseCount;
            try {
                courseCount = stoi(parts[3]);
                if (courseCount <= 0) {
                    cout << "{\"error\":\"Invalid course count\"}" << endl;
                    return;
                }
            } catch (...) {
                cout << "{\"error\":\"Invalid course count format\"}" << endl;
                return;
            }
            
            // Validate we have enough parts for all courses
            if (parts.size() < 4 + (courseCount * 4)) {
                cout << "{\"error\":\"Missing course data\"}" << endl;
                return;
            }
            
            Student student(studentName, prn);
            
            // Process each course
            for (int i = 0; i < courseCount; i++) {
                int baseIndex = 4 + (i * 4);
                string code = parts[baseIndex];
                string name = parts[baseIndex + 1];
                
                // Safely parse marks
                int marks, maxMarks;
                try {
                    marks = stoi(parts[baseIndex + 2]);
                    maxMarks = stoi(parts[baseIndex + 3]);
                    
                    if (marks < 0 || maxMarks <= 0 || marks > maxMarks) {
                        cout << "{\"error\":\"Invalid marks range\"}" << endl;
                        return;
                    }
                } catch (...) {
                    cout << "{\"error\":\"Invalid marks format\"}" << endl;
                    return;
                }
                
                Course c(code, name, marks, maxMarks);
                student.addCourse(c);
            }
            
            manager.addStudent(student);
            cout << student.toJSON() << endl;
        }
        else if (command == "GET_ALL") {
            cout << manager.getAllStudentsJSON() << endl;
        }
        else if (command.substr(0, 6) == "SEARCH") {
            string prn = command.substr(7);
            Student* s = manager.searchStudent(prn);
            if (s) {
                cout << s->toJSON() << endl;
            } else {
                cout << "{\"error\":\"Student not found\"}" << endl;
            }
        }
        else if (command.substr(0, 9) == "CLASSMATE") {
            string prn = command.substr(10);
            cout << manager.getClassmateJSON(prn) << endl;
        }
        else {
            cout << "{\"error\":\"Invalid command\"}" << endl;
        }
    }
    catch (const exception& e) {
        cout << "{\"error\":\"Data processing error\"}" << endl;
    }
}

// ==================== MAIN ====================
int main(int argc, char* argv[]) {
    ResultManager manager("reportcards.txt", "sample_se1.csv");
    
    // Check if running in web mode (with command line argument)
    if (argc > 1 && string(argv[1]) == "--web") {
        // Web bridge mode - process single command from stdin
        handleWebRequest(manager);
        return 0;
    }
    
    // Regular console mode
    cout << "\n╔════════════════════════════════════════════════╗\n";
    cout << "║   STUDENT RESULT MANAGEMENT SYSTEM (OOP)      ║\n";
    cout << "╚════════════════════════════════════════════════╝\n";
    
    int choice;
    do {
        cout << "\n1. Add New Student Result\n";
        cout << "2. Search by PRN\n";
        cout << "3. Search Classmate (CSV)\n";
        cout << "4. View All Students\n";
        cout << "0. Exit\n";
        cout << "\nEnter choice: ";
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1: {
                string prn, name;
                int numCourses;
                
                cout << "\nEnter PRN: ";
                getline(cin, prn);
                cout << "Enter Name: ";
                getline(cin, name);
                
                Student student(name, prn);
                
                cout << "Number of courses: ";
                cin >> numCourses;
                cin.ignore();
                
                for (int i = 0; i < numCourses; i++) {
                    string code, courseName;
                    int marks, maxMarks;
                    
                    cout << "\n--- Course " << (i+1) << " ---\n";
                    cout << "Course Code: ";
                    getline(cin, code);
                    cout << "Course Name: ";
                    getline(cin, courseName);
                    cout << "Marks: ";
                    cin >> marks;
                    cout << "Max Marks: ";
                    cin >> maxMarks;
                    cin.ignore();
                    
                    Course c(code, courseName, marks, maxMarks);
                    student.addCourse(c);
                }
                
                manager.addStudent(student);
                cout << "\n✓ Student added successfully!\n";
                cout << "Percentage: " << fixed << setprecision(2) << student.getPercentage() << "%\n";
                cout << "Grade: " << student.getGrade() << "\n";
                break;
            }
            
            case 2: {
                string prn;
                cout << "\nEnter PRN: ";
                getline(cin, prn);
                
                Student* s = manager.searchStudent(prn);
                if (s) {
                    cout << "\n========= STUDENT FOUND =========\n";
                    cout << s->toJSON() << endl;
                } else {
                    cout << "\n✗ Student not found!\n";
                }
                break;
            }
            
            case 3: {
                string prn;
                cout << "\nEnter PRN: ";
                getline(cin, prn);
                
                cout << manager.getClassmateJSON(prn) << endl;
                break;
            }
            
            case 4: {
                cout << "\n" << manager.getAllStudentsJSON() << endl;
                break;
            }
            
            case 0:
                cout << "\n✓ Exiting...\n";
                break;
                
            default:
                cout << "\n✗ Invalid choice!\n";
        }
    } while (choice != 0);
    
    return 0;
}
