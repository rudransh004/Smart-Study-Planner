 #include <iostream>
 #include <cstring>
 #include <ctime>
 #include <fstream>
 #include <iomanip>
 // Array size constants
 const int MAX_SUBJECTS = 10;
 const int MAX_QUIZ_SCORES = 20;
 const int MAX_SESSIONS = 50;
 const int MAX_QUIZ_QUESTIONS = 20;
 const int MAX_QUIZ_OPTIONS = 4;
 const int MAX_STRING_LENGTH = 100;
 const int MAX_NAME_LENGTH = 50;
 // Forward declarations
 class Student;
 class StudyTracker;
 class Quiz;
 class Scheduler;
 class Visualizer;
 // Utility functions
 namespace utils {
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    int getMenuChoice(int min, int max) {
        int choice;
        while (true) {
            std::cout << "Enter your choice (" << min << "-" << max << "): ";
            if (std::cin >> choice && choice >= min && choice <= max) {
                return choice;
            }
            std::cout << "Invalid choice. Please try again.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
    }
 }
 // Subject structure
 struct Subject {
    char name[MAX_NAME_LENGTH];
    int weeklyGoalHours;
    double quizScores[MAX_QUIZ_SCORES];
    int numScores;
 };
 // Study session structure
 struct StudySession {
    char subject[MAX_NAME_LENGTH];
    time_t startTime;
    time_t endTime;
    double duration;
 };
 // Quiz question structure
 struct QuizQuestion {
    char question[MAX_STRING_LENGTH];
    char options[MAX_QUIZ_OPTIONS][MAX_STRING_LENGTH];
    int numOptions;
    int correctAnswer;
 };
  // Student class
 class Student {
 private:
    char name[MAX_NAME_LENGTH];
    int age;
    Subject subjects[MAX_SUBJECTS];
    int numSubjects;
 public:
    Student() : age(0), numSubjects(0) {
        name[0] = '\0';
    }
    Student(const char* studentName, int studentAge) : numSubjects(0) {
        strncpy(name, studentName, MAX_NAME_LENGTH - 1);
        name[MAX_NAME_LENGTH - 1] = '\0';
        age = studentAge;
    }
    void addSubject(const char* subjectName, int weeklyGoalHours) {
        if (numSubjects >= MAX_SUBJECTS) {
            throw std::runtime_error("Maximum subjects reached");
        }
        
        Subject& subject = subjects[numSubjects];
        strncpy(subject.name, subjectName, MAX_NAME_LENGTH - 1);
        subject.name[MAX_NAME_LENGTH - 1] = '\0';
        subject.weeklyGoalHours = weeklyGoalHours;
        subject.numScores = 0;
        numSubjects++;
    }
    void removeSubject(const char* subjectName) {
        for (int i = 0; i < numSubjects; i++) {
            if (strcmp(subjects[i].name, subjectName) == 0) {
                // Shift remaining subjects left
                for (int j = i; j < numSubjects - 1; j++) {
                    subjects[j] = subjects[j + 1];
                }
                numSubjects--;
                return;
            }
        }
    }
    void addQuizScore(const char* subject, double score) {
        for (int i = 0; i < numSubjects; i++) {
            if (strcmp(subjects[i].name, subject) == 0) {
                if (subjects[i].numScores >= MAX_QUIZ_SCORES) {
                    throw std::runtime_error("Maximum quiz scores reached");
                }
                subjects[i].quizScores[subjects[i].numScores++] = score;
                return;
            }
        }
        throw std::runtime_error("Subject not found");
    }
    double getAverageQuizScore(const char* subject) const {
        for (int i = 0; i < numSubjects; i++) {
            if (strcmp(subjects[i].name, subject) == 0 && subjects[i].numScores > 0) {
                double sum = 0;
                for (int j = 0; j < subjects[i].numScores; j++) {
                    sum += subjects[i].quizScores[j];
                }
                return sum / subjects[i].numScores;
            }
        }
        return 0.0;
    }
 void setWeeklyGoal(const char* subject, int hours) {
        for (int i = 0; i < numSubjects; i++) {
            if (strcmp(subjects[i].name, subject) == 0) {
                subjects[i].weeklyGoalHours = hours;
                return;
            }
        }
        throw std::runtime_error("Subject not found");
    }
    // Getters
    const char* getName() const { return name; }
    int getAge() const { return age; }
    const Subject* getSubjects() const { return subjects; }
    int getNumSubjects() const { return numSubjects; }
    void saveToFile(const char* filename) const {
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Cannot open file for writing");
        
        file << name << "\n" << age << "\n" << numSubjects << "\n";
        
        for (int i = 0; i < numSubjects; i++) {
            file << subjects[i].name << "\n"
                 << subjects[i].weeklyGoalHours << "\n"
                 << subjects[i].numScores << "\n";
            for (int j = 0; j < subjects[i].numScores; j++) {
                file << subjects[i].quizScores[j] << "\n";
            }
        }
    }
    void loadFromFile(const char* filename) {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Cannot open file for reading");
        
        file.getline(name, MAX_NAME_LENGTH);
        file >> age >> numSubjects;
        file.ignore();
        
        for (int i = 0; i < numSubjects; i++) {
            file.getline(subjects[i].name, MAX_NAME_LENGTH);
            file >> subjects[i].weeklyGoalHours >> subjects[i].numScores;
            file.ignore();
            
            for (int j = 0; j < subjects[i].numScores; j++) {
                file >> subjects[i].quizScores[j];
            }
            file.ignore();
        }
    }
 };
 // Study Tracker class
 class StudyTracker {
 private:
    StudySession sessions[MAX_SESSIONS];
    int numSessions;
    StudySession currentSession;
    bool sessionInProgress;
 public:
    StudyTracker() : numSessions(0), sessionInProgress(false) {}
    void startSession(const char* subject) {
        if (sessionInProgress) {
            throw std::runtime_error("Session already in progress");
        }
        
        strncpy(currentSession.subject, subject, MAX_NAME_LENGTH - 1);
        currentSession.subject[MAX_NAME_LENGTH - 1] = '\0';
        currentSession.startTime = time(nullptr);
        sessionInProgress = true;
    }
    void endSession() {
        if (!sessionInProgress) {
            throw std::runtime_error("No session in progress");
        }
              if (numSessions >= MAX_SESSIONS) {
            throw std::runtime_error("Maximum sessions reached");
        }
        currentSession.endTime = time(nullptr);
        currentSession.duration = 
            difftime(currentSession.endTime, currentSession.startTime) / 3600.0;
        sessions[numSessions++] = currentSession;
        sessionInProgress = false;
    }
    double getTotalStudyTime(const char* subject) const {
        double total = 0;
        for (int i = 0; i < numSessions; i++) {
            if (strcmp(sessions[i].subject, subject) == 0) {
                total += sessions[i].duration;
            }
        }
        return total;
    }
    void saveToFile(const char* filename) const {
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Cannot open file for writing");
        
        file << numSessions << "\n";
        for (int i = 0; i < numSessions; i++) {
            file << sessions[i].subject << "\n"
                 << sessions[i].startTime << "\n"
                 << sessions[i].endTime << "\n"
                 << sessions[i].duration << "\n";
        }
    }
    void loadFromFile(const char* filename) {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Cannot open file for reading");
        
        file >> numSessions;
        file.ignore();
        
        for (int i = 0; i < numSessions; i++) {
            file.getline(sessions[i].subject, MAX_NAME_LENGTH);
            file >> sessions[i].startTime;
            file >> sessions[i].endTime;
            file >> sessions[i].duration;
            file.ignore();
        }
    }
 };
 // Quiz class
 class Quiz {
 private:
    QuizQuestion questions[MAX_QUIZ_QUESTIONS];
    int numQuestions;
 public:
    Quiz() : numQuestions(0) {}
    void addQuestion(const char* question, const char* options[], int
 numOptions, int correctAnswer) {
        if (numQuestions >= MAX_QUIZ_QUESTIONS) {
            throw std::runtime_error("Maximum questions reached");
        }
        
        if (numOptions > MAX_QUIZ_OPTIONS) {
            throw std::runtime_error("Too many options");
        }
        QuizQuestion& q = questions[numQuestions];
        strncpy(q.question, question, MAX_STRING_LENGTH - 1);
        q.question[MAX_STRING_LENGTH - 1] = '\0';
        
        q.numOptions = numOptions;
        for (int i = 0; i < numOptions; i++) {
            strncpy(q.options[i], options[i], MAX_STRING_LENGTH - 1);
            q.options[i][MAX_STRING_LENGTH - 1] = '\0';
        }
        
        q.correctAnswer = correctAnswer;
        numQuestions++;
    }
    double takeQuiz() {
        if (numQuestions == 0) {
            throw std::runtime_error("No questions in quiz");
        }
        
        int correctCount = 0;
        
        for (int i = 0; i < numQuestions; i++) {
            std::cout << "\nQuestion " << (i + 1) << ": " 
                      << questions[i].question << "\n\n";
            
            for (int j = 0; j < questions[i].numOptions; j++) {
                std::cout << (j + 1) << ") " << questions[i].options[j] << "\n";
            }
            
            int answer;
            std::cout << "\nYour answer (1-" << questions[i].numOptions << "): ";
            std::cin >> answer;
            
            if (answer - 1 == questions[i].correctAnswer) {
                correctCount++;
            }
        }
        
        return (double)correctCount / numQuestions * 100.0;
    }
    void clearQuestions() {
        numQuestions = 0;
    }
    void saveToFile(const char* filename) const {
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Cannot open file for writing");
        
        file << numQuestions << "\n";
        for (int i = 0; i < numQuestions; i++) {
            file << questions[i].question << "\n"
                 << questions[i].numOptions << "\n";
            for (int j = 0; j < questions[i].numOptions; j++) {
                file << questions[i].options[j] << "\n";
            }
            file << questions[i].correctAnswer << "\n";
        }
    }    void loadFromFile(const char* filename) {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Cannot open file for reading");
        
        file >> numQuestions;
        file.ignore();
        
        for (int i = 0; i < numQuestions; i++) {
            file.getline(questions[i].question, MAX_STRING_LENGTH);
            file >> questions[i].numOptions;
            file.ignore();
            
            for (int j = 0; j < questions[i].numOptions; j++) {
                file.getline(questions[i].options[j], MAX_STRING_LENGTH);
            }
            
            file >> questions[i].correctAnswer;
            file.ignore();
        }
    }
 };
 // Scheduler class
 class Scheduler {
 private:
    Student& student;
    int recommendedHours[MAX_SUBJECTS];
    char subjectNames[MAX_SUBJECTS][MAX_NAME_LENGTH];
    int numRecommendations;
    void adjustForQuizScores() {
        for (int i = 0; i < student.getNumSubjects(); i++) {
            const Subject& subject = student.getSubjects()[i];
            double avgScore = student.getAverageQuizScore(subject.name);
            
            if (avgScore < 60.0) {
                recommendedHours[i] += 2;
            } else if (avgScore < 75.0) {
                recommendedHours[i] += 1;
            } else if (avgScore > 90.0) {
                recommendedHours[i] = std::max(1, recommendedHours[i] - 1);
            }
        }
    }
 public:
    Scheduler(Student& s) : student(s), numRecommendations(0) {}
    void updateRecommendations() {
        numRecommendations = student.getNumSubjects();
        for (int i = 0; i < numRecommendations; i++) {
            const Subject& subject = student.getSubjects()[i];
            strncpy(subjectNames[i], subject.name, MAX_NAME_LENGTH - 1);
            subjectNames[i][MAX_NAME_LENGTH - 1] = '\0';
            recommendedHours[i] = subject.weeklyGoalHours;
        }
        adjustForQuizScores();
    }
    int getNumRecommendations() const { return numRecommendations; }
    const char* getSubjectName(int index) const { return subjectNames[index]; }
    int getRecommendedHours(int index) const { return recommendedHours[index]; }
 };
 class Visualizer {
 private:
    static std::string generateASCIIBar(double value, double max, int width) {
        int filledWidth = static_cast<int>((value / max) * width);
        filledWidth = std::min(width, std::max(0, filledWidth));
        
        std::string bar = "[";
        bar += std::string(filledWidth, '#');
        bar += std::string(width - filledWidth, '-');
        bar += "]";
        
        return bar;
    }
 public:
    static void showStudyProgress(const Student& student, const StudyTracker& tracker) {
        std::cout << "\nStudy Progress for " << student.getName() << "\n";
        std::cout << std::string(40, '=') << "\n\n";
        
        for (int i = 0; i < student.getNumSubjects(); i++) {
            const Subject& subject = student.getSubjects()[i];
            double totalHours = tracker.getTotalStudyTime(subject.name);
            double goalHours = subject.weeklyGoalHours;
            double percentage = (totalHours / goalHours) * 100.0;
            
            std::cout << subject.name << ":\n";
            std::cout << generateASCIIBar(totalHours, goalHours, 40) << "\n";
            std::cout << std::fixed << std::setprecision(1)
                      << totalHours << "/" << goalHours 
                      << " hours (" << percentage << "%)\n\n";
        }
    }
    static void showQuizProgress(const Student& student) {
        std::cout << "\nQuiz Progress:\n";
        std::cout << std::string(40, '=') << "\n\n";
        
        for (int i = 0; i < student.getNumSubjects(); i++) {
            const Subject& subject = student.getSubjects()[i];
            double avgScore = student.getAverageQuizScore(subject.name);
            
            std::cout << subject.name << ":\n";
            std::cout << generateASCIIBar(avgScore, 100.0, 40) << "\n";
            std::cout << std::fixed << std::setprecision(1)
                      << "Average Score: " << avgScore << "%\n\n";
        }
    }
 };
 void displayMenu() {
    std::cout << "\nStudy Management System\n";
    std::cout << "=====================\n";
    std::cout << "1. Manage Profile\n";
    std::cout << "2. Start Study Session\n";
    std::cout << "3. Take Quiz\n";
    std::cout << "4. View Progress\n";
    std::cout << "5. View Study Recommendations\n";
    std::cout << "6. Exit\n";
 }
 void manageProfile(Student& student) {
    while (true) {
        utils::clearScreen();
        std::cout << "\nProfile Management\n";
        std::cout << "1. Add Subject\n";
        std::cout << "2. Remove Subject\n";
        std::cout << "3. Set Weekly Goal\n";
        std::cout << "4. View Current Subjects\n";
        std::cout << "5. Back to Main Menu\n";
        
        int choice = utils::getMenuChoice(1, 5);
        
        if (choice == 1) {
            char name[MAX_NAME_LENGTH];
            int hours;
            std::cout << "Enter subject name: ";
            std::cin.ignore();
            std::cin.getline(name, MAX_NAME_LENGTH);
            std::cout << "Enter weekly goal hours: ";
            std::cin >> hours;
            student.addSubject(name, hours);
        }    else if (choice == 2) {
            char name[MAX_NAME_LENGTH];
            std::cout << "Enter subject name to remove: ";
            std::cin.ignore();
            std::cin.getline(name, MAX_NAME_LENGTH);
            student.removeSubject(name);
        }
        else if (choice == 3) {
            char name[MAX_NAME_LENGTH];
            int hours;
            std::cout << "Enter subject name: ";
            std::cin.ignore();
            std::cin.getline(name, MAX_NAME_LENGTH);
            std::cout << "Enter new weekly goal hours: ";
            std::cin >> hours;
            student.setWeeklyGoal(name, hours);
        }
        else if (choice == 4) {
            for (int i = 0; i < student.getNumSubjects(); i++) {
                const Subject& subject = student.getSubjects()[i];
                std::cout << subject.name << " - " 
                         << subject.weeklyGoalHours << " hours/week\n";
            }
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
            std::cin.get();
        }
        else if (choice == 5) {
            break;
        }
    }
 }
 int main() {
    Student student;
    StudyTracker tracker;
    Quiz quiz;
    
    // Load saved data
    try {
        student.loadFromFile("student_data.txt");
        tracker.loadFromFile("study_sessions.txt");
        quiz.loadFromFile("quiz_data.txt");
    } catch (...) {
        std::cout << "Welcome new user!\n";
        char name[MAX_NAME_LENGTH];
        int age;
        std::cout << "Enter your name: ";
        std::cin.getline(name, MAX_NAME_LENGTH);
        std::cout << "Enter your age: ";
        std::cin >> age;
        student = Student(name, age);
    }
    
    Scheduler scheduler(student);
    
    while (true) {
        utils::clearScreen();
        displayMenu();
        int choice = utils::getMenuChoice(1, 6);
        
        switch (choice) {
            case 1:
                manageProfile(student);
                break;
                
            case 2: {
                char subject[MAX_NAME_LENGTH];
                std::cout << "Enter subject name: ";
                std::cin.ignore();
                std::cin.getline(subject, MAX_NAME_LENGTH);
                tracker.startSession(subject);
                std::cout << "Study session started. Press Enter to end...";
                std::cin.get();
                tracker.endSession();
                break;
            }
 case 3: {
                quiz.clearQuestions();
                const char* options[] = {"3", "4", "5", "6"};
                quiz.addQuestion("What is 2 + 2?", options, 4, 1);
                double score = quiz.takeQuiz();
                std::cout << "Quiz score: " << score << "%\n";
                std::cout << "Press Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
            }
                
            case 4:
                Visualizer::showStudyProgress(student, tracker);
                Visualizer::showQuizProgress(student);
                std::cout << "Press Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
                
            case 5:
                scheduler.updateRecommendations();
                std::cout << "\nRecommended study hours:\n";
                for (int i = 0; i < scheduler.getNumRecommendations(); i++) {
                    std::cout << scheduler.getSubjectName(i) << ": " 
                             << scheduler.getRecommendedHours(i) << " hours/week\n";
                }
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore();
                std::cin.get();
                break;
                
            case 6:
                student.saveToFile("student_data.txt");
                tracker.saveToFile("study_sessions.txt");
                quiz.saveToFile("quiz_data.txt");
                return 0;
        }
    }
    
    return 0;
 }
