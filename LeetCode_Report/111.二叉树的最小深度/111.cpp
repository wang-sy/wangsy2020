#include<cstdio>
#include<cstring>
#include<iostream>
#include<vector>
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