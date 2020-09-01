#include<iostream>
#include<vector>
#include<map>
#include<cstdio>
#include<cmath>
#include<cstring>
#include<set>
#include<algorithm>
#include<queue>
using namespace std;

class Solution {
public:

    /**
     * 记忆化搜索获取当前left -> right 区间为起始时，先手玩家领先的数值
     * @param left 起点
     * @param right 重点
     * @param dp 记忆化搜索存储空间
     * @param vis 标记点，代表是否被成功搜索过
     * @param nums 游戏序列
     * @return 返回一个数字代表当前情况下的先手玩家领先的数值
     */
    int getDpNum(int left, int right, vector<vector<int>>& dp, vector<vector<bool>>& vis, vector<int>& nums){

        cout << left << " " << right << endl;

        if(left == right) return nums[left];

        // 如果曾经搜索过，那么就直接返回
        if(vis[left][right]) return dp[left][right];

        // 没有搜索过就从两种情况里面取最优
        // 取最优
        dp[left][right] = max(
                nums[left] - getDpNum(left + 1, right, dp, vis, nums),
                nums[right] - getDpNum(left, right - 1, dp, vis, nums)
                );

        vis[left][right] = 1;

        return dp[left][right];

    }

    /**
     * 传入一组游戏的初始值，返回先手玩家是否胜利
     * @param nums 游戏的初始值
     * @return 一个布尔值，代表先手玩家是否胜利
     */
    bool PredictTheWinner(vector<int>& nums) {
        // 声明数组，并且进行初始化
        vector<vector<int>> dp(nums.size());
        vector<vector<bool>> vis(nums.size());

        for(int i = 0; i < nums.size(); i++){
            dp[i].resize(nums.size());
            vis[i].resize(nums.size());
        }

        // 获取领先分数，并且判断能否胜利
        return getDpNum(0, nums.size() - 1, dp, vis, nums) >= 0;
    }
};

int main() {
    vector<int> a = {1,5,2};
    Solution solution;
    bool res = solution.PredictTheWinner(a);
    cout << res << endl;
    return 0;
}