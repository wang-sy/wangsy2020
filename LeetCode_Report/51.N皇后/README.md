# LeetCode刷题记录——51.N皇后


## 题面

n 皇后问题研究的是如何将 n 个皇后放置在 n×n 的棋盘上，并且使皇后彼此之间不能相互攻击。

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/8-queens.png" alt="img" style="zoom: 150%;" />

上图为 8 皇后问题的一种解法。

给定一个整数 n，返回所有不同的 n 皇后问题的解决方案。

每一种解法包含一个明确的 n 皇后问题的棋子放置方案，该方案中 'Q' 和 '.' 分别代表了皇后和空位。

 

### 示例：

> 输入：4
> 输出：[
>  [".Q..",  // 解法 1
>   "...Q",
>   "Q...",
>   "..Q."],
> 
>  ["..Q.",  // 解法 2
>   "Q...",
>   "...Q",
>   ".Q.."]
> ]

解释: 4 皇后问题存在两个不同的解法。


### 提示：

皇后彼此不能相互攻击，也就是说：任何两个皇后都不能处于同一条横行、纵行或斜线上。


## 分析



简单的dfs，不多谈，我们只讲一些带算法的，或者是难想的。算法里面也不讲那些最短路之类的模板算法的。我会把代码写好，写的大家都能看得懂，这样就可了。



## 代码

```C++
#include<iostream>
#include<vector>
#include<map>
#include<cstdio>
#include<cmath>
#include<cstring>
#include<set>
#include<unordered_map>
#include<algorithm>
#include<queue>
using namespace std;


class Solution {
public:


    /**
     * 给出前面的皇后序列，以及期望放到的位置，判定是否能在本位置放置皇后
     * @param queenList 前面的皇后序列
     * @param expectQueenPos 期望的皇后放置位置
     * @return 返回一个bool，如果ture则说明可以放置
     */
    static bool checkPos(vector<int>& queenList, int expectQueenPos){
        int curCol = queenList.size();

        for(int i = 0; i < curCol; ++i){
            if(queenList[i] == expectQueenPos || abs(expectQueenPos - queenList[i]) == (curCol - i)){
                return false;
            }
        }
        return true;
    }


    /**
     * 将已经生成好的结果放入res中
     * @param queenList 前面放置的皇后的位置表单，用于生成
     * @param res 最终的结果，根据queenList进行操作添加
     */
    static void generateAns(vector<int>& queenList, vector<vector<string>>& res){
        vector<string> pushBackAns;
        // 遍历、添加
        for(auto i : queenList){
            string pushBackString(queenList.size(), '.');
            pushBackString[i] = 'Q';
            pushBackAns.push_back(pushBackString);
        }
        res.push_back(pushBackAns);
    }


    /**
     * 使用深度优先搜索的方法来搜索n皇后问题
     * @param goalQueenNum 最重要摆放的皇后的数量
     * @param queenList 前面放置的皇后的位置表单，用于判断
     * @param res 最终的结果，再搜搜到一个正确的皇后序列后会进行存储
     */
    static void dfsFindQueens(const int goalQueenNum, vector<int>& queenList, vector<vector<string>>& res){

        // 如果数量到达了的话，那么就直接生成答案
        if(queenList.size() == goalQueenNum){
            generateAns(queenList, res);
        }

        // 遍历这一行可以放置的皇后序列 并且进行尝试
        for(int i = 0; i < goalQueenNum; ++i){
            // 如果不能放，那就跳过
            if(!checkPos(queenList, i)) continue;
            queenList.push_back(i);
            dfsFindQueens(goalQueenNum, queenList, res);
            queenList.pop_back();
        }
    }


    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> res;
        vector<int> queenList;
        dfsFindQueens(n, queenList, res);
        return res;
    }
};

int main() {
    int nQueens = 4;
    Solution solution;
    vector<vector<string>> res = solution.solveNQueens(nQueens);
    for(const auto& i : res){
        for(const auto& j : i){
            cout << j << endl;
        }
        cout << "++++++++++++++++++++++++++" << endl;
    }
    return 0;
}
```
