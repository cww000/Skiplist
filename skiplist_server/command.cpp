#include "command.h"

using namespace std;

std::string Command::get_command() {
    return _command;
}


void Command::split_command() {

    if (_command.size() == 0) {
        return;
    }
    //分割字符串
    string command = get_command();
    auto len = command.size();
    decltype (len) slow = 0, i = 0;

    for (i = 0; i < len; ++i) {
        if (command[i] != ' ') {
            slow = i;   //当前字符不是空格
            while (command[i] != ' ' && i < command.size()) {  //快指针走到下一个空格的位置
                ++i;
            }

            string s = command.substr(slow, i - slow);
            _arg.push_back(s);
        }
    }
    return;
}

bool Command::is_valid_command() {
    split_command();
    if (_arg.size() == 0) {
        return false;
    }
    string _order = _arg[0];
    bool is_valid_order = false;
    for (int i = 0; i < _order_number; i++) {
        if (_order == _all_order[i]) {
            is_valid_order = true;
        }
    }
    if (!is_valid_order) {
        print_error(1);
        return false;
    }

    if (_order == _all_order[0] && _arg.size() != 3) {
        print_error(2);
        return false;
    }

    if (_order == _all_order[1] && _arg.size() != 2) {
        print_error(3);
        return false;
    }

    if (_order == _all_order[2] && _arg.size() != 2) {
        print_error(4);
        return false;
    }
    if (_order == _all_order[3] && _arg.size() != 1) {
        print_error(5);
        return false;
    }

    if (_order == _all_order[4] && _arg.size() != 1) {
        print_error(6);
        return false;
    }

    if (_order == _all_order[5] && _arg.size() != 1) {
        print_error(7);
        return false;
    }

    return true;
}

void Command::print_error(int error_number) {
    switch (error_number)
    {
    case 1/* constant-expression */:
        cout << "(error) ERR unknown command" << endl;
        break;

    case 2/* constant-expression */:
        cout << "ERR wrong number of arguments for 'set' command" << endl;
        break;

    case 3/* constant-expression */:
        cout << "(error) ERR wrong number of arguments for 'get' command" << endl;
        break;
    case 4/* constant-expression */:
        cout << "(error) ERR wrong number of arguments for 'del' command" << endl;
        break;

    case 5/* constant-expression */:
        cout << "(error) ERR wrong number of arguments for 'load' command" << endl;
        break;
    case 6/* constant-expression */:
        cout << "(error) ERR wrong number of arguments for 'dump' command" << endl;
        break;
    case 7/* constant-expression */:
        cout << "(error) ERR wrong number of arguments for 'display' command" << endl;
        break;
    default:
        break;
    }
}
