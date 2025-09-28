#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <vector>  
#include <string>  
#include<iomanip>
#include<regex>
using namespace std;

// ʱ����  
class Time {
public:
    int hours, minutes, seconds, milliseconds;

    Time(int h = 0, int m = 0, int s = 0, int ms = 0)
        : hours(h), minutes(m), seconds(s), milliseconds(ms) {}

    // �ӷ�����  
    Time operator+(const Time& other) const {
        return Time(
            hours + other.hours,
            minutes + other.minutes,
            seconds + other.seconds,
            milliseconds + other.milliseconds
        );
    }

    // ��������  
    Time operator-(const Time& other) const {
        int total_ms = hours * 3600000 + minutes * 60000 + seconds * 1000 + milliseconds;
        int other_total_ms = other.hours * 3600000 + other.minutes * 60000 + other.seconds * 1000 + other.milliseconds;
        int diff_ms = total_ms - other_total_ms;

        return Time(
            diff_ms / 3600000,
            (diff_ms / 60000) % 60,
            (diff_ms / 1000) % 60,
            diff_ms % 1000
        );
    }

    // ת��Ϊ�ַ���  
    std::string toString() const {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << hours << ":"
            << std::setfill('0') << std::setw(2) << minutes << ":"
            << std::setfill('0') << std::setw(2) << seconds << ","
            << std::setfill('0') << std::setw(3) << milliseconds;
        return oss.str();
    }

};

// ��Ļ��  
class Subtitle {
public:
    int id;
    Time startTime, endTime;
    vector<string> lines;

    Subtitle(int id)
    {
        this->id = id;
    }
};

// ��Ƶ��  
class Video {
public:
    vector<Subtitle> subtitles;

    // ���ļ��ж�ȡ��Ļ  
    void readSubtitle(const string& filename) {
        ifstream infile(filename);
        if (!infile.is_open()) {
            std::cerr << "�޷����ļ���" << filename << std::endl;
            return;
        }

        int id = 1;
        Subtitle sub(id);
        Time startTime, endTime;
        vector<string> lines;
        string line;
        // �ж��Ƿ�����Ļ����
        bool inContent = false;
        while (getline(infile, line)) {
            if (line.empty() || line[0] == '\n') {
                if (inContent) {
                    subtitles.push_back(sub);
                    sub = Subtitle(++id); // ����
                    inContent = false;
                }
                continue; // ���Կ��к�ע��  
            }
            regex timeRegex(R"((\d{2}):(\d{2}):(\d{2}),(\d{3}) --> (\d{2}):(\d{2}):(\d{2}),(\d{3}))");
            smatch match;
            if (regex_search(line, match, timeRegex)) { int startHour = stoi(match[1].str()); int startMinute = stoi(match[2].str()); int startSecond = stoi(match[3].str()); int startMillisecond = stoi(match[4].str()); int endHour = stoi(match[5].str()); int endMinute = stoi(match[6].str()); int endSecond = stoi(match[7].str()); int endMillisecond = stoi(match[8].str()); sub.startTime = Time(startHour, startMinute, startSecond, startMillisecond); sub.endTime = Time(endHour, endMinute, endSecond, endMillisecond); inContent = true; continue; }
            if (inContent) {
                sub.lines.push_back(line);
            }
        }
        if (!sub.lines.empty()) {
            subtitles.push_back(sub);
        }
        infile.close();
    }


    // ������Ļʱ��㣨��ÿ����Ļ���ӹ̶�ʱ�䣩  
    void adjustSubtitle(const Time& delta) {
        for (Subtitle& sub : subtitles) {
            sub.startTime = sub.startTime + delta;
            sub.endTime = sub.endTime + delta;
        }
        return;
    }

    // ����Ļ����д���ļ�  
    void writeSubtitle(const string& filename) {
        ofstream outfile(filename);
        if (!outfile.is_open()) {
            cerr << "�޷����ļ���" << filename << endl;
            return;
        }

        for (Subtitle& sub : subtitles) {
            outfile << sub.id << endl;
            outfile << sub.startTime.toString() << " --> " << sub.endTime.toString() << endl;
            for (const string& line : sub.lines) {
                outfile << line << endl;
            }
            outfile << endl; // ��Ļ֮��Ŀ���  
        }

        outfile.close();
    }

};

int main() {
    Video video;
    string File_dir;
    cout << ">>> ��Ļ�ļ�����Ŀ¼��" << endl;
    cin >> File_dir;
    string Flie_name;
    cout << ">>> ��������Ƶ�ļ����ƣ��������ļ���׺����" << endl;
    cin >> Flie_name;
    string subtitle_Flie = File_dir + '/' + Flie_name + ".srt";
    //��ȡ��Ļ
    video.readSubtitle(subtitle_Flie);
    int hour, minute, second, milliseconds;
    cout << "��������Ҫ������ʱ��: " << endl;
    cout << "Сʱ: " << endl;
    cin >> hour;
    cout << "����: " << endl;
    cin >> minute;
    cout << "��: " << endl;
    cin >> second;
    cout << "����: " << endl;
    cin >> milliseconds;
    //������Ҫ������ʱ���С��Time
    Time delta(hour, minute, second, milliseconds);
    //����ʱ��
    video.adjustSubtitle(delta);
    //�½��ļ���
    string adjustedFile = File_dir + "/adjusted_" + Flie_name + ".srt";
    // �����������Ļ��д�����ļ���=�� 
    video.writeSubtitle(adjustedFile);
    //��ʾ����
    cout << "finish" << endl;

    return 0;
}