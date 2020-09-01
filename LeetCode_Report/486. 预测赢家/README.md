# LeetCode刷题记录——486. 预测赢家


给定一个表示分数的非负整数数组。 玩家 1 从数组任意一端拿取一个分数，随后玩家 2 继续从剩余数组任意一端拿取分数，然后玩家 1 拿，…… 。每次一个玩家只能拿取一个分数，分数被拿取之后不再可取。直到没有剩余分数可取时游戏结束。最终获得分数总和最多的玩家获胜。

给定一个表示分数的数组，预测玩家1是否会成为赢家。你可以假设每个玩家的玩法都会使他的分数最大化。

 

### 示例 1：

> 输入：[1, 5, 2]
> 输出：False

#### 解释：
> 一开始，玩家1可以从1和2中进行选择。
> 如果他选择 2（或者 1 ），那么玩家 2 可以从 1（或者 2 ）和 5 中进行选择。如果玩家 2 选择了 5 ，那么玩家 1 则只剩下 1（或者 2 ）可选。
> 所以，玩家 1 的最终分数为 1 + 2 = 3，而玩家 2 为 5 。
> 因此，玩家 1 永远不会成为赢家，返回 False 。


### 示例 2：

> 输入：[1, 5, 233, 7]
> 输出：True

#### 解释：

> 玩家 1 一开始选择 1 。然后玩家 2 必须从 5 和 7 中进行选择。无论玩家 2 选择了哪个，玩家 1 都可以选择 233 。
> 最终，玩家 1（234 分）比玩家 2（12 分）获得更多的分数，所以返回 True，表示玩家 1 可以成为赢家。


### 提示：

- 1 <= 给定的数组长度 <= 20.
- 数组里所有分数都为非负数且不会大于 10000000 。
- 如果最终两个玩家的分数相等，那么玩家 1 仍为赢家。

## 理解

这里我们需要发挥一定的想象力，我们想，在某个状态下，一个人拥有两种选择：

- 从左边拿一个
- 从右边拿一个

我们也知道，两个人都是在做“最正确的决定”，这说明，所以说一个人的选择一定是最有利于自己的，换句话说就是这个人一定会在从左选和从右选这两个之间选择最优的那个决定。

上面的那句话非常的抽象，接下来我们会用更加具体，或者说更加“数学”的语言去做表述，假设我们使用$f[i][j]$来表示当游戏的初始状态是$[i,j]$这个区间时，先手的人会领先多少分，如果$f[i][j] > 0$那么说明在这种情况下先出手的人会赢，反之$f[i][j] < 0$则说明当游戏的初始情况是$[i,j]$这个区间的时候，先出手的人会输。接下来，先出手的人一定会将场上的局势继续按照有利于他的情况发展，我们用公式表示出来也就是：
$$
f[i][j] = max(nums[i] - f[i+1][j], nums[j] - f[i][j-1])
$$
如果你看这个式子感觉很头大的话，可以接着听我说，我们可以从括号里开始看，$f[i+1][j]和f[i][j-1]$代表着当前玩家的两种决策，如果拿$i$的话，$f[i][j]$这个区间的情况下，我领先的分数会是$nums[i] - f[i+1][j]$，另外一边也是同理，我们相当于在两种情况中选择了一个最优情况。那么这样的话，我们就找到了一种可行的方法，我们在搜索的过程中将结果记录下来，这样可以大大减少重复搜索的时间，这种技巧叫做记忆化搜索。



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
```