#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <regex>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

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
    string facultyName;
    
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
            case 1: // Science/Basic Sciences
                facultyName = "Science/Basic Sciences";
                cout << "Required subjects for Science students:\n";
                requiredSubjects = {
                    {"Mathematics", 0},
                    {"English Language", 0},
                    {"Physics", 0},
                    {"Chemistry", 0}
                };
                break;
                
            case 2: // Arts and Humanities
                facultyName = "Arts and Humanities";
                cout << "Required subjects for Arts students:\n";
                requiredSubjects = {
                    {"English Language", 0},
                    {"Government", 0},
                    {"Literature in English", 0},
                    {"Mathematics", 0}
                };
                break;
                
            case 3: // Management Sciences
                facultyName = "Management Sciences";
                cout << "Required subjects for Management Sciences:\n";
                requiredSubjects = {
                    {"English Language", 0},
                    {"Mathematics", 0},
                    {"Economics", 0},
                    {"Commerce", 0}
                };
                break;
                
            case 4: // Engineering
                facultyName = "Engineering";
                cout << "Required subjects for Engineering:\n";
                requiredSubjects = {
                    {"Mathematics", 0},
                    {"English Language", 0},
                    {"Physics", 0},
                    {"Chemistry", 0}
                };
                break;
                
            case 5: // Medicine and Surgery
                facultyName = "Medicine and Surgery";
                cout << "Required subjects for Medicine:\n";
                requiredSubjects = {
                    {"Mathematics", 0},
                    {"English Language", 0},
                    {"Physics", 0},
                    {"Chemistry", 0},
                    {"Biology", 0}
                };
                break;
                
            case 6: // Law
                facultyName = "Law";
                cout << "Required subjects for Law:\n";
                requiredSubjects = {
                    {"English Language", 0},
                    {"Mathematics", 0},
                    {"Government", 0},
                    {"Literature in English", 0}
                };
                break;
                
            case 7: // Education
                facultyName = "Education";
                cout << "Required subjects for Education:\n";
                requiredSubjects = {
                    {"English Language", 0},
                    {"Mathematics", 0},
                    {"Government", 0},
                    {"Economics", 0}
                };
                break;
                
            case 8: // Agriculture
                facultyName = "Agriculture";
                cout << "Required subjects for Agriculture:\n";
                requiredSubjects = {
                    {"Mathematics", 0},
                    {"English Language", 0},
                    {"Chemistry", 0},
                    {"Biology", 0}
                };
                break;
                
            case 9: // Environmental Sciences
                facultyName = "Environmental Sciences";
                cout << "Required subjects for Environmental Sciences:\n";
                requiredSubjects = {
                    {"Mathematics", 0},
                    {"English Language", 0},
                    {"Physics", 0},
                    {"Geography", 0}
                };
                break;
                
            case 10: // Social Sciences
                facultyName = "Social Sciences";
                cout << "Required subjects for Social Sciences:\n";
                requiredSubjects = {
                    {"English Language", 0},
                    {"Mathematics", 0},
                    {"Government", 0},
                    {"Economics", 0}
                };
                break;
                
            case 11: // Allied Medical Sciences
                facultyName = "Allied Medical Sciences";
                cout << "Required subjects for Allied Medical Sciences:\n";
                requiredSubjects = {
                    {"English Language", 0},
                    {"Mathematics", 0},
                    {"Physics", 0},
                    {"Chemistry", 0},
                    {"Biology", 0}
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
        
        // Calculate total score - different calculation for Medicine (5 subjects required)
        totalScore = 0;
        int maxSubjects = (facultyName == "Medicine and Surgery" || facultyName == "Allied Medical Sciences") ? 5 : 4;
        int maxPoints = (facultyName == "Medicine and Surgery" || facultyName == "Allied Medical Sciences") ? 40 : 32;
        
        for (const auto& required : requiredScores) {
            totalScore += required.second;
        }
        
        // Add best optional subjects if we need more than 4 subjects
        if (maxSubjects == 5 && !optionalScores.empty()) {
            // For medicine, we don't need optional since we already have 5 required
        } else if (maxSubjects == 4 && !optionalScores.empty()) {
            totalScore += optionalScores[0].second;
            maxPoints = 40; // 5 subjects total
        }
        
        // Calculate percentage (40% weight for WAEC)
        waecPercentage = (totalScore / (double)maxPoints) * 40.0;
    }
    
    void displayResults() const {
        cout << "\nWAEC SECTION (40% Weight) - " << facultyName << ":\n";
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
        
        // Show optional subjects only if we're not doing Medicine/Allied Medical
        if (facultyName != "Medicine and Surgery" && facultyName != "Allied Medical Sciences") {
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
        }
        
        int maxPossible = (facultyName == "Medicine and Surgery" || facultyName == "Allied Medical Sciences") ? 40 : 40;
        cout << "\nTotal Score: " << totalScore << "/" << maxPossible << "\n";
        cout << "WAEC Percentage: " << fixed << setprecision(1) << waecPercentage << "%\n";
    }
    
    // Getters
    double getPercentage() const { return waecPercentage; }
    int getTotalScore() const { return totalScore; }
    string getFacultyName() const { return facultyName; }
    
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
        
        // cout << "\nCalculation Breakdown:\n";
        // cout << "======================\n";
        // cout << "JAMB Score: " << getJambScore() << "/400 = " 
        //      << fixed << setprecision(1) << getJambPercentage() << "%\n";
        // cout << "WAEC Score: " << getTotalScore() << "/40 = " 
        //      << fixed << setprecision(1) << getPercentage() << "%\n";
        // cout << "Final Aggregate: " << getJambPercentage() << "% + " 
        //      << getPercentage() << "% = " << finalScreeningScore << "%\n";
    }

    double getFinalScreeningScore()
    {
        return finalScreeningScore;
    }
    
    string getAdmissionStatus() {
        // cout << "\n========================================\n";
        // cout << "      LASU SCREENING RESULTS\n";
        // cout << "========================================\n";
        
        // // Display JAMB results
        // displayJambResults();
        
        // // Display WAEC results
        // displayResults();
        
        // // Display final aggregate
        // cout << "\n========================================\n";
        // cout << "FINAL SCREENING AGGREGATE - " << getFacultyName() << ":\n";
        // cout << "========================================\n";
        // cout << "JAMB Contribution: " << fixed << setprecision(1) << getJambPercentage() << "%\n";
        // cout << "WAEC Contribution: " << fixed << setprecision(1) << getPercentage() << "%\n";
        // cout << "TOTAL SCREENING SCORE: " << finalScreeningScore << "%\n";
        

        // Faculty-specific admission guidance
        // cout << "\nADMISSION PROSPECT FOR " << getFacultyName() << ":\n";
        double cutoffThreshold = getCutoffThreshold();


        if (finalScreeningScore >= cutoffThreshold + 10) {
             return "EXCELLENT - High chance of admission!";
        } else if (finalScreeningScore >= cutoffThreshold) {
            return "GOOD - Moderate chance of admission";
        } else if (finalScreeningScore >= cutoffThreshold - 10) {
            return "FAIR - Consider retaking JAMB or improving WAEC";
        } else {
             return "POOR - Strong recommendation to retake JAMB";
        }
        
        // cout << "Estimated Cut-off for " << getFacultyName() << ": " << cutoffThreshold << "%\n";
        // cout << "========================================\n";
    }

    
    double getCutoffThreshold() const {
        // Different faculties have different competition levels
        string faculty = getFacultyName();
        if (faculty == "Medicine and Surgery") return 75.0;
        else if (faculty == "Law") return 70.0;
        else if (faculty == "Engineering") return 65.0;
        else if (faculty == "Allied Medical Sciences") return 65.0;
        else if (faculty == "Management Sciences") return 60.0;
        else if (faculty == "Social Sciences") return 58.0;
        else if (faculty == "Science/Basic Sciences") return 60.0;
        else if (faculty == "Environmental Sciences") return 55.0;
        else if (faculty == "Agriculture") return 50.0;
        else if (faculty == "Education") return 45.0;
        else if (faculty == "Arts and Humanities") return 50.0;
        else return 55.0; // default
    }
    
    double getFinalScreeningScore() const {
        return finalScreeningScore;
    }
};

// HTTP Server Classes
class HttpRequest
{
public:
    string method;
    string path;
    string version;
    unordered_map<string, string> headers;
    string body;

    static HttpRequest parse(const string &raw_request)
    {
        HttpRequest request;
        istringstream stream(raw_request);
        string line;

        // Parse request line
        if (getline(stream, line))
        {
            istringstream line_stream(line);
            line_stream >> request.method >> request.path >> request.version;
        }

        // Parse headers
        while (getline(stream, line) && !line.empty() && line != "\r")
        {
            size_t colon_pos = line.find(':');
            if (colon_pos != string::npos)
            {
                string key = line.substr(0, colon_pos);
                string value = line.substr(colon_pos + 1);

                // Trim whitespace
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);

                request.headers[key] = value;
            }
        }

        // Parse body (if any)
        string body_line;
        while (getline(stream, body_line))
        {
            request.body += body_line + "\n";
        }

        return request;
    }
};

class HttpResponse
{
public:
    int status_code;
    string status_text;
    unordered_map<string, string> headers;
    string body;

    HttpResponse(int code = 200, const string &text = "OK")
        : status_code(code), status_text(text)
    {
        headers["Content-Type"] = "text/html";
        headers["Server"] = "LASU-Screening-Server/1.0";
        headers["Connection"] = "close";
        headers["Access-Control-Allow-Origin"] = "*";
        headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
        headers["Access-Control-Allow-Headers"] = "Content-Type";
    }

    string toString() const
    {
        string response = "HTTP/1.1 " + to_string(status_code) + " " + status_text + "\r\n";

        auto headers_copy = headers;
        headers_copy["Content-Length"] = to_string(body.length());

        for (const auto &header : headers_copy)
        {
            response += header.first + ": " + header.second + "\r\n";
        }

        response += "\r\n" + body;
        return response;
    }
};

class LASUHttpServer
{
private:
    SOCKET server_socket;
    int port;
    bool running;

public:
    LASUHttpServer(int server_port) : port(server_port), running(false)
    {

#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw runtime_error("WSAStartup failed");
        }
#endif

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == INVALID_SOCKET)
        {
            throw runtime_error("Failed to create socket");
        }

        // Set socket options
        int opt = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,
                   reinterpret_cast<const char *>(&opt), sizeof(opt));
    }

    ~LASUHttpServer()
    {
        stop();
        closesocket(server_socket);
#ifdef _WIN32
        WSACleanup();
#endif
    }

    void start()
    {
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(server_socket, reinterpret_cast<sockaddr *>(&server_addr),
                 sizeof(server_addr)) == SOCKET_ERROR)
        {
            throw runtime_error("Failed to bind socket");
        }

        if (listen(server_socket, 10) == SOCKET_ERROR)
        {
            throw runtime_error("Failed to listen on socket");
        }

        running = true;
        cout << "LASU Screening HTTP Server started on port " << port << endl;
        cout << "Access the calculator at: http://localhost:" << port << endl;

        while (running)
        {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);

            SOCKET client_socket = accept(server_socket,
                                          reinterpret_cast<sockaddr *>(&client_addr),
                                          &client_len);

            if (client_socket != INVALID_SOCKET)
            {
                thread(&LASUHttpServer::handleClient, this, client_socket).detach();
            }
        }
    }

    void stop()
    {
        running = false;
    }

private:
    void handleClient(SOCKET client_socket)
    {
        char buffer[4096];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0';
            string raw_request(buffer);

            HttpRequest request = HttpRequest::parse(raw_request);
            HttpResponse response = handleRequest(request);

            string response_str = response.toString();
            send(client_socket, response_str.c_str(), response_str.length(), 0);
        }

        closesocket(client_socket);
    }

    HttpResponse handleRequest(const HttpRequest &request)
    {
        HttpResponse response;

        if (request.method == "OPTIONS")
        {
            response.status_code = 200;
            response.body = "";
            return response;
        }

        if (request.method == "GET")
        {
            if (request.path == "/" || request.path == "/index.html")
            {
                response.body = generateHomePage();
            }
            else if (request.path == "/api/subjects")
            {
                response.headers["Content-Type"] = "application/json";
                response.body = generateSubjectsJSON();
            }
            else
            {
                response = HttpResponse(404, "Not Found");
                response.body = "<h1>404 - Page Not Found</h1>";
            }
        }
        else if (request.method == "POST")
        {
            if (request.path == "/api/calculate")
            {
                response.headers["Content-Type"] = "application/json";
                response.body = handleCalculation(request.body);
            }
            else
            {
                response = HttpResponse(404, "Not Found");
                response.body = "<h1>404 - API Endpoint Not Found</h1>";
            }
        }
        else
        {
            response = HttpResponse(405, "Method Not Allowed");
            response.body = "<h1>405 - Method Not Allowed</h1>";
        }

        return response;
    }
string generateHomePage()
{
    return "<!DOCTYPE html>\n"
           "<html lang=\"en\">\n"
           "<head>\n"
           "    <meta charset=\"UTF-8\">\n"
           "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
           "    <title>LASU Screening Calculator</title>\n"
           "    <style>\n"
           "        * {\n"
           "            margin: 0;\n"
           "            padding: 0;\n"
           "            box-sizing: border-box;\n"
           "        }\n"
           "        \n"
           "        body {\n"
           "            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;\n"
           "            background: linear-gradient(135deg, #B794C7 0%, #A67FB8 50%, #8B6BAE 100%);\n"
           "            min-height: 100vh;\n"
           "            padding: 20px;\n"
           "            line-height: 1.6;\n"
           "        }\n"
           "        \n"
           "        .container {\n"
           "            max-width: 500px;\n"
           "            margin: 0 auto;\n"
           "            background: rgba(255, 255, 255, 0.95);\n"
           "            border-radius: 20px;\n"
           "            box-shadow: 0 20px 40px rgba(0,0,0,0.1);\n"
           "            overflow: hidden;\n"
           "            backdrop-filter: blur(10px);\n"
           "        }\n"
           "        \n"
           "        .header {\n"
           "            background: linear-gradient(135deg, #B794C7, #A67FB8);\n"
           "            color: white;\n"
           "            padding: 30px;\n"
           "            text-align: center;\n"
           "        }\n"
           "        \n"
           "        .header h1 {\n"
           "            font-size: 24px;\n"
           "            font-weight: 700;\n"
           "            margin-bottom: 8px;\n"
           "        }\n"
           "        \n"
           "        .header p {\n"
           "            font-size: 14px;\n"
           "            opacity: 0.9;\n"
           "        }\n"
           "        \n"
           "        .form-container {\n"
           "            padding: 30px;\n"
           "        }\n"
           "        \n"
           "        .section {\n"
           "            margin-bottom: 25px;\n"
           "        }\n"
           "        \n"
           "        .section-title {\n"
           "            font-size: 18px;\n"
           "            font-weight: 600;\n"
           "            color: #2d3748;\n"
           "            margin-bottom: 15px;\n"
           "            display: flex;\n"
           "            align-items: center;\n"
           "            gap: 8px;\n"
           "        }\n"
           "        \n"
           "        .section-title::before {\n"
           "            content: '';\n"
           "            width: 4px;\n"
           "            height: 20px;\n"
           "            background: #B794C7;\n"
           "            border-radius: 2px;\n"
           "        }\n"
           "        \n"
           "        .form-group {\n"
           "            margin-bottom: 20px;\n"
           "        }\n"
           "        \n"
           "        label {\n"
           "            display: block;\n"
           "            margin-bottom: 8px;\n"
           "            font-weight: 500;\n"
           "            color: #4a5568;\n"
           "            font-size: 14px;\n"
           "        }\n"
           "        \n"
           "        input[type=\"number\"], select {\n"
           "            width: 100%;\n"
           "            padding: 12px 16px;\n"
           "            border: 2px solid #e2e8f0;\n"
           "            border-radius: 10px;\n"
           "            font-size: 16px;\n"
           "            background: #ffffff;\n"
           "            transition: all 0.2s ease;\n"
           "            color: #2d3748;\n"
           "        }\n"
           "        \n"
           "        input[type=\"number\"]:focus, select:focus {\n"
           "            outline: none;\n"
           "            border-color: #B794C7;\n"
           "            box-shadow: 0 0 0 3px rgba(183, 148, 199, 0.1);\n"
           "        }\n"
           "        \n"
           "        input[type=\"number\"]::placeholder {\n"
           "            color: #a0aec0;\n"
           "        }\n"
           "        \n"
           "        .helper-text {\n"
           "            font-size: 12px;\n"
           "            color: #718096;\n"
           "            margin-top: 5px;\n"
           "        }\n"
           "        \n"
           "        .course-categories {\n"
           "            display: grid;\n"
           "            grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));\n"
           "            gap: 10px;\n"
           "            margin-top: 10px;\n"
           "        }\n"
           "        \n"
           "        .category-option {\n"
           "            position: relative;\n"
           "        }\n"
           "        \n"
           "        .category-option input[type=\"radio\"] {\n"
           "            display: none;\n"
           "        }\n"
           "        \n"
           "        .category-label {\n"
           "            display: block;\n"
           "            padding: 12px 16px;\n"
           "            background: #f7fafc;\n"
           "            border: 2px solid #e2e8f0;\n"
           "            border-radius: 10px;\n"
           "            text-align: center;\n"
           "            cursor: pointer;\n"
           "            transition: all 0.2s ease;\n"
           "            font-size: 14px;\n"
           "            font-weight: 500;\n"
           "        }\n"
           "        \n"
           "        .category-option input[type=\"radio\"]:checked + .category-label {\n"
           "            background: #B794C7;\n"
           "            color: white;\n"
           "            border-color: #B794C7;\n"
           "        }\n"
           "        \n"
           "        .subjects-container {\n"
           "            margin-top: 15px;\n"
           "        }\n"
           "        \n"
           "        .required-subjects, .optional-subjects {\n"
           "            margin-bottom: 20px;\n"
           "        }\n"
           "        \n"
           "        .subjects-header {\n"
           "            font-size: 14px;\n"
           "            font-weight: 600;\n"
           "            color: #4a5568;\n"
           "            margin-bottom: 10px;\n"
           "            padding: 8px 12px;\n"
           "            background: #f7fafc;\n"
           "            border-radius: 8px;\n"
           "        }\n"
           "        \n"
           "        .subject-row {\n"
           "            display: flex;\n"
           "            align-items: center;\n"
           "            gap: 12px;\n"
           "            margin-bottom: 12px;\n"
           "            padding: 12px;\n"
           "            background: #f8f9fa;\n"
           "            border-radius: 10px;\n"
           "            border: 1px solid #e2e8f0;\n"
           "        }\n"
           "        \n"
           "        .subject-name {\n"
           "            flex: 1;\n"
           "            font-weight: 500;\n"
           "            color: #2d3748;\n"
           "            font-size: 14px;\n"
           "        }\n"
           "        \n"
           "        .grade-select {\n"
           "            width: 80px;\n"
           "            padding: 8px 10px;\n"
           "            font-size: 14px;\n"
           "            border-radius: 6px;\n"
           "        }\n"
           "        \n"
           "        .optional-subject-input {\n"
           "            display: flex;\n"
           "            gap: 12px;\n"
           "            margin-bottom: 12px;\n"
           "        }\n"
           "        \n"
           "        .optional-subject-input input[type=\"text\"] {\n"
           "            flex: 1;\n"
           "            padding: 10px 14px;\n"
           "            border: 1px solid #e2e8f0;\n"
           "            border-radius: 8px;\n"
           "            font-size: 14px;\n"
           "        }\n"
           "        \n"
           "        .add-subject-btn {\n"
           "            padding: 8px 16px;\n"
           "            background: #e2e8f0;\n"
           "            color: #4a5568;\n"
           "            border: none;\n"
           "            border-radius: 6px;\n"
           "            font-size: 12px;\n"
           "            cursor: pointer;\n"
           "            transition: all 0.2s ease;\n"
           "        }\n"
           "        \n"
           "        .add-subject-btn:hover {\n"
           "            background: #cbd5e0;\n"
           "        }\n"
           "        \n"
           "        .btn-calculate {\n"
           "            width: 100%;\n"
           "            padding: 16px;\n"
           "            background: linear-gradient(135deg, #B794C7 0%, #A67FB8 100%);\n"
           "            color: white;\n"
           "            border: none;\n"
           "            border-radius: 12px;\n"
           "            font-size: 16px;\n"
           "            font-weight: 600;\n"
           "            cursor: pointer;\n"
           "            transition: all 0.3s ease;\n"
           "            margin-top: 20px;\n"
           "        }\n"
           "        \n"
           "        .btn-calculate:hover {\n"
           "            transform: translateY(-2px);\n"
           "            box-shadow: 0 10px 20px rgba(183, 148, 199, 0.3);\n"
           "        }\n"
           "        \n"
           "        .btn-calculate:disabled {\n"
           "            opacity: 0.6;\n"
           "            cursor: not-allowed;\n"
           "            transform: none;\n"
           "        }\n"
           "        \n"
           "        .results {\n"
           "            margin-top: 25px;\n"
           "            padding: 20px;\n"
           "            background: linear-gradient(135deg, #f0fff4 0%, #e6fffa 100%);\n"
           "            border-radius: 12px;\n"
           "            border: 1px solid #9ae6b4;\n"
           "        }\n"
           "        \n"
           "        .results.hidden {\n"
           "            display: none;\n"
           "        }\n"
           "        \n"
           "        .results h3 {\n"
           "            color: #2d3748;\n"
           "            margin-bottom: 15px;\n"
           "            font-size: 18px;\n"
           "        }\n"
           "        \n"
           "        .result-item {\n"
           "            display: flex;\n"
           "            justify-content: space-between;\n"
           "            align-items: center;\n"
           "            margin-bottom: 12px;\n"
           "            padding: 10px 0;\n"
           "            border-bottom: 1px solid rgba(0,0,0,0.1);\n"
           "        }\n"
           "        \n"
           "        .result-item:last-child {\n"
           "            border-bottom: none;\n"
           "            font-weight: 700;\n"
           "            font-size: 1.1em;\n"
           "            padding: 15px 0 5px 0;\n"
           "            background: rgba(183, 148, 199, 0.1);\n"
           "            border-radius: 8px;\n"
           "            padding-left: 12px;\n"
           "            padding-right: 12px;\n"
           "        }\n"
           "        \n"
           "        .result-label {\n"
           "            color: #4a5568;\n"
           "        }\n"
           "        \n"
           "        .result-value {\n"
           "            font-weight: 600;\n"
           "            color: #2d3748;\n"
           "        }\n"
           "        \n"
           "        .status-indicator {\n"
           "            display: inline-block;\n"
           "            padding: 4px 12px;\n"
           "            border-radius: 20px;\n"
           "            font-size: 12px;\n"
           "            font-weight: 600;\n"
           "            text-transform: uppercase;\n"
           "        }\n"
           "        \n"
           "        .status-excellent {\n"
           "            background: #c6f6d5;\n"
           "            color: #22543d;\n"
           "        }\n"
           "        \n"
           "        .status-good {\n"
           "            background: #bee3f8;\n"
           "            color: #2a4365;\n"
           "        }\n"
           "        \n"
           "        .status-fair {\n"
           "            background: #fef5e7;\n"
           "            color: #744210;\n"
           "        }\n"
           "        \n"
           "        .status-poor {\n"
           "            background: #fed7d7;\n"
           "            color: #742a2a;\n"
           "        }\n"
           "        \n"
           "        .error-message {\n"
           "            background: #fed7d7;\n"
           "            color: #742a2a;\n"
           "            padding: 12px;\n"
           "            border-radius: 8px;\n"
           "            margin-top: 15px;\n"
           "            font-size: 14px;\n"
           "        }\n"
           ".course-categories-wrapper {\n"
            "  display: flex;\n"
            "  flex-wrap: wrap;\n"
            "  gap: 10px;\n"
            "  max-height: 55px;\n"
            "  overflow: hidden;\n"
            "  position: relative;\n"
            "}\n"
            ".course-categories-wrapper.expanded {\n"
            "  max-height: none;\n"
            "}\n"
            ".show-more-btn {\n"
            "  margin-top: 10px;\n"
            "  background: #B794C7;\n"
            "  color: white;\n"
            "  padding: 6px 14px;\n"
            "  border-radius: 8px;\n"
            "  cursor: pointer;\n"
            "  font-size: 12px;\n"
            "}\n"

           "    </style>\n"
           "</head>\n"
           "<body>\n"
           "    <div class=\"container\">\n"
           "        <div class=\"header\">\n"
           "            <h1>LASU Screening Calculator</h1>\n"
           "            <p>Calculate your admission screening score</p>\n"
           "        </div>\n"
           "        \n"
           "        <div class=\"form-container\">\n"
           "            <form id=\"calculatorForm\">\n"
           "                <!-- JAMB Score Section -->\n"
           "                <div class=\"section\">\n"
           "                    <h2 class=\"section-title\">JAMB Score</h2>\n"
           "                    <div class=\"form-group\">\n"
           "                        <label for=\"jambScore\">JAMB UTME Score</label>\n"
           "                        <input type=\"number\" id=\"jambScore\" min=\"0\" max=\"400\" placeholder=\"Enter your JAMB score (0-400)\" required>\n"
           "                        <div class=\"helper-text\">Enter your JAMB UTME score from your result slip</div>\n"
           "                    </div>\n"
           "                </div>\n"
           "                \n"
           "                <!-- Course Category Section -->\n"
           "                <div class=\"section\">\n"
           "                    <h2 class=\"section-title\">Course Category</h2>\n"
                            "<div class=\"form-group\">\n"
                            "  <label>Select your course category</label>\n"
                            "  <div class=\"course-categories-wrapper course-categories\">\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"science\" name=\"courseCategory\" value=\"1\" required checked>\n"
                            "      <label for=\"science\" class=\"category-label\">Science</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"arts\" name=\"courseCategory\" value=\"2\" required>\n"
                            "      <label for=\"arts\" class=\"category-label\">Arts</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"management\" name=\"courseCategory\" value=\"3\" required>\n"
                            "      <label for=\"management\" class=\"category-label\">Management</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"engineering\" name=\"courseCategory\" value=\"4\" required>\n"
                            "      <label for=\"engineering\" class=\"category-label\">Engineering</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"medicine\" name=\"courseCategory\" value=\"5\" required>\n"
                            "      <label for=\"medicine\" class=\"category-label\">Medicine</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"law\" name=\"courseCategory\" value=\"6\" required>\n"
                            "      <label for=\"law\" class=\"category-label\">Law</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"education\" name=\"courseCategory\" value=\"7\" required>\n"
                            "      <label for=\"education\" class=\"category-label\">Education</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"agriculture\" name=\"courseCategory\" value=\"8\" required>\n"
                            "      <label for=\"agriculture\" class=\"category-label\">Agriculture</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"environmental\" name=\"courseCategory\" value=\"9\" required>\n"
                            "      <label for=\"environmental\" class=\"category-label\">Environmental</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"social\" name=\"courseCategory\" value=\"10\" required>\n"
                            "      <label for=\"social\" class=\"category-label\">Social Sciences</label>\n"
                            "    </div>\n"
                            "    <div class=\"category-option\">\n"
                            "      <input type=\"radio\" id=\"allied\" name=\"courseCategory\" value=\"11\" required>\n"
                            "      <label for=\"allied\" class=\"category-label\">Allied Medical</label>\n"
                            "    </div>\n"
                            "  </div>\n"
                            "  <button type=\"button\" id=\"showMoreBtn\" class=\"show-more-btn\">Show More</button>\n"
                            "</div>\n"

           "                </div>\n"
           "                \n"
           "                <!-- O'Level Grades Section -->\n"
           "                <div class=\"section\">\n"
           "                    <h2 class=\"section-title\">O'Level Results</h2>\n"
           "                    \n"
           "                    <!-- Required Subjects -->\n"
           "                    <div class=\"required-subjects\">\n"
           "                        <div class=\"subjects-header\">Required Subjects</div>\n"
           "                        <div id=\"requiredSubjects\" class=\"subjects-container\">\n"
           "                            <!-- Dynamic content will be inserted here -->\n"
           "                        </div>\n"
           "                    </div>\n"
           "                    \n"
           "                    <!-- Optional Subjects -->\n"
           "                    <div class=\"optional-subjects\">\n"
           "                        <div class=\"subjects-header\">Optional Subjects (Best grade will be selected)</div>\n"
           "                        <div id=\"optionalSubjects\" class=\"subjects-container\">\n"
           "                            <div class=\"optional-subject-input\">\n"
           "                                <input type=\"text\" placeholder=\"Subject name\" class=\"optional-subject-name\">\n"
           "                                <select class=\"grade-select optional-subject-grade\">\n"
           "                                    <option value=\"\">Grade</option>\n"
           "                                    <option value=\"A1\">A1</option>\n"
           "                                    <option value=\"B2\">B2</option>\n"
           "                                    <option value=\"B3\">B3</option>\n"
           "                                    <option value=\"C4\">C4</option>\n"
           "                                    <option value=\"C5\">C5</option>\n"
           "                                    <option value=\"C6\">C6</option>\n"
           "                                    <option value=\"D7\">D7</option>\n"
           "                                    <option value=\"E8\">E8</option>\n"
           "                                    <option value=\"F9\">F9</option>\n"
           "                                </select>\n"
           "                            </div>\n"
           "                        </div>\n"
           "                        <button type=\"button\" class=\"add-subject-btn\" onclick=\"addOptionalSubject()\">+ Add Another Subject</button>\n"
           "                    </div>\n"
           "                </div>\n"
           "                \n"
           "                <button type=\"submit\" class=\"btn-calculate\">Calculate Screening Score</button>\n"
           "            </form>\n"
           "            \n"
           "            <div id=\"results\" class=\"results hidden\">\n"
           "                <h3>Screening Results</h3>\n"
           "                <div id=\"resultsList\"></div>\n"
           "            </div>\n"
           "        </div>\n"
           "    </div>\n"
           "\n"
           "    <script>\n"
"const subjectMappings = {\n"
"  1: { required: ['Mathematics', 'English Language', 'Physics', 'Chemistry'], category: 'Science/Basic Sciences' },\n"
"  2: { required: ['English Language', 'Government', 'Literature in English', 'Mathematics'], category: 'Arts and Humanities' },\n"
"  3: { required: ['English Language', 'Mathematics', 'Economics', 'Commerce'], category: 'Management Sciences' },\n"
"  4: { required: ['Mathematics', 'English Language', 'Physics', 'Chemistry'], category: 'Engineering' },\n"
"  5: { required: ['Mathematics', 'English Language', 'Physics', 'Chemistry', 'Biology'], category: 'Medicine and Surgery' },\n"
"  6: { required: ['English Language', 'Mathematics', 'Government', 'Literature in English'], category: 'Law' },\n"
"  7: { required: ['English Language', 'Mathematics', 'Government', 'Economics'], category: 'Education' },\n"
"  8: { required: ['Mathematics', 'English Language', 'Chemistry', 'Biology'], category: 'Agriculture' },\n"
"  9: { required: ['Mathematics', 'English Language', 'Physics', 'Geography'], category: 'Environmental Sciences' },\n"
" 10: { required: ['English Language', 'Mathematics', 'Government', 'Economics'], category: 'Social Sciences' },\n"
" 11: { required: ['English Language', 'Mathematics', 'Physics', 'Chemistry', 'Biology'], category: 'Allied Medical Sciences' }\n"
"};\n"

           "\n"
"document.addEventListener('DOMContentLoaded', function() {\n"
"  const radioButtons = document.querySelectorAll('input[name=\"courseCategory\"]');\n"
"  radioButtons.forEach(radio => radio.addEventListener('change', updateRequiredSubjects));\n"
"\n"
"  const defaultRadio = document.querySelector('input[name=\"courseCategory\"][value=\"1\"]');\n"
"  if (defaultRadio) {\n"
"    defaultRadio.checked = true;\n"
"    updateRequiredSubjects();\n"
"  }\n"
"\n"
"  const toggleBtn = document.getElementById('showMoreBtn');\n"
"  const categoryWrapper = document.querySelector('.course-categories-wrapper');\n"
"  if (toggleBtn && categoryWrapper) {\n"
"    toggleBtn.addEventListener('click', function() {\n"
"      categoryWrapper.classList.toggle('expanded');\n"
"      toggleBtn.textContent = categoryWrapper.classList.contains('expanded') ? 'Show Less' : 'Show More';\n"
"    });\n"
"  }\n"
"});\n"

           "\n"
           "        function updateRequiredSubjects() {\n"
           "            const selectedCategory = document.querySelector('input[name=\"courseCategory\"]:checked');\n"
           "            if (!selectedCategory) return;\n"
           "\n"
           "            const categoryValue = parseInt(selectedCategory.value);\n"
           "            const subjects = subjectMappings[categoryValue]?.required;\n"
           "            const container = document.getElementById('requiredSubjects');\n"
           "\n"
           "            if (!container || !subjects) return;\n"
           "\n"
           "            container.innerHTML = '';\n"
           "            subjects.forEach(subject => {\n"
           "                const subjectRow = createSubjectRow(subject, true);\n"
           "                container.appendChild(subjectRow);\n"
           "            });\n"
           "        }\n"
           "\n"
           "        function createSubjectRow(subjectName, isRequired = false) {\n"
           "            const div = document.createElement('div');\n"
           "            div.className = 'subject-row';\n"
           "\n"
           "            const nameSpan = document.createElement('span');\n"
           "            nameSpan.className = 'subject-name';\n"
           "            nameSpan.textContent = subjectName;\n"
           "\n"
           "            const select = document.createElement('select');\n"
           "            select.className = 'grade-select';\n"
           "            select.required = isRequired;\n"
           "            select.setAttribute('data-subject', subjectName);\n"
           "\n"
           "            const grades = ['', 'A1', 'B2', 'B3', 'C4', 'C5', 'C6', 'D7', 'E8', 'F9'];\n"
           "            grades.forEach(grade => {\n"
           "                const option = document.createElement('option');\n"
           "                option.value = grade;\n"
           "                option.textContent = grade || 'Grade';\n"
           "                select.appendChild(option);\n"
           "            });\n"
           "\n"
           "            div.appendChild(nameSpan);\n"
           "            div.appendChild(select);\n"
           "            return div;\n"
           "        }\n"
           "\n"
           "        function addOptionalSubject() {\n"
           "            const container = document.getElementById('optionalSubjects');\n"
           "            const newInput = document.createElement('div');\n"
           "            newInput.className = 'optional-subject-input';\n"
           "            newInput.innerHTML = '<input type=\"text\" placeholder=\"Subject name\" class=\"optional-subject-name\"><select class=\"grade-select optional-subject-grade\"><option value=\"\">Grade</option><option value=\"A1\">A1</option><option value=\"B2\">B2</option><option value=\"B3\">B3</option><option value=\"C4\">C4</option><option value=\"C5\">C5</option><option value=\"C6\">C6</option><option value=\"D7\">D7</option><option value=\"E8\">E8</option><option value=\"F9\">F9</option></select>';\n"
           "            container.appendChild(newInput);\n"
           "        }\n"
           "\n"
           "        document.getElementById('calculatorForm').addEventListener('submit', async function(e) {\n"
           "            e.preventDefault();\n"
           "\n"
           "            const jambScore = parseInt(document.getElementById('jambScore').value);\n"
           "            const courseCategory = parseInt(document.querySelector('input[name=\"courseCategory\"]:checked').value);\n"
           "\n"
           "            const requiredSubjects = {};\n"
           "            document.querySelectorAll('#requiredSubjects .subject-row').forEach(row => {\n"
           "                const subject = row.querySelector('.subject-name').textContent;\n"
           "                const grade = row.querySelector('.grade-select').value;\n"
           "                if (grade) {\n"
           "                    requiredSubjects[subject] = grade;\n"
           "                }\n"
           "            });\n"
           "\n"
           "            const optionalSubjects = [];\n"
           "            document.querySelectorAll('#optionalSubjects .optional-subject-input').forEach(row => {\n"
           "                const name = row.querySelector('.optional-subject-name').value.trim();\n"
           "                const grade = row.querySelector('.optional-subject-grade').value;\n"
           "                if (name && grade) {\n"
           "                    optionalSubjects.push({ name, grade });\n"
           "                }\n"
           "            });\n"
           "\n"
           "            const requestData = {\n"
           "                jambScore: jambScore,\n"
           "                courseCategory: courseCategory,\n"
           "                requiredSubjects: requiredSubjects,\n"
           "                optionalSubjects: optionalSubjects\n"
           "            };\n"
           "\n"
           "            try {\n"
           "                const response = await fetch('/api/calculate', {\n"
           "                    method: 'POST',\n"
           "                    headers: { 'Content-Type': 'application/json' },\n"
           "                    body: JSON.stringify(requestData)\n"
           "                });\n"
           "\n"
           "                if (!response.ok) {\n"
           "                    throw new Error('Network response was not ok');\n"
           "                }\n"
           "\n"
           "                const result = await response.json();\n"
           "                displayResults(result);\n"
           "            } catch (error) {\n"
           "                console.error('Error:', error);\n"
           "                displayError('Failed to calculate screening score. Please try again.');\n"
           "            }\n"
           "        });\n"
           "\n"
           "        function displayResults(result) {\n"
           "            const resultsDiv = document.getElementById('results');\n"
           "            const resultsList = document.getElementById('resultsList');\n"
           "\n"
           "            if (result.error) {\n"
           "                resultsList.innerHTML = '<div class=\"error-message\">Error: ' + result.error + '</div>';\n"
           "            } else {\n"
           "                const statusClass = 'status-' + result.status;\n"
           "                resultsList.innerHTML = '<div class=\"result-item\"><span class=\"result-label\">JAMB Score:</span><span class=\"result-value\">' + result.jambScore + '/400</span></div><div class=\"result-item\"><span class=\"result-label\">JAMB Contribution:</span><span class=\"result-value\">' + result.jambPercentage.toFixed(1) + '%</span></div><div class=\"result-item\"><span class=\"result-label\">O\\'Level Points:</span><span class=\"result-value\">' + result.waecScore + '/40</span></div><div class=\"result-item\"><span class=\"result-label\">O\\'Level Contribution:</span><span class=\"result-value\">' + result.waecPercentage.toFixed(1) + '%</span></div><div class=\"result-item\"><span class=\"result-label\">Final Screening Score:</span><span class=\"result-value\">' + result.finalScore.toFixed(2) + '%</span></div><div class=\"result-item\"><span class=\"result-label\">Admission Status:</span><span class=\"status-indicator ' + statusClass + '\">' + result.admissionStatus + '</span></div>';\n"
           "            }\n"
           "\n"
           "            resultsDiv.classList.remove('hidden');\n"
           "            resultsDiv.scrollIntoView({ behavior: 'smooth' });\n"
           "        }\n"
           "\n"
           "        function displayError(message) {\n"
           "            const resultsDiv = document.getElementById('results');\n"
           "            const resultsList = document.getElementById('resultsList');\n"
           "            resultsList.innerHTML = '<div class=\"error-message\">Error: ' + message + '</div>';\n"
           "            resultsDiv.classList.remove('hidden');\n"
           "        }\n"
           "    </script>\n"
           "</body>\n"
           "</html>";
}
    string generateSubjectsJSON()
    {
        return R"({
            "science": ["Mathematics", "English Language", "Physics", "Chemistry"],
            "arts": ["English Language", "Government", "Literature in English", "Mathematics"],
            "commercial": ["English Language", "Mathematics", "Economics", "Commerce"]
        })";
    }

    string handleCalculation(const string &json_body)
    {
        try
        {
            // JSON parsing
            regex course_regex("\"courseCategory\"\\s*:\\s*(\\d+)");
            regex jamb_regex("\"jambScore\"\\s*:\\s*(\\d+)");
            regex required_regex("\"([^\"]+)\"\\s*:\\s*\"([^\"]+)\"");
            regex optional_name_regex("\"name\"\\s*:\\s*\"([^\"]+)\"");
            regex optional_grade_regex("\"grade\"\\s*:\\s*\"([^\"]+)\"");

            smatch match;

            // Parse course category
            int courseCategory = 0;
            if (regex_search(json_body, match, course_regex))
            {
                courseCategory = stoi(match[1].str());
            }

            // Parse JAMB score
            int jambScore = 0;
            if (regex_search(json_body, match, jamb_regex))
            {
                jambScore = stoi(match[1].str());
            }

            if (courseCategory == 0 || jambScore == 0)
            {
                return "{\"error\": \"Invalid course category or JAMB score\"}";
            }

            // Create calculator instance
            LASUScreeningAggregator calculator;
            calculator.setJambScore(jambScore);
            calculator.setRequiredSubjects(courseCategory);

            // Parse required subjects
            string requiredSubjectsSection = extractSection(json_body, "requiredSubjects");
            if (!requiredSubjectsSection.empty())
            {
                auto subjects = parseRequiredSubjects(requiredSubjectsSection);
                for (const auto &subject : subjects)
                {
                    calculator.addGrade(subject.first, subject.second);
                }
            }

            // Parse optional subjects
            string optionalSubjectsSection = extractSection(json_body, "optionalSubjects");
            if (!optionalSubjectsSection.empty())
            {
                auto optionalSubjects = parseOptionalSubjects(optionalSubjectsSection);
                for (const auto &subject : optionalSubjects)
                {
                    calculator.addOptionalSubject(subject.first, subject.second);
                }
            }

            // Calculate results
            calculator.calculateScreeningResults();

            // Generate JSON response
            ostringstream response;
            response << fixed << setprecision(1);
            response << "{"
                     << "\"jambScore\": " << calculator.getJambScore() << ","
                     << "\"jambPercentage\": " << calculator.getJambPercentage() << ","
                     << "\"waecScore\": " << calculator.getTotalScore() << ","
                     << "\"waecPercentage\": " << calculator.getPercentage() << ","
                     << "\"finalScore\": " << calculator.getFinalScreeningScore() << ","
                     << "\"admissionStatus\": \"" << calculator.getAdmissionStatus() << "\","
                     << "\"status\": \"" << getStatusClass(calculator.getFinalScreeningScore()) << "\""
                     << "}";

            return response.str();
        }
        catch (const exception &e)
        {
            return "{\"error\": \"" + string(e.what()) + "\"}";
        }
    }

private:
    string extractSection(const string &json, const string &section)
    {
        string pattern = "\"" + section + "\"\\s*:\\s*\\{([^}]*)\\}";
        regex section_regex(pattern);
        smatch match;

        if (regex_search(json, match, section_regex))
        {
            return match[1].str();
        }

        // Try array format for optional subjects
        if (section == "optionalSubjects")
        {
            pattern = "\"" + section + "\"\\s*:\\s*\\[([^\\]]*)\\]";
            regex array_regex(pattern);
            if (regex_search(json, match, array_regex))
            {
                return match[1].str();
            }
        }

        return "";
    }

    vector<pair<string, string>> parseRequiredSubjects(const string &section)
    {
        vector<pair<string, string>> subjects;
        regex subject_regex("\"([^\"]+)\"\\s*:\\s*\"([^\"]+)\"");
        sregex_iterator iter(section.begin(), section.end(), subject_regex);
        sregex_iterator end;

        for (; iter != end; ++iter)
        {
            smatch match = *iter;
            string subject = match[1].str();
            string grade = match[2].str();

            // Replace underscores with spaces (from form field names)
            replace(subject.begin(), subject.end(), '_', ' ');

            subjects.push_back({subject, grade});
        }

        return subjects;
    }

    vector<pair<string, string>> parseOptionalSubjects(const string &section)
    {
        vector<pair<string, string>> subjects;

        // Parse array of objects: [{"name":"Subject","grade":"A1"}...]
        regex object_regex("\\{[^}]*\\}");
        sregex_iterator iter(section.begin(), section.end(), object_regex);
        sregex_iterator end;

        for (; iter != end; ++iter)
        {
            string object = iter->str();

            regex name_regex("\"name\"\\s*:\\s*\"([^\"]+)\"");
            regex grade_regex("\"grade\"\\s*:\\s*\"([^\"]+)\"");

            smatch name_match, grade_match;

            if (regex_search(object, name_match, name_regex) &&
                regex_search(object, grade_match, grade_regex))
            {
                subjects.push_back({name_match[1].str(), grade_match[1].str()});
            }
        }

        return subjects;
    }

    string getStatusClass(double score)
    {
        if (score >= 70.0)
            return "excellent";
        if (score >= 60.0)
            return "good";
        if (score >= 50.0)
            return "fair";
        return "poor";
    }
};

int main()
{
    try
    {
        cout << "Starting LASU Screening HTTP Server..." << endl;

        LASUHttpServer server(8080);

        cout << "Server will start on port 8080" << endl;
        cout << "Press Ctrl+C to stop the server" << endl;

        server.start();
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
