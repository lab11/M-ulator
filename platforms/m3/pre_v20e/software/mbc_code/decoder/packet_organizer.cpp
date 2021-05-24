/*******************************
 *  
 *  Packet organizer v1.0.0
 *  Author: Roger Hsiao
 *  
 *  Works with mbc_code.c v5.2.3 and above
 *  Usage: ./packet_organizer.exe [CHIP ID] [/path/to/ah_100.csv] [/path/to/output.csv]
 *
 *  Reads the raw radio packets and generate decoder input
 *
 *  v1.0.0: 
 *    Basic functionality
 *
 *  v1.0.1: 
 *    Added XXX for missing packets
 *
 *
 *******************************/


#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <cassert>

using namespace std;

struct packet {
    bool is_light     = false;
    int chip_id       = 0;
    int packet_number = 0;
    int full_packet_number = 0;
    string packet;
    string data;

    void parse_packet(string input) {
        packet = input;
        // Remove CRC bits
        string header_str = input.substr(0, 3);
        int header = stoi(header_str, 0, 16);
        is_light = ((header >> 11) & 0b1) != 0;
        chip_id = (header >> 6) & 0b11111;
        packet_number = header & 0b111111;
        data = input.substr(3);
    }

    struct hash {
        size_t operator()(const struct packet& p) const {
            return std::hash<string>{}(p.data);
        }
    };

};

bool operator==(const packet& rhs, const packet& lhs) {
    return rhs.is_light == lhs.is_light && (rhs.packet_number & 0b11) == (lhs.packet_number & 0b11) && rhs.chip_id == lhs.chip_id && rhs.data == lhs.data;
}

bool operator!=(const packet& rhs, const packet& lhs) {
    return !(rhs == lhs);
}


struct unit {
    // packet number group, will have the first same 4 bits
    unsigned int packet_number = -1;
    bool is_light = false;

    vector<bool> valid = vector<bool>(4, false);
    vector<packet> packets{4};

};

int main(int argc, char** argv) {

    // 1. Read through the input packet by packet, place packets into their respective positions until first repetition
    // 2. If see a repeated packet, update full packet number

    map<int, packet> light_result, temp_result;
    unordered_set<packet, packet::hash> light_packets, temp_packets;

    int CHIP_ID = stoi(string(argv[1]), 0, 16);
    ifstream fin(argv[2]);
    if(!fin.is_open()) {
        cout << "Error opening file: " << argv[2] << endl;
        return 1;
    }
    packet last_packet;
    unit last_unit;
    int cur_light_full_packet_num = 0;
    int cur_temp_full_packet_num = 0;

    string str;
    while(fin >> str) {
        // filter out repeated packets
        if(str == last_packet.packet) {
            continue;
        }

        packet p;
        p.parse_packet(str);

        // filter out chip_ids that don't match
        if(p.chip_id != CHIP_ID) {
            continue;
        }

        cout << p.is_light << " " << p.chip_id << " " << p.packet_number << " " << p.packet << endl;

        if(p.is_light) {
            auto it = light_packets.find(p);
            if(it != light_packets.end()) {
                // if can find packet in set, update full_packet_number
                cur_light_full_packet_num = it->full_packet_number;
            }
            else {
                // if can't find packet in set, add packet to set at the right position
                while((cur_light_full_packet_num & 0b111111) != p.packet_number) {
                    cur_light_full_packet_num++;
                }
                p.full_packet_number = cur_light_full_packet_num;

                light_packets.insert(p);
                light_result[cur_light_full_packet_num] = p;
            }
        }
        else {
            auto it = temp_packets.find(p);
            if(it != temp_packets.end()) {
                // if can find packet in set, update full_packet_number
                cur_temp_full_packet_num = it->full_packet_number;
            }
            else {
                // if can't find packet in set, add packet to set at the right position
                while((cur_temp_full_packet_num & 0b111111) != p.packet_number) {
                    cur_temp_full_packet_num++;
                }
                p.full_packet_number = cur_temp_full_packet_num;

                temp_packets.insert(p);
                temp_result[cur_temp_full_packet_num] = p;
            }
        }
    }

    // Move last packets to the front if applicable
    // This is to account for the possibility that the first few packets/units will not be received in the first round of radio-out
    if(light_result.rbegin()->second.packet_number < light_result.begin()->second.packet_number) {
        int full_packet_number = light_result.rbegin()->second.full_packet_number;
        auto it = light_result.end();
        it--;
        while(it != light_result.begin() && (it->second.full_packet_number >> 6) == (full_packet_number >> 6)) {
            auto temp_it = light_result.find(it->second.packet_number);
            assert(temp_it == light_result.end());
            it->second.full_packet_number = it->second.packet_number;
            light_result[it->second.packet_number] = it->second;
            cout << "Moving packet to front: " << it->second.packet << endl;
            // deleting the moved packet
            auto vic = it;
            it--;
            light_result.erase(vic);
        }
    }

    if(temp_result.rbegin()->second.packet_number < temp_result.begin()->second.packet_number) {
        int full_packet_number = temp_result.rbegin()->second.full_packet_number;
        auto it = temp_result.end();
        it--;
        while(it != temp_result.begin() && (it->second.full_packet_number >> 6) == (full_packet_number >> 6)) {
            auto temp_it = temp_result.find(it->second.packet_number);
            assert(temp_it == temp_result.end());
            it->second.full_packet_number = it->second.packet_number;
            temp_result[it->second.packet_number] = it->second;
            cout << "Moving packet to front: " << it->second.packet << endl;
            // deleting the moved packet
            auto vic = it;
            it--;
            temp_result.erase(vic);
        }
    }

    cout << "Final result\n";
    ofstream fout(argv[3]);
    if(!fout.is_open()) {
        cout << "Error opening file: " << argv[3] << endl;
        return 1;
    }

    // Fill in empty packets with XXX
    for(int i = 0; i < (light_result.rbegin()->first & 0xFFFFFFFC) + 0x4; i++) {
        auto it = light_result.find(i);
        if(it == light_result.end()) {
            packet p;
            p.packet = "XXXXXXXXXXXXXXXXXXXX";
            light_result[i] = p;
            cout << "Added all X's at the " << i << "th packet" << endl;
        }
    }
    for(auto pair : light_result) {
        auto p = pair.second;
        cout << pair.first << " " << p.is_light << " " << p.chip_id << " " << p.packet_number << " " << p.packet << endl;
        fout << p.packet << endl;
    }
    cout << endl;
    fout << endl;

    // Fill in empty packets with XXX
    for(int i = 0; i < (temp_result.rbegin()->first & 0xFFFFFFFC) + 0x4; i++) {
        auto it = temp_result.find(i);
        if(it == temp_result.end()) {
            packet p;
            p.packet = "XXXXXXXXXXXXXXXXXXXX";
            temp_result[i] = p;
        }
    }
    for(auto pair : temp_result) {
        auto p = pair.second;
        cout << pair.first << " " << p.is_light << " " << p.chip_id << " " << p.packet_number << " " << p.packet << endl;
        fout << p.packet << endl;
    }

}
