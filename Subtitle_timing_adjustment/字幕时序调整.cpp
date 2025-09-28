#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <vector>  
#include <string>  
#include<iomanip>
#include<regex>
using namespace std;

// 时间类  
class Time {
public:
    int hours, minutes, seconds, milliseconds;

    Time(int h = 0, int m = 0, int s = 0, int ms = 0)
        : hours(h), minutes(m), seconds(s), milliseconds(ms) {}

    // 加法运算  
    Time operator+(const Time& other) const {
        return Time(
            hours + other.hours,
            minutes + other.minutes,
            seconds + other.seconds,
            milliseconds + other.milliseconds
        );
    }

    // 减法运算  
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

    // 转换为字符串  
    std::string toString() const {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << hours << ":"
            << std::setfill('0') << std::setw(2) << minutes << ":"
            << std::setfill('0') << std::setw(2) << seconds << ","
            << std::setfill('0') << std::setw(3) << milliseconds;
        return oss.str();
    }

};

// 字幕类  
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

// 视频类  
class Video {
public:
    vector<Subtitle> subtitles;

    // 从文件中读取字幕  
    void readSubtitle(const string& filename) {
        ifstream infile(filename);
        if (!infile.is_open()) {
            std::cerr << "无法打开文件：" << filename << std::endl;
            return;
        }

        int id = 1;
        Subtitle sub(id);
        Time startTime, endTime;
        vector<string> lines;
        string line;
        // 判断是否处于字幕部分
        bool inContent = false;
        while (getline(infile, line)) {
            if (line.empty() || line[0] == '\n') {
                if (inContent) {
                    subtitles.push_back(sub);
                    sub = Subtitle(++id); // 重置
                    inContent = false;
                }
                continue; // 忽略空行和注释  
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


    // 调整字幕时间点（给每个字幕增加固定时间）  
    void adjustSubtitle(const Time& delta) {
        for (Subtitle& sub : subtitles) {
            sub.startTime = sub.startTime + delta;
            sub.endTime = sub.endTime + delta;
        }
        return;
    }

    // 将字幕内容写入文件  
    void writeSubtitle(const string& filename) {
        ofstream outfile(filename);
        if (!outfile.is_open()) {
            cerr << "无法打开文件：" << filename << endl;
            return;
        }

        for (Subtitle& sub : subtitles) {
            outfile << sub.id << endl;
            outfile << sub.startTime.toString() << " --> " << sub.endTime.toString() << endl;
            for (const string& line : sub.lines) {
                outfile << line << endl;
            }
            outfile << endl; // 字幕之间的空行  
        }

        outfile.close();
    }

};

int main() {
    Video video;
    string File_dir;
    cout << ">>> 字幕文件所在目录：" << endl;
    cin >> File_dir;
    string Flie_name;
    cout << ">>> 请输入视频文件名称（不包含文件后缀）：" << endl;
    cin >> Flie_name;
    string subtitle_Flie = File_dir + '/' + Flie_name + ".srt";
    //读取字幕
    video.readSubtitle(subtitle_Flie);
    int hour, minute, second, milliseconds;
    cout << "请输入所要调整的时间: " << endl;
    cout << "小时: " << endl;
    cin >> hour;
    cout << "分钟: " << endl;
    cin >> minute;
    cout << "秒: " << endl;
    cin >> second;
    cout << "毫秒: " << endl;
    cin >> milliseconds;
    //构建需要调整的时序大小的Time
    Time delta(hour, minute, second, milliseconds);
    //调整时序
    video.adjustSubtitle(delta);
    //新建文件夹
    string adjustedFile = File_dir + "/adjusted_" + Flie_name + ".srt";
    // 将调整后的字幕到写到新文件李=里 
    video.writeSubtitle(adjustedFile);
    //提示结束
    cout << "finish" << endl;

    return 0;
}