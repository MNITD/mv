#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>

using namespace std;

/*
*  Function Declaration
*/

void print_help();
void parse_argv(int size, char ** argv, vector<string>*args, vector<string>* opts);
string get_path();
bool confirm(string message);
int get_file_status(string & path);

/*
 * Constants
 */

const int OPTIONS_SIZE = 3;
const string OPTIONS[OPTIONS_SIZE] = {
        "-f",
        "-h",
        "--help"
};

void parse_argv(int size, char ** argv, vector<string>*args, vector<string>*opts){
    int i;
    int j;

    for(i = 1; i < size; i++){
        string arg(argv[i]);
        for(j = 0; j < OPTIONS_SIZE; j++){
            if(arg.compare(OPTIONS[j]) == 0){
                opts->push_back(arg);
                break;
            }
        }
        if(j == OPTIONS_SIZE){
            args->push_back(arg);
        }
    }
}

void print_help(){
    printf("This is a command for moving and renaming files \n");
    printf("To move some file write: mv <filename> <newfilename> \n");
    printf("To move several files to some directory write: сp <filename1> <filename2> <directory> \n");
    printf("To rename file move it to the same directory");
    printf("Additional options: \n");
    printf("-f                silently re-write file with existing name\n");
}

string get_path(){
    const  int SIZE = 1024;
    char buffer[SIZE];
    char *path = getcwd(buffer, sizeof(buffer));
    string s_cwd;


    if (path) {
        s_cwd = path;
        s_cwd += "/";
        return s_cwd;
    } else {
        printf("mv: cannot determine current directory\n");
        exit(EXIT_FAILURE);
    }
}

bool confirm(string message){
    printf("%s[y/n]:\n", message.c_str());
    string answer;
    cin >> answer;
    return answer.compare("y") == 0;
}

/* Check status of file: whether it dir, not dir or does not exist
 * @param path Full path of the file
 * @return code of status 1 - directory, 0 - file, -1 - does not exist
 */

int get_file_status(string & path){
    struct stat info;

    if(stat( path.c_str(), &info ) != 0) {
        return -1;
    } else if(info.st_mode & S_IFDIR){
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    string path;
    vector<string> args;
    vector<string> opts;
    bool f_option = false;

    parse_argv(argc, argv, &args, &opts);

    for(unsigned long i = 0; i < opts.size(); i++){
        printf("%s \n",opts.at(i).c_str() );

        if(opts.at(i).compare("--help") == 0 || opts.at(i).compare("-h") == 0){
            print_help();
            exit(EXIT_SUCCESS);
        }
        if(opts.at(i).compare("-f") == 0){
            f_option = true;
        }
    }
    if (args.size() < 2){
        printf("missing file operand\n");
        printf("Try 'mv --help' for more information.\n");
        exit(EXIT_FAILURE);
    }
    else if(args.size() == 2){
        int status;
        string full_path1;
        string full_path2;

        path = get_path();
        full_path1 = path + args.at(0);
        full_path2 = path + args.at(1);


        if( get_file_status(full_path1) == -1){
            printf("mv: cannot stat '%s': No such file or directory\n",args.at(0).c_str());
            return EXIT_FAILURE;
        }


        status = get_file_status(full_path2);
        if ( status == 1 ) {
            full_path2 = path + args.at(1) + "/" + args.at(0);
        } else if(status == 0) {
            printf("mv: File with name '%s' already exist\n", args.at(1).c_str());
            if( !f_option && !confirm("Would you like to re-write file?") ){  // file is not dir and silent mode switch off
                exit(EXIT_SUCCESS);
            }
        }

        rename( full_path1.c_str() , full_path2.c_str());
    }
    else{
        string full_path1;
        string full_path2;

        path = get_path();

        full_path2 = path + args.at(args.size() - 1);

        if(get_file_status(full_path2) != 1) {
            printf("mv: cannot stat '%s': No such directory\n",args.at(args.size() - 1).c_str());
            return EXIT_FAILURE;
        }


        for(unsigned long i = 0; i < args.size() - 1; i++){
            full_path1 = path + args.at(i);
            full_path2 = path + args.at(args.size() - 1) +"/"+ args.at(i);

            if( get_file_status(full_path1) == -1){
                printf("mv: cannot stat '%s': No such file or directory\n",args.at(i).c_str());
                continue;
            }

            if(get_file_status(full_path2) != -1){
                printf("mv: File with name '%s' already exist\n", args.at(i).c_str());
                if( !f_option && !confirm("Would you like to re-write file?") ) {  // file exists and silent mode switch off
                    continue;
                }
            }

            rename( full_path1.c_str() , full_path2.c_str());
        }
    }

    return EXIT_SUCCESS;
}