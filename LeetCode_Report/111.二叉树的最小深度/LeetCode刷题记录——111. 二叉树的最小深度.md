# LeetCode刷题记录——111. 二叉树的最小深度

## 题面



给定一个二叉树，找出其最小深度。

最小深度是从根节点到最近叶子节点的最短路径上的节点数量。

**说明:** 叶子节点是指没有子节点的节点。



## 分析

除了各种脑瘫方法外，比较普通的就两种：

- bfs
- dfs

他们两个各有优劣，bfs的好处在于可以遍历尽量少的点就把最终的结果直接找出（具体来说就是遍历完最浅的叶子后就会停掉）但是由于需要储存中间点，所以需要的空间较大，在极端情况下空间复杂度为O(n)，不过dfs在树极深的情况下空间复杂度也是O(n)，同时dfs还需要将所有节点遍历完才能算出最终结果，所以还是bfs要好一些。



## 代码

写个bfs就可以了，一直做遍历，直到有一个节点没有孩子就停下；

```C++
#include<iostream>
#include<queue>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
    int minDepth(TreeNode* root) {
        if(root == nullptr) return 0;

        // initial
        int res = 0;
        typedef pair<TreeNode*, int> bfsTreeNode;

        // 申请一个队列并且清空
        queue<bfsTreeNode> bfsQueue;
        while(!bfsQueue.empty()) bfsQueue.pop();

        // bfs_pre
        bfsQueue.push(make_pair(root, 0));
        // bfs
        while(!bfsQueue.empty()){
            bool haveSonFlag = false;
            // 出队
            bfsTreeNode curNode = bfsQueue.front();
            bfsQueue.pop();
            // 添加子节点
            if(curNode.first->left != nullptr){
                haveSonFlag = true;
                bfsQueue.push(make_pair(curNode.first->left,curNode.second + 1));
            }
            if(curNode.first->right != nullptr){
                haveSonFlag = true;
                bfsQueue.push(make_pair(curNode.first->right,curNode.second + 1));
            }
            // 如果没有儿子，那么就直接返回即可
            if(!haveSonFlag){
                return curNode.second + 1;
            }
        }
        // 如果在这里退出，那么说明这棵树只有个根
        return 0;
    }
};
```

