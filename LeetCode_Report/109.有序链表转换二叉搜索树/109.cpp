#include<cstdio>
#include<cstring>
#include<iostream>
#include<vector>
using namespace std;


struct ListNode {
     int val;
     ListNode *next;
     ListNode() : val(0), next(nullptr) {}
     ListNode(int x) : val(x), next(nullptr) {}
     ListNode(int x, ListNode *next) : val(x), next(next) {}
 };

 struct TreeNode {
      int val;
      TreeNode *left;
      TreeNode *right;
      TreeNode() : val(0), left(nullptr), right(nullptr) {}
      TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
      TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
  };
 

class Solution {
public:
    TreeNode* sortedListToBST(ListNode* head) {
        // 定义一个可变长数组来储存他
        auto* listArray = new vector<int>;
        listArray->clear(); // 用之前先清空

        //将列表内的内容压入这个vector中
        while(head != nullptr) {
            listArray->push_back(head->val);
            head = head->next;
        }
        return sortedVectorToBST(*listArray, 0, (int)listArray->size() - 1);
    }

    TreeNode* sortedVectorToBST(const vector<int>& givenArray, int begin, int end){
        if(end < begin) return nullptr; // 不存在就返回空
        auto* resRoot = new TreeNode;
        int mid = (begin + end) / 2;
        // 为根节点赋值
        resRoot->val  = (givenArray)[mid];
        resRoot->left = sortedVectorToBST(givenArray, begin, mid - 1);
        resRoot->right= sortedVectorToBST(givenArray, mid + 1, end);

        return resRoot;
    }
};