#include "system.h"

System::System(SystemType system_type) { this->systemType = system_type; }

void System::copy(std::string source, std::string destination) {
    if (this->exists(source)) {
        std::filesystem::copy(source, destination);
    } else {
        throw Exception(source + " does not exist");
    }
}

void System::move(std::string source, std::string destination) {
    if (this->exists(source)) {
        std::filesystem::rename(source, destination);
    } else {
        throw Exception(source + " does not exist");
    }
}

void System::remove(std::string source) {
    if (this->exists(source)) {
        std::filesystem::remove(source);
    } else {
        throw Exception(source + " does not exist");
    }
}

void System::rename(std::string source, std::string destination) {
    if (this->exists(source)) {
        this->move(source, destination);
    } else {
        throw Exception(source + " does not exist");
    }
}

void System::make_directory(std::string directory) {
    if (!this->exists(directory)) {
        std::filesystem::create_directory(directory);
    } else {
        throw Exception(directory + " already exists");
    }
}

void System::remove_directory(std::string directory) {
    if (this->exists(directory)) {
        std::filesystem::remove_all(directory);
    } else {
        throw Exception(directory + " does not exist");
    }
}

std::vector<std::string> System::list_directory(std::string path) {
    std::vector<std::string> ls;
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        std::string out = entry.path().filename().string();
        ls.push_back(out);
    }
    return ls;
}

std::string System::read(std::string file) {
    std::fstream reader(file);
    std::string content;
    reader >> content;
    reader.close();
    return content;
}

void System::write(std::string file, std::string content) {
    std::fstream writer(file, std::ios::out);
    writer << content;
    writer.close();
}

bool System::exists(std::string path) { return std::filesystem::exists(path); }

SystemType System::get_system_type() { return this->systemType; }

std::string System::get_system_type_string() {
    std::vector<std::string> system_strings = {"WINDOWS", "LINUX", "UNKNOWN"};
    return system_strings[static_cast<int>(systemType)];
}

std::string System::get_config_base(std::string application_name) {
    if (this->systemType == SystemType::WINDOWS) {
        return std::string(getenv("APPDATA")) + "\\" + application_name;
    } else if (this->systemType == SystemType::LINUX) {
        return std::string(getenv("HOME")) + "/." + application_name;
    } else {
        return "";
    }
}

std::string System::join_path(std::string path1, std::string path2) {
    if (this->systemType == SystemType::WINDOWS) {
        return path1 + "\\" + path2;
    }
    return path1 + "/" + path2;
}