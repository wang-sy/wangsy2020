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

    void spfa(const vector<vector<int>>& edges, int _st, vector<int>& distances){

        queue<int> q;
        vector<int> vis(edges.size(), 0);
        distances.resize(edges.size());
        distances.insert(distances.begin(), distances.size(), 0x7fffffff);
        distances[_st] = 0;

        q.push(_st);

        while(!q.empty()){
            int curPoint = q.front();
            q.pop();
            // 遍历每一条边
            for(int nexPoint : edges[curPoint]){
                // 如果更优就更新
                if(distances[nexPoint] > distances[curPoint] + 1){
                    distances[nexPoint] = distances[curPoint] + 1;
                    if(!vis[nexPoint]){
                        vis[nexPoint] = 1;
                        q.push(nexPoint);
                    }
                }
            }
        }
    }

    bool canVisitAllRooms(vector<vector<int>>& rooms) {

        vector<int> distances;

        spfa(rooms, 0, distances);
        return !any_of(distances.begin(), distances.end(), [](int i ){return i == 0x7fffffff;});
    }
};

int main() {
    vector<vector<int>> a = {
            {1, 3},
            {3, 0, 1},
            {2},
            {0}
    };
    Solution solution;
    bool res = solution.canVisitAllRooms(a);
    cout << res << endl;
    return 0;
}