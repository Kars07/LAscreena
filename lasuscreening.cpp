#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

class JAMBAllocation {
private:
    int jambScore;
    double jambPercentage;
    
public:
    JAMBAllocation() : jambScore(0), jambPercentage(0.0) {}
    
    void setJambScore(int score) {
        if (score < 0 || score > 400) {
            cout << "Invalid JAMB score. Score must be between 0 and 400.\n";
            return;
        }
        jambScore = score;
        calculateJambPercentage();
    }
    
    void calculateJambPercentage() {
        jambPercentage = (jambScore / 400.0) * 60.0; // 60% for JAMB
    }
    
    // Getters
    int getJambScore() const { return jambScore; }
    double getJambPercentage() const { return jambPercentage; }
    
    void displayJambResults() const {
        cout << "\nJAMB SECTION (60% Weight):\n";
        cout << "JAMB Score: " << jambScore << "/400\n";
        cout << "JAMB Percentage: " << fixed << setprecision(1) << jambPercentage << "%\n";
    }
};

class WAECAllocation {
private:
    vector<pair<string, string>> waecGrades;
    unordered_map<string, int> gradePoints;
    unordered_map<string, int> requiredSubjects;
    vector<pair<string, string>> optionalSubjects;
    double waecPercentage;
    int totalScore;
    
    void initializeGradePoints() {
        gradePoints = {
            {"A1", 8}, {"B2", 7}, {"B3", 6}, {"C4", 5},
            {"C5", 4}, {"C6", 3}, {"D7", 2}, {"E8", 1}, {"F9", 0}
        };
    }
    
public:
    WAECAllocation() : waecPercentage(0.0), totalScore(0) {
        initializeGradePoints();
    }
    
    void setRequiredSubjects(int choice) {
        requiredSubjects.clear();
        
        switch (choice) {
            case 1: // Science
                cout << "Required subjects for Science students:\n";
                requiredSubjects = {
                    {"Mathematics", 0},
                    {"English Language", 0},
                    {"Physics", 0},
                    {"Chemistry", 0}
                };
                break;
                
            case 2: // Arts
                cout << "Required subjects for Arts students:\n";
                requiredSubjects = {
                    {"English Language", 0},
                    {"Government", 0},
                    {"Literature in English", 0},
                    {"Mathematics", 0}
                };
                break;
                
            case 3: // Commercial
                cout << "Required subjects for Commercial students:\n";
                requiredSubjects = {
                    {"English Language", 0},
                    {"Mathematics", 0},
                    {"Economics", 0},
                    {"Commerce", 0}
                };
                break;
                
            default:
                cout << "Invalid choice!\n";
                return;
        }
        
        // Display required subjects
        for (const auto& subject : requiredSubjects) {
            cout << "- " << subject.first << "\n";
        }
    }
    
    void addGrade(const string& subject, const string& grade) {
        // Validate grade
        if (gradePoints.find(grade) == gradePoints.end()) {
            cout << "Invalid grade: " << grade << "\n";
            return;
        }
        waecGrades.push_back({subject, grade});
    }
    
    void addOptionalSubject(const string& subject, const string& grade) {
        if (gradePoints.find(grade) == gradePoints.end()) {
            cout << "Invalid grade: " << grade << "\n";
            return;
        }
        optionalSubjects.push_back({subject, grade});
    }
    
    int getGradePoint(const string& grade) const {
        auto it = gradePoints.find(grade);
        return (it != gradePoints.end()) ? it->second : 0;
    }
    
    void calculateWaecAllocation() {
        // Map for quick lookup of available grades
        unordered_map<string, string> gradesMap;
        for (const auto& grade : waecGrades) {
            gradesMap[grade.first] = grade.second;
        }
        
        // Process required subjects
        vector<pair<string, int>> requiredScores;
        for (auto& required : requiredSubjects) {
            const string& subject = required.first;
            auto it = gradesMap.find(subject);
            if (it != gradesMap.end()) {
                int points = getGradePoint(it->second);
                required.second = points;
                requiredScores.push_back({subject, points});
            } else {
                cout << "Warning: Grade not found for required subject: " << subject << "\n";
                required.second = 0;
                requiredScores.push_back({subject, 0});
            }
        }
        
        // Process optional subjects and sort by points (descending)
        vector<pair<string, int>> optionalScores;
        for (const auto& optional : optionalSubjects) {
            int points = getGradePoint(optional.second);
            optionalScores.push_back({optional.first, points});
        }
        
        sort(optionalScores.begin(), optionalScores.end(), 
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });
        
        // Calculate total score (4 required + 1 best optional = 5 subjects)
        totalScore = 0;
        for (const auto& required : requiredScores) {
            totalScore += required.second;
        }
        
        // Add best optional subject if available
        if (!optionalScores.empty()) {
            totalScore += optionalScores[0].second;
        }
        
        // Calculate percentage (40% weight for WAEC, max 40 points for 5 subjects)
        waecPercentage = (totalScore / 40.0) * 40.0;
    }
    
    void displayResults() const {
        cout << "\nWAEC SECTION (40% Weight):\n";
        cout << "=========================\n";
        
        cout << "\nRequired Subjects:\n";
        cout << "------------------\n";
        for (const auto& required : requiredSubjects) {
            // Find the grade for this subject
            string grade = "N/A";
            for (const auto& gradeEntry : waecGrades) {
                if (gradeEntry.first == required.first) {
                    grade = gradeEntry.second;
                    break;
                }
            }
            cout << required.first << ": " << grade << " (" << required.second << " points)\n";
        }
        
        cout << "\nOptional Subjects (Best Selected):\n";
        cout << "----------------------------------\n";
        if (!optionalSubjects.empty()) {
            // Find the best optional subject
            int bestScore = 0;
            string bestSubject = "";
            string bestGrade = "";
            
            for (const auto& optional : optionalSubjects) {
                int points = getGradePoint(optional.second);
                if (points > bestScore) {
                    bestScore = points;
                    bestSubject = optional.first;
                    bestGrade = optional.second;
                }
            }
            
            if (!bestSubject.empty()) {
                cout << bestSubject << ": " << bestGrade << " (" << bestScore << " points)\n";
            }
        } else {
            cout << "No optional subjects provided.\n";
        }
        
        cout << "\nTotal Score: " << totalScore << "/40\n";
        cout << "WAEC Percentage: " << fixed << setprecision(1) << waecPercentage << "%\n";
    }
    
    // Getters
    double getPercentage() const { return waecPercentage; }
    int getTotalScore() const { return totalScore; }
    
    const unordered_map<string, int>& getRequiredSubjects() const {
        return requiredSubjects;
    }
    
    const vector<pair<string, string>>& getOptionalSubjects() const {
        return optionalSubjects;
    }
};

class LASUScreeningAggregator : public JAMBAllocation, public WAECAllocation {
private:
    double finalScreeningScore;
    
public:
    LASUScreeningAggregator() : finalScreeningScore(0.0) {}
    
    void calculateScreeningResults() {
        // Calculate WAEC allocation
        calculateWaecAllocation();
        
        // Calculate final screening score
        finalScreeningScore = getJambPercentage() + getPercentage();
        
        cout << "\nCalculation Breakdown:\n";
        cout << "======================\n";
        cout << "JAMB Score: " << getJambScore() << "/400 = " 
             << fixed << setprecision(1) << getJambPercentage() << "%\n";
        cout << "WAEC Score: " << getTotalScore() << "/40 = " 
             << fixed << setprecision(1) << getPercentage() << "%\n";
        cout << "Final Aggregate: " << getJambPercentage() << "% + " 
             << getPercentage() << "% = " << finalScreeningScore << "%\n";
    }
    
    void displayScreeningResults() {
        cout << "\n========================================\n";
        cout << "      LASU SCREENING RESULTS\n";
        cout << "========================================\n";
        
        // Display JAMB results
        displayJambResults();
        
        // Display WAEC results
        displayResults();
        
        // Display final aggregate
        cout << "\n========================================\n";
        cout << "FINAL SCREENING AGGREGATE:\n";
        cout << "========================================\n";
        cout << "JAMB Contribution: " << fixed << setprecision(1) << getJambPercentage() << "%\n";
        cout << "WAEC Contribution: " << fixed << setprecision(1) << getPercentage() << "%\n";
        cout << "TOTAL SCREENING SCORE: " << finalScreeningScore << "%\n";
        
        // Provide admission guidance
        cout << "\nADMISSION PROSPECT:\n";
        if (finalScreeningScore >= 70.0) {
            cout << "STATUS: EXCELLENT - High chance of admission!\n";
        } else if (finalScreeningScore >= 60.0) {
            cout << "STATUS: GOOD - Moderate chance of admission\n";
        } else if (finalScreeningScore >= 50.0) {
            cout << "STATUS: FAIR - Consider retaking JAMB or improving WAEC\n";
        } else {
            cout << "STATUS: POOR - Strong recommendation to retake JAMB\n";
        }
        
        cout << "========================================\n";
    }
    
    double getFinalScreeningScore() const {
        return finalScreeningScore;
    }
};

// Utility functions
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool getValidChoice(int& choice, int min, int max) {
    while (!(cin >> choice) || choice < min || choice > max) {
        cout << "Invalid input. Please enter a number between " << min << " and " << max << ": ";
        clearInputBuffer();
    }
    return true;
}

string getValidGrade() {
    string grade;
    vector<string> validGrades = {"A1", "B2", "B3", "C4", "C5", "C6", "D7", "E8", "F9"};
    
    while (true) {
        cin >> grade;
        // Convert to uppercase
        transform(grade.begin(), grade.end(), grade.begin(), ::toupper);
        
        if (find(validGrades.begin(), validGrades.end(), grade) != validGrades.end()) {
            return grade;
        }
        cout << "Invalid grade. Please enter a valid grade (A1, B2, B3, C4, C5, C6, D7, E8, F9): ";
    }
}

string getSubjectName() {
    string subject;
    while (true) {
        getline(cin, subject);
        // Trim whitespace
        subject.erase(0, subject.find_first_not_of(" \t"));
        subject.erase(subject.find_last_not_of(" \t") + 1);
        
        if (!subject.empty()) {
            return subject;
        }
        cout << "Subject name cannot be empty. Please enter a valid subject name: ";
    }
}

int main() {
    LASUScreeningAggregator calculator;
    int choice;
    
    cout << "=== LASU SCREENING CALCULATOR ===\n";
    cout << "Select your course category:\n";
    cout << "1. Science\n";
    cout << "2. Arts\n";
    cout << "3. Commercial\n";
    cout << "Enter choice (1-3): ";
    
    getValidChoice(choice, 1, 3);
    
    // Set required subjects based on choice
    calculator.setRequiredSubjects(choice);
    
    // Get JAMB score
    int jambScore;
    cout << "\nEnter your JAMB score (out of 400): ";
    while (!(cin >> jambScore) || jambScore < 0 || jambScore > 400) {
        cout << "Invalid score. Please enter a score between 0 and 400: ";
        clearInputBuffer();
    }
    calculator.setJambScore(jambScore);
    
    // Get WAEC grades for required subjects
    cout << "\nEnter your WAEC grades for REQUIRED subjects:\n";
    cout << "=============================================\n";
    
    vector<string> requiredSubjectsList;
    if (choice == 1) {
        requiredSubjectsList = {"Mathematics", "English Language", "Physics", "Chemistry"};
    } else if (choice == 2) {
        requiredSubjectsList = {"English Language", "Government", "Literature in English", "Mathematics"};
    } else if (choice == 3) {
        requiredSubjectsList = {"English Language", "Mathematics", "Economics", "Commerce"};
    }
    
    for (const string& subject : requiredSubjectsList) {
        cout << "Enter grade for " << subject << " (A1, B2, B3, C4, C5, C6, D7, E8, F9): ";
        string grade = getValidGrade();
        calculator.addGrade(subject, grade);
    }
    
    // Get optional subjects
    cout << "\nEnter your OPTIONAL subjects:\n";
    cout << "============================\n";
    cout << "Note: You need at least 1 optional subject. The best one will be selected.\n";
    
    int numOptional;
    cout << "How many optional subjects do you have? ";
    while (!(cin >> numOptional) || numOptional < 1 || numOptional > 10) {
        cout << "Please enter a number between 1 and 10: ";
        clearInputBuffer();
    }
    
    clearInputBuffer(); // Clear buffer after reading number
    
    for (int i = 0; i < numOptional; ++i) {
        cout << "Enter optional subject " << (i + 1) << " name: ";
        string subject = getSubjectName();
        
        cout << "Enter grade for " << subject << " (A1, B2, B3, C4, C5, C6, D7, E8, F9): ";
        string grade = getValidGrade();
        clearInputBuffer(); // Clear buffer after reading grade
        
        calculator.addOptionalSubject(subject, grade);
    }
    
    // Calculate and display results
    calculator.calculateScreeningResults();
    calculator.displayScreeningResults();
    
    return 0;
}