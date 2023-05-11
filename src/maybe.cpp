
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>

#include <re2/re2.h>

#include "glob.h"


using namespace std;


const vector<string> get_files() {
    vector<string> input_files;
    for (auto& p : glob::glob("./logs/i.ezproxy*.log")) {
        input_files.push_back(p);
    }
    sort(input_files.begin(), input_files.end());
    // remove last (incomplete log)
    if (input_files.size() < 365)
        input_files.pop_back();
    return input_files;
}

const string make_tab_delimited(const string& ip, const string& barcode,
                                const string& session,
                                const string& date_and_time, const string& url,
                                const string& fullurl) {
    return ip + "\t" + barcode + "\t" + session + "\t" + date_and_time +
           "\t" + url + "\t" + fullurl;
}


void process_line(string* all_fields, const string& line) {
    uint8_t counter = 0;
    string item;
    stringstream ss (line);

    while (getline(ss, item, ' ')) {
        all_fields[counter] = item;
        counter++;
        if (counter > 5)
            return;
    }
}


const string fix_whole_date(const string& adate) {
    const string day = adate.substr(1, 2);
    const string year = adate.substr(8, 4);
    const string time = adate.substr(13, string::npos);
    string premonth = adate.substr(4, 3);

    if      (premonth == "Jan") premonth = "01";
    else if (premonth == "Feb") premonth = "02";
    else if (premonth == "Mar") premonth = "03";
    else if (premonth == "Apr") premonth = "04";
    else if (premonth == "May") premonth = "05";
    else if (premonth == "Jun") premonth = "06";
    else if (premonth == "Jul") premonth = "07";
    else if (premonth == "Aug") premonth = "08";
    else if (premonth == "Sep") premonth = "09";
    else if (premonth == "Oct") premonth = "10";
    else if (premonth == "Nov") premonth = "11";
    else if (premonth == "Dec") premonth = "12";

    return year + "-" + premonth + "-" + day + " " + time;
}


int main() {
    using namespace re2;

    cout << endl << endl << "::alice glass:: HI!" << endl;

    const vector<string> input_files = get_files();
    const uint16_t count = input_files.size();
    const string last_date = input_files[count-1].substr(24, 10);
    const string output_file = "intermediate/cpp-cleaned-logs-" + last_date + ".dat";
    const RE2 re1  { "^https?[^A-Za-z]*(.+?):\\d.+$" };
    double counter { 1.0f };

    ofstream outfile(output_file);
    outfile << make_tab_delimited("ip", "barcode", "session", "date_and_time",
                                  "url", "fullurl") << endl;

    for (auto& item : input_files) {
        cout << round(counter/count*100) << endl;

        ifstream infile(item);
        string line;
        while (getline(infile, line)) {
            string tmp[6] = {"", "", "", "", "", ""};
            process_line(&tmp, line);

            string ip       = tmp[0];
            string barcode  = tmp[1];
            string sessionp = tmp[2];
            string date     = tmp[3];
            string url      = tmp[6];
            cout << "ip: " << tmp[0] << endl;
            cout << "url: " << tmp[6] << endl;
            cout << "url: " << url << endl;
            string fullurl  = url;
            cout << "full url: " << fullurl << endl << endl;

            if (barcode == "-")
                continue;

            // fixing urls
            // RE2::Replace(&url, re1, "\\1");

            date = fix_whole_date(date);

            outfile << make_tab_delimited(ip, barcode, sessionp, date, url,
                                          fullurl) << endl;
            break;
        }
        break;
        counter++;
    }
    outfile.close();
}


