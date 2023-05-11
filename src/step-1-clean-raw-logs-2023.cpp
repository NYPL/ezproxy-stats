
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "glob.h"


using namespace std;




const vector<string> get_files() {
    vector<string> input_files;
    for (auto& p : glob::glob("./logs/i.ezproxy*.log")) {
        input_files.push_back(p);
    }
    sort(input_files.begin(), input_files.end());
    // remove last (incomplete log)
    input_files.pop_back();
    return input_files;
}

int main() {
    cout << "::alice glass:: HI!" << endl;

    const vector<string> input_files = get_files();

    for (auto& item : input_files) {
        cout << item << endl;
    }

    cout << "First: " << input_files[0] << endl;
    cout << "Last: " << input_files[input_files.size()-1] << endl;

}


