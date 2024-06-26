#include "../include/Parser.h"

int Parser::ParseInt(const std::string& line) {
    try {
        int number = std::stoi(line);

        if (number < 0) {
            throw std::invalid_argument("Input data format error");
        }

        return number;
    }
    catch (const std::invalid_argument& e) {
        throw std::invalid_argument(std::string("ParseInt throw an exception"));
    }
}

std::vector<std::string> Parser::SplitString(const std::string& line) {
    std::vector<std::string> lineSplit;
    std::string part;
    std::istringstream iss(line);

    while (iss >> part) lineSplit.push_back(part);

    return lineSplit;
}

void Parser::ParseConfig(std::ifstream& file, ComputerClub& compClub) {
    std::string line;
    try {
        if (!std::getline(file, line) || line.empty()) {
            throw std::invalid_argument(line);
        }
        int numTables = ParseInt(line);

        if (!std::getline(file, line) || line.empty()) {
            throw std::invalid_argument(line);
        }
        std::vector<std::string> lineSplit = SplitString(line);

        if (lineSplit.size() != 2) {
            throw std::invalid_argument(line);
        }

        Time startTime = ParseTime(lineSplit[0]);
        Time endTime = ParseTime(lineSplit[1]);

        if (startTime > endTime || !std::getline(file, line) || line.empty()) {
            throw std::invalid_argument(line);
        }
        int pricePerHour = ParseInt(line);

        compClub = ComputerClub(numTables, startTime, endTime, pricePerHour);
        compClub.OpenShift();
    }
    catch (const std::invalid_argument& e) {
        throw std::invalid_argument(line);
    }
}

void Parser::ParseEvents(std::ifstream& file, ComputerClub& compClub) {
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> lineSplit = SplitString(line);

        if (lineSplit.size() < 3 || lineSplit.size() > 4) {
            throw std::invalid_argument(line);
        }

        Event newEvent;
        try {
            newEvent = {
                ParseTime(lineSplit[0]),
                ParseInt(lineSplit[1]),
                lineSplit[2],
                lineSplit.size() == 4 ? ParseInt(lineSplit[3]) : 0
            };

            if (newEvent.id == ComputerClub::Events::CLIENTSIT && lineSplit.size() == 4) {
                newEvent.tableId = ParseInt(lineSplit[3]);
            }
            else if (lineSplit.size() != 3) {
                throw std::invalid_argument(line);
            }
        }
        catch (const std::invalid_argument& e) {
            throw std::invalid_argument(line);
        }

        try {
            compClub.HandleEvent(newEvent);
        }
        catch (const std::invalid_argument& e) {
            throw std::invalid_argument(line);
        }
    }

    compClub.CloseShift();
}

void Parser::ParseFile(std::string& filename, ComputerClub& compClub) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Can't open the file" << std::endl;
        return;
    }

    try {
        ParseConfig(file, compClub);
        ParseEvents(file, compClub);
    }
    catch (const std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
        return;
    }
}