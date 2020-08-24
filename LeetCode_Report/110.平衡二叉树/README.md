## LeetCode刷题记录——110. 平衡二叉树



这个系列是记录LeetCode刷题的，力扣的题比较简单，对我来说基本上当作练手的，保持状态用的了。



## 题面

[题目链接](https://leetcode-cn.com/problems/balanced-binary-tree/)


<p>本题中，一棵高度平衡二叉树定义为：</p>

<blockquote>
<p>一个二叉树<em>每个节点&nbsp;</em>的左右两个子树的高度差的绝对值不超过1。</p>
</blockquote>

<p><strong>示例 1:</strong></p>

<p>给定二叉树 <code>[3,9,20,null,null,15,7]</code></p>

<pre>    3
   / \
  9  20
    /  \
   15   7</pre>

<p>返回 <code>true</code> 。<br>
<br>
<strong>示例 2:</strong></p>

<p>给定二叉树 <code>[1,2,2,3,3,null,null,4,4]</code></p>

<pre>       1
      / \
     2   2
    / \
   3   3
  / \
 4   4
</pre>

<p>返回&nbsp;<code>false</code> 。</p>



## 解析

这也不用解析了，这都看不懂建议回炉重造了，就是说最深的和最浅的之间高度最大只能差1.

实质上就是对比根节点左子树和右子树的深度差，一颗子树的深度就等于左子树深度+右子树深度+1，即：
$$
dep(root) = (root == NULL) ?\ 0 :max(dep(tree[root].leftson) , dep(tree[root].rightson)) + 1;
$$
这样递归着去求就可以了，时间复杂度O(n)

## 代码

方法非常简单，实质上就是一个递归。

```C++
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
```



