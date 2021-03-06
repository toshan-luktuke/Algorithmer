#include<iostream>
#include<fstream>
#include <bits/stdc++.h>
using namespace std;

void processLine(string, int, ofstream&);
bool checkIf(string);
bool checkLoop(string);
bool checkDataType(string);
string removeComments(string);

int main(){
    ifstream infile;
    ofstream outfile;
    infile.open("input.txt");
    outfile.open("output.txt");
    string line;
    int i = 1;
    while(getline(infile,line)){ //reads lines from a file
        //each line is one where a \n character is at the end
        //in resultant string, \n character does not appear
        line = removeComments(line);
        if(!line.empty() && line.find("*/") == string::npos){
            line = regex_replace(line, regex("^ +| +$|( ) +"), "$1"); //replace trailing, leading and extra spaces from the string
            processLine(line, i, outfile);
            i++;
            
        }

    }

    infile.close();
    outfile.close();
    return 0;
}

//To-Do
//Start blocks Partially DONE -> check after ifs and loops if they exist DONE
//Starts blocks after functions are yet to be implemented DONE
//Initialization DONE
//if statements DONE
//else if and else statements DONE
//for, while, do-while loops DONE

void processLine(string line, int step, ofstream &outfile){
    int l = line.length();
    cout << line << endl;
    
    if(line[0] == '#'){ //step for preprocessor declaration
        outfile << "Step " << step << ": Preprocessor declaration " << line.substr(line.find("<"),line.find(">")) << endl;
        return;
    }
    else if(line.find("using") == 0 && line.find("using")!=string::npos){
        //cout << step << endl;
        outfile << "Step " << step << ": Declaration of std namespace to be used" << endl;
        return;
    }
    else if( checkDataType(line) || (line.find("void") == 0) && line.find("(") != string::npos && line.find(")") != string::npos && line.find("=") == string::npos){
        //Functions
        if(line.rfind(";") == line.length()-1){//to ensure this is not variable declaration
            outfile << "Step " << step <<  ": Declare function prototype of return type " << line.substr(0,line.find(' ')) << " with name " << line.substr(line.find(' ')+1, (line.find('(') - line.find(' ') - 1));;  
        }else if(line.rfind("{") == line.length()-1){
            outfile << "Step " << step << ": Declare function " << line.substr(line.find(' ')+1, (line.find('(') - line.find(' ') - 1) ) << " with return type " << line.substr(0,line.find(' '));;
        }
        if(line.find('{') != string::npos){
            outfile << " and start of function block";
        }
        outfile << endl;
        return;
    }
    else if(line.find("cout")==0){
        outfile << "Step " << step << ": Output: ";
        int eindex = line.find("endl");
        for(int i = line.find('t')+1 ;i<line.length();i++){//starts from the end of the cout word
            if(line[i] == '<' || line[i]==';'){
                continue;
            } 
            if(i == eindex){
                outfile << " then end line and go to the next one";
                break;
            }
            outfile << line[i];
        }
        outfile << endl;
    }
    else if((!checkIf(line) && !checkLoop(line)) && (line.find("=")!=string::npos || checkDataType(line))){ //assignment statement
        //a = b;
        //int a,b,c;
        //int d = a + b + c;
        outfile << "Step " << step << ":";
        if(checkDataType(line) && line.find("=")!=string::npos){ //if data type and equal-to sign exists
            int equalto = line.find("=");
            outfile << " Assign " << line.substr(equalto+1, l-equalto-1) << " to " << line.substr(line.find(' ')+1, (equalto-line.find(' ')-1)) << " of data type " << line.substr(0, line.find(' ')) << endl;
        }
        else if(checkDataType(line) && line.find("=")==string::npos ){//only data type existsB
            outfile << " Initialize " <<  line.substr(line.find(' ')+1, l) << " of data type " << line.substr(0,line.find(' ')) << endl;  
        }else if(!checkDataType(line) && line.find("=")!=string::npos ){//only = in line
            int equalto = line.find("=");
            outfile <<" Assign the value of " << line.substr(equalto+1, line.find_last_of(';')-1) << " to " << line.substr(0,equalto-1) << endl;
        }
    }
    else if(line.find("return") == 0){ // Return statement
        outfile << "Step " << step << ": Return " << line.substr(line.find(' '), (l - line.find(' ') - 1)) << " to the user"<< endl;
    }
    else if(line.find("{") == 0){ // Starting bracket
        outfile << "Step " << step << ": Start block" << endl;
    }
    else if(line.find("}") == 0){ // Ending bracket
        outfile << "Step " << step <<": End block"<< endl;
    }
    else if(checkIf(line)){ //if statements
        outfile << "Step " << step << ":";
        if(line.find("if") != string::npos && line.find("else if") == string::npos){
            //check for brace at end
            outfile << " Check if the condition " << line.substr(line.find_first_of('(')+1, line.find_last_of(')') - line.find_first_of('(')-1 ) << " is satisfied";
        }
        else if(line.find("else if") != string::npos){
            outfile << " Otherwise check if the condition " << line.substr(line.find_first_of('(')+1, line.find_last_of(')') - line.find_first_of('(') ) << " is satisfied";
        }
        else if(line.find("else") != string::npos && line.find("else if") == string::npos){
            outfile << " At last execute ";
        }
        if(line.find("{") != string::npos){
            outfile << " and start block"; 
        }
        outfile << endl;
    }
    else if(checkLoop(line)){
        outfile << "Step " << step << ":";
        
        if(line.find("for") != string::npos){ 
            if(checkDataType(line)){
                //for( int a = 10; a <= 10; a+= 10){}
                int first_space_after_dtype = line.find(' ', (line.find('(')+2));
                outfile << " Initialise variable " << line.substr(first_space_after_dtype+1, line.find(';')-first_space_after_dtype-1) << " of data type " << line.substr(line.find('(')+1,first_space_after_dtype - line.find('('));
            }else{
                //for( a = 0; a <= 10; a+= 10){}
                outfile << " Initialise variable " << line.substr(line.find('(')+1, line.find(';')-line.find('(')-1);
            }

            //a <= 10
            outfile << " Until condition " << line.substr(line.find(';')+1, (line.find(';',line.find(';')+1) - line.find(';')-1)) << " is satisfied, ";
            outfile << "at the end of the loop execute " <<  line.substr(line.find_last_of(';')+1,line.find_last_of(')')-line.find_last_of(';')-1);
            
        }
        else if(line.find("while")){
            outfile << " While condition " << line.substr(line.find('(')+1, line.find_last_of(')')-line.find('(')-1);
        }
        else if(line.find("do")){
            outfile << " Start Do loop ";
        }


        if(line.find("{") != string::npos){
            outfile << " and start block"; 
        }
        outfile << endl;
    }
    else if(line.find("break") != string::npos){ //break statement
        outfile << "Step " << step << ": Break out of the loop " << endl;
    }
    else if(line.find("continue") != string::npos){//continue statement
        outfile << "Step " << step << ": Continue with the loop " << endl; 
    }
    else if(line.find("=") != string::npos && line.find('(') != string::npos && line.find(')') != string::npos && !checkIf(line) && !checkLoop(line) && !checkDataType(line)){
        //Object initialization
        //Student s = Student();
        outfile << "Step " << step << ":";
        outfile << " Initialize object of type " << line.substr(0, line.find(' ') - 0) << " with name " << line.substr(line.find(' ')+1, line.find('=')-line.find(' ')-1);
        outfile << endl;
    }
}

bool checkIf(string line){
    return (line.find("if") != string::npos || line.find("else if") != string::npos \
    || line.find("else") != string::npos); //if 'else if'/'if'//'else' exists in the string return true
}

bool checkLoop(string line){
    return (line.find("for") != string::npos || line.find("while") != string::npos \
    || line.find("do") != string::npos);
    //if 'for' exists in the string return true
}

bool checkDataType(string line){
    return (line.find("int") != string::npos || line.find("float") != string::npos \
    || line.find("double") != string::npos || line.find("long") != string::npos \
    || line.find("vector") != string::npos || line.find("string") != string::npos \
    || line.find("char") != string::npos);
}

string removeComments(string prgm)
{
    int n = prgm.length();
    string res;
 
    // Flags to indicate that single line and multiple line comments
    // have started or not.
    bool s_cmt = false;
    bool m_cmt = false;
 
 
    // Traverse the given program
    for (int i=0; i<n; i++)
    {
        // If single line comment flag is on, then check for end of it
        if (s_cmt == true && prgm[i] == '\n')
            s_cmt = false;
 
        // If multiple line comment is on, then check for end of it
        else if  (m_cmt == true && prgm[i] == '*' && prgm[i+1] == '/')
            m_cmt = false,  i++;
 
        // If this character is in a comment, ignore it
        else if (s_cmt || m_cmt)
            continue;
 
        // Check for beginning of comments and set the approproate flags
        else if (prgm[i] == '/' && prgm[i+1] == '/')
            s_cmt = true, i++;
        else if (prgm[i] == '/' && prgm[i+1] == '*')
            m_cmt = true,  i++;
 
        // If current character is a non-comment character, append it to res
        else  res += prgm[i];
    }
    return res;
}