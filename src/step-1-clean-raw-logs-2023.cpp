
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
using namespace re2;


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

const string make_tab_delimited(const string ip, const string barcode,
                                const string session,
                                const string date_and_time, const string url,
                                const string fullurl) {
    return ip + "\t" + barcode + "\t" + session + "\t" + date_and_time +
           "\t" + url + "\t" + fullurl;
}


const vector<string> process_line(const string& line) {
    vector<string> all_fields;
    string item;
    stringstream ss (line);

    while (getline(ss, item, ' ')) {
        all_fields.push_back(item);
    }
    return all_fields;
}

const string fix_month(const string& adate) {
    string before   = adate.substr(0, 5);
    string premonth = adate.substr(5, 3);
    string after    = adate.substr(8, string::npos);
    string realone  = "00";

    if      (premonth == "Jan") realone = "01";
    else if (premonth == "Feb") realone = "02";
    else if (premonth == "Mar") realone = "03";
    else if (premonth == "Apr") realone = "04";
    else if (premonth == "May") realone = "05";
    else if (premonth == "Jun") realone = "06";
    else if (premonth == "Jul") realone = "07";
    else if (premonth == "Aug") realone = "08";
    else if (premonth == "Sep") realone = "09";
    else if (premonth == "Oct") realone = "10";
    else if (premonth == "Nov") realone = "11";
    else if (premonth == "Dec") realone = "12";

    return before + realone + after;
}


int main() {
    cout << endl << endl << "::alice glass:: HI!" << endl;

    const vector<string> input_files = get_files();
    const uint16_t count = input_files.size();
    const string last_date = input_files[count-1].substr(24, 10);
    const string output_file = "intermediate/cleaned-logs-" + last_date + ".dat";

    ofstream outfile(output_file);
    outfile << make_tab_delimited("ip", "barcode", "session", "date_and_time",
                                  "url", "fullurl") << endl;

    auto counter { 1.0f };
    for (auto& item : input_files) {
        cout << round(counter/count*100) << endl;

        ifstream infile(item);
        string line;
        while (getline(infile, line)) {
            vector<string> tmp = process_line(line);

            string ip       = tmp[0];
            string barcode  = tmp[1];
            string sessionp = tmp[2];
            string date     = tmp[3];
            string url      = tmp[6];
            string fullurl  = url;

            if (barcode == "-")
                continue;

            // fixing urls
            RE2 re1  { "^https?[^A-Za-z]*(.+?):\\d.+$" };
            RE2::Replace(&url, re1, "\\1");

            // fixing dates
            RE2 re2 { "\\[(\\d+)/(.+?)/(\\d+):(\\d+):(\\d+):(\\d+)" };
            RE2::Replace(&date, re2, "\\3-\\2-\\1 \\4:\\5:\\6");
            date = fix_month(date);

            outfile << make_tab_delimited(ip, barcode, sessionp, date, url,
                                          fullurl) << endl;
        }
        counter++;
        break;
    }

    outfile.close();

}


