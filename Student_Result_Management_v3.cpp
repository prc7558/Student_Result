#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
using namespace std;

// ---------------------- CLASS: Course ----------------------
class Course {
public:
    string code, name;
    int marks, maxMarks;

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

    void show() const {
        cout << left << setw(12) << code
             << setw(25) << name
             << setw(12) << marks
             << setw(12) << maxMarks << endl;
    }
};

// ---------------------- CLASS: Student ----------------------
class Student {
public:
    string id, name;

    void input() {
        cout << "\nEnter student ID: ";
        cin >> id;
        cout << "Enter student name: ";
        cin.ignore();
        getline(cin, name);
    }

    void show() const {
        cout << "\nStudent ID   : " << id;
        cout << "\nStudent Name : " << name << endl;
    }
};

// ---------------------- CLASS: Result ----------------------
class Result {
    Student stu;
    vector<Course> courses;
    float percent;
    char grade;

public:
    void createResult() {
        stu.input();

        int n;
        cout << "How many courses? ";
        cin >> n;

        courses.resize(n);
        int total = 0, maxTotal = 0;

        for (int i = 0; i < n; i++) {
            cout << "\n--- Course " << i + 1 << " ---\n";
            courses[i].input();
            total += courses[i].marks;
            maxTotal += courses[i].maxMarks;
        }

        percent = (float)total / maxTotal * 100;

        if (percent >= 90) grade = 'A';
        else if (percent >= 75) grade = 'B';
        else if (percent >= 60) grade = 'C';
        else if (percent >= 50) grade = 'D';
        else if (percent >= 40) grade = 'E'; 
        else grade = 'F';

        cout << "\nResult calculated successfully!\n";
    }

    void displayReportCard() const {
        cout << "\n\n=====================================================\n";
        cout << "                  STUDENT REPORT CARD                \n";
        cout << "=====================================================\n";
        stu.show();

        cout << "\n-----------------------------------------------------\n";
        cout << left << setw(12) << "Code"
             << setw(25) << "Course Name"
             << setw(12) << "Marks"
             << setw(12) << "Max Marks" << endl;
        cout << "-----------------------------------------------------\n";

        for (const auto& c : courses)
            c.show();

        cout << "-----------------------------------------------------\n";
        cout << right << setw(35) << "Percentage: " 
             << fixed << setprecision(2) << percent << "%\n";
        cout << right << setw(35) << "Grade: " << grade << "\n";
        cout << "=====================================================\n\n";
    }

    void saveToFile() const {
        ofstream fout("reportcards.txt", ios::app);
        if (!fout) {
            cout << "Error opening file!\n";
            return;
        }

        fout << "---------------------------------------------\n";
        fout << "Student ID: " << stu.id << "\n";
        fout << "Student Name: " << stu.name << "\n";
        fout << "Courses:\n";
        for (const auto& c : courses)
            fout << "  " << c.code << " - " << c.name 
                 << " : " << c.marks << "/" << c.maxMarks << "\n";

        fout << "Percentage: " << fixed << setprecision(2) << percent << "%\n";
        fout << "Grade: " << grade << "\n";
        fout << "---------------------------------------------\n\n";
        fout.close();

        cout << "\nReport card saved successfully!\n";
    }

    void showAllFromFile() {
        ifstream fin("reportcards.txt");
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

    void searchByID(const string& searchID) {
        ifstream fin("reportcards.txt");
        if (!fin) {
            cout << "\nNo saved report cards found!\n";
            return;
        }

        string line;
        bool found = false, show = false;

        cout << "\nSearching for Student ID: " << searchID << "...\n";

        while (getline(fin, line)) {
            if (line.find("Student ID:") != string::npos) {
                if (line.find(searchID) != string::npos) {
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
            cout << "\nNo record found for Student ID: " << searchID << endl;

        fin.close();
    }
};

// ---------------------- MAIN MENU ----------------------
int main() {
    Result r;
    int ch;

    do {
        cout << "\n====== Student Result Management System ======\n";
        cout << "1. Create New Result\n";
        cout << "2. Display Current Report Card\n";
        cout << "3. Save Current Report Card to File\n";
        cout << "4. Show All Saved Report Cards\n";
        cout << "5. Search Report by Student ID\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> ch;

        switch (ch) {
        case 1:
            r.createResult();
            break;
        case 2:
            r.displayReportCard();
            break;
        case 3:
            r.saveToFile();
            break;
        case 4:
            r.showAllFromFile();
            break;
        case 5: {
            string id;
            cout << "\nEnter Student ID to search: ";
            cin >> id;
            r.searchByID(id);
            break;
        }
        case 0:
            cout << "\nExiting program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }
    } while (ch != 0);

    return 0;
}