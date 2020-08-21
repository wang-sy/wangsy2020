# SLAM学习笔记（二）——相机与图像



相机的实质就是将三维空间中的信息压缩到了二维平面中，这一节主要了解针孔相机模型，了解普通相机成像原理





## 理想的小孔成像模型

![](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817143935.png)

这个是在初中就学过的了，我们就不做展开了。

根据上面的相似三角形关系，就可以得到下面的一组关系：

![](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817144142.png)

### 内参

我们先来看一下这里的一个小结论吧，我觉得如果你懂得多的话，先看结论反而更容易知道他在说什么：

![](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817144754.png)

这里再说的就是，由于相机出场时有一些“误差”，导致相机直接拍摄出来的图像与“真实”的图像不同。它们之间的不同可以用这个内参矩阵$K$来进行描述，接着，这个$K$我们仔细一看，就是我们之前说的仿射变换矩阵，其中：

- $f_x$描述了x轴方向的缩放，是由镜头凸透镜不是正圆而导致的
- $f_y$描述了y轴方向的缩放，同上
- $c_x$描述了x轴方向的平移，是由镜头凸透镜的中心和镜头的中心没有对齐而导致的
- $c_y$描述了y轴方向的平移，同上

这个式子中，左侧的u,v,1是一个齐次矩阵，描述的是点在图像中的位置，同时，XYZ是非齐次坐标，描述的是点在现实中的位置，所以我们知道了内参和实际距离，实际上是可以刻画出点在现实中的位置的。

### 外参

当然，除了内参之外，摄像机本身也有自己的位置和角度，我们用外参矩阵来对他进行描述。

我们在学习上一节的时候就知道，可以用变换矩阵来描述旋转+平移，也就是下面的形式：

![](https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817145714.png)

可以看到，K(RP+t)这里的K就是内参矩阵，在此之前，我们先将现实中的点，通过R旋转，t平移，将其变换到摄像机坐标系中，然后再乘上一个内参矩阵，将其变化为图像坐标系。当然，在运算的过程中，隐含了一次齐次变换，变换的具体内容就是：
$$
Pw = [X,Y,Z] --> Pw=[X,Y,Z,1]
$$
变换的目的就是为了让他能和4*4的变换矩阵来进行矩阵乘法运算而已，这里这个变换矩阵就被我们称为外参，外参是跟随摄像机的位置、角度变化而不断变化的，也是SLAM估计的目标，拿一张图来概括这个过程就是：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817150218.png" style="zoom:67%;" />



### 畸变

广角和鱼眼的镜头会带来较大的畸变，我们可以简单研究以下两种常见的畸变（但是我的项目好像用不到，我随便一学就好啦）：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817150541.png" style="zoom:67%;" />

 

可以用这样的模型来描述径向变：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817150704.png" style="zoom: 150%;" />

然后还有另一个式子来描述切向畸变：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817150833.png" style="zoom: 50%;" />

也可以把他俩加一块，来一起描述：

<img src="https://saiyu-wang-blog.oss-cn-shenzhen.aliyuncs.com/20200817150926.png" style="zoom:50%;" />



这些东西在买相机的时候都会给出，我们用的是普通的双目（暂定的是ZED），应该暂时用不到它。



## 程序



高博给出第一个程序就是用opencv打开并且查看一些图片信息，我们就不多提了：

```C++
#include <iostream>
#include <chrono>

using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char **argv) {
  // 读取argv[1]指定的图像
  cv::Mat image;
  image = cv::imread(argv[1]); //cv::imread函数读取指定路径下的图像

  // 判断图像文件是否正确读取
  if (image.data == nullptr) { //数据不存在,可能是文件不存在
    cerr << "文件" << argv[1] << "不存在." << endl;
    return 0;
  }

  // 文件顺利读取, 首先输出一些基本信息
  cout << "图像宽为" << image.cols << ",高为" << image.rows << ",通道数为" << image.channels() << endl;
  cv::imshow("image", image);      // 用cv::imshow显示图像
  cv::waitKey(0);                  // 暂停程序,等待一个按键输入

  // 判断image的类型
  if (image.type() != CV_8UC1 && image.type() != CV_8UC3) {
    // 图像类型不符合要求
    cout << "请输入一张彩色图或灰度图." << endl;
    return 0;
  }

  // 遍历图像, 请注意以下遍历方式亦可使用于随机像素访问
  // 使用 std::chrono 来给算法计时
  chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
  for (size_t y = 0; y < image.rows; y++) {
    // 用cv::Mat::ptr获得图像的行指针
    unsigned char *row_ptr = image.ptr<unsigned char>(y);  // row_ptr是第y行的头指针
    for (size_t x = 0; x < image.cols; x++) {
      // 访问位于 x,y 处的像素
      unsigned char *data_ptr = &row_ptr[x * image.channels()]; // data_ptr 指向待访问的像素数据
      // 输出该像素的每个通道,如果是灰度图就只有一个通道
      for (int c = 0; c != image.channels(); c++) {
        unsigned char data = data_ptr[c]; // data为I(x,y)第c个通道的值
      }
    }
  }
  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  chrono::duration<double> time_used = chrono::duration_cast < chrono::duration < double >> (t2 - t1);
  cout << "遍历图像用时：" << time_used.count() << " 秒。" << endl;

  // 关于 cv::Mat 的拷贝
  // 直接赋值并不会拷贝数据
  cv::Mat image_another = image;
  // 修改 image_another 会导致 image 发生变化
  image_another(cv::Rect(0, 0, 100, 100)).setTo(0); // 将左上角100*100的块置零
  cv::imshow("image", image);
  cv::waitKey(0);

  // 使用clone函数来拷贝数据
  cv::Mat image_clone = image.clone();
  image_clone(cv::Rect(0, 0, 100, 100)).setTo(255);
  cv::imshow("image", image);
  cv::imshow("image_clone", image_clone);
  cv::waitKey(0);

  // 对于图像还有很多基本的操作,如剪切,旋转,缩放等,限于篇幅就不一一介绍了,请参看OpenCV官方文档查询每个函数的调用方法.
  cv::destroyAllWindows();
  return 0;
}

```



第二个程序就是去畸变的：

```C++
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;

string image_file = "./distorted.png";   // 请确保路径正确

int main(int argc, char **argv) {

  // 本程序实现去畸变部分的代码。尽管我们可以调用OpenCV的去畸变，但自己实现一遍有助于理解。
  // 畸变参数
  double k1 = -0.28340811, k2 = 0.07395907, p1 = 0.00019359, p2 = 1.76187114e-05;
  // 内参
  double fx = 458.654, fy = 457.296, cx = 367.215, cy = 248.375;

  cv::Mat image = cv::imread(image_file, 0);   // 图像是灰度图，CV_8UC1
  int rows = image.rows, cols = image.cols;
  cv::Mat image_undistort = cv::Mat(rows, cols, CV_8UC1);   // 去畸变以后的图

  // 计算去畸变后图像的内容
  for (int v = 0; v < rows; v++) {
    for (int u = 0; u < cols; u++) {
      // 按照公式，计算点(u,v)对应到畸变图像中的坐标(u_distorted, v_distorted)
      double x = (u - cx) / fx, y = (v - cy) / fy;
      double r = sqrt(x * x + y * y);
      double x_distorted = x * (1 + k1 * r * r + k2 * r * r * r * r) + 2 * p1 * x * y + p2 * (r * r + 2 * x * x);
      double y_distorted = y * (1 + k1 * r * r + k2 * r * r * r * r) + p1 * (r * r + 2 * y * y) + 2 * p2 * x * y;
      double u_distorted = fx * x_distorted + cx;
      double v_distorted = fy * y_distorted + cy;

      // 赋值 (最近邻插值)
      if (u_distorted >= 0 && v_distorted >= 0 && u_distorted < cols && v_distorted < rows) {
        image_undistort.at<uchar>(v, u) = image.at<uchar>((int) v_distorted, (int) u_distorted);
      } else {
        image_undistort.at<uchar>(v, u) = 0;
      }
    }
  }

  // 画图去畸变后图像
  cv::imshow("distorted", image);
  cv::imshow("undistorted", image_undistort);
  cv::waitKey();
  return 0;
}
```

这里他已知了相机的参数矩阵以及畸变的参数，然后来去畸变。

- 对于每一个点，先计算他对应的x，y，这里的x，y是通过相机的内参进行计算的
- 然后再使用畸变公式进行计算
- 最后反着用内参的公式对他进行还原即可

通过这一过程，我们可以认识到：畸变公式描述的是照相机坐标系下的坐标的畸变





第三个叫双目视觉

```C++
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <pangolin/pangolin.h>
#include <unistd.h>

using namespace std;
using namespace Eigen;

// 文件路径
string left_file = "./left.png";
string right_file = "./right.png";

// 在pangolin中画图，已写好，无需调整
void showPointCloud(
    const vector<Vector4d, Eigen::aligned_allocator<Vector4d>> &pointcloud);

int main(int argc, char **argv) {

    // 内参
    double fx = 718.856, fy = 718.856, cx = 607.1928, cy = 185.2157;
    // 基线
    double b = 0.573;

    // 读取图像
    cv::Mat left = cv::imread(left_file, 0);
    cv::Mat right = cv::imread(right_file, 0);
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(
        0, 96, 9, 8 * 9 * 9, 32 * 9 * 9, 1, 63, 10, 100, 32);    // 神奇的参数
    cv::Mat disparity_sgbm, disparity;
    sgbm->compute(left, right, disparity_sgbm);
    disparity_sgbm.convertTo(disparity, CV_32F, 1.0 / 16.0f);

    // 生成点云
    vector<Vector4d, Eigen::aligned_allocator<Vector4d>> pointcloud;

    // 如果你的机器慢，请把后面的v++和u++改成v+=2, u+=2
    for (int v = 0; v < left.rows; v++)
        for (int u = 0; u < left.cols; u++) {
            if (disparity.at<float>(v, u) <= 0.0 || disparity.at<float>(v, u) >= 96.0) continue;

            Vector4d point(0, 0, 0, left.at<uchar>(v, u) / 255.0); // 前三维为xyz,第四维为颜色

            // 根据双目模型计算 point 的位置
            double x = (u - cx) / fx;
            double y = (v - cy) / fy;
            double depth = fx * b / (disparity.at<float>(v, u));
            point[0] = x * depth;
            point[1] = y * depth;
            point[2] = depth;

            pointcloud.push_back(point);
        }

    cv::imshow("disparity", disparity / 96.0);
    cv::waitKey(0);
    // 画出点云
    showPointCloud(pointcloud);
    return 0;
}

void showPointCloud(const vector<Vector4d, Eigen::aligned_allocator<Vector4d>> &pointcloud) {

    if (pointcloud.empty()) {
        cerr << "Point cloud is empty!" << endl;
        return;
    }

    pangolin::CreateWindowAndBind("Point Cloud Viewer", 1024, 768);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
    );

    pangolin::View &d_cam = pangolin::CreateDisplay()
        .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f / 768.0f)
        .SetHandler(new pangolin::Handler3D(s_cam));

    while (pangolin::ShouldQuit() == false) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        d_cam.Activate(s_cam);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        glPointSize(2);
        glBegin(GL_POINTS);
        for (auto &p: pointcloud) {
            glColor3f(p[3], p[3], p[3]);
            glVertex3d(p[0], p[1], p[2]);
        }
        glEnd();
        pangolin::FinishFrame();
        usleep(5000);   // sleep 5 ms
    }
    return;
}
```

他这个实际上就是引入两个图象之后，自己调用了一个opencv自带的SGBM也就是半全局的立体匹配，然后做了一个立体匹配，相当于是获得了一个深度图，然后再根据深度图就可以知道我们前面公式里的Z，然后又知道相机的一些参数，就可以把坐标投影到三维空间中了。

（我们后面会出专门的系列，来讨论类似于SGBM的算法）