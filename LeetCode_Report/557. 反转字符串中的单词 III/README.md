# LeetCode刷题记录——557.反转字符串中的单词III

给定一个字符串，你需要反转字符串中每个单词的字符顺序，同时仍保留空格和单词的初始顺序。

### 示例：

> 输入："Let's take LeetCode contest"
> 输出："s'teL ekat edoCteeL tsetnoc"
 

## 理解

nt题，不多说了，要是能用python的话，那就更nt了

## 代码

```C++
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
```