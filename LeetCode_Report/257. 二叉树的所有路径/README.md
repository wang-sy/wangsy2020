# LeetCode刷题记录——332.重新安排行程+


## 题面

给定一个二叉树，返回所有从根节点到叶子节点的路径。

说明: 叶子节点是指没有子节点的节点。

### 示例:

> 输入:
> 
>    1
>  /   \
> 2     3
>  \
>   5
> 
> 输出: ["1->2->5", "1->3"]
> 
> 解释: 所有根节点到叶子节点的路径为: 1->2->5, 1->3


## 解析

没啥好讲的，直接遍历就可以了。


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



struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:

    static void dfsGetBinaryTreePaths(TreeNode* root, vector<string>& res, string& curString){
        if(root->right == nullptr && root->left == nullptr){
            res.push_back(curString);
            return;
        }
        if(root->left != nullptr){
            curString += "->";
            curString += to_string(root->left->val);
            dfsGetBinaryTreePaths(root->left, res, curString);
            curString.erase(curString.length() - to_string(root->left->val).length() - 2);
        }
        if(root->right != nullptr){
            curString += "->";
            curString += to_string(root->right->val);
            dfsGetBinaryTreePaths(root->right, res, curString);
            curString.erase(curString.length() - to_string(root->right->val).length() - 2);
        }
    }

    vector<string> binaryTreePaths(TreeNode* root) {
        string curString = to_string(root->val);
        vector<string> res;
        dfsGetBinaryTreePaths(root, res, curString);
        return res;
    }
};
```
