# python图像处理笔记-九-简单标定、姿态估计与增强现实

## 一种简单的标定方法：

在上一个笔记中我们学习了**张正友标定法**，并且挖了一个用张正友标定法标定相机的坑，但是这个坑目前还填补上。这一节里面我们要学的是用一种非常简单的方法进行标定，方法如下：


### 需要准备的东西

- 一个矩形（一本书就可以了）
- 一个卷尺
- 一个平面

### 操作步骤

#### 数据测量

- 测量你选的矩形的边长：$dX,dY$。
- 将照相机和标定物体放置在平面上，使得照相机的背面和标定物体平行，同时物体位于照相机图像视图的中心，你可能需要调整照相机或物体来获得良好的对齐效果。
- 测量标定物体到照相机的距离$dZ$
- 拍摄一幅图像来测试是否正确（被摄物体的竖边和图像竖边平行，横边也一样）
- 使用像素数来测量标定物体图像的宽和高：$dx,dy$

#### 计算

- 焦距：
  - $f_x = \frac{dx}{dX}dZ$
  - $f_y = \frac{dy}{dY}dZ$



#### 实操

我选用了我的iapd进行标定，因为我的ipad有个支架，他非常适合进行固定和拍摄。我利用几本书将ipad固定在地上，然后再将另外一本书靠在墙上进行拍摄。我刚拍摄回来，接着写：

在第一次实拍过程中我是这样布局的：

[![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9zYWl5dXdhbmctYmxvZy5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vSU1HXzIwMjAwNDI1XzE5NTYyOS5qcGc?x-oss-process=image/format,png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/IMG_20200425_195629.jpg)

拍摄出来的效果如下：

[![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9zYWl5dXdhbmctYmxvZy5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vOEYxMEE5MDUwOUZGMjAzRUQ5MTVDMzVENTE0NjY5MTQuanBn?x-oss-process=image/format,png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/8F10A90509FF203ED915C35D51466914.jpg)

可以看出来拍摄的结果是存在一定问题的，问题主要在于：拍出来的书的边是斜的，这主要是由于：

- 书的最后一页有张开
- ipad有点斜

于是我进行了一些调整，调整后的如下：

[![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9zYWl5dXdhbmctYmxvZy5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vSU1HXzIwMjAwNDI1XzE5NTkxOS5qcGc?x-oss-process=image/format,png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/IMG_20200425_195919.jpg)

这里用牛奶盒子避免了书翻页的问题，结果如下：

[![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9zYWl5dXdhbmctYmxvZy5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vOEFEQ0JCQzBEQjgwRDkxRTZDOUNBNDY4OEVFMDMwODEuanBn?x-oss-process=image/format,png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/8ADCBBC0DB80D91E6C9CA4688EE03081.jpg)

接下来进行测量：

- 盒子宽：168mm
- 盒子长：282mm
- 盒子到相机：556mm

接下来我们写个程序来获取物体在图片中的高和宽：

```python
import pylab as pl
from PIL import Image

im = Image.open("box.jpg")
pl.imshow(im)
# 选择盒子的两个对角点，获得盒子的长和宽
points = pl.ginput(2)
lenth = max(abs(points[0][0] - points[1][0]), abs(points[1][1] - points[0][1]))
width = min(abs(points[0][0] - points[1][0]), abs(points[1][1] - points[0][1]))
print(lenth, width)
```

在图片上我们的盒子的尺寸是：1496.883116883117*801.4285714285716

图像的尺寸是：2448 *3264

经过计算我们知道：
$$
f_x = 2951,f_y = 2652
$$
与书中相比，我这个误差还是比较大的。

我们给出一个根据已经标定好的内参生成内参矩阵的方法：

```
def myCaliBration(sz):
    row, col = sz
    fx = 2951 * col /3264
    fy = 2652 * row /2248
    K = np.diag([fx,fy,1])
    K[0,2] = 0.5*col
    K[1,2] = 0.5*row
    return K
```

这个方法和张正友比起来已经非常精简了，但是精简的代价是结果比较扣脚。

## 姿态估计

这里主要就是前面的知识的应用了，直接贴代码：

```python
from PCV.geometry import homography
from scipy import linalg
from PCV.geometry import camera
from PIL import Image
import numpy as np
from PCV.localdescriptors import sift
import pylab as pl

def cubePoints(c, wid):
    """
    用于绘制立方体的点列表
    """
    p = []
    # 底部
    p.append([c[0] - wid, c[1] - wid, c[2] - wid])
    p.append([c[0] - wid, c[1] + wid, c[2] - wid])
    p.append([c[0] + wid, c[1] + wid, c[2] - wid])
    p.append([c[0] + wid, c[1] - wid, c[2] - wid])
    # 为了闭合图像，和第一个相同
    p.append([c[0] - wid, c[1] - wid, c[2] - wid])

    # 顶部
    p.append([c[0] - wid, c[1] - wid, c[2] + wid])
    p.append([c[0] - wid, c[1] + wid, c[2] + wid])
    p.append([c[0] + wid, c[1] + wid, c[2] + wid])
    p.append([c[0] + wid, c[1] - wid, c[2] + wid])
    # 为了闭合图像，和第一个相同
    p.append([c[0] - wid, c[1] - wid, c[2] + wid])

    # 竖直
    p.append([c[0] - wid, c[1] - wid, c[2] + wid])
    p.append([c[0] - wid, c[1] + wid, c[2] + wid])
    p.append([c[0] - wid, c[1] + wid, c[2] - wid])
    p.append([c[0] + wid, c[1] + wid, c[2] - wid])
    p.append([c[0] + wid, c[1] + wid, c[2] + wid])
    p.append([c[0] + wid, c[1] - wid, c[2] + wid])
    p.append([c[0] + wid, c[1] - wid, c[2] - wid])

    return np.array(p).T

def my_calibration(sz):
    """
    Calibration function for the camera (iPhone4) used in this example.
    """
    row, col = sz
    fx = 2555*col/2592
    fy = 2586*row/1936
    K = np.diag([fx, fy, 1])
    K[0, 2] = 0.5*col
    K[1, 2] = 0.5*row
    return K


# 计算特征
sift.process_image(r'book_frontal.JPG',r'im0.sift')
l0, d0 = sift.read_features_from_file(r'im0.sift')

sift.process_image(r'book_perspective.JPG',r'im1.sift')
l1, d1 = sift.read_features_from_file(r'im1.sift')

# 匹配特征，计算单应性矩阵
matches = sift.match_twosided(d0, d1)

ndx = matches.nonzero()[0]
fp = homography.make_homog(l0[ndx, :2].T)
ndx2 = [int(matches[i]) for i in ndx]
tp = homography.make_homog(l1[ndx2, :2].T)

model = homography.RansacModel()
H ,liners = homography.H_from_ransac(fp,tp,model)

K = my_calibration((747,1000))
box = cubePoints([ 0, 0, 0.1], 0.1)
cam1 = camera.Camera(np.hstack((K,np.dot(K,np.array([[0],[0],[-1]])))))
boxCam1 = cam1.project(homography.make_homog( box[:,: 5]))
boxTrans = homography.normalize(np.dot(H,boxCam1))

cam2 = camera.Camera(np.dot(H, cam1.P))
A = np.dot(linalg.inv(K), cam2.P[:,:3])
A = np.array([A[:,0], A[:, 1],np.cross(A[:, 0],A[:, 1])]).T
cam2.P[:,:3] = np.dot(K,A)
boxCam2 = cam2.project(homography.make_homog(box))
point = np.array([1,1,0,1]).T
print(homography.normalize(np.dot(np.dot(H,cam1.P), point)))
print(cam2.project(point))



im0 = np.array(Image.open(r'book_frontal.JPG'))
im1 = np.array(Image.open(r'book_perspective.JPG'))
pl.figure()
pl.imshow(im0)
pl.plot(boxCam1[0,:],boxCam1[1,:], linewidth = 3)

pl.figure()
pl.imshow(im1)
pl.plot(boxTrans[0,:],boxTrans[1,:], linewidth = 3)

pl.figure()
pl.imshow(im1)
pl.plot(boxCam2[0,:],boxCam2[1,:], linewidth = 3)
pl.show()
```

最后的结果之一如下：

[![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9zYWl5dXdhbmctYmxvZy5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vJUU1JTlCJUJFJUU1JTgzJThGJUU1JUE0JTg0JUU3JTkwJTg2NyVFNSU5MCU4RS9TY3JlZW5zaG90JTIwZnJvbSUyMDIwMjAtMDQtMjYlMjAyMi0yMS0wMy5wbmc?x-oss-process=image/format,png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/图像处理7后/Screenshot from 2020-04-26 22-21-03.png)

## 增强现实

### 从照相机矩阵到OpenGL格式

这个用的也都是之前的知识，openGL和以往的用法有所不同，在代码中我都有所标注，大家直接越读代码即可：

```python
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
glutInit()
import pygame, pygame.image
from pygame.locals import *
import numpy as np
import pylab as pl
import pickle
from scipy import linalg
import sys
# 设置图片大小
width,height = 1000,747
def setProjectionFromCamera(K):
	"""
	从照相机标定矩阵获得视图的大小等信息，并且创建相应视图
	"""
	# 将当前的工作区域设置为GL_PROJECTION，即操作该矩阵
	glMatrixMode(GL_PROJECTION)
	# 将GL_PROJECTION归为单位矩阵
	glLoadIdentity()

	fx = K[0][0] # 获取横轴焦距
	fy = K[1][1] # 获取纵轴焦距
	# 这里的具体含义是:视角的大小，即眼睛睁开的角度
	# 如果设置为0，那么就相当于闭上了眼睛
	#　如果设置为180，就可以认为你的视野开阔
	fovy = 2 * np.arctan(0.5 * height / fy ) * 180 / np.pi

	# 就是窗口的的纵横比x/y
	aspect = (width * fy) / (height * fx)

	# 定义近的和远的剪裁平面
	near = 0.1
	far = 100.0

	# 设定透视
	# 这个函数有四个参数，他们分别代表：
	# 1. fovy ,  已经在上面介绍过了
	# 2. aspect, 这个是窗口的纵横比，也介绍了
	# 3. zNear,  近处的截面离眼睛的距离
	# 4. zFar,   远处的截面离眼睛的距离
	gluPerspective(fovy, aspect, near, far)

	# 指定了窗口的可见区域，其中前两个值代表的是窗口起点
	# 后两个参数代表窗口的宽度与高度
	glViewport(0, 0, width, height)


# 从相机姿态中获得模拟视图矩阵
def setModelviewFromCamera(Rt):
	# 这里我们将工作区切换到GL_MODELVIEW矩阵
	glMatrixMode(GL_MODELVIEW)
	# 将GL_MODELVIEW归为单位矩阵
	glLoadIdentity()

	# 创建一个90度旋转矩阵
	Rx = np.array([[1, 0, 0], [0, 0, -1], [0, 1, 0]])

	# 获得旋转的最佳逼近
	R = Rt[:, :3]
	# 矩阵撕裂
	U, S, V = linalg.svd(R)
	R = np.dot(U, V)
	R[0, :] = - R[0, :] # 改变x轴符号

	# 获得平移量
	t = Rt[: ,3]

	# 获得４*４的模拟视图矩阵
	M = np.eye(4)
	M[:3, :3] = np.dot(R, Rx)
	M[:3, 3]  = t

	# 转置并压平以获取序列数值
	M = M.T
	m = M.flatten()

	# 将模拟视图矩阵替换为新的矩阵
	glLoadMatrixf(m)


def drawBackground(imname):
	"""
	使用四边形绘制背景图像
	"""

	# 载入背景图像
	bgImage = pygame.image.load(imname).convert()
	bgData  = pygame.image.tostring(bgImage, "RGBX", 1)

	glMatrixMode(GL_MODELVIEW)
	glLoadIdentity()
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

	# 绑定纹理
	glEnable(GL_TEXTURE_2D)
	glBindTexture(GL_TEXTURE_2D, glGenTextures(1))
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bgData)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
	
	# 创建四方形填充整个窗口
	glBegin(GL_QUADS)
	glTexCoord2f(0.0, 0.0)
	glVertex3f(-1.0, -1.0, -1.0)
	glTexCoord2f(1.0, 0.0)
	glVertex3f( 1.0, -1.0, -1.0)
	glTexCoord2f(1.0, 1.0)
	glVertex3f( 1.0, 1.0, -1.0)
	glTexCoord2f(0.0, 1.0)
	glVertex3f(-1.0, 1.0, -1.0)
	glEnd()

	# 清除纹理
	glDeleteTextures(1)

def drawTeapot(size):
	"""
	在原点处绘制红色茶壶
	"""
	# glEnable 可以开启ＯｐｅｎＧＬ的一些特性
	# 这里，开启了一些ＯＰＥＮＧＬ的特性这里激活了灯光效果
	glEnable(GL_LIGHTING)
	# 这里激活了一盏灯
	glEnable(GL_LIGHT0)
	# 这里打开了深度测试，使得物体按照其深度表示出来(远处的物体不能绘制在近处物体的前面)
	glEnable(GL_DEPTH_TEST)
	# 清理深度缓存
	glClear(GL_DEPTH_BUFFER_BIT)

	# 绘制红色茶壶
	# 制定物体的物质特性，比如镜面反射颜色等
	glMaterialfv(GL_FRONT, GL_AMBIENT, [0, 0, 0, 0])
	glMaterialfv(GL_FRONT, GL_DIFFUSE, [0.5, 0.0, 0.0, 0.0])
	glMaterialfv(GL_FRONT, GL_SPECULAR, [0.7, 0.6, 0.6, 0.0])
	glMaterialf(GL_FRONT, GL_SHININESS, 0.25 * 128.0)
	# 将制定的物质特性加入到茶壶上
	glutWireTeapot(size)



# 集成

def setUp():
	"""
		设置窗口和ｐｙｇｍａｅ环境
	"""
	pygame.init()
	pygame.display.set_mode((width, height), OPENGL | DOUBLEBUF)
	pygame.display.set_caption('OpenGL AR demo')
# 载入照相机数据
with open(r'/home/wangsy/Codes/PCVLearning/myCodes/Carma/ar_camera.pkl','rb') as f:
	K = pickle.load(f)
	Rt = pickle.load(f)

setUp()
drawBackground(r'/home/wangsy/Codes/PCVLearning/myCodes/Carma/book_perspective.bmp')
setProjectionFromCamera(K)
setModelviewFromCamera(Rt)
drawTeapot(0.051)

while True:
	event = pygame.event.poll()
	if(event.type in (QUIT, KEYDOWN)):
		break
	pygame.display.flip()
```

值得注意的是：作者在创建茶壶时所使用的方法是`glutSolidTeapot(size)`这种方法在我的环境中会报错：

> freeglut ERROR: Function called without first calling ‘glutInit’.

发现该错误后，我在程序的头部加上了`glutInit()`，但是仍然报错：

> Current thread 0x00007f9484371700 (most recent call first):
> File “[AR.py](http://ar.py/)”, line 139 in drawTeapot
> File “[AR.py](http://ar.py/)”, line 161 in
> zsh: abort (core dumped) python [AR.py](http://ar.py/)

上网搜了很多方法（几乎都用了），都没有解决，最终发现，将`glutSolidTeapot(size)`方法更改为`glutWireTeapot(size)`就可以运行无误了，最终结果如下：
![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9zYWl5dXdhbmctYmxvZy5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vU2NyZWVuc2hvdCUyMGZyb20lMjAyMDIwLTA0LTI4JTIwMTAtMzUtMDYucG5n?x-oss-process=image/format,png)