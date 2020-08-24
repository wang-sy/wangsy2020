#include<cstdio>
#include<cstring>
#include<iostream>
#include<vector>
#include<queue>
using namespace std;

class Solution {
public:
    /**
     * 获取next数组的函数
     * @param pattern 用于生成next数组的模式串
     * @return 返回next数组
     */
    int* get_next(const string& pattern){
        int curPos = 0;
        int* next = new int[pattern.size()];
        memset(next, -1, sizeof(int) * pattern.size());

        // 这直接学习的Leet Code官方代码
        for (int i = 1; i < pattern.size(); i++) {
            int j = next[i - 1];
            while (j != -1 && pattern[j + 1] != pattern[i]) {
                j = next[j];
            }
            if (pattern[j + 1] == pattern[i]) {
                next[i] = j + 1;
            }
        }
        return next;
    }


    /**
     * kmp匹配过程
     * @param query 查询的串
     * @param pattern 模式串
     * @param next next数组
     * @return 返回是否匹配成功
     */
    bool kmpMatch(const string& query, const string& pattern, const int* next){
        int match = -1;
        for (char i : query) {
            while (match != -1 && pattern[match + 1] != i) {
                match = next[match];
            }
            if (pattern[match + 1] == i) {
                ++match;
                if (match == pattern.size() - 1) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * 字符串匹配函数
     * @param query 搜索串
     * @param pattern 模式串
     * @return 返回一个布尔值，表示是否相同
     */
    bool strCMP(const string& query, const string& pattern){
        int* next = get_next(pattern);
        bool res = kmpMatch(query, pattern, next);
        return res;
    }

    bool repeatedSubstringPattern(string s) {
        if(s.size() == 1 || s.size() == 0) return false;
        return strCMP(s.substr(1, s.size() - 1) + s.substr(0, s.size() - 1), s);
    }
};