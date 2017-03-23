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
        printf("current path: %s \n", s_cwd.c_str());
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

int main(int argc, char **argv) {
    string path;
    struct stat info;
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
        int result;
        string full_path1;
        string full_path2;

        path = get_path();
        full_path1 = path + args.at(0);
        full_path2 = path + args.at(1);

        if( stat( full_path2.c_str(), &info ) != 0 ){
            printf( "cannot access %s\n", full_path2.c_str() );
        }
        else if( info.st_mode & S_IFDIR ) { // S_ISDIR() doesn't exist on my windows
            printf( "%s is a directory\n", full_path2.c_str() );
            full_path2 = path + args.at(1) + "/" + args.at(0);
        }
        else {
            printf( "%s is no directory\n", full_path2.c_str() );
            if(!confirm("Would you like to re-write file?")) {
                exit(EXIT_SUCCESS);
            }
        }
        result = rename( full_path1.c_str() , full_path2.c_str());
        printf("result %d", result);
    }
    /*else{
        string full_path1;
        string full_path2;

        path = get_path();
        printf("more then 2 args\n");




        for(unsigned long i = 0; i <args.size() -1; i++){
            full_path1 = path + args.at(i);
            full_path2 = path + args.at(args.size() - 1) +"/"+ args.at(i);

            printf("string1: %s\n", full_path1.c_str());
            printf("string2: %s\n", full_path2.c_str());
            in = fopen( full_path1.c_str(), "r" );
            out = fopen( full_path2.c_str(), "r" );

            if(in==NULL)
            {
                printf("cp: cannot stat '%s': No such file or directory\n",args.at(i).c_str());
                return EXIT_FAILURE;
            }

            if(out != NULL && !f_option){
                printf("cp: File with name '%s' already exist\n", (args.at(args.size() - 1)+"/"+args.at(i)).c_str());

                if(!confirm("Would you like to re-write file?")) {
                    continue;
                }

            }

            out = fopen( full_path2.c_str(), "w" );

            if(out == NULL){
                printf("cp: target '%s' is not a directory\n", args.at(args.size() - 1).c_str());
                return EXIT_FAILURE;
            }

            while((c=getc(in))!=EOF)
                putc(c,out);

            fclose(in);
            fclose(out);
        }
    }*/

    return EXIT_SUCCESS;
}