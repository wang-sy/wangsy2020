# SLAM学习笔记（五）——后端优化

先说明一下，书里面第八讲视觉里程计二，我写的文档丢失了，暂时先不去管他了，先继续往后看吧，之前是准备先做一个VO框架的，但是仔细思考了一下，准备先来实现一下这边，然后再去做那个VO框架。

我个人认为这里在我未来的项目中，不会是我做的重点，所以我在这块不做太过详细的学习。





## EKF后端

我们知道，实际上，仅有前端的SLAM就可以支持简单的定位、建图的工作了，但是前端的结果往往是粗糙的，后端的存在就是为了继续优化前端的结果。它的本质是一种状态估计问题，主流的后端可以分为以下两类：

- 渐进式： 保持当前状态估计， 加入新信息时，更新已有的估计
- 批量式： 给定一定规模的数据， 计算该数据下的最优估计



接下来回到我们看了无数遍的方程：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/PN7J%5DR0J3%25PDES%5DG_G%7D%28%5DB9.png" alt="image-20200830224611941" style="zoom:50%;" />

这里的x是机器人的位姿，z是观测数据，w与v是噪声，我们如果做一个不太严谨的定义：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/H%5B%7D%29%29%7DASVT1%7D%7EF9D%7EK_FJI7.png" alt="image-20200830225535458" style="zoom:67%;" />

这里的$x_k$代表k时刻所有的未知量，那么我们就可以重写之前的方程：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/QQ%E6%88%AA%E5%9B%BE20200830225717.png" style="zoom:50%;" />



那么这个时候，我们就可以用数学语言来表示我们的问题了：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/slam5/1.png" alt="1" style="zoom:67%;" />

我们要做的事情实质上是：已知初始时刻的状态，以及中间过程的观测数据与误差（这里的误差或者说是噪声是可以人为假设出来的，比如高斯）：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/slam5/2.png" style="zoom: 50%;" />

这里就是把原来的拆成了一个似然和一个先验的乘积，这个先验非常眼熟，上面的方程中$z_k$的计算方法蕴含了其计算形式，后面的先验可以继续展开：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/slam5/3.png" style="zoom: 50%;" />

这个可以拆成两部分： 

- k时刻是如何受到之前状态的影响的
- k-1时刻的状态估计



在这里时产生了一定的分歧：

- 可以假设k时刻的状态只和k-1时刻有关

- 假设k时刻与之前的所有状态有关

  



如果按照第一种方法来，那么第一项可以被写为：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/slam5/4.png" style="zoom: 50%;" />

第二项可以被写为：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/slam5/5.png" style="zoom:50%;" />



这里的第一项表示：已知上一时刻的状态时与当前误差时，产生当前情况的概率，也就是我们所说的当前状态只与上一时刻状态相关。

这里的第二项与之前的差别在于将k时刻的误差去除了，因为想要预估k-1时刻的状态，无需知道所有时刻的误差，只需要知道到k-1时刻即可。



在开始下一步之前，我们先来明确一个问题，啥是卡尔曼滤波？

### 卡尔曼滤波（KF）



### 回到EKF后端

我们需要回顾一下之前的方程，然后再明确一下：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/QQ%E6%88%AA%E5%9B%BE20200830225717.png" style="zoom:50%;" />

我们可以假设我们的误差都是服从正态分布的，那么我们就有如下关系：

![image-20200830235202249](C:\Users\wangsy\AppData\Roaming\Typora\typora-user-images\image-20200830235202249.png)

最后，我们求解的目标是：

![image-20200830235226686](C:\Users\wangsy\AppData\Roaming\Typora\typora-user-images\image-20200830235226686.png)

