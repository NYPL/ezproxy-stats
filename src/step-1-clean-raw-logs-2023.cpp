
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <cmath>
#include <algorithm>
#include <csignal>

#include <re2/re2.h>

#include <fmt/core.h>

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



string display_time() noexcept {
    return ("[" + to_string(time(0)) + "]  ");
}


void handle_sigint(const int signum) {
    cerr << endl << endl << fg::red << display_time() << "caught SIGINT" << endl;
    outfile.close();
    indicators::show_console_cursor(true);
    cerr << style::reset << endl;
    exit(1);
}


const vector<string> get_files() noexcept {
    vector<string> input_files;
    for (const auto& p : glob::glob("./logs/i.ezproxy.nypl.org.2023-*.log")) {
        input_files.push_back(p);
    }
    sort(input_files.begin(), input_files.end());
    // remove last (incomplete log)
    if (input_files.size() < 365)
        input_files.pop_back();
    return input_files;
}


inline const string make_tab_delimited(const string& ip, const string& barcode,
                                const string& session,
                                const string& date_and_time, const string& url,
                                const string& fullurl) noexcept {
    return fmt::format("{}\t{}\t{}\t{}\t{}\t{}",
                       ip, barcode, session, date_and_time, url, fullurl);

}


void process_line(string* all_fields, const string& line) noexcept {
    uint8_t counter = 0;
    string item;
    stringstream ss (line);

    while (getline(ss, item, ' ')) {
        all_fields[counter] = item;
        counter++;
        if (counter > 6)
            return;
    }
}


const string fix_whole_date(const string& adate) noexcept {
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

    return fmt::format("{}-{}-{} {}", year, premonth, day, time);
}


int main() {
    using namespace re2;

    signal(SIGINT, handle_sigint);

    cout << endl << endl << fg::gray << style::dim
         << display_time() << "::alice glass:: HI!" << style::reset
         << endl << style::bold << fg::cyan << display_time()
         << "Processing raw logs" << style::reset << endl << endl;

    const vector<string> input_files = get_files();
    const double count = input_files.size();
    const string last_date = input_files[count-1].substr(24, 10);
    const string output_file = fmt::format("intermediate/cleaned-logs-{}.dat", last_date);
    const RE2 re1  { "^https?[^A-Za-z]*(.+?):\\d.+$" };
    uint16_t counter { 0 };

    outfile.open(output_file);
    outfile << make_tab_delimited("ip", "barcode", "session", "date_and_time",
                                  "url", "fullurl") << endl;

    // reusing this to store the necessary fields
    string tmp[7];

    show_console_cursor(false);

    for (const auto& item : input_files) {
        ++counter;
        uint8_t perc = static_cast<int>(round(counter/count*100));
        bar.set_option(option::PostfixText{
                fmt::format("  {}/{}  {}%", counter, count, perc) });
        bar.set_progress(perc);

        ifstream infile(item);
        string line;
        while (getline(infile, line)) {
            process_line(tmp, line);

            string ip       = tmp[0];
            string barcode  = tmp[1];
            string sessionp = tmp[2];
            string date     = tmp[3];
            string url      = tmp[6];
            string fullurl  = url;

            if (barcode == "-")
                continue;

            // fixing urls
            RE2::Replace(&url, re1, "\\1");

            date = fix_whole_date(date);

            outfile << make_tab_delimited(ip, barcode, sessionp, date, url,
                                          fullurl) << endl;
        }
        infile.close();
    }
    cout << endl << endl << style::bold << fg::cyan << display_time()
         << "Closing output file" << endl;

    outfile.close();
    show_console_cursor(true);

    cout << style::bold << fg::green << display_time()
         << "Done!" << endl;
    cout << style::reset << fg::reset;
}


