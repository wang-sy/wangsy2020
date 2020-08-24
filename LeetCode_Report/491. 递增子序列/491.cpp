#include<iostream>
#include<vector>
#include<map>
#include<set>
using namespace std;

class Solution {
public:

    /**
     * 通过原始数组和选择数字获取相应串的函数
     * @param nums 原始数组
     * @param curSel 选择数字
     * @return 生成的子串
     */
    vector<int>* generateSubSequence(const vector<int>& nums, const int& curSel){
        int copyCurSel = curSel;
        int curPos = 0;
        auto* resSubSequence = new vector<int>();
        while(copyCurSel){
            if(copyCurSel & 1){
                resSubSequence->push_back(nums[curPos]);
            }
            curPos ++;
            copyCurSel = (copyCurSel >> 1);
        }
        return resSubSequence;
    }

    /**
     * 判断子串是否为升序串
     * @param nums 需要判断的串
     * @return 返回一个bool类型变量，表示是否为升序串
     */
    bool isSubSequenceIncrement(const vector<int>* nums){
        if(nums->size() < 2) {
            return false;
        }
        for(int i = 1; i < nums->size(); i ++){
            if((*nums)[i] < (*nums)[i - 1]) {
                return false;
            }
        }
        return true;
    }

    vector<vector<int>> findSubsequences(vector<int>& nums) {
        int maxRange = (1 << nums.size());
        auto* resSet = new set<vector<int>>;
        for(int i = 1; i < maxRange; i ++){
            vector<int>* curStr = generateSubSequence(nums, i);
            // 如果是子串的话
            if(isSubSequenceIncrement(curStr)){
                // 如果成功就加入之
                resSet->insert(*curStr);
            }
            else {
                // 否则就销毁这段空间
                delete curStr;
            }
        }
        auto* resVec = new vector<vector<int>>;
        resVec->assign(resSet->begin(), resSet->end());
        return *resVec;
    }
};

int main(){
    vector<int> asd;
    asd.push_back(4);
    asd.push_back(6);
    asd.push_back(7);
    asd.push_back(7);
    Solution asda;
    vector<vector<int>> res = asda.findSubsequences(asd);
    return 0;
}