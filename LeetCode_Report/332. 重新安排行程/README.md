# LeetCode刷题记录——332.重新安排行程+


## 题面

给定一个机票的字符串二维数组 [from, to]，子数组中的两个成员分别表示飞机出发和降落的机场地点，对该行程进行重新规划排序。所有这些机票都属于一个从 JFK（肯尼迪国际机场）出发的先生，所以该行程必须从 JFK 开始。

### 说明:

如果存在多种有效的行程，你可以按字符自然排序返回最小的行程组合。例如，行程 ["JFK", "LGA"] 与 ["JFK", "LGB"] 相比就更小，排序更靠前
所有的机场都用三个大写字母表示（机场代码）。
假定所有机票至少存在一种合理的行程。

#### 示例 1:

> 输入: [["MUC", "LHR"], ["JFK", "MUC"], ["SFO", "SJC"], ["LHR", "SFO"]]
> 输出: ["JFK", "MUC", "LHR", "SFO", "SJC"]

#### 示例 2:

> 输入: [["JFK","SFO"],["JFK","ATL"],["SFO","ATL"],["ATL","JFK"],["ATL","SFO"]]
> 输出: ["JFK","ATL","JFK","SFO","ATL","SFO"]

解释: 另一种有效的行程是 ["JFK","SFO","ATL","JFK","ATL","SFO"]。但是它自然排序更大更靠后。



## 解析

相信大家都能看得出来，这次是一个对票进行排序的问题，这个问题有几个约束：
- 1、 必须从JFK机场出发
- 2、 票的字典序在所有解决方案中必须最小
- 3、 必须用到给出的所有边（机票）

那就非常简单啦，我们直接dfs，dfs的顺序就是字典序，如果能出答案，那么就直接出答案就可以了。dfs太基础了，我们就不做讲解了，作为一个科班出身的hxd，如果不会dfs那就有点给学校丢人了。


## 代码

```C++
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<algorithm>
using namespace std;

class Solution {
public:

    /**
     * 用于对机票初始化排序时的比较函数
     * @param a 参数1
     * @param b 参数2
     * @return 返回两个数字的大小关系
     */
    static bool ticketsCmp(const vector<string>& a, const vector<string>& b){
        return a[1] < b[1];
    }

    /**
     * 递归搜索最终序列的函数
     * @param tickets 所有机票的集合
     * @param resItinerary 返回的答案
     * @param ticketIsUsed 表示机票是否被占用
     * @param curCity 当前正处在哪个城市
     * @param okAndShutDownAll 如果是true的话那么终止一切操作，直接返回
     */
    void getEulersRoadLeastLexicographicalOrder(
            vector<vector<string>>& tickets, vector<string>& resItinerary, vector<bool>& ticketIsUsed,
            const string& curCity, bool& okAndShutDownAll
            ){

        // 如果票的序列足够长，那么就说明已经找到了最优结果，那么就可以停止了
        if(resItinerary.size() == tickets.size() + 1) okAndShutDownAll = true;

        // 如果是true，那么就终止，直接返回
        if(okAndShutDownAll) return;

        for(int i = 0; i <tickets.size(); i ++){

            // 如果机票已经被占用了，或者当前的城市并不是当前所在的城市，那么就直接跳过
            if(ticketIsUsed[i] || tickets[i][0] != curCity){
                continue;
            }
            // 否则就进行尝试
            ticketIsUsed[i] = true;
            resItinerary.push_back(tickets[i][1]); // 将下一个城市压入
            getEulersRoadLeastLexicographicalOrder(tickets, resItinerary, ticketIsUsed, tickets[i][1], okAndShutDownAll);
            if(okAndShutDownAll) return;
            ticketIsUsed[i] = false;
            resItinerary.pop_back(); // 进行反向操作
        }
    }


    /**
     * 给出一个机票序列，对其行程进行重新规划
     * @param tickets 给出的机票序列
     * @return 返回一个数组，为访问行程
     */
    vector<string> findItinerary(vector<vector<string>>& tickets) {
        sort(tickets.begin(), tickets.end(), ticketsCmp);
        auto* resItinerary = new vector<string>;
        resItinerary->clear();
        auto* ticketIsUsed = new vector<bool>;
        ticketIsUsed->clear();

        string curCity = "JFK";
        resItinerary->push_back(curCity);
        ticketIsUsed->resize(tickets.size() + 1);

        bool isOK = false;

        getEulersRoadLeastLexicographicalOrder(
                tickets, *resItinerary, *ticketIsUsed, curCity, isOK
        );

        delete ticketIsUsed;

        return *resItinerary;
    }
};
int main(){
    vector<vector<string>> myStr{
            {"MUC","LHR"},
            {"JFK","MUC"},
            {"SFO","SJC"},
            {"LHR","SFO"}
    } ;
    Solution solution;
    vector<string> res = solution.findItinerary(myStr);

    return 0;
}
```