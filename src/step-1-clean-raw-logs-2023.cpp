
#include "main.h"

using namespace std;
using namespace rang;
using namespace indicators;



constexpr auto LOG_LOC {"./logs/i.ezproxy.nypl.org.2023-01-*.log"};

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



const string display_time() noexcept {
    string ret {fmt::format("[{}] ", to_string(time(0)))};
    return ret;
}


void handle_sigint(const int signum) {
    if (signum != 2)
        throw runtime_error {"received unhandled signal"};
    indicators::show_console_cursor(true);
    cerr << "\n" << fg::red << display_time() << "caught SIGINT\n"
         << style::reset << endl;
    exit(1);
}


const vector<string> get_files() noexcept {
    vector<string> input_files;
    for (const auto& p : glob::glob(LOG_LOC)) {
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
    return fmt::format("{}\t{}\t{}\t{}\t{}\t{}\n",
                       ip, barcode, session, date_and_time, url, fullurl);

}


void process_line(string* all_fields, const string& line) noexcept {
    uint8_t counter {0};
    string item {};
    stringstream ss {line};

    while (getline(ss, item, ' ')) {
        all_fields[counter] = move(item);
        ++counter;
        if (counter > 6)
            return;
    }
}


const string fix_whole_date(const string& adate) noexcept {
    struct tm tm{};
    strptime(adate.c_str(), "[%d/%b/%Y:%T", &tm);
    char datestring[20] {0};
    strftime(datestring, 20, "%F %T", &tm);
    string ret {datestring};
    return ret;
}


int main() {
    using namespace re2;

    signal(SIGINT, handle_sigint);

    cout << "\n\n" << fg::gray << style::dim
         << display_time() << "::alice glass:: HI!\n" << style::reset
         << style::bold << fg::cyan << display_time()
         << "Processing raw logs\n" << style::reset << endl;

    const vector<string> input_files = get_files();
    const auto count = input_files.size();
    const string last_date = input_files[count-1].substr(24, 10);
    const string output_file = fmt::format("intermediate/cleaned-logs-{}.dat", last_date);
    const RE2 re1  { "^https?[^A-Za-z]*(.+?):\\d.+$" };
    uint16_t counter { 0 };

    ofstream outfile {output_file};
    outfile << make_tab_delimited("ip", "barcode", "session", "date_and_time",
                                  "url", "fullurl");

    // reusing this to store the necessary fields
    string tmp[7];

    show_console_cursor(false);

    for (const auto& item : input_files) {
        ++counter;
        auto perc = static_cast<uint8_t>(round(counter*100/count));
        bar.set_option(option::PostfixText{
                fmt::format("  {}/{}  {}%", counter, count, perc) });
        bar.set_progress(perc);

        ifstream infile {item};
        string line {};
        while (getline(infile, line)) {
            process_line(tmp, line);

            string barcode  { move(tmp[1]) }; if (barcode == "-") continue;
            string ip       { move(tmp[0]) };
            string sessionp { move(tmp[2]) };
            string date     { move(tmp[3]) };
            string url      { move(tmp[6]) };
            string fullurl  { url };

            // fixing urls
            RE2::Replace(&url, re1, "\\1");

            date = fix_whole_date(date);

            outfile << make_tab_delimited(ip, barcode, sessionp, date, url,
                                          fullurl);
        }
    }

    show_console_cursor(true);
    cout << style::bold << fg::green << display_time() << "Done!"
         << style::reset << fg::reset << endl;
}


