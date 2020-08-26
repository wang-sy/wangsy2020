# LeetCode刷题记录——17. 电话号码的字母组合



## 题面

给定一个仅包含数字 2-9 的字符串，返回所有它能表示的字母组合。

给出数字到字母的映射如下（与电话按键相同）。注意 1 不对应任何字母。



### 示例:

> 输入："23"
> 输出：["ad", "ae", "af", "bd", "be", "bf", "cd", "ce", "cf"].

### 说明:
尽管上面的答案是按字典序排列的，但是你可以任意选择答案输出的顺序。





## 分析

这题也能被标记成中等？就很简单一个深度优先搜索，就硬搜就可以了。




## 代码

```C++

#include<iostream>
#include<vector>
#include<map>
#include<set>
using namespace std;

class Solution {
public:
    /**
     * dfs搜索所有结果
     * @param combinations 储存最终结果
     * @param phoneMap 相应按键与可能的对应关系
     * @param digits 用户的按键顺序
     * @param curPos 当前按下键的位置
     * @param combination 当前生成的串
     */
    void dfsCombinations(vector<string>& combinations, map<char, string>& phoneMap, const string& digits, int curPos, string combination){
        if(curPos >= digits.length()) {
            combinations.push_back(combination);
            return ;
        }
        for(char i : phoneMap[digits[curPos]]){
            combination.push_back(i);
            dfsCombinations(combinations, phoneMap, digits, curPos + 1, combination);
            combination.pop_back();
        }
        return;
    }


    /**
     * 给出一个字符串输出其相应的可生成的结果
     * @param digits 输入的键盘序列
     * @return 可能输出的所有结果
     */
    vector<string> letterCombinations(string digits) {
        map<char, string> phoneMap{
                {'2', "abc"},
                {'3', "def"},
                {'4', "ghi"},
                {'5', "jkl"},
                {'6', "mno"},
                {'7', "pqrs"},
                {'8', "tuv"},
                {'9', "wxyz"}
        };
        vector<string> res;

        if(digits.empty()) return res;
        string combination = "";
        dfsCombinations(res, phoneMap, digits, 0, combination);
        return res;
    }
};

int main(){
    string myStr = "233233";
    Solution asda;
    vector<string> res = asda.letterCombinations(myStr);
    for(string i : res){
        cout<<i<<endl;
    }
    return 0;
}

```

就是一个简单的不能再简单的深度优先搜索了，这里不做基础知识点的科普，如果不会的话建议自行百度。