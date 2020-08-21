# SLAM学习笔记（一）——李群与李代数

## 前言

如果大家看过我之前写的博客的话应该也知道，到目前为止我的博客主要涉及到：

- 组成原理和最优化为代表的课内知识
- PCV学习的课外知识

到目前为止已经有一段时间没有更新过博客了，这是因为我在参加NSCSCC比赛，这个比赛是一个制造CPU的比赛，由于某些原因，我们小组的作品并没有通过初赛，所以我的学习重点再次回归到了CV方面，之前在做的是一些比较泛化的学习。但是最近发生了几件事情，导致我对未来的规划有了一些变动，在未来的一到两年中，我学习的主要方向将改变为：

- SLAM方面知识的学习
- C++后台找工作方面的学习

如果有带佬能提供寒暑假实习机会的话那就太nice了。

这个系列主要是阅读《SLAM十四讲》，以书上的知识为基础，进行进一步深挖的系列，预计在十一假期结束前完成，与这个系列同步的是立体匹配论文阅读系列。



## 李群与李代数基础

因为我们是为了学SLAM才来学的李群李代数，所以我们就只用关心较小的一部分就可以了。

首先我们来复习一下旋转、平移矩阵的概念，对于二维的点，我们可以通过一个3*3的矩阵，对其进行仿射变换。其实质就是对空间中点的**平移、旋转、缩放**，仿射变换的关系如下：
$$
\left[\begin{matrix}
x'\\y' \\1\end{matrix}\right]
 = \left[\begin{matrix}
 a_1 & a_2 &t_x \\
 a_3 & a_4 & t_y \\
 0 & 0 & 1
 \end{matrix}\right]
 \left[\begin{matrix}
x\\y \\1\end{matrix}\right]
$$
其中，a开头的表示旋转、缩放，t开头的表示平移，我们可以将这个变换矩阵简写为：
$$
\left[\begin{matrix}
R & t\\
0 & 1
 \end{matrix}\right]
$$
其中的**R被称为旋转矩阵**，**t被称为平移矩阵**，他们共同构成了**变换矩阵**。

其中又包含非常多的变换方法，来达到不同的效果，如果有兴趣了解可以看下面的图：

![](https://images2015.cnblogs.com/blog/120296/201602/120296-20160222070732869-1123994329.png)

那么这和我们讲的李群李代数有什么关系呢？如果我们将旋转平移拓展为三维空间的旋转平移，呢么就可以说三维旋转矩阵构成了**特殊正交群SO(3)**，而变换矩阵构成了**特殊欧氏群SE(3)**。

当然，通过性质就可以知道，这两种群内是不存在加法运算的，因为旋转矩阵的和没有意义。同时他们只符合乘法运算，两个变换矩阵的乘积代表两次变换的累加。同时，可以对旋转矩阵或是变换矩阵求逆，代表一次复原，如下图所示：

![image-20200815122209856](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815122209856.png)

同时，特殊的是：**对于这种只有一个良好的运算的集合，我们称之为群**



### 群

大家如果学过离散数学的话（应该在大一学的吧），就会知道，群就是一种**集合**加上与该集合相关的**运算**的代数结构。群有一些性质：

- 封闭性：两个群中的集合运算后还在群里面
- 结合律：和乘法的结合律一样
- 幺元：和整数乘法运算群里面的1一样，1与任何数字b相乘都是b
- 逆：我们上面也提到过这个概念，相当于整数乘法中的倒数



### 李代数的引出

![IMG_20200815_210622](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/IMG_20200815_210622.jpg)



观察最后一排的式子，我们发现，实质上我们得到了一旋转矩阵在任意时刻的导数与其实际值之间的关系：**每对旋转矩阵求一次导数，只需左乘一个 $\phi(t)^{\bigwedge}$就可以了**。同时，我们假设在$t_0 = 0 $的时候，$R(t_0) = I$，我们使用泰勒展开在$t= 0 $附近进行一阶泰勒展开：
$$
R(t) \approx R(t_0) + R'(t_0) (t - t _0)\\
 = I +\phi(t)^{\bigwedge} * t
$$
在$t_0$附近，设$\phi(t)$保持不变，恒为$\phi(t_0)$，那么就有：
$$
R'(t) =\phi(t_0)^{\bigwedge}R(t)
$$
上面这个式子可以看作一个微分方程的形式，但是我现在快大三了，已经不会解这个方程了，所以就直接把结果搬过来用就行了，得到了这样的关系：
$$
R(t) = exp(\phi(t_0)^{\bigwedge}t)
$$
这个式子说明，对于任意t，都可以找到R和$\phi$的关系，这种关系被称为指数映射。

### 李代数

#### so(3)

上面说的$\phi$就是李代数了，对于李群SO(3)来说，我们将其李代数记为so(3)，他们用大小写进行区别。李代数和李群之间的区别在于，李群只定义了乘法运算，不便于研究，而李代数位于一个**正切空间**中，他更好研究，我们可以将其与向量空间进行类比即可，有这些性质（来自[b站](https://www.bilibili.com/video/BV1Ai4y1V7r4?p=11)）

![image-20200815214256027](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815214256027.png)

我们定义了一种运算方法，名字叫做李括号，其具体运算方式如下：
$$
[\phi_1, \phi_2] = (\phi_1^{\bigwedge}\phi_2^{\bigwedge} - \phi_2^{\bigwedge}\phi_1^{\bigwedge})^{\bigvee}
$$

#### se(3)

同理，我们可以对李群SE(3)做相应的定义，李群se(3)：

![image-20200815214814783](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815214814783.png)

需要注意的是，这里的反对称符号${\bigwedge}$与前面的符号含义不再相同，而是作为一种记号。

最后，李括号的定义：
$$
[\xi_1, \xi_2] = (\xi_1^{\bigwedge}\xi_2^{\bigwedge} - \xi_2^{\bigwedge}\xi_1^{\bigwedge})^{\bigvee}
$$

## 指数与对数映射

在上一节中，我们得到了这样一组关系：
$$
R(t) = exp(\phi(t_0)^{\bigwedge}t)
$$
随之而出的是一个问题，那就是矩阵的指数运算应当如何操作呢？我们可以从泰勒展开来看起：
$$
e^x = 1 + \frac{x}{1!}+\frac{x^2}{2!} + ...
$$
这样我们就可以把他写成一个级数的形式。同时，由于$\phi$是一个向量，所以我们可以将其方向与长度进行分离，写作：
$$
\phi = \theta *a
$$
其中，a是一个长度为1的方向向量，$\theta$则是长度。通过验证，我们可以得到以下关系：
$$
a^{\bigwedge}a^{\bigwedge}=aa^T-I\\
a^{\bigwedge}a^{\bigwedge}a^{\bigwedge}=-a^{\bigwedge}
$$
这样， 我们就可以对上面的泰勒展开进行化简，化简过程如下，直接来自书中：

![image-20200815221748746](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815221748746.png)

结果就在最后一行了。

同时知道了正向的关系后，我们也可以用对数的方法来反过来求：

![image-20200815222034652](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815222034652.png)

但是，实质上是没有必要的：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815222052930.png" alt="image-20200815222052930" style="zoom:50%;" />

同时，用这套方法，我们也可以得到SE(3)和SO(3)之间的关系了。

![image-20200815222227601](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815222227601.png)

其中，J是雅可比矩阵

![image-20200815222253053](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815222253053.png)

最后，我们知道了SO(3), so(3)以及SE(3),se(3)之间的关系

![image-20200815222328046](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815222328046.png)

### 李代数求导与扰动模型

首先来思考一个问题，为什么要有李群和李代数？我们发现所谓的李群实际上是没有加法运算的，也就是说它是没有办法进行求导的。那么我们做出来这么一套定义后，最关心的问题就是，李群的乘法能否转换成李代数的加法

![image-20200815222823396](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815222823396.png)

一个叫做BCH的公式给了我们答案：

![image-20200815223400902](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815223400902.png)

当处理两个矩阵指数之积的时候，会产生一些余项，正如上面所示的一样，考虑SO(3)上的李代数，有以下关系：

![image-20200815223539877](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815223539877.png)

这里的J又是雅可比矩阵

![](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815223608231.png)

它们的关系是：

![image-20200815223629140](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815223629140.png)

同理，对于SE(3)也可以定义出一下关系：

![image-20200815223835150](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815223835150.png)

### 扰动模型

这个也是为了对R进行求导的，它的原理是对R进行一次扰动，看结果相对于扰动的变化率，公式如下：

![image-20200815224324597](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815224324597.png)

同理，在SE （3）上面，也可以使用扰动模型：

![image-20200815224657069](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/image-20200815224657069.png)



## 代码

这里直接copy高博的代码，然后我们来看一下里面的一些语法就可以了，这里我尽量直接阅读官方文档，让大家尽量只看二手信息，而不是十八手信息😂，先把高博的代码copy过来：

```C++
#include <iostream>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "sophus/se3.hpp"

using namespace std;
using namespace Eigen;

/// 本程序演示sophus的基本用法

int main(int argc, char **argv) {

  // 沿Z轴转90度的旋转矩阵
  Matrix3d R = AngleAxisd(M_PI / 2, Vector3d(0, 0, 1)).toRotationMatrix();
  // 或者四元数
  Quaterniond q(R);
  Sophus::SO3d SO3_R(R);              // Sophus::SO3d可以直接从旋转矩阵构造
  Sophus::SO3d SO3_q(q);              // 也可以通过四元数构造
  // 二者是等价的
  cout << "SO(3) from matrix:\n" << SO3_R.matrix() << endl;
  cout << "SO(3) from quaternion:\n" << SO3_q.matrix() << endl;
  cout << "they are equal" << endl;

  // 使用对数映射获得它的李代数
  Vector3d so3 = SO3_R.log();
  cout << "so3 = " << so3.transpose() << endl;
  // hat 为向量到反对称矩阵
  cout << "so3 hat=\n" << Sophus::SO3d::hat(so3) << endl;
  // 相对的，vee为反对称到向量
  cout << "so3 hat vee= " << Sophus::SO3d::vee(Sophus::SO3d::hat(so3)).transpose() << endl;

  // 增量扰动模型的更新
  Vector3d update_so3(1e-4, 0, 0); //假设更新量为这么多
  Sophus::SO3d SO3_updated = Sophus::SO3d::exp(update_so3) * SO3_R;
  cout << "SO3 updated = \n" << SO3_updated.matrix() << endl;

  cout << "*******************************" << endl;
  // 对SE(3)操作大同小异
  Vector3d t(1, 0, 0);           // 沿X轴平移1
  Sophus::SE3d SE3_Rt(R, t);           // 从R,t构造SE(3)
  Sophus::SE3d SE3_qt(q, t);            // 从q,t构造SE(3)
  cout << "SE3 from R,t= \n" << SE3_Rt.matrix() << endl;
  cout << "SE3 from q,t= \n" << SE3_qt.matrix() << endl;
  // 李代数se(3) 是一个六维向量，方便起见先typedef一下
  typedef Eigen::Matrix<double, 6, 1> Vector6d;
  Vector6d se3 = SE3_Rt.log();
  cout << "se3 = " << se3.transpose() << endl;
  // 观察输出，会发现在Sophus中，se(3)的平移在前，旋转在后.
  // 同样的，有hat和vee两个算符
  cout << "se3 hat = \n" << Sophus::SE3d::hat(se3) << endl;
  cout << "se3 hat vee = " << Sophus::SE3d::vee(Sophus::SE3d::hat(se3)).transpose() << endl;

  // 最后，演示一下更新
  Vector6d update_se3; //更新量
  update_se3.setZero();
  update_se3(0, 0) = 1e-4d;
  Sophus::SE3d SE3_updated = Sophus::SE3d::exp(update_se3) * SE3_Rt;
  cout << "SE3 updated = " << endl << SE3_updated.matrix() << endl;

  return 0;
}
```

我们一段一段来看

- `M_PI`是在`cmath`库下面定义的，就是3.1415926...

- `AngleAxisd(angle, axis)`给出角度和转轴，生成一个描述围绕转轴转动相应角度的旋转向量，在程序中，使用`.toRotationMatrix()`的方法将其转换为更加实用的旋转矩阵。

- `Sophus::SO3d SO3_q(q)`可以通过四元数或者是旋转矩阵的形式来构造SO3

- `SO3_R.log()`通过SO3获取so3

- ```C++
    Vector3d update_so3(1e-4, 0, 0); //假设更新量为这么多
    Sophus::SO3d SO3_updated = Sophus::SO3d::exp(update_so3) * SO3_R;
  ```

  通过左乘的方法用小量来做扰动

- 后面是SE3，都一样

