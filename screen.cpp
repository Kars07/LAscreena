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

// LASU Screening Calculator Classes (from your screen.cpp)
class JAMBAllocation {
private:
    int jambScore;
    double jambPercentage;
    
public:
    JAMBAllocation() : jambScore(0), jambPercentage(0.0) {}
    
    void setJambScore(int score) {
        if (score < 0 || score > 400) {
            throw  invalid_argument("Invalid JAMB score. Score must be between 0 and 400.");
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
};

class WAECAllocation {
private:
     vector< pair< string,  string>> waecGrades;
     unordered_map< string, int> gradePoints;
     unordered_map< string, int> requiredSubjects;
     vector< pair< string,  string>> optionalSubjects;
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
                requiredSubjects = {
                    {"Mathematics", 0},
                    {"English Language", 0},
                    {"Physics", 0},
                    {"Chemistry", 0}
                };
                break;
                
            case 2: // Arts
                requiredSubjects = {
                    {"English Language", 0},
                    {"Government", 0},
                    {"Literature in English", 0},
                    {"Mathematics", 0}
                };
                break;
                
            case 3: // Commercial
                requiredSubjects = {
                    {"English Language", 0},
                    {"Mathematics", 0},
                    {"Economics", 0},
                    {"Commerce", 0}
                };
                break;
                
            default:
                throw  invalid_argument("Invalid choice");
        }
    }
    
    void addGrade(const  string& subject, const  string& grade) {
        if (gradePoints.find(grade) == gradePoints.end()) {
            throw  invalid_argument("Invalid grade: " + grade);
        }
        waecGrades.push_back({subject, grade});
    }
    
    void addOptionalSubject(const  string& subject, const  string& grade) {
        if (gradePoints.find(grade) == gradePoints.end()) {
            throw  invalid_argument("Invalid grade: " + grade);
        }
        optionalSubjects.push_back({subject, grade});
    }
    
    int getGradePoint(const  string& grade) const {
        auto it = gradePoints.find(grade);
        return (it != gradePoints.end()) ? it->second : 0;
    }
    
    void calculateWaecAllocation() {
        unordered_map< string,  string> gradesMap;
        for (const auto& grade : waecGrades) {
            gradesMap[grade.first] = grade.second;
        }
        
        vector<pair<string, int>> requiredScores;
        for (auto& required : requiredSubjects) {
            const  string& subject = required.first;
            auto it = gradesMap.find(subject);
            if (it != gradesMap.end()) {
                int points = getGradePoint(it->second);
                required.second = points;
                requiredScores.push_back({subject, points});
            } else {
                required.second = 0;
                requiredScores.push_back({subject, 0});
            }
        }
        
        vector< pair< string, int>> optionalScores;
        for (const auto& optional : optionalSubjects) {
            int points = getGradePoint(optional.second);
            optionalScores.push_back({optional.first, points});
        }
        
        sort(optionalScores.begin(), optionalScores.end(), 
             [](const  pair< string, int>& a, const  pair< string, int>& b) {
                 return a.second > b.second;
             });
        
        totalScore = 0;
        for (const auto& required : requiredScores) {
            totalScore += required.second;
        }
        
        if (!optionalScores.empty()) {
            totalScore += optionalScores[0].second;
        }
        
        waecPercentage = (totalScore / 40.0) * 40.0;
    }
    
    // Getters
    double getPercentage() const { return waecPercentage; }
    int getTotalScore() const { return totalScore; }
    
    const  unordered_map< string, int>& getRequiredSubjects() const {
        return requiredSubjects;
    }
    
    const  vector< pair< string,  string>>& getOptionalSubjects() const {
        return optionalSubjects;
    }
};

class LASUScreeningAggregator : public JAMBAllocation, public WAECAllocation {
private:
    double finalScreeningScore;
    
public:
    LASUScreeningAggregator() : finalScreeningScore(0.0) {}
    
    void calculateScreeningResults() {
        calculateWaecAllocation();
        finalScreeningScore = getJambPercentage() + getPercentage();
    }
    
    double getFinalScreeningScore() const {
        return finalScreeningScore;
    }
    
     string getAdmissionStatus() const {
        if (finalScreeningScore >= 70.0) {
            return "EXCELLENT - High chance of admission!";
        } else if (finalScreeningScore >= 60.0) {
            return "GOOD - Moderate chance of admission";
        } else if (finalScreeningScore >= 50.0) {
            return "FAIR - Consider retaking JAMB or improving WAEC";
        } else {
            return "POOR - Strong recommendation to retake JAMB";
        }
    }
};

// HTTP Server Classes
class HttpRequest {
public:
     string method;
     string path;
     string version;
     unordered_map< string,  string> headers;
     string body;
    
    static HttpRequest parse(const  string& raw_request) {
        HttpRequest request;
         istringstream stream(raw_request);
         string line;
        
        // Parse request line
        if ( getline(stream, line)) {
             istringstream line_stream(line);
            line_stream >> request.method >> request.path >> request.version;
        }
        
        // Parse headers
        while ( getline(stream, line) && !line.empty() && line != "\r") {
            size_t colon_pos = line.find(':');
            if (colon_pos !=  string::npos) {
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
        while ( getline(stream, body_line)) {
            request.body += body_line + "\n";
        }
        
        return request;
    }
};

class HttpResponse {
public:
    int status_code;
     string status_text;
     unordered_map< string,  string> headers;
     string body;
    
    HttpResponse(int code = 200, const  string& text = "OK") 
        : status_code(code), status_text(text) {
        headers["Content-Type"] = "text/html";
        headers["Server"] = "LASU-Screening-Server/1.0";
        headers["Connection"] = "close";
        headers["Access-Control-Allow-Origin"] = "*";
        headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
        headers["Access-Control-Allow-Headers"] = "Content-Type";
    }
    
     string toString() const {
         string response = "HTTP/1.1 " +  to_string(status_code) + " " + status_text + "\r\n";
        
        auto headers_copy = headers;
        headers_copy["Content-Length"] =  to_string(body.length());
        
        for (const auto& header : headers_copy) {
            response += header.first + ": " + header.second + "\r\n";
        }
        
        response += "\r\n" + body;
        return response;
    }
};

class LASUHttpServer {
private:
    SOCKET server_socket;
    int port;
    bool running;

public:
    LASUHttpServer(int server_port) : port(server_port), running(false) {
        
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw  runtime_error("WSAStartup failed");
        }
#endif
        
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == INVALID_SOCKET) {
            throw  runtime_error("Failed to create socket");
        }
        
        // Set socket options
        int opt = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 
                  reinterpret_cast<const char*>(&opt), sizeof(opt));
    }
    
    ~LASUHttpServer() {
        stop();
        closesocket(server_socket);
#ifdef _WIN32
        WSACleanup();
#endif
    }
    
    void start() {
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);
        
        if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), 
                sizeof(server_addr)) == SOCKET_ERROR) {
            throw  runtime_error("Failed to bind socket");
        }
        
        if (listen(server_socket, 10) == SOCKET_ERROR) {
            throw  runtime_error("Failed to listen on socket");
        }
        
        running = true;
         cout << "LASU Screening HTTP Server started on port " << port <<  endl;
         cout << "Access the calculator at: http://localhost:" << port <<  endl;
        
        while (running) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            
            SOCKET client_socket = accept(server_socket, 
                                        reinterpret_cast<sockaddr*>(&client_addr), 
                                        &client_len);
            
            if (client_socket != INVALID_SOCKET) {
                 thread(&LASUHttpServer::handleClient, this, client_socket).detach();
            }
        }
    }
    
    void stop() {
        running = false;
    }

private:
    void handleClient(SOCKET client_socket) {
        char buffer[4096];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
             string raw_request(buffer);
            
            HttpRequest request = HttpRequest::parse(raw_request);
            HttpResponse response = handleRequest(request);
            
             string response_str = response.toString();
            send(client_socket, response_str.c_str(), response_str.length(), 0);
        }
        
        closesocket(client_socket);
    }
    
    HttpResponse handleRequest(const HttpRequest& request) {
        HttpResponse response;
        
        if (request.method == "OPTIONS") {
            response.status_code = 200;
            response.body = "";
            return response;
        }
        
        if (request.method == "GET") {
            if (request.path == "/" || request.path == "/index.html") {
                response.body = generateHomePage();
            } else if (request.path == "/api/subjects") {
                response.headers["Content-Type"] = "application/json";
                response.body = generateSubjectsJSON();
            } else {
                response = HttpResponse(404, "Not Found");
                response.body = "<h1>404 - Page Not Found</h1>";
            }
        } else if (request.method == "POST") {
            if (request.path == "/api/calculate") {
                response.headers["Content-Type"] = "application/json";
                response.body = handleCalculation(request.body);
            } else {
                response = HttpResponse(404, "Not Found");
                response.body = "<h1>404 - API Endpoint Not Found</h1>";
            }
        } else {
            response = HttpResponse(405, "Method Not Allowed");
            response.body = "<h1>405 - Method Not Allowed</h1>";
        }
        
        return response;
    }
    
string generateHomePage() {
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
           "            font-family: 'Arial', sans-serif;\n"
           "            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
           "            min-height: 100vh;\n"
           "            padding: 20px;\n"
           "        }\n"
           "        \n"
           "        .container {\n"
           "            max-width: 800px;\n"
           "            margin: 0 auto;\n"
           "            background: white;\n"
           "            border-radius: 10px;\n"
           "            box-shadow: 0 10px 30px rgba(0,0,0,0.3);\n"
           "            overflow: hidden;\n"
           "        }\n"
           "        \n"
           "        .header {\n"
           "            background: linear-gradient(135deg, #2c3e50 0%, #3498db 100%);\n"
           "            color: white;\n"
           "            padding: 30px;\n"
           "            text-align: center;\n"
           "        }\n"
           "        \n"
           "        .header h1 {\n"
           "            font-size: 2.5em;\n"
           "            margin-bottom: 10px;\n"
           "        }\n"
           "        \n"
           "        .header p {\n"
           "            font-size: 1.2em;\n"
           "            opacity: 0.9;\n"
           "        }\n"
           "        \n"
           "        .form-container {\n"
           "            padding: 30px;\n"
           "        }\n"
           "        \n"
           "        .form-group {\n"
           "            margin-bottom: 20px;\n"
           "        }\n"
           "        \n"
           "        label {\n"
           "            display: block;\n"
           "            margin-bottom: 8px;\n"
           "            font-weight: bold;\n"
           "            color: #333;\n"
           "        }\n"
           "        \n"
           "        select, input[type=\"number\"] {\n"
           "            width: 100%;\n"
           "            padding: 12px;\n"
           "            border: 2px solid #ddd;\n"
           "            border-radius: 6px;\n"
           "            font-size: 16px;\n"
           "            transition: border-color 0.3s;\n"
           "        }\n"
           "        \n"
           "        select:focus, input[type=\"number\"]:focus {\n"
           "            outline: none;\n"
           "            border-color: #3498db;\n"
           "        }\n"
           "        \n"
           "        .subjects-grid {\n"
           "            display: grid;\n"
           "            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));\n"
           "            gap: 20px;\n"
           "            margin-bottom: 20px;\n"
           "        }\n"
           "        \n"
           "        .subject-group {\n"
           "            background: #f8f9fa;\n"
           "            padding: 15px;\n"
           "            border-radius: 8px;\n"
           "            border: 1px solid #e9ecef;\n"
           "        }\n"
           "        \n"
           "        .subject-group h3 {\n"
           "            color: #2c3e50;\n"
           "            margin-bottom: 15px;\n"
           "            font-size: 1.1em;\n"
           "        }\n"
           "        \n"
           "        .subject-item {\n"
           "            display: flex;\n"
           "            align-items: center;\n"
           "            margin-bottom: 10px;\n"
           "        }\n"
           "        \n"
           "        .subject-item label {\n"
           "            flex: 1;\n"
           "            margin-bottom: 0;\n"
           "            margin-right: 10px;\n"
           "        }\n"
           "        \n"
           "        .subject-item select {\n"
           "            flex: 0 0 80px;\n"
           "        }\n"
           "        \n"
           "        .optional-subjects {\n"
           "            margin-top: 20px;\n"
           "        }\n"
           "        \n"
           "        .optional-subject-item {\n"
           "            display: flex;\n"
           "            align-items: center;\n"
           "            margin-bottom: 10px;\n"
           "            gap: 10px;\n"
           "        }\n"
           "        \n"
           "        .optional-subject-item input[type=\"text\"] {\n"
           "            flex: 1;\n"
           "            padding: 8px;\n"
           "            border: 1px solid #ddd;\n"
           "            border-radius: 4px;\n"
           "        }\n"
           "        \n"
           "        .optional-subject-item select {\n"
           "            flex: 0 0 80px;\n"
           "        }\n"
           "        \n"
           "        .btn {\n"
           "            background: linear-gradient(135deg, #3498db 0%, #2980b9 100%);\n"
           "            color: white;\n"
           "            padding: 15px 30px;\n"
           "            border: none;\n"
           "            border-radius: 6px;\n"
           "            cursor: pointer;\n"
           "            font-size: 16px;\n"
           "            font-weight: bold;\n"
           "            transition: transform 0.2s;\n"
           "            width: 100%;\n"
           "        }\n"
           "        \n"
           "        .btn:hover {\n"
           "            transform: translateY(-2px);\n"
           "        }\n"
           "        \n"
           "        .btn-add {\n"
           "            background: #27ae60;\n"
           "            padding: 8px 16px;\n"
           "            font-size: 14px;\n"
           "            width: auto;\n"
           "        }\n"
           "        \n"
           "        .results {\n"
           "            margin-top: 30px;\n"
           "            padding: 20px;\n"
           "            background: #f8f9fa;\n"
           "            border-radius: 8px;\n"
           "            border-left: 4px solid #3498db;\n"
           "        }\n"
           "        \n"
           "        .result-item {\n"
           "            display: flex;\n"
           "            justify-content: space-between;\n"
           "            margin-bottom: 10px;\n"
           "            padding: 8px 0;\n"
           "            border-bottom: 1px solid #eee;\n"
           "        }\n"
           "        \n"
           "        .result-item:last-child {\n"
           "            border-bottom: none;\n"
           "            font-weight: bold;\n"
           "            font-size: 1.2em;\n"
           "        }\n"
           "        \n"
           "        .status {\n"
           "            padding: 15px;\n"
           "            border-radius: 6px;\n"
           "            margin-top: 15px;\n"
           "            text-align: center;\n"
           "            font-weight: bold;\n"
           "        }\n"
           "        \n"
           "        .status.excellent { background: #d4edda; color: #155724; }\n"
           "        .status.good { background: #d1ecf1; color: #0c5460; }\n"
           "        .status.fair { background: #fff3cd; color: #856404; }\n"
           "        .status.poor { background: #f8d7da; color: #721c24; }\n"
           "        \n"
           "        .hidden {\n"
           "            display: none;\n"
           "        }\n"
           "    </style>\n"
           "</head>\n"
           "<body>\n"
           "    <div class=\"container\">\n"
           "        <div class=\"header\">\n"
           "            <h1>LASU Screening Calculator</h1>\n"
           "            <p>Calculate your LASU admission screening score</p>\n"
           "        </div>\n"
           "        \n"
           "        <div class=\"form-container\">\n"
           "            <form id=\"calculatorForm\">\n"
           "                <div class=\"form-group\">\n"
           "                    <label for=\"courseCategory\">Course Category:</label>\n"
           "                    <select id=\"courseCategory\" required>\n"
           "                        <option value=\"\">Select Category</option>\n"
           "                        <option value=\"1\">Science</option>\n"
           "                        <option value=\"2\">Arts</option>\n"
           "                        <option value=\"3\">Commercial</option>\n"
           "                    </select>\n"
           "                </div>\n"
           "                \n"
           "                <div class=\"form-group\">\n"
           "                    <label for=\"jambScore\">JAMB Score (out of 400):</label>\n"
           "                    <input type=\"number\" id=\"jambScore\" min=\"0\" max=\"400\" required>\n"
           "                </div>\n"
           "                \n"
           "                <div id=\"requiredSubjects\" class=\"subjects-grid hidden\">\n"
           "                    <div class=\"subject-group\">\n"
           "                        <h3>Required Subjects</h3>\n"
           "                        <div id=\"requiredSubjectsList\"></div>\n"
           "                    </div>\n"
           "                </div>\n"
           "                \n"
           "                <div id=\"optionalSubjectsSection\" class=\"hidden\">\n"
           "                    <div class=\"subject-group\">\n"
           "                        <h3>Optional Subjects</h3>\n"
           "                        <div id=\"optionalSubjects\">\n"
           "                            <div class=\"optional-subject-item\">\n"
           "                                <input type=\"text\" placeholder=\"Subject name\" required>\n"
           "                                <select required>\n"
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
           "                        <button type=\"button\" class=\"btn btn-add\" onclick=\"addOptionalSubject()\">Add Subject</button>\n"
           "                    </div>\n"
           "                </div>\n"
           "                \n"
           "                <button type=\"submit\" class=\"btn\">Calculate Screening Score</button>\n"
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
           "        var subjectsByCategory = {\n"
           "            1: [\"Mathematics\", \"English Language\", \"Physics\", \"Chemistry\"],\n"
           "            2: [\"English Language\", \"Government\", \"Literature in English\", \"Mathematics\"],\n"
           "            3: [\"English Language\", \"Mathematics\", \"Economics\", \"Commerce\"]\n"
           "        };\n"
           "        \n"
           "        document.getElementById('courseCategory').addEventListener('change', function() {\n"
           "            var category = this.value;\n"
           "            var requiredSubjects = document.getElementById('requiredSubjects');\n"
           "            var optionalSubjects = document.getElementById('optionalSubjectsSection');\n"
           "            \n"
           "            if (category) {\n"
           "                showRequiredSubjects(category);\n"
           "                requiredSubjects.classList.remove('hidden');\n"
           "                optionalSubjects.classList.remove('hidden');\n"
           "            } else {\n"
           "                requiredSubjects.classList.add('hidden');\n"
           "                optionalSubjects.classList.add('hidden');\n"
           "            }\n"
           "        });\n"
           "        \n"
           "        function showRequiredSubjects(category) {\n"
           "            var subjects = subjectsByCategory[category];\n"
           "            var container = document.getElementById('requiredSubjectsList');\n"
           "            container.innerHTML = '';\n"
           "            \n"
           "            subjects.forEach(function(subject) {\n"
           "                var div = document.createElement('div');\n"
           "                div.className = 'subject-item';\n"
           "                div.innerHTML = '<label>' + subject + ':</label>' +\n"
           "                    '<select name=\"required_' + subject + '\" required>' +\n"
           "                        '<option value=\"\">Grade</option>' +\n"
           "                        '<option value=\"A1\">A1</option>' +\n"
           "                        '<option value=\"B2\">B2</option>' +\n"
           "                        '<option value=\"B3\">B3</option>' +\n"
           "                        '<option value=\"C4\">C4</option>' +\n"
           "                        '<option value=\"C5\">C5</option>' +\n"
           "                        '<option value=\"C6\">C6</option>' +\n"
           "                        '<option value=\"D7\">D7</option>' +\n"
           "                        '<option value=\"E8\">E8</option>' +\n"
           "                        '<option value=\"F9\">F9</option>' +\n"
           "                    '</select>';\n"
           "                container.appendChild(div);\n"
           "            });\n"
           "        }\n"
           "        \n"
           "        function addOptionalSubject() {\n"
           "            var container = document.getElementById('optionalSubjects');\n"
           "            var div = document.createElement('div');\n"
           "            div.className = 'optional-subject-item';\n"
           "            div.innerHTML = '<input type=\"text\" placeholder=\"Subject name\" required>' +\n"
           "                '<select required>' +\n"
           "                    '<option value=\"\">Grade</option>' +\n"
           "                    '<option value=\"A1\">A1</option>' +\n"
           "                    '<option value=\"B2\">B2</option>' +\n"
           "                    '<option value=\"B3\">B3</option>' +\n"
           "                    '<option value=\"C4\">C4</option>' +\n"
           "                    '<option value=\"C5\">C5</option>' +\n"
           "                    '<option value=\"C6\">C6</option>' +\n"
           "                    '<option value=\"D7\">D7</option>' +\n"
           "                    '<option value=\"E8\">E8</option>' +\n"
           "                    '<option value=\"F9\">F9</option>' +\n"
           "                '</select>';\n"
           "            container.appendChild(div);\n"
           "        }\n"
           "        \n"
           "        document.getElementById('calculatorForm').addEventListener('submit', function(e) {\n"
           "            e.preventDefault();\n"
           "            \n"
           "            var data = {\n"
           "                courseCategory: parseInt(document.getElementById('courseCategory').value),\n"
           "                jambScore: parseInt(document.getElementById('jambScore').value),\n"
           "                requiredSubjects: {},\n"
           "                optionalSubjects: []\n"
           "            };\n"
           "            \n"
           "            // Get required subjects\n"
           "            var requiredSelects = document.querySelectorAll('select[name^=\"required_\"]');\n"
           "            requiredSelects.forEach(function(select) {\n"
           "                var subject = select.name.replace('required_', '');\n"
           "                data.requiredSubjects[subject] = select.value;\n"
           "            });\n"
           "            \n"
           "            // Get optional subjects\n"
           "            var optionalItems = document.querySelectorAll('.optional-subject-item');\n"
           "            optionalItems.forEach(function(item) {\n"
           "                var nameInput = item.querySelector('input[type=\"text\"]');\n"
           "                var gradeSelect = item.querySelector('select');\n"
           "                if (nameInput.value && gradeSelect.value) {\n"
           "                    data.optionalSubjects.push({\n"
           "                        name: nameInput.value,\n"
           "                        grade: gradeSelect.value\n"
           "                    });\n"
           "                }\n"
           "            });\n"
           "            \n"
           "            // Make the request\n"
           "            var xhr = new XMLHttpRequest();\n"
           "            xhr.open('POST', '/api/calculate', true);\n"
           "            xhr.setRequestHeader('Content-Type', 'application/json');\n"
           "            \n"
           "            xhr.onreadystatechange = function() {\n"
           "                if (xhr.readyState === 4 && xhr.status === 200) {\n"
           "                    var result = JSON.parse(xhr.responseText);\n"
           "                    displayResults(result);\n"
           "                }\n"
           "            };\n"
           "            \n"
           "            xhr.send(JSON.stringify(data));\n"
           "        });\n"
           "        \n"
           "        function displayResults(result) {\n"
           "            var resultsDiv = document.getElementById('results');\n"
           "            var resultsList = document.getElementById('resultsList');\n"
           "            \n"
           "            if (result.error) {\n"
           "                resultsList.innerHTML = '<div class=\"status poor\">Error: ' + result.error + '</div>';\n"
           "            } else {\n"
           "                resultsList.innerHTML = \n"
           "                    '<div class=\"result-item\">' +\n"
           "                        '<span>JAMB Score:</span>' +\n"
           "                        '<span>' + result.jambScore + '/400</span>' +\n"
           "                    '</div>' +\n"
           "                    '<div class=\"result-item\">' +\n"
           "                        '<span>JAMB Percentage:</span>' +\n"
           "                        '<span>' + result.jambPercentage.toFixed(1) + '%</span>' +\n"
           "                    '</div>' +\n"
           "                    '<div class=\"result-item\">' +\n"
           "                        '<span>WAEC Score:</span>' +\n"
           "                        '<span>' + result.waecScore + '/40</span>' +\n"
           "                    '</div>' +\n"
           "                    '<div class=\"result-item\">' +\n"
           "                        '<span>WAEC Percentage:</span>' +\n"
           "                        '<span>' + result.waecPercentage.toFixed(1) + '%</span>' +\n"
           "                    '</div>' +\n"
           "                    '<div class=\"result-item\">' +\n"
           "                        '<span>Final Screening Score:</span>' +\n"
           "                        '<span>' + result.finalScore.toFixed(1) + '%</span>' +\n"
           "                    '</div>' +\n"
           "                    '<div class=\"status ' + result.status.toLowerCase() + '\">' +\n"
           "                        result.admissionStatus +\n"
           "                    '</div>';\n"
           "            }\n"
           "            \n"
           "            resultsDiv.classList.remove('hidden');\n"
           "        }\n"
           "    </script>\n"
           "</body>\n"
           "</html>";
};
     string generateSubjectsJSON() {
        return R"({
            "science": ["Mathematics", "English Language", "Physics", "Chemistry"],
            "arts": ["English Language", "Government", "Literature in English", "Mathematics"],
            "commercial": ["English Language", "Mathematics", "Economics", "Commerce"]
        })";
    }
    
 string handleCalculation(const  string& json_body) {
    try {
        //JSON parsing 
         regex course_regex("\"courseCategory\"\\s*:\\s*(\\d+)");
         regex jamb_regex("\"jambScore\"\\s*:\\s*(\\d+)");
         regex required_regex("\"([^\"]+)\"\\s*:\\s*\"([^\"]+)\"");
         regex optional_name_regex("\"name\"\\s*:\\s*\"([^\"]+)\"");
         regex optional_grade_regex("\"grade\"\\s*:\\s*\"([^\"]+)\"");
        
         smatch match;
        
        // Parse course category
        int courseCategory = 0;
        if ( regex_search(json_body, match, course_regex)) {
            courseCategory =  stoi(match[1].str());
        }
        
        // Parse JAMB score
        int jambScore = 0;
        if ( regex_search(json_body, match, jamb_regex)) {
            jambScore =  stoi(match[1].str());
        }
        
        if (courseCategory == 0 || jambScore == 0) {
            return "{\"error\": \"Invalid course category or JAMB score\"}";
        }
        
        // Create calculator instance
        LASUScreeningAggregator calculator;
        calculator.setJambScore(jambScore);
        calculator.setRequiredSubjects(courseCategory);
        
        // Parse required subjects
         string requiredSubjectsSection = extractSection(json_body, "requiredSubjects");
        if (!requiredSubjectsSection.empty()) {
            auto subjects = parseRequiredSubjects(requiredSubjectsSection);
            for (const auto& subject : subjects) {
                calculator.addGrade(subject.first, subject.second);
            }
        }
        
        // Parse optional subjects
         string optionalSubjectsSection = extractSection(json_body, "optionalSubjects");
        if (!optionalSubjectsSection.empty()) {
            auto optionalSubjects = parseOptionalSubjects(optionalSubjectsSection);
            for (const auto& subject : optionalSubjects) {
                calculator.addOptionalSubject(subject.first, subject.second);
            }
        }
        
        // Calculate results
        calculator.calculateScreeningResults();
        
        // Generate JSON response
         ostringstream response;
        response <<  fixed <<  setprecision(1);
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
        
    } catch (const  exception& e) {
        return "{\"error\": \"" +  string(e.what()) + "\"}";
    }
}

private:
     string extractSection(const  string& json, const  string& section) {
         string pattern = "\"" + section + "\"\\s*:\\s*\\{([^}]*)\\}";
         regex section_regex(pattern);
         smatch match;
        
        if ( regex_search(json, match, section_regex)) {
            return match[1].str();
        }
        
        // Try array format for optional subjects
        if (section == "optionalSubjects") {
            pattern = "\"" + section + "\"\\s*:\\s*\\[([^\\]]*)\\]";
             regex array_regex(pattern);
            if ( regex_search(json, match, array_regex)) {
                return match[1].str();
            }
        }
        
        return "";
    }
    
     vector< pair< string,  string>> parseRequiredSubjects(const  string& section) {
         vector< pair< string,  string>> subjects;
         regex subject_regex("\"([^\"]+)\"\\s*:\\s*\"([^\"]+)\"");
         sregex_iterator iter(section.begin(), section.end(), subject_regex);
         sregex_iterator end;
        
        for (; iter != end; ++iter) {
             smatch match = *iter;
             string subject = match[1].str();
             string grade = match[2].str();
            
            // Replace underscores with spaces (from form field names)
             replace(subject.begin(), subject.end(), '_', ' ');
            
            subjects.push_back({subject, grade});
        }
        
        return subjects;
    }
    
     vector< pair< string,  string>> parseOptionalSubjects(const  string& section) {
         vector< pair< string,  string>> subjects;
        
        // Parse array of objects: [{"name":"Subject","grade":"A1"}...]
         regex object_regex("\\{[^}]*\\}");
         sregex_iterator iter(section.begin(), section.end(), object_regex);
         sregex_iterator end;
        
        for (; iter != end; ++iter) {
             string object = iter->str();
            
             regex name_regex("\"name\"\\s*:\\s*\"([^\"]+)\"");
             regex grade_regex("\"grade\"\\s*:\\s*\"([^\"]+)\"");
            
             smatch name_match, grade_match;
            
            if ( regex_search(object, name_match, name_regex) && 
                 regex_search(object, grade_match, grade_regex)) {
                subjects.push_back({name_match[1].str(), grade_match[1].str()});
            }
        }
        
        return subjects;
    }
    
 string getStatusClass(double score) {
        if (score >= 70.0) return "excellent";
        if (score >= 60.0) return "good";
        if (score >= 50.0) return "fair";
        return "poor";
    }
};

int main() {
    try {
         cout << "Starting LASU Screening HTTP Server..." <<  endl;
        
        LASUHttpServer server(8080);
        
         cout << "Server will start on port 8080" <<  endl;
         cout << "Press Ctrl+C to stop the server" <<  endl;
        
        server.start();
        
    } catch (const  exception& e) {
         cerr << "Error: " << e.what() <<  endl;
        return 1;
    }
    
    return 0;
}
