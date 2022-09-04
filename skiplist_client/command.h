#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <iostream>
#include <vector>


const std::string _all_order[6] = { "set","get","del","load","dump","display" };
const int _order_number = 6;

class Command {
public:
    std::string get_command();
    Command(std::string s) :_command(s) {};
    bool is_valid_command();
    void split_command();
    void print_error(int);
    std::vector<std::string> _arg;
private:

    std::string _command;

};
#endif
