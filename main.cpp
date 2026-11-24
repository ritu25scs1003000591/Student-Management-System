
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <limits>
using namespace std;

sqlite3* db;

// INPUT VALIDATION FUNCTION
int getValidNumber(string message, int minValue, int maxValue)
{
    int value;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (value >= minValue && value <= maxValue)
                return value;

            cout << "Invalid Input! Enter a value between "
                 << minValue << " and " << maxValue << "." << endl;
        }
        else
        {
            cout << "Invalid Input! Please enter numbers only." << endl;

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// TEXT INPUT VALIDATION
string getValidText(string message)
{
    string value;

    while (true)
    {
        cout << message;
        getline(cin, value);

        if (!value.empty() &&
            value.find_first_not_of(" \t") != string::npos)
            return value;

        cout << "Input cannot be empty! Please try again." << endl;
    }
}

// HANDLE APOSTROPHES IN TEXT
string escapeSQL(string value)
{
    string result = "";

    for (char ch : value)
    {
        if (ch == '\'')
            result += "''";
        else
            result += ch;
    }

    return result;
}

// DATABASE CALLBACK
int callback(void* data, int argc, char** argv, char** azColName)
{
    cout << "\n----------------------------------------" << endl;

    for (int i = 0; i < argc; i++)
    {
        cout << azColName[i] << " : "
             << (argv[i] ? argv[i] : "NULL") << endl;
    }

    cout << "----------------------------------------" << endl;

    return 0;
}

// ADD STUDENT
void addStudent()
{
    int roll, marks;
    string name, course;

    cout << "\n========== ADD STUDENT ==========" << endl;

    roll = getValidNumber("Enter Roll No: ", 1, 2147483647);
    name = getValidText("Enter Name: ");
    course = getValidText("Enter Course: ");
    marks = getValidNumber("Enter Marks (0-100): ", 0, 100);

    string sql =
        "INSERT INTO students (roll_no, name, course, marks) VALUES (" +
        to_string(roll) + ", '" + escapeSQL(name) + "', '" +
        escapeSQL(course) + "', " + to_string(marks) + ");";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage) == SQLITE_OK)
    {
        cout << "\nStudent Added Successfully!" << endl;
    }
    else
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// VIEW ALL STUDENTS
void viewStudents()
{
    cout << "\n========== ALL STUDENTS ==========" << endl;

    string sql = "SELECT * FROM students;";
    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), callback, nullptr, &errorMessage) != SQLITE_OK)
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// SEARCH STUDENT
void searchStudent()
{
    int choice;

    cout << "\n========== SEARCH STUDENT ==========" << endl;
    cout << "1. Search by Roll Number" << endl;
    cout << "2. Search by Name" << endl;
    cout << "0. Back to Main Menu" << endl;

    choice = getValidNumber("Enter Your Choice: ", 0, 2);

    string sql;

    if (choice == 1)
    {
        int roll = getValidNumber("Enter Roll No to Search: ", 1, 2147483647);

        sql = "SELECT * FROM students WHERE roll_no = " +
              to_string(roll) + ";";
    }
    else if (choice == 2)
    {
        string name = getValidText("Enter Student Name: ");

        sql = "SELECT * FROM students WHERE name = '" +
              escapeSQL(name) + "';";
    }
    else
    {
        return;
    }

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), callback, nullptr, &errorMessage) != SQLITE_OK)
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// UPDATE STUDENT
void updateStudent()
{
    int roll, marks;
    string name, course;

    cout << "\n========== UPDATE STUDENT ==========" << endl;

    roll = getValidNumber("Enter Roll No to Update: ", 1, 2147483647);
    name = getValidText("Enter New Name: ");
    course = getValidText("Enter New Course: ");
    marks = getValidNumber("Enter New Marks (0-100): ", 0, 100);

    string sql =
        "UPDATE students SET name = '" + escapeSQL(name) +
        "', course = '" + escapeSQL(course) +
        "', marks = " + to_string(marks) +
        " WHERE roll_no = " + to_string(roll) + ";";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage) == SQLITE_OK)
    {
        if (sqlite3_changes(db) > 0)
            cout << "Student Updated Successfully!" << endl;
        else
            cout << "Student Not Found!" << endl;
    }
    else
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// DELETE STUDENT
void deleteStudent()
{
    int roll;

    cout << "\n========== DELETE STUDENT ==========" << endl;

    roll = getValidNumber("Enter Roll No to Delete: ", 1, 2147483647);

    string sql = "DELETE FROM students WHERE roll_no = " +
                 to_string(roll) + ";";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage) == SQLITE_OK)
    {
        if (sqlite3_changes(db) > 0)
            cout << "Student Deleted Successfully!" << endl;
        else
            cout << "Student Not Found!" << endl;
    }
    else
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// SORT STUDENTS
void sortStudents()
{
    int choice;

    cout << "\n========== SORT STUDENTS ==========" << endl;
    cout << "1. Ascending Order (Low to High)" << endl;
    cout << "2. Descending Order (High to Low)" << endl;

    choice = getValidNumber("Enter Your Choice: ", 1, 2);

    string sql;

    if (choice == 1)
        sql = "SELECT * FROM students ORDER BY marks ASC;";
    else
        sql = "SELECT * FROM students ORDER BY marks DESC;";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), callback, nullptr, &errorMessage) != SQLITE_OK)
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// DISPLAY TOPPER
void displayTopper()
{
    cout << "\n========== TOPPER STUDENT ==========" << endl;

    string sql =
        "SELECT * FROM students ORDER BY marks DESC LIMIT 1;";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), callback, nullptr, &errorMessage) != SQLITE_OK)
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// COUNT STUDENTS
void countStudents()
{
    cout << "\n========== TOTAL STUDENTS ==========" << endl;

    string sql =
        "SELECT COUNT(*) AS total_students FROM students;";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), callback, nullptr, &errorMessage) != SQLITE_OK)
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// CALCULATE AVERAGE
void calculateAverage()
{
    cout << "\n========== AVERAGE MARKS ==========" << endl;

    string sql =
        "SELECT AVG(marks) AS average_marks FROM students;";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), callback, nullptr, &errorMessage) != SQLITE_OK)
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// DISPLAY TOP 3
void displayTop3()
{
    cout << "\n========== TOP 3 STUDENTS ==========" << endl;

    string sql =
        "SELECT * FROM students ORDER BY marks DESC LIMIT 3;";

    char* errorMessage = nullptr;

    if (sqlite3_exec(db, sql.c_str(), callback, nullptr, &errorMessage) != SQLITE_OK)
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// SEARCH STUDENT BY ID
void searchStudentByID()
{
    int id;

    cout << "\n========== SEARCH STUDENT BY ID ==========" << endl;

    id = getValidNumber("Enter Student ID: ", 1, 2147483647);

    string sql =
        "SELECT * FROM students WHERE id = " +
        to_string(id) + ";";

    char* errorMessage = nullptr;
    sqlite3_stmt* stmt = nullptr;

    string checkSQL =
        "SELECT COUNT(*) FROM students WHERE id = " +
        to_string(id) + ";";

    if (sqlite3_prepare_v2(db, checkSQL.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW &&
            sqlite3_column_int(stmt, 0) == 0)
        {
            cout << "Student Not Found!" << endl;
            sqlite3_finalize(stmt);
            return;
        }

        sqlite3_finalize(stmt);
    }

    if (sqlite3_exec(db, sql.c_str(), callback, nullptr, &errorMessage) != SQLITE_OK)
    {
        cout << "Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// MAIN FUNCTION
int main()
{
    if (sqlite3_open("students.db", &db) != SQLITE_OK)
    {
        cout << "Database Connection Failed!" << endl;
        return 1;
    }

    int choice;

    do
    {
        cout << "\n";
        cout << "========================================" << endl;
        cout << "       STUDENT MANAGEMENT SYSTEM        " << endl;
        cout << "========================================" << endl;
        cout << "  1.  Add Student" << endl;
        cout << "  2.  View All Students" << endl;
        cout << "  3.  Search Student" << endl;
        cout << "  4.  Update Student" << endl;
        cout << "  5.  Delete Student" << endl;
        cout << "  6.  Sort Students by Marks" << endl;
        cout << "  7.  Display Topper Student" << endl;
        cout << "  8.  Count Total Students" << endl;
        cout << "  9.  Calculate Average Marks" << endl;
        cout << "  10. Display Top 3 Students" << endl;
        cout << "  11. Search Student by ID" << endl;
        cout << "  0.  Exit" << endl;
        cout << "========================================" << endl;

        choice = getValidNumber("Enter Your Choice: ", 0, 11);

        switch (choice)
        {
            case 1:
                addStudent();
                break;

            case 2:
                viewStudents();
                break;

            case 3:
                searchStudent();
                break;

            case 4:
                updateStudent();
                break;

            case 5:
                deleteStudent();
                break;

            case 6:
                sortStudents();
                break;

            case 7:
                displayTopper();
                break;

            case 8:
                countStudents();
                break;

            case 9:
                calculateAverage();
                break;

            case 10:
                displayTop3();
                break;

            case 11:
                searchStudentByID();
                break;

            case 0:
                cout << "\n========================================" << endl;
                cout << "     PROGRAM EXITED SUCCESSFULLY!       " << endl;
                cout << "========================================" << endl;
                break;
        }

    } while (choice != 0);

    sqlite3_close(db);

    return 0;
}