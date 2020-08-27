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