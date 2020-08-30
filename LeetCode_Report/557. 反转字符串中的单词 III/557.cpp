#include<iostream>
#include<vector>
#include<map>
#include<cstdio>
#include<cmath>
#include<cstring>
#include<set>
#include<algorithm>
using namespace std;

class Solution {
public:
    void subStringReverse(string& s, int beg, int end){
        for(int left = beg, right = end; left < right; left ++, right --){
            swap(s[left], s[right]);
        }
    }

    string reverseWords(string s) {
        int beg = 0, end = 0;
        for(int i = 0; i < s.length(); i++){
            if(s[i] == ' '){
                subStringReverse(s, beg, i - 1);
                beg = i + 1;
            }
        }
        subStringReverse(s, beg, s.length() - 1);
        return s;
    }
};

int main(){
    string a = "abbacd";
    Solution solution;
    string res = solution.reverseWords(a);
    cout << res << endl;
    return 0;
}