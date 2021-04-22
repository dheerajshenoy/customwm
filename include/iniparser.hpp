#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>

using namespace std;

class iniparser {

    public:
        int temp;
        bool isFile = false, isModified = false;
        fstream f;
        string line, brackets, comment, stemp, ss;
        string delimiters;
        vector<string> all, sections, allkeys, vtemp;
        vector<int> sectionpos;
    
    // OPEN THE INI FILE. ALSO MENTION THE TYPE OF BRACKETS FOR SECTION
    void Open(string path, string bracket="[]", string delimiter="=", string comment="-") {
        if(bracket == "" || bracket == " ")
            return;
        f.open(path, ios::in);
        brackets = bracket;
        delimiters = delimiter;
        comment = comment;
        isFile = true;
        while(getline(f, line))
            all.push_back(line);
        for(int i=0; i<all.size(); i++) {
            if(all.at(i) != "" &&
                    all.at(i).find(brackets[0]) != string::npos &&
                    all.at(i).find(brackets[1]) != string::npos)
            {
                sections.push_back(all.at(i));
                sectionpos.push_back(i);
            }
            else if(all.at(i) != "")
                allkeys.push_back(all.at(i));
        }
    }

    // GET ALL THE SECTIONS
    vector<string> Sections() {
        if(!isFile)
            return {""};
        if(isModified) {
            filereload();
            while(getline(f, line)){
                if(line.find(brackets[0]) != string::npos && line.find(brackets[1]) != string::npos)
                    sections.push_back(line);
            }
        }
        return sections;
    }

    // GET ALL THE KEYS
    vector<string> Allkeys() {
        if(!isFile)
            return {""};
        if(isModified) {
            filereload();
            while(getline(f, line)){
                if(line.find(brackets[0]) == string::npos && line != "")
                    allkeys.push_back(line);        
            }
        }
        return allkeys;
    }

    // GET VALUE OF KEY FROM SECTION MENTIONED WITHOUT BRACKETS
    vector<string> getValue(string key, string section, string return_this_if_null={""}) {
        int temp = 0;
        if(!isFile)
            return {""};
        if(!isSection(section))
            return {""};
        for(int i=0; i<all.size(); i++) {
            if(all.at(i) != "") {
                if(all.at(i) == brackets[0] + section + brackets[1]) {
                    temp=1;
                    continue;
                }
                else if(temp) {
                    ss = all.at(i);
                    regex r("\\s+");
                    ss = regex_replace(ss, r, "");
                    vtemp = split(ss, delimiters);
                    if(vtemp[0] == key) {
                        vtemp.erase(vtemp.begin());
                        return vtemp;
                    }
                }
                else if(allkeys.at(i)[0] == brackets[0]) break;
            }
        }
        return {return_this_if_null};
}

    //
    bool setValue(string key, string section) {
        
        if(!isFile)
            return false;
        else {
            if(isSection(section) && isKey(key, section)) {
                //TODO: set value
                //updatemodified();
            }
        }
        return false;
    }

    int sectionNumber(string section) {
    
        if(!isFile)
            return 0;
        if(!isSection(section))
            return 0;
        for(int i=0; i<sections.size(); i++)
            if(sections.at(i) == brackets[0] + section + brackets[1])
                return i;
        return 0;
    }

    // CHECK IF THE GIVEN SECTION EXISTS
    bool isSection(string section) {
        if(!isFile)
            return false;
        if(find(sections.begin(), sections.end(), brackets[0] + section + brackets[1]) != sections.end())
            return true;

        return false;
    }

    // TRUE IF THE KEY EXISTS IN THE SECTION
    bool isKey(string key, string section)
    {
        int temp = 0;
        string ss;
        if(!isFile)
            return false;
        if(!isSection(section))
            return false;
        for(int i=0; i<all.size(); i++) {
            if(all.at(i) == brackets[0] + section + brackets[1])
            {
                temp = 1;
                continue;
            }
            else if(temp && all.at(i) != "")
            {
                ss = all.at(i);
                if(all.at(i)[0] == brackets[0])
                    break;
                regex r("\\s+");
                ss = regex_replace(ss, r, "");
                ss = split(ss, delimiters).at(0);
                if(ss == key)
                    return true;
            }
        }
        return false;
    }

    // GET ALL THE KEYS WITH VALUES UNTIL THE NEXT SECTION
    vector<vector<string>> getKeyValuePairTillNextSection(string section, string delim)
    {
        vector<string> vtemp1;
        vector<vector<string>> vtemp2;
        int posOfSection;
        if(!isFile)
            return {{"",""}};
        if(!isSection(section))
            return {{"",""}};
        string temp, ss;
        regex r("\\s");
        posOfSection = sectionpos.at(sectionNumber(section));
        for(int i=posOfSection+1; i<all.size(); i++)
        {
            if(all.at(i) == "")
                continue;
            if(all.at(i)[0] == brackets[0])
                break;
            ss = all.at(i);
            ss = regex_replace(ss, r, "");
            vtemp1 = split(ss, delim);
            vtemp2.push_back(vtemp1);
        }
        
        return vtemp2;
    }
// FUNCTION TO RETURN LINES FROM A SECTION TILL THE NEXT ONE
    vector<string> getLines(string section)
    {   
        vector<string> v;
        int pos = sectionpos.at(sectionNumber(section));
        for(int i=pos+1; i<all.size(); i++)
        {
            if(all.at(i) == "")
                continue;
            if(all.at(i)[0] == brackets[0])
                break;
            v.push_back(all.at(i));
        }
        return v;
    }
// FUNCTION TO CLOSE THE OPENED CONFIG FILE
    void Close() {
        if(!isFile)
            return;
        f.close();
    }




    protected:
    void filereload() {
        if(!isFile)
            return;
        f.clear();
        f.seekg(0, ios::beg);
    }

    vector<string> split(string STR, string DELIM) {

        vector<string> temp;

        int start = 0;
        int end = STR.find(DELIM);
        if(end != -1) {
            while(end != -1) {
                temp.push_back(STR.substr(start, end-start));
                start = end + DELIM.size();
                end = STR.find(DELIM, start);
            }
            temp.push_back(STR.substr(start, end-start));
            return temp;
        }
        return {};

    }
    
    void printvector(vector<string>v) {
        for(int i=0; i<v.size(); i++)
            cout << v.at(i);
    }

    void updatemodified() {
        isModified = !isModified;
        allkeys = Allkeys();
        sections = Sections();
        isModified = !isModified;
    }
};
