#include "shush.h"

Shush::Shush(SystemType system_type) : system(system_type), arguments(0, {}, ArgumentTypes) {}

void Shush::help() {
    cout << "Shush - SSH Key and Password Manager" << endl;
    cout << "Usage: shush [options] [value]" << endl;
    cout << "Options:" << endl;
    cout << "  --help, -h: Show this help message" << endl;
    cout << "  --yes, -y: Skip confirmation prompts" << endl;

    cout << "  --add, -a: Add a new key" << endl;
    cout << "  --connect, -c: Connect to a system" << endl;
    cout << "  --list, -l: List all Systems / Hostnames" << endl;
    cout << "  --list --system <system_name>, -ls <system_name>: List Users in a systems" << endl;
    cout << "  --remove, -r: Remove a system configuration" << endl;
    cout << "  --update, -e: Update a system configuration" << endl;

    cout << "  --user, -u: Specify the login user" << endl;
    cout << "  --system, --host, -s: Specify the system/hostname to add" << endl;
    cout << "  --key, -k: Specify the ssh private key location" << endl;
    cout << "  --password, -p: Specify the login credentials" << endl;

    cout << "Examples:" << endl;
    cout << "  shush --add -s example.com -u user -k /path/to/key.pem" << endl;
    cout << "  shush --add -sup example.com user password" << endl;
    cout << "  shush -asup example.com user password" << endl;
    cout << "  shush --connect -s example.com -u user" << endl;
    cout << "  shush --connect -su example.com user" << endl;
    cout << "  shush -csu example.com user" << endl;
    cout << "  shush --list" << endl;
    cout << "  shush --list --system example.com" << endl;
    cout << "  shush -ls example.com" << endl;
    cout << "  shush -rsy localhost" << endl;
    cout << endl;
}

void Shush::prepare() {
    if (!this->system.exists(this->system.get_config_base(APPLICATION))) {
        this->system.make_directory(this->system.get_config_base(APPLICATION));
    }
    // check if ssh is installed on the system
    Command runner(this->system.get_system_type());
    runner.execute("ssh -V");
    if (runner.get_output().substr(0, 7) != "OpenSSH" && this->connectFlag) {
        throw Exception("OpenSSH is not installed on the system. Please install OpenSSH to use this application");
    }
    runner.execute("sshpass -V");
    if (runner.get_output().substr(0, 7) != "sshpass" && this->connectFlag) {
        throw Exception("sshpass is not installed on the system. Please install sshpass to use this application");
    }
}

vector<SystemObject> Shush::get_system_objects() {
    const string config_base = this->system.get_config_base(APPLICATION);
    const vector<string> systems = this->system.list_directory(config_base);
    vector<SystemObject> system_objects;
    for (string system : systems) {
        const string system_path = this->system.join_path(config_base, system);
        const string users_path = this->system.join_path(system_path, "users");
        const vector<string> users = this->system.list_directory(users_path);
        for (string user : users) {
            const string user_path = this->system.join_path(users_path, user);
            const bool password_exists = this->system.exists(this->system.join_path(user_path, "password.txt"));
            const bool ssh_key_exists = this->system.exists(this->system.join_path(user_path, "key.pem"));
            SystemObject system_object = {password_exists, ssh_key_exists, user, system};
            system_objects.push_back(system_object);
        }
    }
    return system_objects;
}

void Shush::init(int argc, char const *argv[]) {
    this->prepare();

    // Parse arguments
    this->arguments = ArgumentParser(argc, argv, ArgumentTypes);
    vector<string> keys = this->arguments.Keys();
    for (string key : keys) {
        if (key == "help" || key == "h") {
            this->helpFlag = true;
        } else if (key == "yes" || key == "y") {
            this->yesFlag = true;
        } else if (key == "add" || key == "a") {
            this->addFlag = true;
        } else if (key == "list" || key == "l") {
            this->listFlag = true;
        } else if (key == "remove" || key == "r") {
            this->removeFlag = true;
        } else if (key == "update" || key == "e") {
            this->updateFlag = true;
        } else if (key == "connect" || key == "c") {
            this->connectFlag = true;
        } else if (key == "system" || key == "host" || key == "s") {
            this->systemFlag = true;
            this->_system =
                (key == "system" || key == "host") ? this->arguments.Get("system") : this->arguments.Get("s");
            if (this->_system.size() == 0) {
                this->_system = this->arguments.Get("host");
            }
        } else if (key == "user" || key == "u") {
            this->userFlag = true;
            this->_user = key == "user" ? this->arguments.Get("user") : this->arguments.Get("u");
        } else if (key == "key" || key == "k") {
            this->KeyFlag = true;
            this->_ssh_key = key == "key" ? this->arguments.Get("key") : this->arguments.Get("k");
        } else if (key == "password" || key == "p") {
            this->passwordFlag = true;
            this->_password = key == "password" ? this->arguments.Get("password") : this->arguments.Get("p");
        } else {
            cout << "Unknown argument: " << key << endl;
            cout << "Use --help to see available arguments" << endl;
            break;
        }
    }

    // Run command
    if (this->helpFlag) {
        this->help();
        return;
    }
    if (this->listFlag) {
        this->list_systems(this->systemFlag && !this->arguments.isParserDefault(this->_system));
        return;
    }
    if (this->connectFlag) {
        this->connect_system();
        return;
    }
    if (this->addFlag || this->updateFlag) {
        this->add_system(updateFlag > 0);
        return;
    }
    if (this->removeFlag) {
        this->remove_system();
        return;
    }
    cout << "Shush v" << APPLICATION_VERSION << " - SSH Key and Password Manager." << endl;
    cout << "Use --help to see available arguments" << endl;
}

void message(string system, string user = "") {
    cout << "System/Host added successfully" << endl;
    if (user.size() == 0) {
        cout << "Use " << bold("shush --connect -s " + system) << " to connect to the system" << endl;
    } else {
        cout << "Use " << bold("shush --connect -s " + system + " -u " + user) << " to connect to the system" << endl;
    }
    cout << "or " << bold("shush --connect") << " to connect via an interactive menu" << endl;
}

void Shush::add_system(bool update = false) {
    // system validation
    if (!this->systemFlag) {
        return;
    }
    if (this->_system.size() == 0) {
        throw Exception("System/Host is required");
    }
    const string system_path = this->system.join_path(this->system.get_config_base(APPLICATION), this->_system);
    if (!this->system.exists(system_path)) {
        this->system.make_directory(system_path);
    } else if (!this->userFlag) {
        throw Exception("System/Host already exists");
    }
    // user validation
    if (!this->userFlag) {
        message(this->_system);
        return;
    }
    const string users_path = this->system.join_path(system_path, "users");
    const string user_path = this->system.join_path(users_path, this->_user);
    if (!this->system.exists(users_path)) {
        this->system.make_directory(users_path);
    }
    if (!this->system.exists(user_path)) {
        this->system.make_directory(user_path);
    } else if (!this->KeyFlag && !this->passwordFlag) {
        throw Exception("User already exists");
    }
    // Credentials validation
    if (!this->KeyFlag && !this->passwordFlag) {
        message(this->_system, this->_user);
        return;
    }
    const string key_path = this->system.join_path(user_path, "key.pem");
    const string password_path = this->system.join_path(user_path, "password.txt");
    if (this->_password.size() == 0 && !this->system.exists(this->_ssh_key)) {
        throw Exception("SSH Key does not exist");
    }
    if (!update && (this->system.exists(key_path) && this->_ssh_key.size() > 0)) {
        throw Exception("Key already exists");
    } else if (!update && this->system.exists(password_path) && this->_password.size() > 0) {
        throw Exception("Password already exists");
    }
    if (this->_ssh_key.size() > 0) {
        this->system.copy(this->_ssh_key, key_path);
    } else if (this->_password.size() > 0) {
        this->system.write(password_path, this->_password);
    }
    if (update) {
        cout << "System/Host updated successfully" << endl;
        return;
    }
    message(this->_system, this->_user);
}

void Shush::list_systems(bool detailed = false) {
    vector<SystemObject> system_objects = this->get_system_objects();
    if (!detailed && (!this->systemFlag || this->arguments.isParserDefault(this->_system))) {
        vector<string> headers = SHUSH_LIST_HEADERS;
        vector<vector<string>> data;
        unordered_map<string, bool> unique_systems;
        for (SystemObject system_object : system_objects) {
            if (unique_systems[system_object.system]) {
                continue;
            }
            vector<string> row = {system_object.system};
            data.push_back(row);
            unique_systems[system_object.system] = true;
        }
        print_tabular(headers, data, "No systems / hosts found");
        return;
    }
    vector<string> headers = SHUSH_HEADERS;
    vector<vector<string>> data;
    for (SystemObject system_object : system_objects) {
        if (this->_system.size() > 0 && system_object.system != this->_system) {
            continue;
        }
        vector<string> row = {system_object.system, system_object.username, system_object.ssh_key_exists ? "Yes" : "No",
                              system_object.password_exisits ? "Yes" : "No"};
        data.push_back(row);
    }
    print_tabular(headers, data, "No users found for system/hostname: " + this->_system);
}

void Shush::remove_system() {
    // delete system
    if (!this->systemFlag) {
        return;
    }
    if (this->_system.size() == 0) {
        throw Exception("System/Host is required");
    }
    const string system_path = this->system.join_path(this->system.get_config_base(APPLICATION), this->_system);
    if (!this->system.exists(system_path)) {
        throw Exception("System/Host does not exist");
    }
    if (!this->userFlag) {
        if (this->yesFlag || prompt("Delete system/hostname: " + this->_system + " ?")) {
            this->system.remove_directory(system_path);
            cout << "System/Host removed successfully" << endl;
        }
        return;
    }
    // delete user
    if (this->_user.size() == 0) {
        throw Exception("User is required");
    }
    const string users_path = this->system.join_path(system_path, "users");
    const string user_path = this->system.join_path(users_path, this->_user);
    if (!this->system.exists(users_path) || !this->system.exists(user_path)) {
        throw Exception("User does not exist");
    }
    if (!this->passwordFlag && !this->KeyFlag) {
        if (this->yesFlag || prompt("Delete user: " + this->_user + " ?")) {
            this->system.remove_directory(user_path);
            cout << "User removed successfully" << endl;
        }
        return;
    }
    // delete credentials
    if (this->passwordFlag) {
        const string password_path = this->system.join_path(user_path, "password.txt");
        if (!this->system.exists(password_path)) {
            throw Exception("Password does not exist");
        }
        if (this->yesFlag || prompt("Delete password for user: " + this->_user + " ?")) {
            this->system.remove(password_path);
            cout << "Password removed successfully" << endl;
        }
    }
    if (this->KeyFlag) {
        const string key_path = this->system.join_path(user_path, "key.pem");
        if (!this->system.exists(key_path)) {
            throw Exception("SSH Key does not exist");
        }
        if (this->yesFlag || prompt("Delete SSH Key for user: " + this->_user + " ?")) {
            this->system.remove(key_path);
            cout << "SSH Key removed successfully" << endl;
        }
    }
}

void Shush::connect_system() {
    vector<SystemObject> system_objects = this->get_system_objects();
    if (this->_system.size() == 0 || !this->systemFlag) {
        vector<string> menu_options_select_system;
        unordered_map<string, SystemObject> system_objects_map;
        for (SystemObject system_object : system_objects) {
            system_objects_map[system_object.system] = system_object;
        }
        for (SystemObject system_object : system_objects) {
            menu_options_select_system.push_back(system_object.system);
        }
        Menu menu = Menu("Select System -", menu_options_select_system);
        int selected_system = menu.navigate();
        if (selected_system == -1) {
            throw Exception("System not selected");
        }
        this->_system = menu_options_select_system[selected_system];
    }
    if (this->_user.size() == 0 || !this->userFlag) {
        vector<string> users;
        for (SystemObject system_object : system_objects) {
            if (system_object.system == this->_system) {
                users.push_back(system_object.username);
            }
        }

        Menu menu = Menu("Select User -", users);
        int selected_user = menu.navigate();
        if (selected_user == -1) {
            throw Exception("User not selected");
        }
        this->_user = users[selected_user];
    }
    const string system_path = this->system.join_path(this->system.get_config_base(APPLICATION), this->_system);
    const string user_path = this->system.join_path(this->system.join_path(system_path, "users"), this->_user);
    if (!this->system.exists(system_path) || !this->system.exists(user_path)) {
        throw Exception("System/Host or User does not exist");
    }
    const string key_path = this->system.join_path(user_path, "key.pem");
    const string password_path = this->system.join_path(user_path, "password.txt");
    if (!this->system.exists(key_path) && !this->system.exists(password_path)) {
        throw Exception("No credentials found for user: " + this->_user);
    }

    cout << "Connecting to " << bold(this->_user + "@" + this->_system) << endl;
    Command runner = Command(this->system.get_system_type());
    if (this->system.exists(key_path)) {
        runner.execute("ssh -i " + key_path + " " + this->_user + "@" + this->_system);
    } else {
        const string password = this->system.read(password_path);
        runner.execute("sshpass -p '" + password + "' ssh " + this->_user + "@" + this->_system, true);
    }
}