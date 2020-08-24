#include<cstdio>
#include<cstring>
#include<iostream>
#include<vector>
using namespace std;


 struct TreeNode {
     int val;
     TreeNode *left;
     TreeNode *right;
     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 };

class Solution {
public:
    // 给出树的根，判断树的叶子节点的深度差最大值是否超过1
    bool isBalanced(TreeNode* root) {
        if(root == nullptr) { // 如果树根就是空的的话那么会造成访问错误的情况，应当避免
            return true;
        }
        // 否则就检查当前的跟，并且进行下放
        return abs(get_deepth(root->right) - get_deepth(root->left)) <= 1 && isBalanced(root->left) && isBalanced(root->right);
    }
    int get_deepth(TreeNode* root){// 递归查找以root为树根的子树的深度
        return root == nullptr ? 0 : max(get_deepth(root->left), get_deepth(root->right)) + 1;
    }
};