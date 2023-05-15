
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>
#include <csignal>

#include <re2/re2.h>

#include "glob.h"
#include "indicators.hpp"
#include "rang.hpp"


using namespace std;
using namespace rang;
using namespace indicators;


static ofstream outfile;

static ProgressBar bar{
        option::BarWidth{70},
        option::Start{"["},
        option::Fill{"■"},
        option::Lead{"■"},
        option::Remainder{"-"},
        option::End{"]"},
        option::PostfixText{""},
        option::ForegroundColor{Color::yellow},
        option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
};



string display_time() {
    return ("[" + to_string(time(0)) + "]  ");
}


void handle_sigint(const int signum) {
    cerr << endl << endl << fg::red << display_time() << "caught SIGINT" << endl;
    outfile.close();
    indicators::show_console_cursor(true);
    cerr << style::reset << endl;
    exit(1);
}


const vector<string> get_files() {
    vector<string> input_files;
    for (const auto& p : glob::glob("./logs/i.ezproxy.nypl.org.2023-01*.log")) {
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


const tuple<string, string, string, string, string> process_line(const string& line) {
    stringstream ss (line);
    string ip, barcode, sessionp, date, url, garbage;

    getline(ss, ip, ' ');
    getline(ss, barcode, ' ');
    getline(ss, sessionp, ' ');
    getline(ss, date, ' ');
    getline(ss, garbage, ' ');
    getline(ss, garbage, ' ');
    getline(ss, url, ' ');

    return {ip, barcode, sessionp, date, url};
}


const string fix_whole_date(const string& adate) {
    const string day  = adate.substr(1, 2);
    const string year = adate.substr(8, 4);
    const string time = adate.substr(13, string::npos);
    string premonth   = adate.substr(4, 3);

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

    signal(SIGINT, handle_sigint);

    cout << endl << endl << fg::gray << style::dim
         << display_time() << "::alice glass:: HI!"
         << endl << style::bold << fg::cyan << display_time()
         << "Processing raw logs" << style::reset << endl << endl;

    const vector<string> input_files = get_files();
    const uint16_t count = input_files.size();
    const string last_date = input_files[count-1].substr(24, 10);
    const string output_file = "intermediate/cleaned-logs-" + last_date + ".dat";
    const RE2 re1  { "^https?[^A-Za-z]*(.+?):\\d.+$" };
    double counter { 1.0f };

    outfile.open(output_file);
    outfile << make_tab_delimited("ip", "barcode", "session", "date_and_time",
                                  "url", "fullurl") << endl;

    show_console_cursor(false);

    for (const auto& item : input_files) {
        uint8_t perc = round(counter/count*100);
        bar.set_progress(perc);
        bar.set_option(option::PostfixText{ "  " + to_string(int(counter)) +
                                            "/" + to_string(count) + "  " +
                                            to_string(perc) + "%" });

        ifstream infile(item);
        string line;
        string ip, barcode, sessionp, date, url;

        while (getline(infile, line)) {
            tie(ip, barcode, sessionp, date, url) = std::move(process_line(line));
            string fullurl  = url;

            if (barcode == "-")
                continue;

            // fixing urls
            RE2::Replace(&url, re1, "\\1");

            date = fix_whole_date(date);

            outfile << make_tab_delimited(ip, barcode, sessionp, date, url,
                                          fullurl) << endl;
        }
        counter++;
    }
    cout << endl << endl << style::bold << fg::cyan << display_time()
         << "Closing output file" << endl;

    outfile.close();
    show_console_cursor(true);

    cout << style::bold << fg::green << display_time()
         << "Done!" << endl;
    cout << style::reset << fg::reset;
}


