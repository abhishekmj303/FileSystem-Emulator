#include <bits/stdc++.h>
using namespace std;

string time_now();

class TreeNode{
public:
    string name;
    list<string> content;
    char type;
    string cdate;
    string mdate;
    int perm;
    TreeNode* link;
    TreeNode* parent;
    TreeNode* child;

    TreeNode(TreeNode* pwd, string n)
    {
        name = n;
        cdate = mdate = time_now();
        perm = 6;
        link = NULL;
        child = NULL;
        parent = pwd;
    }
    
    string get_perm()
    {
        string res;
        if (perm == 0) res = "---";
        else if (perm == 1) res = "--x";
        else if (perm == 2) res = "-w-";
        else if (perm == 3) res = "-wx";
        else if (perm == 4) res = "r--";
        else if (perm == 5) res = "r-x";
        else if (perm == 6) res = "rw-";
        else if (perm == 7) res = "rwx";

        return res;
    }
};

void linux_tree(TreeNode* root);
void print_help();
void print_tree(TreeNode* root, string prev);
void print_ls(TreeNode* pwd);
void print_stat(TreeNode* root, TreeNode* pwd, string path);
string pwd_str(TreeNode* root, TreeNode* pwd);
list<string> find_names(TreeNode* root, TreeNode* pwd, string name);
TreeNode* find_node(TreeNode* root, TreeNode* pwd, string path);
TreeNode* find_on_pwd(TreeNode* pwd, string name);
list<string> split(string str, char delim);
string join(list<string> str, char delim);
string* split_name(string str);
TreeNode* cd(TreeNode* root, TreeNode* pwd, string path);
TreeNode* create(TreeNode* root, TreeNode* pwd, string path, char type);
void remove(TreeNode* root, TreeNode* pwd, string path);
void dupl(TreeNode* root, TreeNode* pwd, string src, string dst, int keep);
void edit(TreeNode* root, TreeNode* pwd, string path);
void cat(TreeNode* root, TreeNode* pwd, string path);
void chmod(TreeNode* root, TreeNode* pwd, string path, string new_modes);

int main()
{
    TreeNode* root = new TreeNode(NULL, "");
    root->type = 'd';
    TreeNode* pwd = root;
    linux_tree(root);
    print_help();

    string cmd;
    cout << endl << pwd_str(root, pwd) << ">> ";
    while (getline(cin >> ws, cmd))
    {
        list<string> args = split(cmd, ' ');
        TreeNode* temp_pwd;
        if (cmd == "help")
            print_help();
        else if (args.front() == "ls")
        {
            if (args.size() == 1)
                print_ls(pwd->child);
            else while (args.size() != 1)
            {
                args.pop_front();
                cout << args.front() << ":" << endl;
                temp_pwd = cd(root, pwd, args.front());
                if (temp_pwd != NULL)
                    print_ls(temp_pwd->child);
            }
        }
        else if (args.front() == "tree")
        {
            if (args.size() == 1)
                print_tree(pwd->child, "");
            else while (args.size() != 1)
            {
                args.pop_front();
                cout << args.front() << ":\n.\n";
                temp_pwd = cd(root, pwd, args.front());
                if (temp_pwd != NULL)
                    print_tree(temp_pwd->child, "");
            }
            
        }
        else if (cmd == "pwd")
            cout << pwd_str(root, pwd) << endl;
        else if (args.front() == "cd")
        {
            if (args.size() == 1)
                pwd = root;
            else
            {
                args.pop_front();
                temp_pwd = cd(root, pwd, args.front());
                if (temp_pwd != NULL)
                    pwd = temp_pwd;
            }
            
        }
        else if (args.front() == "find")
        {
            if (args.size() == 1)
                cout << "find: missing operand" << endl;
            else while(args.size() != 1)
            {
                args.pop_front();
                list<string> res;
                if (args.front()[0] == '/')
                    res = find_names(root, root, args.front());
                else
                    res = find_names(pwd, pwd, args.front());
                if (res.size() == 0)
                    cout << "find: '" << args.front() << "': no such file or directory" << endl;
                else
                    for (list<string>::iterator it = res.begin(); it != res.end(); it++)
                        cout << *it << endl;
            }
        }
        else if (args.front() == "stat")
        {
            if (args.size() == 1)
                cout << "stat: missing operand" << endl;
            else while (args.size() != 1)
            {
                args.pop_front();
                print_stat(root, pwd, args.front());
            }
        }
        else if (args.front() == "mkdir")
        {
            if (args.size() == 1)
                cout << "mkdir: missing operand" << endl;
            else while(args.size() != 1)
            {
                args.pop_front();
                create(root, pwd, args.front(), 'd');
            }
        }
        else if (args.front() == "touch")
        {
            if (args.size() == 1)
                cout << "touch: missing operand" << endl;
            else while(args.size() != 1)
            {
                args.pop_front();
                create(root, pwd, args.front(), '-');
            }
        }
        else if (args.front() == "rm" || args.front() == "rmdir")
        {
            if (args.size() == 1)
                cout << args.front() << ": missing operand" << endl;
            else while(args.size() != 1)
            {
                args.pop_front();
                remove(root, pwd, args.front());
            }
        }
        else if (args.front() == "cp" || args.front() == "mv")
        {
            if (args.size() != 3)
                cout << args.front() << ": missing operand" << endl;
            else
            {
                int keep;
                if (args.front() == "cp") keep = 1;
                else keep = 0;
                args.pop_front();
                string src = args.front();
                args.pop_front();
                string dst = args.front();
                args.pop_front();
                dupl(root, pwd, src, dst, keep);
            }
        }
        else if (args.front() == "edit")
        {
            if (args.size() == 1)
                cout << "edit: missing operand" << endl;
            else
            {
                args.pop_front();
                edit(root, pwd, args.front());
            }
        }
        else if (args.front() == "cat")
        {
            if (args.size() == 1)
                cout << "cat: missing operand" << endl;
            else
            {
                args.pop_front();
                cat(root, pwd, args.front());
            }
        }
        else if (args.front() == "chmod")
        {
            if (args.size() < 3)
                cout << "chmod: missing operand" << endl;
            else
            {
                args.pop_front();
                string perm = args.front();
                args.pop_front();
                chmod(root, pwd, args.front(), perm);
            }
        }
        else if (cmd == "exit")
            return 0;
        else
            cout << "Unknown command" << endl;
        
        cout << endl << pwd_str(root, pwd) << ">> ";
    }

    cout << endl;
    return 0;
}

void print_help()
{
    cout << "*** Follows the syntax of linux shell commands ***" << endl << endl;
    cout << "\thelp - print following message" << endl;
    cout << "\tls - list directory" << endl;
    cout << "\tcd - change directory" << endl;
    cout << "\ttree - display the files/directory in tree view" << endl;
    cout << "\tpwd - print working directory" << endl;
    cout << "\tfind - find file/directory path by name" << endl;
    cout << "\tstat - display file/directory information" << endl;
    cout << "\tcp - copy file" << endl;
    cout << "\tmv - move file" << endl;
    cout << "\tmkdir - make directory" << endl;
    cout << "\trmdir - remove directory" << endl;
    cout << "\ttouch - create file" << endl;
    cout << "\trm - remove file" << endl;
    cout << "\tcat - Display file content" << endl;
    cout << "\tedit - edit file" << endl;
    cout << "\tchmod - change file permission" << endl;
    cout << "\texit - exit program" << endl;
}

void print_tree(TreeNode* root, string prev)
{
    if (root != NULL)
    {
        string next, branch, branch_next;

        if (root->link == NULL)
        {
            branch = "`";
            branch_next = " ";
        }
        else
        {
            branch = "|";
            branch_next = "|";
        }
        if (prev == "")
        {
            cout << prev + branch << "-- " << root->name;
            next = prev + branch_next;
        }
        else
        {
            cout << prev + "   " + branch << "-- " << root->name;
            next = prev + "   " + branch_next;
        }
        
        if (root->type == 'd')
            cout << "/";
        cout << endl;

        print_tree(root->child, next);
        print_tree(root->link, prev);
    }
}

void print_ls(TreeNode* pwd)
{
    if (pwd != NULL)
    {
        cout << pwd->type << pwd->get_perm() << " " << pwd->mdate << " " << pwd->name;
        if (pwd->type == 'd') cout << "/";
        cout << endl;

        print_ls(pwd->link);
    }
}

void print_stat(TreeNode* root, TreeNode* pwd, string path)
{
    TreeNode* node = find_node(root, pwd, path);
    cout << "File: " << node->name;
    if (node->type == 'd') cout << "/";
    cout << endl;
    cout << "Access: " << node->type << node->get_perm() << endl;
    cout << "Modified: " << node->mdate << endl;
    cout << "Created: " << node->cdate << endl;
}

string pwd_str(TreeNode* root, TreeNode* pwd)
{
    string res = "";
    if (pwd == root)
        return "/";
    while (pwd != root)
    {
        res = "/" + pwd->name + res;
        pwd = pwd->parent;
    }
    return res;
}

list<string> find_names(TreeNode* root, TreeNode* pwd, string name)
{
    static list<string> res;
    if (root == pwd)
        res.clear();
    if (pwd != NULL)
    {
        string path = pwd_str(root, pwd);
        if (path.find(name.c_str()) != string::npos)
        {
            res.push_back(path);
        }
        find_names(root, pwd->child, name);
        find_names(root, pwd->link, name);
    }
    return res;
}

TreeNode* find_node(TreeNode* root, TreeNode* pwd, string path)
{
    string parent_path = split_name(path)[0];
    string name = split_name(path)[1];
    pwd = cd(root, pwd, parent_path);
    if (pwd == NULL)
    {
       return NULL;
    }
    return find_on_pwd(pwd, name);
}

TreeNode* find_on_pwd(TreeNode* pwd, string name)
{
    pwd = pwd->child;
    while (pwd != NULL)
    {
        if (pwd->name == name)
        {
            return pwd;
        }
        pwd = pwd->link;
    }
    return NULL;
}

list<string> split(string str, char delim)
{
    list<string> res;
    string temp = "";
    for (int i = 0; i < (int)str.size(); i++)
    {
        if (str[i] != delim)
            temp += str[i];
        else
        {
            res.push_back(temp);
            temp = "";
        }
    }
    if (temp != "")
        res.push_back(temp);
    return res;
}

string join(list<string> str, char delim)
{
    string res = "";
    for (list<string>::iterator it = str.begin(); it != str.end(); ++it)
    {
        res += *it;
        if (it != --str.end())
            res += delim;
    }
    return res;
}

string* split_name(string str)
{
    string* res = new string[2];

    list<string> temp = split(str, '/');
    res[1] = temp.back();
    temp.pop_back();
    res[0] = join(temp, '/');

    return res;
}

TreeNode* cd(TreeNode* root, TreeNode* pwd, string path)
{
    list<string> path_list = split(path, '/');
    list<string>::iterator it = path_list.begin();
    TreeNode* temp;

    if (path[0] == '/')
    {
        temp = root;
        it++;
    }
    else if (path_list.front() == ".")
    {
        if (path_list.size() == 1)
            return pwd;
        else
        {
            temp = pwd;
            it++;
        }
    }
    else if (path_list.front() == "..")
    {
        if (path_list.size() == 1)
            return pwd->parent;
        else
        {
            temp = pwd->parent;
            it++;
        }
    }
    else
    {
        temp = pwd;
    }
    
    for (; it != path_list.end(); it++)
    {
        temp = find_on_pwd(temp, *it);
        if (temp == NULL || temp->type != 'd')
        {
            cout << "the directory '" << path << "' does not exist" << endl;
            return NULL;
        }
    }
    return temp;
}

TreeNode* create(TreeNode* root, TreeNode* pwd, string path, char type)
{
    string parent_path = split_name(path)[0];
    string name = split_name(path)[1];

    TreeNode* new_pwd = cd(root, pwd, parent_path);
    if (new_pwd == NULL)
    {
        cout << "the directory '" << parent_path << "' does not exist" << endl;
        return NULL;
    }

    TreeNode* temp = find_on_pwd(new_pwd, name);
    string choice = "";
    if (temp != NULL)
    {
        cout << "the file or directory '" << path << "' already exists" << endl;
        cout << "overwrite? (y/n) ";
        cin >> choice;
        if (choice != "y" && choice != "Y" && choice != "yes" && choice != "Yes")
            return NULL;
        remove(root, new_pwd, name);
    }

    temp = new TreeNode(new_pwd, name);
    temp->type = type;
    temp->parent = new_pwd;

    if (new_pwd->child == NULL)
    {
        new_pwd->child = temp;
        return temp;
    }
    TreeNode* prev = new_pwd->child;
    while (prev->link != NULL)
        prev = prev->link;
    prev->link = temp;

    return temp;
}

void remove(TreeNode* root, TreeNode* pwd, string path)
{
    TreeNode* temp = find_node(root, pwd, path);
    if (temp == NULL)
    {
        cout << "the file or directory '" << path << "' does not exist" << endl;
        return;
    }
    string choice = "";
    if (temp->type == 'd' && temp->child != NULL)
    {
        cout << "the directory '" << path << "' is not empty" << endl;
        cout << "proceed? (y/n) ";
        cin >> choice;
        if (choice != "y" && choice != "Y" && choice != "yes" && choice != "Yes")
            return;
    }

    if (temp->parent->child == temp)
    {
        temp->parent->child = temp->link;
        return;
    }
    TreeNode* prev = temp->parent->child;
    while (prev->link != temp)
        prev = prev->link;
    prev->link = temp->link;
}

void dupl(TreeNode* root, TreeNode* pwd, string src, string dst, int keep)
{
    TreeNode* src_node = find_node(root, pwd, src);
    if (src_node == NULL)
    {
        cout << "the file or directory '" << src << "' does not exist" << endl;
        return;
    }
    string src_cdate = src_node->cdate;
    int src_perm = src_node->perm;
    list<string> src_content = src_node->content;

    TreeNode* dst_node = find_node(root, pwd, dst);
    if (dst_node != NULL && dst_node->type == 'd')
    {
        dst += "/" + src_node->name;
    }

    TreeNode* new_node = create(root, pwd, dst, src_node->type);
    if (new_node != NULL)
    {
        new_node->cdate = src_cdate;
        new_node->perm = src_perm;
        new_node->content = src_content;
    }
    else
        return;

    if (keep == 0)
    {
        remove(root, pwd, src);
    }

    return;
}

void cat(TreeNode* root, TreeNode* pwd, string path)
{
    TreeNode* temp = find_node(root, pwd, path);
    if (temp == NULL)
    {
        cout << "the file '" << path << "' does not exist" << endl;
        return;
    }
    if (temp->type != '-')
    {
        cout << "'" << path << "' is not a file" << endl;
        return;
    }
    if (temp->perm < 4)
    {
        cout << "you don't have permission to read '" << path << "'" << endl;
        return;
    }
    for (list<string>::iterator it = temp->content.begin(); it != temp->content.end(); it++)
        cout << *it << endl;
}

void edit(TreeNode* root, TreeNode* pwd, string path)
{
    TreeNode* temp = find_node(root, pwd, path);
    if (temp == NULL)
    {
        cout << "the file '" << path << "' does not exist" << endl;
        string choice = "";
        cout << "create it? (y/n) ";
        cin >> choice;
        if (choice != "y" && choice != "Y" && choice != "yes" && choice != "Yes")
            return;
        temp = create(root, pwd, path, '-');
    }
    if (temp->type != '-')
    {
        cout << "'" << path << "' is not a file" << endl;
        return;
    }

    if (!temp->content.empty())
    {
        if (temp->perm < 4)
        {
            cout << "you don't have permission to read '" << path << "'" << endl;
        }
        else
        {
            list<string> old_content = temp->content;
            cout << "content: " << endl;
            for (list<string>::iterator it = old_content.begin(); it != old_content.end(); it++)
                cout << *it << endl;
        }

        if (temp->perm < 2 || temp->perm == 4 || temp->perm == 5)
        {
            cout << "you don't have permission to write '" << path << "'" << endl;
            return;
        }
        
        string choice = "";
        cout << endl << "overwrite? (y/n) ";
        cin >> choice;
        if (choice != "y" && choice != "Y" && choice != "yes" && choice != "Yes")
            return;
    }

    temp->content.clear();
    cout << endl << temp->name << " : (enter \\n to save)" << endl;
    string line;
    while (1)
    {
        getline(cin, line);
        if (line == "\\n")
            break;
        temp->content.push_back(line);
    }
}

void chmod(TreeNode* root, TreeNode* pwd, string path, string new_modes)
{
    TreeNode* temp = find_node(root, pwd, path);
    if (temp == NULL)
    {
        cout << "the file or directory '" << path << "' does not exist" << endl;
        return;
    }

    string perm_str = temp->get_perm();
    if (new_modes[0] == '+')
    {
        for (int i = 1; i < new_modes.size(); i++)
        {
            if (new_modes[i] == 'r' && perm_str.find('r') == string::npos)
                temp->perm += 4;
            else if (new_modes[i] == 'w' && perm_str.find('w') == string::npos)
                temp->perm += 2;
            else if (new_modes[i] == 'x' && perm_str.find('x') == string::npos)
                temp->perm += 1;
            else
                cout << "chmod: permission '" << new_modes[i] << "' already present or invalid\n";
        }
    }
    else if (new_modes[0] == '-')
    {
        for (int i = 1; i < new_modes.size(); i++)
        {
            if (new_modes[i] == 'r' && perm_str.find('r') != string::npos)
                temp->perm -= 4;
            else if (new_modes[i] == 'w' && perm_str.find('w') != string::npos)
                temp->perm -= 2;
            else if (new_modes[i] == 'x' && perm_str.find('x') != string::npos)
                temp->perm -= 1;
            else
                cout << "chmod: permission '" << new_modes[i] << "' already present or invalid\n";
        }
    }
    else if (new_modes.size() == 1 && new_modes[0] >= 48 && new_modes[0] <= 55)
    {
        temp->perm = new_modes[0]-48;
    }
    else
    {
        cout << "chmod: invalid permissions '" << new_modes << "'\n";
    }

}

string time_now()
{
    string res = "", temp;
    time_t now = time(0);
    tm *ltm = localtime(&now);

    res += to_string(ltm->tm_mday) + " ";

    int month = ltm->tm_mon + 1;
    if (month == 1) res += "Jan ";
    else if (month == 2) res += "Feb ";
    else if (month == 3) res += "Mar ";
    else if (month == 4) res += "Apr ";
    else if (month == 5) res += "May ";
    else if (month == 6) res += "Jun ";
    else if (month == 7) res += "Jul ";
    else if (month == 8) res += "Aug ";
    else if (month == 9) res += "Sep ";
    else if (month == 10) res += "Oct ";
    else if (month == 11) res += "Nov ";
    else if (month == 12) res += "Dec ";

    if (to_string(ltm->tm_hour).size() == 1) res += "0";
    res += to_string(ltm->tm_hour) + ":";
    if (to_string(ltm->tm_min).size() == 1) res += "0";
    res += to_string(ltm->tm_min);

    return res;
}

void linux_tree(TreeNode* root)
{
    vector<string> Dirs{"bin", "etc", "etc/systemd", "etc/systemd/system", "etc/systemd/user", 
        "home", "home/user", "home/user/Desktop", "home/user/Documents", "home/user/Downloads", 
        "home/user/Pictures", "tmp"};
    vector<string> Files{"bin/bash", "bin/cd", "bin/gcc", "bin/ls", "bin/python", 
        "etc/systemd/system/display-manager.service", "etc/systemd/system.conf", "etc/systemd/user.conf", 
        "etc/hosts", "etc/passwd", "etc/profile", "home/user/.bashrc"};

    for (string d : Dirs)
    {
        create(root, root, d, 'd');
    }
    for (string f : Files)
    {
        create(root, root, f, '-');
    }
}