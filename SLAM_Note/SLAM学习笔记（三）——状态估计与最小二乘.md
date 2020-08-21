# SLAM学习笔记（三）——状态估计与最小二乘

## 观测模型

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817164237.png" style="zoom:33%;" />

这里就回到了那个古老的问题，就是：我们在干什么？实质上我们就是在干上面的两个式子，$x_k$就是相机的外参，也就是描述相机在空间中的位置、角度信息的，他就是slam想要得到的对象，因为有了外参，啥都能求出来了。那么我们看上面的两个式子，他就是通过上一个时刻的摄像机的姿态和位置，以及这一时刻输入的传感器信号以及噪声，来对当前时刻的摄像机的位置、角度等外参进行估计（这是第一个式子的含义），第二个式子表示观测方程，这个比较简单，就是知道每一个时刻的坐标等信息后，对地图进行更新。





## 批量状态估计问题

### 批量式

一次给定所有状态变量和结果，然后让你算出分布

这种的有很多方法，这里书上介绍的是两种：

#### 贝叶斯（最大后验概率估计）

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817164949.png" style="zoom: 33%;" />

相信大家门清，不门清的我这里不保会了，你们可以去百度一下，一百度一片。



#### 最大似然估计

算的是

> 在哪种情况下，最容易产生当前的观测

我都有点忘了怎么做最大似然了（虽然这个学期刚学了），于是又去汤家凤老师那里学了一手：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817170650.png" style="zoom: 33%;" />

可以看到，整体的思路就是：

- 第一步：列出最大似然函数，最大似然函数表示：**在参数条件θ成立的条件下，结果x出现的概率**，可以看出，这个函数的值越大，就表示这个参数在这组数据上越正确。
- 第二步：对数化一下，把连乘化成连加，原来函数的最大值现在还是最大值，但是好求了很多
- 第三步：求导，让导数为0，以求出极大值（这个只是一种方法，最终目的都是为了找最大值）

#### 最小二乘估计

顺下来看，假设我们想通过观测到的xy来推测z，我们假设观测数据的噪声符合正态分布，就有了下面的式子：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817172027.png" style="zoom: 33%;" />

我们使用最大似然估计的时候，就是要最大化这里的$P(z_{j,k}|x_k,y_j)$，实质上就是最大化一个高斯：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817172144.png" style="zoom: 33%;" />

我们将这个式子取一个对数，然后为了方便表示我们在乘上一个-1，把他从原来的最大化变成现在的最小化，也就是说
$$
max (a) -->min(-ln(a))
$$
这个过程很好理解，化出来的式子变成了这样：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817172329.png" style="zoom: 33%;" />

我们上面说了，现在的任务变成了最小化这个式子，我们发现如果要最小化x的话，那么就只需要关心后面被框出来的部分就可以了。后面这部分是什么呢？就是马氏距离，马氏距离听起来很高端，实际上就是对每个分量做标准化后再算和平均值之间的欧式距离就可以了。



最终，我们根据原有的观测模型，定义出外参的误差与观测结果的误差，就可以进行估计了，

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817173007.png" style="zoom: 67%;" />

需要注意的是，这里使用了协方差矩阵R来调整每一项在最终的结果中的占比，他的思路是，协方差越大的占比越高，这说明数据波动越大含有的信息越高，这种思想和熵权法是相似的。



## 非线性最小二乘



这里的我都学过，我就不多介绍了，我列个目录：

- 最速下降法

- [牛顿法]([http://www.wangsaiyu.cn/2020/05/28/%E6%9C%80%E4%BC%98%E5%8C%96%E6%8A%80%E6%9C%AF%E2%80%94%E2%80%94%E7%89%9B%E9%A1%BF%E6%B3%95/](http://www.wangsaiyu.cn/2020/05/28/最优化技术——牛顿法/))

- [GN（高斯牛顿法）](https://blog.csdn.net/qq_39521554/article/details/79919041)

  牛顿法长这样：

  <img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817174420.png" style="zoom: 50%;" />

  大家看之前一定要搞懂一个问题，那就是，高斯-牛顿法研究的目标函数一定是：

  ![](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817180526.png)

  只有这样，才能够满足高斯-牛顿的条件，才能看得懂他在干啥，再者，还要明确，这里的r也就是残差足够小，小到第二个矩阵O可以被忽略，所以高斯牛顿是有效的。

- LM

想看这些最优化算法的话，为大家墙裂推荐一个老师

https://space.bilibili.com/509445577

我上个学期的最优化基本上没听课，都是跟着这个老师在学的



## 程序

在这一节中我们主要学习这个g20怎么用，代码依旧是高博的：

```C++
#include <iostream>
#include <g2o/core/g2o_core_api.h>
#include <g2o/core/base_vertex.h>
#include <g2o/core/base_unary_edge.h>
#include <g2o/core/block_solver.h>
#include <g2o/core/optimization_algorithm_levenberg.h>
#include <g2o/core/optimization_algorithm_gauss_newton.h>
#include <g2o/core/optimization_algorithm_dogleg.h>
#include <g2o/solvers/dense/linear_solver_dense.h>
#include <Eigen/Core>
#include <opencv2/core/core.hpp>
#include <cmath>
#include <chrono>

using namespace std;

// 曲线模型的顶点，模板参数：优化变量维度和数据类型
class CurveFittingVertex : public g2o::BaseVertex<3, Eigen::Vector3d> {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  // 重置
  virtual void setToOriginImpl() override {
    _estimate << 0, 0, 0;
  }

  // 更新
  virtual void oplusImpl(const double *update) override {
    _estimate += Eigen::Vector3d(update);
  }

  // 存盘和读盘：留空
  virtual bool read(istream &in) {}

  virtual bool write(ostream &out) const {}
};

// 误差模型 模板参数：观测值维度，类型，连接顶点类型
class CurveFittingEdge : public g2o::BaseUnaryEdge<1, double, CurveFittingVertex> {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  CurveFittingEdge(double x) : BaseUnaryEdge(), _x(x) {}

  // 计算曲线模型误差
  virtual void computeError() override {
    const CurveFittingVertex *v = static_cast<const CurveFittingVertex *> (_vertices[0]);
    const Eigen::Vector3d abc = v->estimate();
    _error(0, 0) = _measurement - std::exp(abc(0, 0) * _x * _x + abc(1, 0) * _x + abc(2, 0));
  }

  // 计算雅可比矩阵
  virtual void linearizeOplus() override {
    const CurveFittingVertex *v = static_cast<const CurveFittingVertex *> (_vertices[0]);
    const Eigen::Vector3d abc = v->estimate();
    double y = exp(abc[0] * _x * _x + abc[1] * _x + abc[2]);
    _jacobianOplusXi[0] = -_x * _x * y;
    _jacobianOplusXi[1] = -_x * y;
    _jacobianOplusXi[2] = -y;
  }

  virtual bool read(istream &in) {}

  virtual bool write(ostream &out) const {}

public:
  double _x;  // x 值， y 值为 _measurement
};

int main(int argc, char **argv) {
  double ar = 1.0, br = 2.0, cr = 1.0;         // 真实参数值
  double ae = 2.0, be = -1.0, ce = 5.0;        // 估计参数值
  int N = 100;                                 // 数据点
  double w_sigma = 1.0;                        // 噪声Sigma值
  double inv_sigma = 1.0 / w_sigma;
  cv::RNG rng;                                 // OpenCV随机数产生器

  vector<double> x_data, y_data;      // 数据
  for (int i = 0; i < N; i++) {
    double x = i / 100.0;
    x_data.push_back(x);
    y_data.push_back(exp(ar * x * x + br * x + cr) + rng.gaussian(w_sigma * w_sigma));
  }

  // 构建图优化，先设定g2o
  typedef g2o::BlockSolver<g2o::BlockSolverTraits<3, 1>> BlockSolverType;  // 每个误差项优化变量维度为3，误差值维度为1
  typedef g2o::LinearSolverDense<BlockSolverType::PoseMatrixType> LinearSolverType; // 线性求解器类型

  // 梯度下降方法，可以从GN, LM, DogLeg 中选
  auto solver = new g2o::OptimizationAlgorithmGaussNewton(
    g2o::make_unique<BlockSolverType>(g2o::make_unique<LinearSolverType>()));
  g2o::SparseOptimizer optimizer;     // 图模型
  optimizer.setAlgorithm(solver);   // 设置求解器
  optimizer.setVerbose(true);       // 打开调试输出

  // 往图中增加顶点
  CurveFittingVertex *v = new CurveFittingVertex();
  v->setEstimate(Eigen::Vector3d(ae, be, ce));
  v->setId(0);
  optimizer.addVertex(v);

  // 往图中增加边
  for (int i = 0; i < N; i++) {
    CurveFittingEdge *edge = new CurveFittingEdge(x_data[i]);
    edge->setId(i);
    edge->setVertex(0, v);                // 设置连接的顶点
    edge->setMeasurement(y_data[i]);      // 观测数值
    edge->setInformation(Eigen::Matrix<double, 1, 1>::Identity() * 1 / (w_sigma * w_sigma)); // 信息矩阵：协方差矩阵之逆
    optimizer.addEdge(edge);
  }

  // 执行优化
  cout << "start optimization" << endl;
  chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
  optimizer.initializeOptimization();
  optimizer.optimize(10);
  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
  cout << "solve time cost = " << time_used.count() << " seconds. " << endl;

  // 输出优化值
  Eigen::Vector3d abc_estimate = v->estimate();
  cout << "estimated model: " << abc_estimate.transpose() << endl;

  return 0;
}
```

