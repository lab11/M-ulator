/*******************************
 *  
 *  Monarch decoder v1.0.5
 *  Author: Roger Hsiao
 *  
 *  Works with mbc_code.c v5.2.3 and above
 *  Usage: ./decoder.exe [packets] [date.log | 0] [time.log | 0] [light out file] [temp out file] [delimeter]
 *
 *  v1.0.0: 
 *    Added automatic time translation
 *
 *  v1.0.1:
 *    Decoding doesn't halt when encountering an Unrecognized code anymore
 *
 *  v1.0.2:
 *    Updated decoder to use local date instead of epoch date
 *
 *  v1.0.3:
 *    Not subtracting timezone difference in time.log to get GMT time
 *
 *  v1.0.4:
 *    Fixed decoder index bug that doesn't update index properly, which causes a timeshift
 *
 *  v1.0.5:
 *    Reading date_tz correctly now. And adding date_tz to the temp timestamps
 *
 *  v1.0.6:
 *    Adding delimeter option
 *    Adding support for missing packets XXX
 *
 *
 *******************************/


#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <map>
#include <cmath>
#include <unistd.h>
#include <stdexcept>
#include "../pre_v20e/software/mbc_code/huffman_encodings.h"
// #include "/home/rogerhh/M-ulator/platforms/m3/pre_v20e/software/mbc_code/huffman_encodings.h"

#define DAWN 0
#define NOON 1
#define DUSK 2
#define NIGHT 3

using namespace std;

// using Unit = bitset<272>;
struct Unit {
    int len = 0;
    bitset<272> data;
};

struct NoMoreData {
    string str = "No more data in unit\n";
};

Unit create_unit(const string packets[]);
uint32_t get_data(Unit& u, int len);

uint16_t resample_indices[4] = {32, 40, 44, 1000};
uint16_t intervals[4] = {1, 2, 8, 32};

class LightParser {
public:
    uint32_t sys_time_in_min = 0;
    int day_state = 0;
    int index = 0;
    map<uint32_t, double> data;
    map<int, map<int, int>> codes;

    void parse_unit(Unit& u);
    void read_header(Unit& u);
    void read_timestamp(Unit& u);

    LightParser();
};

class TempParser {
public:
    map<uint32_t, double> data;
    map<int, map<int, int>> codes;

    void parse_unit(Unit& u);

    TempParser();
};

int main(int argc, char** argv) {

    if(argc != 6 && argc != 7) {
        cerr << "Usage: ./decoder.exe packet_file date.log time.log light output file temp output file [delimeter]" << endl;
        return 1;
    }

    char delim = ' ';
    if(argc >= 7) {
        delim = argv[6][0];
        cout << "delim = \'" << delim << "\'" << endl;
    }

    string packet_filename = string(argv[1]);
    ifstream fin(packet_filename);
    if(!fin.is_open()) {
        cerr << "Error opening: " << packet_filename << endl;
        return 1;
    }
    string date_log = string(argv[2]);
    ifstream date_fin(date_log);
    if(date_log != "0" && !date_fin.is_open()) {
        cerr << "Error opening: " << date_log << endl;
        return 1;
    }
    string time_log = string(argv[3]);
    ifstream time_fin(time_log);
    if(time_log != "0" && !time_fin.is_open()) {
        cerr << "Error opening: " << time_log << endl;
        return 1;
    }
    bool no_time_translation = date_log == "0";

    string light_filename = string(argv[4]);
    ofstream light_fout(light_filename);
    if(!light_fout.is_open()) {
        cerr << "Error opening: " << light_filename << endl;
        return 1;
    }
    string temp_filename = string(argv[5]);
    ofstream temp_fout(temp_filename);
    if(!temp_fout.is_open()) {
        cerr << "Error opening: " << temp_filename << endl;
        return 1;
    }

    double abs_time = 0, programmed_time = 0, tz = 0, trigger_fire_date = 0, date_tz = 0;

    if(!no_time_translation) {
        // Read UTC offset
        int programmed_date = 0;
        string trash;
        getline(date_fin, trash, ':');
        date_fin >> trigger_fire_date;
        getline(date_fin, trash, '(');
        date_fin >> date_tz;
        getline(date_fin, trash, ':');
        date_fin >> programmed_date;
        cout << trigger_fire_date << " " << programmed_date << endl;

        char c;
        getline(time_fin, trash, ':');
        time_fin >> abs_time;
        getline(time_fin, trash, ':');
        time_fin >> programmed_time >> c >> c >> tz;

        cout << programmed_date << " " << (int) abs_time << " " << programmed_time << " " << tz << endl;
    }
    else {
        cout << "No auto time translation" << endl;
    }

    LightParser lp;
    TempParser tp;

    string packets[4];
    while(fin >> packets[0] >> packets[1] >> packets[2] >> packets[3]) {
        auto u = create_unit(packets);
        if(u.len > 0) {
            cout << u.len << endl;
            cout << packets[0].substr(0, 1) << endl;
            if(stoi(packets[0].substr(0, 1), 0, 16) & 0b1000) {
                // is light
                lp.parse_unit(u);
            } 
            else {
                // is temp
                tp.parse_unit(u);
            }
        }
    }

    for(auto p : lp.data) {
        // Not doing anything with timezone because it seems like it's already done by python
        light_fout << fixed << p.first << delim << p.first + abs_time + 0 * tz << delim << p.second << delim << pow(2, (p.second / 32.0)) << endl;
    }
    for(auto p : tp.data) {
        temp_fout << fixed << p.first + trigger_fire_date * 86400 + date_tz << delim << pow(2, ((p.second + 128) / 16.0)) << endl;
    }
}

Unit create_unit(const string packets[]) {
    Unit u;
    u.data = bitset<272>(0);
    bool flag = false;
    for(int i = 0; i < 4; i++) {
        for(int j = 3; j < packets[i].size(); j++) {
            // If found an X, stop reading unit
            if(packets[i].substr(j, 1) == "X") {
                flag = true;
                break;
            }
            u.len += 4;
            u.data <<= 4;
            u.data |= bitset<272>(stoi(packets[i].substr(j, 1), 0, 16));
        }
        if(flag) { break; }
    }
    // Fill in 0s for the rest of the unit that was not read in due to a missing packet
    u.data <<= (272 - u.len);
    cout << u.data << endl;
    return u;
}

uint32_t get_data(Unit& u, int len) {
    // if len > maximum valid data, throw an error
    if(u.len < len) {
        throw NoMoreData{};
    }
    u.len -= len;
    uint32_t mask = (1 << len) - 1;
    auto temp = u.data;
    temp >>= (272 - len);
    temp &= mask;
    u.data <<= len;
    return temp.to_ulong();
}

LightParser::LightParser() {
    // preprocess codes
    for(int i = 0; i < 67; i++) {
        auto code = light_diff_codes[i];
        auto len = light_code_lengths[i];
        if(i == 64) {
            codes[len][code] = 0x1FF;
        }
        else if(i == 65) {
            codes[len][code] = 0x7FF;
        }
        else if(i == 66) {
            codes[len][code] = 0x1000;
        }
        else {
            codes[len][code] = i - 32;
        }
    }
}

uint32_t sign_extend(uint32_t src, int len) {
    if(src & (1 << (len - 1))) {
        cout << "sign extend neg" << endl;
        return src - (1 << len);
    }
    else {
        return src;
    }
}

void LightParser::read_timestamp(Unit& u) {
    auto t = get_data(u, 17); 
    // Not worried about overflow for now
    // TODO: add support for potential overflow of the 17bits of time
    sys_time_in_min = t;
    cout << "parsed time in min = " << sys_time_in_min << endl;
}

void LightParser::read_header(Unit& u) {
    day_state = get_data(u, 2);
    cout << "day state = " << day_state << endl;
    if(day_state == NIGHT) {
        throw runtime_error("Invalid day state!");
    }

    index = get_data(u, 7);
    cout << "index: " << index << endl;
    cout << "day state = " << day_state << endl;
}

void LightParser::parse_unit(Unit& u) {
    bool has_header = false;
    bool has_timestamp = false;
    bool has_cur = false;
    int cur = 0;

    cout << "light_unit" << endl;

    if(u.len != 272) {
        // missing packet in this unit
        cout << "Warning: Missing packet in this unit" << endl;
    }

    try {
        // If an error is throw, it means that there is no more valid data in this unit
        // it could be that there is a missing packet or there is no space in the last packet to put
        // in the ending code of the unit. In both cases, just abort reading the unit
        while(u.data.any()) {
            cout << "u any = " << u.data.any() << " " << u.data << endl;
            if(!has_timestamp) {
                read_timestamp(u);
                has_timestamp = true;
            }
            if(!has_header) {
                read_header(u);
                has_header = true;
            }
            if(!has_cur) {
                // read initial val
                cur = get_data(u, 11);
                has_cur = true;
            }
            else {
                // else read code
                int len = 0;
                int tmp = 0;
                bool flag = false;
                // According to the Huffman encoding table, there cannot be a code longer than 12 bits
                while(len < 12) {
                    int b = get_data(u, 1);
                    tmp <<= 1;
                    tmp |= b;
                    len++;
                    cout << "codes: " << len << " " << tmp << " " << bitset<12>(tmp) << endl;
                    auto it = codes.find(len);
                    if(it == codes.end()) {
                        continue;
                    }
                    auto it2 = codes[len].find(tmp);
                    if(it2 == codes[len].end()) {
                        continue;
                    }
                    auto code = it2->second;

                    cout << "code: " << code << endl;

                    if(code == 0x1000) {
                        day_state = (day_state + 1) & 0b11;
                        if(day_state == NIGHT) {
                            day_state = DAWN;
                        }
                        index = 0;
                        cout << "end day state" << endl;
                        cout << "day state = " << day_state << "; index = " << 0 << endl;
                        has_timestamp = false;
                    }
                    else if(code == 0x7FF) {
                        // don't use diff
                        code = get_data(u, 11);
                        cout << "diff 11" << endl;
                        cur = code;
                    }
                    else if(code == 0x1FF) {
                        code = get_data(u, 9);
                        cout << "diff 9: code: " << bitset<9>(code) << endl;
                        code = sign_extend(code, 9);
                        cout << "exteneded: " << code << endl;
                        cur += code;
                    }
                    else {
                        cur += code;
                    }
                    // set flag to true if found code
                    flag = true;
                    break;
                }
                if(!flag) {
                    cout << u.data << endl;
                    cout << "Unrecognized code" << endl;
                    continue;
                    throw runtime_error("Unrecognized code: " + to_string(tmp));

                }
                if(!has_timestamp) {
                    continue;
                }
            }
            data[sys_time_in_min * 60] = cur;
            cout << sys_time_in_min << " " << sys_time_in_min * 60 << " " << cur << endl;
            index++;

            // update sys_time_in_min for next data point
            if(u.data.any()) {
                if(day_state == DAWN) {
                    for(int i = 0; i < 4; i++) {
                        if(index < resample_indices[i]) {
                            sys_time_in_min += intervals[i];
                            cout << " test" << endl;
                            break;
                        }
                    }
                }
                else if(day_state == DUSK) {
                    for(int i = 0; i < 4; i++) {
                        if(index < resample_indices[i]) {
                            sys_time_in_min -= intervals[i];
                            break;
                        }
                    }
                }
                else {
                    sys_time_in_min += 32;
                }
            }
        }
    }
    catch(NoMoreData& e) {
        cout << "No more valid data in this unit." << endl;
    }
    cout << "End unit. sys_time_in_min = " << sys_time_in_min << endl << endl;

    return;
}

void TempParser::parse_unit(Unit& u) {
    uint32_t day_count = get_data(u, 7);
    uint32_t timestamp_in_half_hour = get_data(u, 6);
    uint32_t cur_value = get_data(u, 7);

    data[day_count * 86400 + timestamp_in_half_hour * 1800] = cur_value;
    cout << day_count * 86400 + timestamp_in_half_hour * 1800 << " " << cur_value << endl;

    if(u.len != 272) {
        // missing packet in this unit
        cout << "Warning: Missing packet in this unit" << endl;
    }

    try {
        while(u.data.any()) {
            cout << u.data << endl;
            int len = 0;
            int tmp = 0;
            bool flag = false;
            while(len < 4) {
                int b = get_data(u, 1);
                tmp <<= 1;
                tmp |= b;
                len++;
                cout << "temp codes: " << len << " " << tmp << " " << bitset<4>(tmp) << endl;
                auto it = codes.find(len);
                if(it == codes.end()) {
                    continue;
                }
                auto it2 = codes[len].find(tmp);
                if(it2 == codes[len].end()) {
                    continue;
                }
                auto code = it2->second;

                cout << "temp code: " << code << endl;

                if(code == 0x7F) {
                    code = get_data(u, 7);
                    cout << "diff 7" << endl;
                    cur_value = code;
                }
                else {
                    cur_value += code;
                }
                flag = true;
                break;
            }

            if(!flag) {
                cout << "Unrecognized code" << endl;
                continue;
                throw runtime_error("Unrecognized code: " + to_string(tmp));
            }

            timestamp_in_half_hour++;
            if(timestamp_in_half_hour >= 48) {
                timestamp_in_half_hour = 0;
                day_count++;
            }

            data[day_count * 86400 + timestamp_in_half_hour * 1800] = cur_value;
            cout << day_count << " " << timestamp_in_half_hour << endl;
            cout << day_count * 86400 + timestamp_in_half_hour * 1800 << " " << cur_value << endl;
        
        }
    }
    catch(NoMoreData& e) {
        cout << "No more valid data in this unit." << endl;
    }

    cout << "End temp unit" << endl;

    return;
}

TempParser::TempParser() {
    // preprocess codes
    for(int i = 0; i < 5; i++) {
        auto code = temp_diff_codes[i];
        auto len = temp_code_lengths[i];
        if(i == 4) {
            codes[len][code] = 0x7F;
        }
        else {
            codes[len][code] = i - 2;
        }
    }
}

