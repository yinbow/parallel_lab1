#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<arm_neon.h>
#include<cmath>
#include<map>
#include<sys/time.h>
using namespace std;

void readData(map<unsigned int, vector<unsigned int> >& data1, vector< vector<unsigned int> >& data2, vector< vector<unsigned int> >& data3, vector<unsigned int>& head2, int& len, string test);
void split(const string& s, vector<string>& res, const char delim);
unsigned int getFirstSetBitPos(unsigned int n);
#define HIGH (unsigned int)(1 << 31)

int main(){
    map<unsigned int, vector<unsigned int> > data1;
    vector< vector<unsigned int> > data2, data3;
    vector<unsigned int> head2;
    vector<bool> flags; // 是否是需要处理的被消元行
    int len;
    readData(data1, data2, data3, head2, len, "7_8399_6375_4535");
    flags.resize(data2.size(), true);
    bool update = true;

    struct timeval t1,t2;
    double timeuse;
    gettimeofday(&t1,NULL);

    while(update){
        update = false;
        for(int i = 0; i < data2.size(); i++){
            if(!flags[i]){
                continue;
            }
            while(flags[i] && data1.find(head2[i]) != data1.end()){
                for(int k=0; k < head2[i] / 32 + 1; k++){
                    data2[i][k] = data2[i][k] ^ data1[head2[i]][k];
                }
                //新的首项
                for(int k=head2[i] / 32; k >=0; k--){
                    if (data2[i][k] > 0){
                        head2[i] = k * 32 + (32 - getFirstSetBitPos(data2[i][k]));
                        break;
                    }
                    if(k == 0 & data2[i][k] == 0){
                        //空行
                        flags[i] = false;
                    }
                }
            }
            
            //是否是新的消元子
            if(flags[i] && (data1.find(head2[i]) == data1.end())){
                data1.insert(pair<unsigned int, vector<unsigned int> >(head2[i], data2[i]));
                flags[i] = false;
                update=true;
            }
        }
    }

    gettimeofday(&t2,NULL);
    timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000000.0;

    // for(int i = 0; i < data2.size(); i++){
    //     for(int j = 0; j < data2[i].size(); j++){
    //         cout << data2[i][j] << ' ';
    //     }
    //     cout << endl;
    // }
    // cout << endl;
    // for(int i = 0; i < data3.size(); i++){
    //     for(int j = 0; j < data3[i].size(); j++){
    //         cout << data3[i][j] << ' ';
    //     }
    //     cout << endl;
    // }
    cout << "Time: " << timeuse << endl;
    return 0;
}

void readData(
    map<unsigned int, vector<unsigned int> >& data1, 
    vector< vector<unsigned int> >& data2, 
    vector< vector<unsigned int> >& data3, 
    vector<unsigned int>& head2,
    int& len, string test){

    fstream in_a, in_b, in_c;

    in_a.open(test + "/1.txt");
    in_b.open(test + "/2.txt");
    in_c.open(test + "/3.txt");

    string data;
    len = 0;
    while(getline(in_a, data)){
        vector<string> tokens;
        vector<unsigned int> itokens;
        int cur = 0, head=0;
        split(data, tokens, ' ');
        for(int i=0; i<tokens.size(); i++){
            if(tokens[i].size() == 1 && (tokens[i][0] < '0' || tokens[i][0] > '9')){
               continue;
           }
            cur = atoi(tokens[i].c_str());
            if(i == 0){
                for(int j=0; j < cur / 32 + 1; j++){
                    itokens.push_back(0);
                }
                len = len > cur ? len : cur;
                head = cur;
            }
            itokens[cur / 32] = itokens[cur / 32] | (HIGH >> cur % 32);
        }
        data1[head] = itokens;
    }

    int  n = 0;
    while(getline(in_b, data)){
        vector<string> tokens;
        vector<unsigned int> itokens;
        int cur = 0;
        split(data, tokens, ' ');
        for(int i=0; i<tokens.size(); i++){
            if(tokens[i].size() == 1 && (tokens[i][0] < '0' || tokens[i][0] > '9')){
                continue;
            }
            cur = atoi(tokens[i].c_str());
            if(i == 0){
                for(int j=0; j < cur / 32 + 1; j++){
                    itokens.push_back(0);
                }
                len = len > cur ? len : cur;
            }
            if(i == 0){
                head2.push_back(cur);
            }
            itokens[cur / 32] = itokens[cur / 32] | (HIGH >> cur % 32);
        }
        data2.push_back(itokens);
                n += 1;
    }

    while(getline(in_c, data)){
        vector<string> tokens;
        vector<unsigned int> itokens;
        int cur = 0;
        split(data, tokens, ' ');
        if(data.size() == 1 && (data[0] < '0' || data[0] > '9')){
            itokens.push_back(0);
            data3.push_back(itokens);
            continue;        
        }
        for(int i=0; i<tokens.size(); i++){
            if(tokens[i].size() == 1 && (tokens[i][0] < '0' || tokens[i][0] > '9')){
                continue;
            }
            cur = atoi(tokens[i].c_str());
            if(i == 0){
                for(int j=0; j < cur / 32 + 1; j++){
                    itokens.push_back(0);
                }
            }
            itokens[cur / 32] = itokens[cur / 32] | (HIGH >> cur % 32);
        }
        data3.push_back(itokens);
    }

    map<unsigned int, vector<unsigned int> >::iterator iter = data1.begin();
    while(iter != data1.end()){
        iter->second.resize(len / 32 + 1 , 0);
        iter++;
    }
    for(int i = 0; i < data2.size(); i++){
        data2[i].resize(len / 32 + 1, 0);
    }
    for(int i = 0; i < data3.size(); i++){
        data3[i].resize(len / 32 + 1, 0);
    }
    
    in_a.close();
    in_b.close();
    in_c.close();
}

// 切分字符串
void split(const string& s, vector<string>& res, const char delim = ' ') {
    istringstream data(s);
    string temp;

    while (std::getline(data, temp, delim)) {
        if (!temp.empty()) {
            res.push_back(temp);
        }
    }

    return;
}

unsigned int getFirstSetBitPos(unsigned int n){
    return log2(n & -n) + 1;
}