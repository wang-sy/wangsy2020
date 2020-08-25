# LeetCode刷题记录——109. 有序链表转换二叉搜索树



## 题面

给定一个单链表，其中的元素按升序排序，将其转换为高度平衡的二叉搜索树。

本题中，一个高度平衡二叉树是指一个二叉树每个节点 的左右两个子树的高度差的绝对值不超过 1。



## 分析

先来看到概念，什么是二叉搜索树？就是对于每一个节点，都满足以下性质：

- 左子树上所有点的值都小于自己
- 右子树上所有点的值都大于自己

接下来，我们将这个问题分为三个阶段：

- 谁来当根？
- 谁来当左儿子？
- 谁来当右儿子？

首先我们知道，我们要建一颗平衡的二叉搜索树，所以根据我们的直觉，拿区间中点来当根节点肯定没问题，接下来中点左边的就是左子树，右边的就是右子树，这个就非常简单了。最后，谁是左儿子，谁是右儿子呢？那就更简单了：左子树的根就是左儿子，右子树的根就是右儿子。





## 代码

```C++
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
```

一个简单的递归，不多谈了