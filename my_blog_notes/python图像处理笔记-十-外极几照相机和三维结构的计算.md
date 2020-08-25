# python图像处理笔记-十-外极几照相机和三维结构的计算

到这里就已经到书中的第五章了，进度大概是在50%。这一章介绍的如何处理多个视图，以及如何利用多个视图的几何关系来回复照相机位置信息和三维结构。通过在不同视点拍摄出的图像，可以使用特征匹配来计算出三维场景点以及照相机位置。

## 外极几何

**多视图集合**是利用在不同视点所拍摄图像间的关系，来研究照相机之间或特征之间关系的一门科学。图像的特征往往是兴趣点，这一章使用的也是兴趣点特征，多视图几何中最重要的内容是双视图几何。

### 做什么？

有一个场景的两个视图以及视图中的对应图像点，根据照相机间的空间相对位置关系、照相机的性质以及三维场景点的位置，可以得到对这些图像点的一些几何关系约束。我们**通过外极几何来描述这些几何关系**。当然，按照惯例，书里讲的都是外极几何的基本内容。

### 外极几何

没有关于照相机的先验知识，会出现固有二义性，因为三维场景点X经过4*4单应性矩阵H变换为HX后，HX在照相机$PH^{-1}$里得到的图像点和X在照相机P里得到的图像点相同。我们可以把上面的话用公式表示为：


$$
\lambda x = PX = PH^{-1}HX = \hat{P} \hat{X}
$$
说人话就是，我们现在只知道图像上面的点，并不知道点在三维空间中的分布情况，这就像是我们探究a+b=c的时候，只知道c的值一样，有很多组不同的a,b会得到同样的解，但是实际情况下，只有一组a,b是正确的。

因此，当我们分析双视图几何关系时，可以将照相机间的相对位置关系用单影行矩阵加以简化。这里的单应性矩阵通常只做刚体变换，即之通过单应矩阵变换了坐标系。一个比较好的做法是：将原点和坐标轴与第一个照相机对齐，即：
$$
P_1 = K_1[I|0]\ P_2 = K_2[R|t]
$$
其中，$K_1,K_2$是相机的标定矩阵，R是第二个照相机的旋转矩阵，t是第二个照相机的平移向量。利用这些参数矩阵，我们可以找到点X的投影点x_1,x_2的关系，这样我们就可以从图像出发，恢复照相机参数矩阵。

同一个图像点经过不同投影矩阵产生的不同投影点必须满足：
$$
x_2^TFx_1=0
$$
其中:
$$
F = K_2^{-T}S_tRK_1^{-1}
$$
矩阵$S_t$为反对称矩阵：
$$
S_t = \left[ \begin{matrix} 0&t_3&t_2\ t_3&0&-t_1\ -t_2&t_1&0 \end{matrix} \right]
$$
这个约束就是外极约束条件。矩阵F为基础矩阵，基础矩阵可以由两个照相机的参数矩阵表示。由于det(F) = 0所以基础矩阵的秩小于等于2。

上面的公式表明，我们可以借助F来恢复出照相机参数，而F可以从对应的图像点计算出。在已知照相机内参的情况下，重建是基于度量的，所谓**度量重建**，即能够在三维重建中正确的表示距离和角度。

### 一些其他的几何知识

给定图像中的一个点，例如第二个图中的$x_2$，我们可以找到对应第一幅图像的一条直线：
$$
x_2^TFx_1 = l_1^Tx_1 = 0
$$
其中：
$$
l_1^T = x_2^TF
$$
这条直线称为对应于点$x_2$的外极线，也就是说，点$x_2$在第一幅图像中的对应点一定在这条直线上。外极线都经过一点 **e**，称为外极点。实际上，外极点是另一个照相机光心对应的图像点。外极点可以在我们看到的图像外，这取决于照相机间的相对方向。

[![img](http://www.ituring.com.cn/figures/2014/Python/09.d05z.001.png)](http://www.ituring.com.cn/figures/2014/Python/09.d05z.001.png)

### 一个例子

#### 读取数据并进行可视化

从牛津数据集中下载数据，读入后进行可视化，由于书上的网址已经过期，所以在这里给出新的网址：

```
http://www.robots.ox.ac.uk/~vgg/data/mview/
```



```python
from PCV.geometry import camera
import numpy as np
from PIL import Image
# 载入一些图像
im1 = np.array(Image.open('data/001.jpg'))
im2 = np.array(Image.open('data/002.jpg'))
# 载入每个视图的二维点到列表中
points2D = [np.loadtxt('data/00'+str(i+1)+'.corners').T for i in range(3)]
# 载入三维点
points3D = np.loadtxt('data/p3d').T
# 载入对应
corr = np.genfromtxt('data/nview-corners',dtype='int',missing_values='*')
#载入照相机矩阵到Camera 对象列表中
P = [camera.Camera(np.loadtxt('data/00'+str(i+1)+'.P')) for i in range(3)]
```

读入后进行数据可视化，在书中，作者使用了python2中的`

```python
execfile('loadData.py')`
```

来在别的程序中进行加载，loadData.py就是我们刚才写的脚本，但是该方法在python3中已经弃用，可以用下面的方法进行代替：

```
with open('loadData.py','r') as f:    
	exec(f.read())
```

但是在pycharm中，loadData中声明的数据并没有被pycharm识别，这种调用方法影响了高亮显示，所以不是特别推荐，下面对2D数据进行可视化：

```python
import pylab as pl
import numpy as np
with open('loadData.py','r') as f:    
	exec(f.read())
# 将三维点转换成齐次坐标表示，并投影
X = np.vstack( (points3D,np.ones(points3D.shape[1])) )
x = P[0].project(X)
# 在视图1 中绘制点
pl.figure()
pl.imshow(im1)
pl.plot(points2D[0][0],points2D[0][1],'*')
pl.axis('off')
pl.figure()
pl.imshow(im1)
pl.plot(x[0],x[1],'r.')
pl.axis('off')
pl.show()
```

[![img](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-28 22-59-34.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-28 22-59-34.png)

据作者所说，第二张图的点是比第一张图要多的，因为第二张图上的点是根据三张图重建出来的点根据照相机参数投影到图像中的。

接下来我们对其中的3D点进行可视化，这个也是非常的简单：

```python
# 绘制三维点
from mpl_toolkits.mplot3d import axes3
dfig = pl.figure()
ax = fig.gca(projection='3d')
ax.plot(points3D[0],points3D[1],points3D[2],'k.')
pl.show()
```

[![img](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-28 23-19-58.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-28 23-19-58.png)

#### 八点法

##### 原理

在上面，我们知道有这样的式子：
$$
x_2^TFx_1=0
$$
目前我们知道：$x_2,x_1$分别是两张图像上的点，他们都以其次座标表示，而F是一个3*3的基本矩阵。我们将它用完整形式再来写一遍：
$$
\left[ \begin{matrix} x_2& y_2& 1 \end{matrix} \right] \left[ \begin{matrix} f_{11}&f_{12}&f_{13}\\ f_{21}&f_{22}&f_{23}\\ f_{31}&f_{32}&f_{33} \end{matrix} \right] \left[ \begin{matrix} x_1 \\ y_1 \\ 1 \end{matrix} \right] =0
$$


将这个式子进行展开：
$$
\left[ \begin{matrix} x_2f_{11}+y_2f_{12}+f_{13} & x_2f_{21}+y_2f_{22}+f_{23} & x_2f_{31}+y_2f_{32}+f_{33} \end{matrix} \right] \left[ \begin{matrix} x_1\\y_1\\1 \end{matrix} \right] =0
$$
进一步展开，可以得到下面的式子：
$$
x_1x_2f_{11} + x_1y_2f_{12}+x_1f_{13}+x_2y_1f_{21}+y_1y_2f_{22}+y_1f_{23}+x_2f_{31}+y_2f_{32}+f_{33}=0
$$
经过归纳后可以写成：
$$
\left[ \begin{matrix} x_1x_2& x_1y_2& x_1& x_2y_1& y_1y_2& y_1& x_2& y_2& 1 \end{matrix} \right]f=0
$$


其中，f是之前的F矩阵被打入一维后的矩阵。

给定n组点的集合，我们就可以得到以下方程：

![img](https://private.codecogs.com/gif.latex?\begin{bmatrix}x^1_2x^1_1 & x^1_2y^1_1 & x^1_2w^1_1 & \cdots & w^1_2w^1_1\x^2_2x^2_1 & x^2_2y^2_1 & x^2_2w^2_1 & \cdots & w^2_2w^2_1\\vdots & \vdots & \vdots & \ddots & \vdots\x^n_2x^n_1 & x^n_2y^n_1 & x^n_2w^n_1 & \cdots & w^n_2w^n_1\end{bmatrix}\begin{bmatrix}\boldsymbol{F}_{11}\\boldsymbol{F}_{12}\\boldsymbol{F}_{13}\\vdots\\boldsymbol{F}_{33}\\end{bmatrix}=\boldsymbol{Af}=0)

由于尺度是固定的，所以最少需要八个对应点来计算基础矩阵F，所以叫八点法。

##### 代码

这部分东西有所丢失，你可以到www.wangsaiyu.cn去查看相应的内容，这是我的私人博客。

最终的结果：

[![img](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-29 01-49-22.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-29 01-49-22.png)

这就是一个极点在画面外面的例子。

在这一过程中，我们选择两幅图像的对应点，然后将它们转换为齐次坐标。这里的对应点是从一个文本文件中读取得到的；而实际上，我们可以按照第 2 章的方法提取图像特征，然后通过匹配来找到它们。由于缺失的数据在对应列表 `corr` 中为 -1，所以程序中有可能 选取这些点。因此，上面的程序通过数组操作符 `&` 只选取了索引大于等于 0 的点。

## 照相机和三维结构的计算

### 三角剖分

给定照相机模型，图像的点可以通过三角剖分获得其三维位置。对于两个照相机P_1,P_2的视图，三维实物点X的投影点为x_1,x_2，照相机方程定义了下面的关系：
$$
\left[ \begin{matrix} P_1 & -x_1 & 0\\ P_2 & 0 & -x_2 \end{matrix} \right] \left[ \begin{matrix} X\\ \lambda_1\\ \lambda_2 \end{matrix} \right] = 0
$$






由于图像噪声、照相机参数误差和其他系统误差等，上面的方程可能没有精确解。我们可以通过ＳＶＤ来得到三维点的最小二乘估计。

进行三角剖分的代码如下：(这里也丢了)

还是老亚子，写个Ｄｅｍｏ用之前的数据集把它跑起来：（没错，又丢了）

最终的结果如下：

[![img](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-29 21-39-03.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-29 21-39-03.png)

估计点和实际点能够很好的匹配。

### **由三维点计算照相机矩阵**

和上一个是一个理论：

[![img](https://private.codecogs.com/gif.latex?\begin{bmatrix}\text{X}^T_1 & 0 & 0 & -x_1 & 0 & 0 & \cdots\0 & \text{X}^T_1 & 0 & -y_1 & 0 & 0 & \cdots\0 & 0 & \text{X}^T_1 & -1 & 0 & 0 & \cdots\\text{X}^T_2 & 0 & 0 & 0 & -x_2 & 0 & \cdots\0 & \text{X}^T_2 & 0 & 0 & -y_2 & 0 & \cdots\0 & 0 & \text{X}^T_2 & 0 & -1 & 0 & \cdots\\vdots & \vdots & \vdots & \vdots & \vdots & \vdots & \vdots\end{bmatrix}\begin{bmatrix}\text{p}^T_1\\text{p}^T_2\\text{p}^T_3\\lambda_1\\lambda_2\\vdots\end{bmatrix}=0)](https://private.codecogs.com/gif.latex?\begin{bmatrix}\text{X}^T_1 & 0 & 0 & -x_1 & 0 & 0 & \cdots\0 & \text{X}^T_1 & 0 & -y_1 & 0 & 0 & \cdots\0 & 0 & \text{X}^T_1 & -1 & 0 & 0 & \cdots\\text{X}^T_2 & 0 & 0 & 0 & -x_2 & 0 & \cdots\0 & \text{X}^T_2 & 0 & 0 & -y_2 & 0 & \cdots\0 & 0 & \text{X}^T_2 & 0 & -1 & 0 & \cdots\\vdots & \vdots & \vdots & \vdots & \vdots & \vdots & \vdots\end{bmatrix}\begin{bmatrix}\text{p}^T_1\\text{p}^T_2\\text{p}^T_3\\lambda_1\\lambda_2\\vdots\end{bmatrix}=0)

用ＳＶＤ来做就可以了：（双丢了）

老亚子，写个Ｄｅｍｏ跑一下试试：（叒丢了）

运行结果如下：

![img](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/Screenshot from 2020-04-29 22-09-15.png)

### **由基础矩阵计算照相机矩阵**

#### 未标定的情况下：

在没有任何照相机内参数的情况下，照相机矩阵只能通过摄影变换恢复出來。也就是说，如果利用照相机信息来重建三维点，那么该重建只能由摄影变换计算出来。在这里，我们不考虑角度和距离。因此，在无标定的情况下，可以通过一个摄影变换得出，一个方法是：
$$
P_2 = \left[S_eF|e\right]
$$
其中，e是左极点，满足$e^TF=0，S_e$是一个反对称矩阵，但是值得注意的是，用该矩阵做出的三角形剖分很有可能会发生畸变。