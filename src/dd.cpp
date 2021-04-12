#include <iostream>
#include <vector>

using namespace std;
vector<string>args;
int main(int argc, char* argv[])
{
    for(int i=1; i<argc; i++)
        args.push_back(argv[i]);

    for(int i=0; i<argc-1; i++)
        cout << args.at(i) << endl;
    return 0;
}
