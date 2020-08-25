# SLAM学习笔记（四）——视觉里程计

终于到了激动人心的应用环节了。



## 复习

我们先来回顾一下相机是怎么工作的，这里就必须回到我们的三个坐标系的转换了！

```flow
st=>start: 世界坐标
op=>operation: 相机坐标
e=>end: 图片坐标

st->op->e

```

世界坐标转换到相机坐标是通过变换矩阵进行转换的，相机坐标转移到图片坐标，是通过内参矩阵进行转换的。

那么如果我们知道摄像机的内参，并且在摄像机运动过程中，能够在不同的时间点，不同的位置点，提取到相同的的点，就可以给场景建立联系，以求出外参矩阵，来达到估计的目的。





## 特征点匹配

这个点，是需要进行选取的，因为如果对图像中所有的点进行匹配的话，那么需要的时间太久，我们可以使用很多的方法进行匹配：

比如： 

- SIFT [链接]([http://www.wangsaiyu.cn/2020/04/16/python%E5%9B%BE%E5%83%8F%E5%A4%84%E7%90%86%E7%AC%94%E8%AE%B0-%E4%BA%94SIFT%EF%BC%88%E5%B0%BA%E5%BA%A6%E4%B8%8D%E5%8F%98%E7%89%B9%E5%BE%81%E5%8F%98%E6%8D%A2%EF%BC%89/](http://www.wangsaiyu.cn/2020/04/16/python图像处理笔记-五SIFT（尺度不变特征变换）/))
- Harris [链接]([http://www.wangsaiyu.cn/2020/04/16/python%E5%9B%BE%E5%83%8F%E5%A4%84%E7%90%86%E7%AC%94%E8%AE%B0-%E5%9B%9B-Harris%E8%A7%92%E7%82%B9%E6%A3%80%E6%B5%8B%E5%99%A8%E5%BB%B6%E7%94%B3/](http://www.wangsaiyu.cn/2020/04/16/python图像处理笔记-四-Harris角点检测器延申/))
- ORB

之前没学过ORB，今天就着重学习一下。

和SIFT相比，ORB要快很多，并且也带有旋转不变性，想要学习ORB，需要先从FAST来入手。



### FAST特征点

![](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200818104802.png)

FAST是一种提取特征点的方法，他只负责提取，不负责描述特征点，他提取的方法非常简单，我们以点p为例：

- 第一步： 以3为半径，找到这个圆上的16个点
- 第二步： 设定阈值，假设为20%
- 第三步： 检测16个点中有无连续的9个点在区间$[p * 80\%,p*120\% ]$之外的，注意需要连续

如果有那么p就是FAST特征点，如果没有，那么就不是特征点。

FAST特征点非常强大的一点在于：它不需要计算图像中的其他信息，只用对比一个点以及周围的16个点的信息即可，非常快速。当然，还可以有FAST9、FAST11、FAST10等等，他们不同点在于连续满足条件的点的数量不同。



### ORB特征点



我们主要从特征点提取和描述子表示两个方面来看ORB



#### 特征点提取

相较于FAST，ORB主要有以下不同：

- 第一点就是对比的点的数量的优化：在上面的FAST中，我们要对比16个点，并且看有没有连续的n个满足条件的，但是在这里，我们只需要判断1、5、9、13，这四个点是否满足条件，如果有三个点满足条件，那么就可以认定中心点为orb特征点
- 非最大值抑制：在某一个区域内进行特征点提取时，将1、5、9、13与中心点做差，取其差的绝对值的和作为强度描述子，在某一区域内，仅取极大值点。
- 金字塔：这里的金字塔就是最简单的下采样，多下采样几次看起来就像金字塔了，不需要高斯也不需要差分
- 方向：带有方向的考虑，方向主要靠灰度质心法

#### 描述子

描述子是基于**BRIEF**描述子进行改进的，根据提取过程中提取的方向，进行了反转后再去做的比对。

传统的BRIEF描述子是在点的周围随机选取128对点对进行比较，这里先旋转后比较。



## 代码

高博的，我加了点注释

```C++
// compute the descriptor
void ComputeORB(const cv::Mat &img, vector<cv::KeyPoint> &keypoints, vector<DescType> &descriptors) {
  const int half_patch_size = 8;
  const int half_boundary = 16;
  int bad_points = 0;
  for (auto &kp: keypoints) {
    // 如果不在范围内的话，那么久直接舍弃掉了
    if (kp.pt.x < half_boundary || kp.pt.y < half_boundary ||
        kp.pt.x >= img.cols - half_boundary || kp.pt.y >= img.rows - half_boundary) {
      // outside
      bad_points++;
      descriptors.push_back({});
      continue;
    }

    // 在窗口内进行遍历
    float m01 = 0, m10 = 0;
    for (int dx = -half_patch_size; dx < half_patch_size; ++dx) {
      for (int dy = -half_patch_size; dy < half_patch_size; ++dy) {
        uchar pixel = img.at<uchar>(kp.pt.y + dy, kp.pt.x + dx);
        m01 += dx * pixel;// 方向就是距离中心点的距离*灰度的和
        m10 += dy * pixel;
      }
    }

    // 计算方向角对应的三角函数
    // angle should be arc tan(m01/m10);  
    float m_sqrt = sqrt(m01 * m01 + m10 * m10) + 1e-18; // avoid divide by zero
    float sin_theta = m01 / m_sqrt;
    float cos_theta = m10 / m_sqrt;

    // compute the angle of this point
    DescType desc(8, 0);
    for (int i = 0; i < 8; i++) {
      uint32_t d = 0;
      for (int k = 0; k < 32; k++) {
        int idx_pq = i * 8 + k;
        // 旋转前
        cv::Point2f p(ORB_pattern[idx_pq * 4], ORB_pattern[idx_pq * 4 + 1]);
        cv::Point2f q(ORB_pattern[idx_pq * 4 + 2], ORB_pattern[idx_pq * 4 + 3]);
        // 旋转后
        // rotate with theta
        cv::Point2f pp = cv::Point2f(cos_theta * p.x - sin_theta * p.y, sin_theta * p.x + cos_theta * p.y)
                         + kp.pt;
        cv::Point2f qq = cv::Point2f(cos_theta * q.x - sin_theta * q.y, sin_theta * q.x + cos_theta * q.y)
                         + kp.pt;
        // 比较大小,给相应位置赋1
        if (img.at<uchar>(pp.y, pp.x) < img.at<uchar>(qq.y, qq.x)) {
          d |= 1 << k;
        }
      }
      desc[i] = d;
    }
    descriptors.push_back(desc);
  }

  cout << "bad/total: " << bad_points << "/" << keypoints.size() << endl;
}
```



比较就直接用汉明距离

```C++
// brute-force matching
void BfMatch(const vector<DescType> &desc1, const vector<DescType> &desc2, vector<cv::DMatch> &matches) {
  const int d_max = 40;

  for (size_t i1 = 0; i1 < desc1.size(); ++i1) {
    if (desc1[i1].empty()) continue;
    cv::DMatch m{i1, 0, 256};
    for (size_t i2 = 0; i2 < desc2.size(); ++i2) {
      if (desc2[i2].empty()) continue;
      int distance = 0;
      for (int k = 0; k < 8; k++) {
        distance += _mm_popcnt_u32(desc1[i1][k] ^ desc2[i2][k]);
      }
      if (distance < d_max && distance < m.distance) {
        m.distance = distance;
        m.trainIdx = i2;
      }
    }
    if (m.distance < d_max) {
      matches.push_back(m);
    }
  }
}
```





## 计算相机位置



### 几种匹配方法：

- 只有两个单目的情况： 2D-2D——对极几何
- 帧和地图之间匹配：  3D-2D——PnP
- 匹配的是RBG-D：3D-3D——ICP



### 对极几何

![](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200821095605.png)

先说一下什么是极线约束吧， 图上的两个蓝色框框代表相机的拍摄位置，P为真实存在的点，p1,p2是P点在图像上的点，如果我们只确定了p1,那么我们也可以确定，p2一定在p2->e2的直线上，所以说我们不必再在整张图上匹配特征点，而是可以直接在极线上去寻找匹配点就可以了。

存在两种对极约束：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200821100947.png" style="zoom:67%;" />

可以简写一波：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200821101034.png" style="zoom:67%;" />

在SLAM的实际运用的过程中，p1,p2已知，其他的都不知道。

我们知道x且知道他的内参，所以我们是可以将两个相机的相机坐标系下的p点算出来的，也就是x1，x2,然后再通过上面的式子中的E算出来，然后再通过E将Rt矩阵算出即可。E是一个自由度为5的矩阵，最少要用5个点来把她算出来，但是这样比较麻烦，所以我们可以用八点法来将他算出。

可以看一下我之前写过的八点法 [链接]([http://www.wangsaiyu.cn/2020/04/29/python%E5%9B%BE%E5%83%8F%E5%A4%84%E7%90%86%E7%AC%94%E8%AE%B0-%E5%8D%81-%E5%A4%96%E6%9E%81%E5%87%A0%E7%85%A7%E7%9B%B8%E6%9C%BA%E5%92%8C%E4%B8%89%E7%BB%B4%E7%BB%93%E6%9E%84%E7%9A%84%E8%AE%A1%E7%AE%97/#more](http://www.wangsaiyu.cn/2020/04/29/python图像处理笔记-十-外极几照相机和三维结构的计算/#more))；有了E之后，就可以直接用奇异值分解来算出来Rt，会得到四个解，但是只有一个解的深度为正，该解就是最终解。

当然，比八点还多的时候，可以用最优化方法或者是RANSAC，除此之外，八点法会在特征点共面的时候退化，这里就应当使用单应矩阵来恢复Rt，假设该平面的法线为n，到平面的距离为d，那么就有这么一个方程：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200821102541.png" style="zoom:67%;" />

最终，就有这么一个方程：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200821102604.png" style="zoom:67%;" />

我们就把中间这一坨写成H再来解就可以了，最终再从H去恢复Rt、n、d、K



### PnP

已经知道3D点的空间位置和相机上的投影点，求相机的旋转和平移。



#### 线性解法：DLT

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200821104317.png" style="zoom:67%;" />

这个方法将Rt看作独立的未知量，所以在求出结果之后需要将他的结果投影回SO(3)。

结束，DLT就叫直接线性解法，光解出来t1到t12是不难的。



### BA

现在我们通过DLT解出来了Rt，我们就可以通过摄像机变换，来通过第一帧的坐标系中的坐标，来求出第二帧中对应的坐标，我们就优化Rt变换后得到的像素坐标与实际的像素坐标的差值就可以了，解的话就直接用最优化方法就可以了，定义的误差是：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200821111517.png" style="zoom:67%;" />