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