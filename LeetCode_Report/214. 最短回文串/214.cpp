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

    /**
     * 马拉车算法的初始化程序，解决基偶串的问题
     * 返回的串一定是一个偶串，同时以^为开头，以$结尾，以#作为间隔插入字符
     * @param manacherString 需要进行规格化的字符串
     * @return 返回一个符合马拉车规格化的字符串
     */
    static inline string manacherInit(const string& manacherString){
        string resStr = "^";

        for(char i : manacherString){
            resStr.push_back(i);
            resStr.push_back('#');
        }

        resStr.push_back('$');

        return resStr;
    }

    /**
     * 给出一个规格化后的马拉车字符串，返回每个字符位置上的最长回文半径
     * @param manacherString 规格化后的马拉车字符串
     * @return 返回一个vector<int>类型的数组，代表每个节点的最长回文半径， 但是需要注意的是这个数组的下标代表规格化后的数组，后续需要用户自行处理
     */
    static inline vector<int>* getManacherR(const string& manacherString){
        int maxRight = 0, mid = 0;
        auto* resManacherR = new vector<int>;
        resManacherR->resize(manacherString.length());

        for(int i = 1; i < manacherString.length() - 1; i ++){
            int mirrorI = (mid << 1) - i;

            // 初步确定当前点的回文半径
            if(i < maxRight){ // 如果当前在中心点半径内的话
                (*resManacherR)[i] = min((*resManacherR)[mirrorI], maxRight - i);
            }
            else {
                (*resManacherR)[i] = 0;
            }

            // 对当前点的半径进行检验扩张
            while(manacherString[i - (*resManacherR)[i]] == manacherString[i + (*resManacherR)[i]]) ++(*resManacherR)[i];

            // 检查是否需要更新最右端节点位置
            if(i + (*resManacherR)[i] > maxRight){
                maxRight = i + (*resManacherR)[i];
                mid = i;
            }
        }

        return resManacherR;
    }

    string shortestPalindrome(string s) {
        string manacherStr = manacherInit(s);
        auto manacherR = getManacherR(manacherStr);
        auto strLen = s.size();
        for(int i = 0; i < manacherStr.size(); i++){
            cout << manacherStr[i] << ":" << (*manacherR)[i] << endl;
        }

        int maxEndPos = 0;

        // 获取了马拉车的数组后，需要找到左端点为最左边的，最长的回文子串
        for(int i = 0; i < strLen; i++){
            // 这里考虑的是奇数串
            int curPos = i * 2 + 1; // 原串中的地方在马拉车串中对应的位置
            int thisPosGroundTruthR = ((*manacherR)[curPos] -  1) / 2; // 真实的半径
            if(i - thisPosGroundTruthR == 0) {// 如果串的起点是开头的话那么就进行比较
                maxEndPos = max(maxEndPos, i + thisPosGroundTruthR);
            }

            // 这里考虑的是偶数串
            curPos = (i + 1) * 2;
            int manacherRightPos = curPos + ((*manacherR)[curPos] -  1);
            int manacherLeftPos = curPos - ((*manacherR)[curPos] -  1);
            if(manacherLeftPos == manacherRightPos) continue;
            if(manacherStr[manacherRightPos] == '#') manacherRightPos --;
            if(manacherStr[manacherLeftPos]  == '#') manacherLeftPos  ++;
            int groundTruthLeftPos = (manacherLeftPos - 1) / 2;
            int groundTruthRightPos = (manacherRightPos - 1) / 2;
            if(groundTruthLeftPos== 0) {// 如果串的起点是开头的话那么就进行比较
                maxEndPos = max(maxEndPos, groundTruthRightPos);
            }
        }
        // 找到最长回文后，将后面的反转后加入构成最终的回文
        string resPalindromeStr;
        for (int i = strLen - 1; i > maxEndPos; --i) {
            resPalindromeStr.push_back(s[i]);
        }
        resPalindromeStr.append(s);

        return resPalindromeStr;
    }

};

int main(){
    string a = "abbacd";
    Solution solution;
    string res = solution.shortestPalindrome(a);
    cout << res << endl;
    return 0;
}