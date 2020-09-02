# LeetCode刷题记录——剑指 Offer 20. 表示数值的字符串

请实现一个函数用来判断字符串是否表示数值（包括整数和小数）。例如，字符串"+100"、"5e2"、"-123"、"3.1416"、"-1E-16"、"0123"都表示数值，但"12e"、"1a3.14"、"1.2.3"、"+-5"及"12e+5.4"都不是。

## 理解

总体来说，就是逐层拆分，先拆分e，然后再拆分小数点，最后判断是否是纯数字串

## 代码

```C++
class Solution {
public:

    /**
     * 给定一个串和相应区域判断该区域内的字符是否仅由数字构成
     * @param s 给出的串
     * @param begin 搜索的起始点，闭区间
     * @param end 搜索的中点，闭区间
     * @param canBeNull 是否允许这个串是空的
     * @return 返回一个布尔值，表示是否全部由数字构成
     */
    static bool isNumberOnlyString(const string& s, int begin, int end, bool canBeNull){

        // 如果这个串是空的的话，那么就返会是否允许为空就可以了
        if(begin > end) return canBeNull;

        for(int i = begin; i <= end; ++i){
            if(s[i] >= '0' && s[i] <= '9'){
                continue;
            }
            return false;
        }
        return true;
    }


    /**
     * 判断一个字符串段是否是符合要求的数字
     * @param s 用于判断的字符串
     * @param begin 判断起点
     * @param end 判断终点
     * @param canBeFloat 是否允许该部分是浮点数
     * @return 返回一个布尔值，如果正确，那么返回true
     */
    bool judgeNumber(const string& s, int begin, int end, bool canBeFloat){
        // 如果串是空的，那么就返回允许为空的参数
        if(begin > end) return false;

        // 规则： 先找正负号，如果有正负号那么进行舍弃，然后再找小数点进行判断与下放

        // 寻找正负号的位置
        bool haveOperator = s[begin] == '+' || s[begin] == '-';
        int pointPos = s[begin] == '.' ? begin : -1;
        for(int i = begin + 1; i <= end; ++i){
            if(s[i] == '.'){// 记录位置，如果前面已经出现过那么就直接返回错误
                if(pointPos != -1){
                    return  false;
                }
                pointPos = i;
            }
            // 如果出现这两个，那么就直接返回false，因为该符号只能出现在开头
            if(s[i] == '+' || s[i] == '-'){
                return false;
            }
        }



        // 现在我们找到了是否有符号位，并且找到了小数点的位置（当然也有可能根本没有小数点）
        if(pointPos == -1){ // 如果有符号位那么就直接搜索当前剩下的位置就可以了
            return isNumberOnlyString(s, begin + (haveOperator ? 1 : 0), end, false);
        }// 否则就返回两个区间的and
        // 如果有小数点的话，那么左边不能为空

        return canBeFloat && (begin + (haveOperator ? 1 : 0) <= pointPos - 1 || pointPos + 1 <= end) &&
                isNumberOnlyString(s, begin + (haveOperator ? 1 : 0), pointPos - 1, true) &&
                isNumberOnlyString(s, pointPos + 1, end, true);
    }

    bool isNumber(string s) {
        // 规则： 先找e如果有两个以上的e，那么就直接返回false，否则就以e为起点进行分割，进入下一个例程

        s.erase(0,s.find_first_not_of(' '));
        s.erase(s.find_last_not_of(' ') + 1);

        // 先看看有没有不符合规范的字符出现
        for(char i : s){
            if((i > '9' || i < '0') && i != '+'&& i != '-' && i != 'e' && i != '.' && i != 'E'){
                return false;
            }
        }


        // 寻找e的位置
        int ePos = -1;
        for(int i = 0; i < s.length(); ++ i){
            if(s[i] == 'e' || s[i] == 'E'){
                if(ePos >= 0) return false;
                ePos = i;
            }
        }

        // 如果没有e
        if(ePos == -1){
            return judgeNumber(s, 0, s.length() - 1, true);
        }
        // 如果有e
        return judgeNumber(s, 0, ePos - 1, true) && judgeNumber(s, ePos + 1, s.length() - 1, false);
    }
};
```