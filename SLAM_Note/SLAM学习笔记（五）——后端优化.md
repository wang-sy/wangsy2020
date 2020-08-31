SLAM学习笔记（五）——后端优化

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



假设我们的运动方程和观测方程可以用线性方程来描述：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831112404112.png" alt="image-20200831112404112" style="zoom: 67%;" />

这里的$w_k, v_k$代表的是噪声，同时我们也假设这两个噪声服从高斯分布：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831112440905.png" alt="image-20200831112440905" style="zoom:67%;" />

首先我们通过运动方程来确定$x_k$的先验分布，后面的高斯是通过上面的线性方程得到的：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831114803713.png" alt="image-20200831114803713" style="zoom:67%;" />

这个式子显示了如何通过上一时刻的变换来计算下一时刻的状态分布。这个分布也就是先验。

#### 先验和后验（补充一下）

- 可以说先验就是不基于经验的
- 与之相对的是后验是基于经验的，后验需考虑当前的情况来对估计进行调整（贝叶斯）



那么我们这里的先验就记作：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831115701029.png" alt="image-20200831115701029" style="zoom:67%;" />

这里我们找到了每个时刻的先验的状态量与协方差与上一时刻的状态与协方差的关系，这个过程实质上就是直接将上面高斯的均值与方差拿出来了。

接下来，我们可以计算在某个状态下会产生怎么样的观测数据（这里是通过上面线性方程的第二个式子得来的）

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831115954342.png" alt="image-20200831115954342" style="zoom:67%;" />

这个时候我们回看一下上一小节中我们想要求解的公式：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831120246627.png" alt="image-20200831120246627" style="zoom: 80%;" />

我们发现，我们已经将两项的分布都求出来了，接下来我们对他们两个相乘就可以求出我们目标的概率，但是在此之前，我们需要假设我们乘出来的结果也是一个高斯分布，那么我们就得到了下面的结果：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831120013960.png" alt="image-20200831120013960" style="zoom:67%;" />

等式后面的第二项的均值与方差来自于先验，而第一项来自于我们刚才的推导，我们这里先来复习一下高斯（这里开始参考[这篇博客](https://www.cnblogs.com/bingjianing/p/9117330.html)）：
$$
f(x) = \frac{1}{\sqrt{2π}σ}e^{-\frac{(x-μ)^2}{2σ^{2}}}
$$
上面我们看到的是一个普通一元的高斯，其中$\mu$是均值，$σ$是方差。显然，我们的变量（状态变量）是非常多的变量构成的，也就是说这里的高斯不再是简单的一元高斯，而是多元高斯，那么多元高斯是怎么写的呢？我们假设所有的变量互不相关，就可以有下面的关系：
$$
f(x) = p(x_{1},x_{2}....x_{n}) = p(x_{1})p(x_{2})....p(x_{n}) = \frac{1}{(\sqrt{2π})^nσ_{1}σ_{2}\cdotsσ_{n}}e^{-\frac{(x_{1}-μ_{1})^2}{2σ_{1}^2}-\frac{(x_{2}-μ_{2})^2}{2σ_{2}^2}\cdots-\frac{(x_{n}-μ_{n})^2}{2σ_{n}^2}}
$$
假如我们有：$z^{2} = \frac{(x_{1}-μ_{1})^2}{σ_{1}^2}+\frac{(x_{2}-μ_{2})^2}{σ_{2}^2}\cdots+\frac{(x_{n}-μ_{n})^2}{σ_{n}^2}$，那么我们能够把原来的式子简写成：
$$
f(z) = \frac{1}{(\sqrt{2π})^nσ_{z}}e^{-\frac{z^2}{2}}
$$
我们可以将$z^2$展开为矩阵的形式：
$$
z^2 = z^\mathrm{T}z = \left[ \begin{matrix} x_{1} - μ_{1}, x_{2} - μ_{2}, \cdots,x_{n} - μ_{n}\end{matrix}\right] \left[ \begin{matrix} \frac{1}{σ_{1}^2}&0&\cdots&0\\ 0&\frac{1}{σ_{2}^2}&\cdots&0\\ \vdots&\cdots&\cdots&\vdots\\ 0&0&\cdots&\frac{1}{σ_{n}^2}  \end{matrix}\right]\left[ \begin{matrix} x_{1} - μ_{1}, x_{2} - μ_{2}, \cdots,x_{n} - μ_{n}\end{matrix}\right]^\mathrm{T}
$$
我们可以把左边的矩阵转换成两个矩阵差的形式，即：$x - μ_{x} = \left[ \begin{matrix} x_{1} - μ_{1}, x_{2} - μ_{2}, \cdots,x_{n} - μ_{n}\end{matrix}\right]^\mathrm{T}$

再把中间的协方差矩阵做一个符号的约定：
$$
∑_{}^{} = \left[ \begin{matrix} σ_{1}^2&0&\cdots&0\\ 0&σ_{2}^2&\cdots&0\\ \vdots&\cdots&\cdots&\vdots\\ 0&0&\cdots&σ_{n}^2  \end{matrix}\right]
$$
因为他是一个对角矩阵，所以他的逆就是全都取倒数就可以了：
$$
( (∑_{}^{})^{-1} = \left[ \begin{matrix} \frac{1}{σ_{1}^2}&0&\cdots&0\\ 0&\frac{1}{σ_{2}^2}&\cdots&0\\ \vdots&\cdots&\cdots&\vdots\\ 0&0&\cdots&\frac{1}{σ_{n}^2}  \end{matrix}\right]
$$
当然了，我们在分母项里面需要做一个累乘，所以我们还是做下面的约定：
$$
σ_{z}= \left|∑_{}^{}\right|^\frac{1}{2} =σ_{1}σ_{2}.....σ_{n}
$$
最后，我们把说的这些东西综合起来，$z^2$就可以写成下面的形式：
$$
z^\mathrm{T}z = (x - μ_{x})^\mathrm{T}  \sum_{}{}^{-1} (x - μ_{x})
$$
最后，我们来看部分展开的多元高斯：
$$
f(z) = \frac{1}{(\sqrt{2π})^nσ_{z}}e^{-\frac{z^2}{2}} = \frac{1}{(\sqrt{2π})^{n}\left|∑_{}^{}\right|^\frac{1}{2}}e^{-\frac{ (x\  -\  μ_{x})^\mathrm{T}\  (\sum_{}{})^{-1}\  (x\  -\  μ_{x})}{2}}
$$
由于这里的两侧都是高斯，所以我们就只考虑指数部分就可以了，我们就有：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831153731206.png" alt="image-20200831153731206" style="zoom:80%;" />

可以将两侧展开，分别比较x的二次和一次系数，对于二次系数，有：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831153828975.png" alt="image-20200831153828975" style="zoom: 67%;" />

这里的二次项就是展开后含有$x_k^2$的项，这里为了简化计算，我们定义一个中间变量：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831154546755.png" alt="image-20200831154546755" style="zoom:67%;" />

接下来，我们将上面式子的左边通过乘上他的逆的形式转换为一个单位矩阵，就可以得到下面的式子：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831154754733.png" alt="image-20200831154754733" style="zoom:67%;" />

这里转换的过程中，同时左乘了一个$P_k$帽，然后又使用刚才定义的$K$，简化了这个式子。然后通过简单变换，可以写出：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831154940187.png" alt="image-20200831154940187" style="zoom: 60%;" />

这里是二次项，接下来我们来比较一次项：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831155043748.png" alt="image-20200831155043748" style="zoom:67%;" />

经过一波化简，（先取转置，然后再消元）就可以得到下面的形式：

<img src="C:\Users\wangsy\AppData\Roaming\Typora\typora-user-images\image-20200831161548477.png" alt="image-20200831161548477" style="zoom:67%;" />

最后，两侧同时乘以$P_k$帽，并且带入上面的中间变量$K$与$P_k$帽的表达就可以得到下面的式子：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831161832674.png" alt="image-20200831161832674" style="zoom:67%;" />

于是呢，我们又找到了$x_k$帽与其他之间的关系，同时我们根据二次项也可以知道$P_k$帽与其他之间的关系，于是我们就可以根据之前的信息对当前的信息进行更新，这一步我们叫做**预测**：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831163451457.png" alt="image-20200831163451457" style="zoom:67%;" />

那么预测完成后，我们要计算非常重要的K：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831163549235.png" alt="image-20200831163549235" style="zoom: 50%;" />

最终，计算后验分布的概率：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831163614442.png" alt="image-20200831163614442" style="zoom: 50%;" />

这就是卡尔曼滤波的全过程，说实话我感觉我顺一遍还是有点小问题，难顶。

### 回到EKF后端

我们上面说的是KF，也就是标准的卡尔曼滤波，他只能运用到线性系统中，那么我们需要用EKF来做实际应用，接下来我们看具体是怎么做的：

首先，我们用泰勒做一个一阶展开：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831164541206.png" alt="image-20200831164541206" style="zoom:67%;" />

我们将这里的偏导数记作：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831164606971.png" alt="image-20200831164606971" style="zoom: 67%;" />

对于观测方程也是如此：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831164635364.png" alt="image-20200831164635364" style="zoom:67%;" />

一样的道理，我们用符号来表达一下：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831164653903.png" alt="image-20200831164653903" style="zoom:50%;" />

这里与上面的卡尔曼滤波的过程相似，最终得到的结果是：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831165036774.png" alt="image-20200831165036774" style="zoom:67%;" />



我们目前对卡尔曼滤波仅作简单掌握吧，我们可以看到，卡尔曼滤波存在一个较大的局限：我们展开的过程是做的一阶展开，实质上是在用线性函数来近似非线性函数，这样的做法有非常大的局限性，并且不够精准，接下来我们来看下一个方法——BA与图优化。



## BA与图优化

在看这个之前，我们需要复习一下我们的相机模型

- 首先从世界坐标到相机坐标：<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831172305492.png" alt="image-20200831172305492" style="zoom:50%;" />
- 从相机坐标到归一化坐标： <img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831172341318.png" alt="image-20200831172341318" style="zoom:50%;" />
- 进行畸变处理： <img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831172411110.png" alt="image-20200831172411110" style="zoom:50%;" />
- 根据内参模型，来计算图像坐标：<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831172433324.png" alt="image-20200831172433324" style="zoom:50%;" />

我们将上面四个步骤使用一个函数来概括：
$$
z = h(x,y)
$$
我们可以用流程图来概括$h(x,y)$的作用：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831173018908.png" alt="image-20200831173018908" style="zoom:67%;" />

这里的x指代当前相机的位姿，也就是外参$R,t$，它对应的李代数是$\xi$。路标y是这里的三维点，而观测数据是：$z^{\Delta} = \left[ u_s, v_s\right]^T$，我们可以构建残差：
$$
e = z - h(x,y)
$$
他的实质含义与我们以前经常说的一样：实际观测到的数据，与我们预测出的数据的差异。接下来我们将总体（也就是不同时刻）的残差一并考虑进来，即可构建代价函数：

<img src="https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/image-20200831173535197.png" alt="image-20200831173535197" style="zoom:67%;" />

我们对这个目标方程来做一个优化就是传说中的BA了。

