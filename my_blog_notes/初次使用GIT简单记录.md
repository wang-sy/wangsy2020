# 初次使用GIT简单记录

之前一直听说过git，但是一直没有直接使用git来管理自己的代码，现在随着需要coding的科目的增多，再加上自己有了一台阿里云服务器，于是想到使用远程git来管理自己的代码，于是就有了这篇博客，最近事情有点多，实在没时间更新图形学的博客了，等期末考试结束了，我必更新。

本文内容主要参考：[git - 简易指南](https://www.bootcss.com/p/git-guide/)

上面这篇文章，看起来非常舒服，比我写的好多了，推荐大家直接看上面这个，下面些属于我自己探索路上写的一些笔记。

## git的安装

以我为例，我的环境是本地`deepinv20`，服务器`centos7`，安装命令如下（deepin和debian系的都一样）：

```
deepinv20
sudo apt-get install git
centos7
sudo yum install git
```

## 简单使用

在服务器端新建文件夹：

```
mkdir ~\test.git
```

进入该文件夹，并且执行`git init`，这一步中我们新建了一个仓库。

接着，我们就可以从本地下载该仓库了

```
git clone git@xxx.xxx.xxx.xxx:/home/git/test.git
```

> 正克隆到 ‘test’…
> [git@xxx.xxx.xxx.xxx](mailto:git@xxx.xxx.xxx.xxx) password:
> warning: 您似乎克隆了一个空仓库。

这样，我们就可以把我们的仓库克隆下来了。

## 工作流

git的工作流由三个部分组成：

- 工作目录：实际文件
- 缓存区：临时保存我的改动
- HEAD：指向我最近一次提交后的结果

工作目录中的实际文件通过`add`方法加入到缓存区，最后通过`commit`方法实际提交改动，这两个方法的使用方法如下：

首先我们新建一个文本文件:`touch test.py`，并对其进行简单的编辑。

随后使用`git add test.py`指令，将改动添加到缓存区。

再使用`git commit -m "create a python file."`来将更改提交到HEAD。

最终，使用`git push origin master`提交到远端

如果你在这一步出错，你可以尝试在远端仓库中使用：`git config receive.denyCurrentBranch ignore `来允许push。

## 分支

我们之前使用的指令：`git push origin master`指的是将master推送到远端，这里的master指的是默认的分支即`主分支`。我们可以新建一个分支，并在该分支进行开发，完成后再将其合并到主分支上。



[![img](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/截图录屏_选择区域_20200528085251.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/截图录屏_选择区域_20200528085251.png)

首先，我们创建一个叫做feature_x的分支，并且切换过去：`git checkout -b feature_x`

> 切换到一个新分支 ‘feature_x’

我们对他进行一些更改，并且进行更改提交：

```
touch temp2.py
git add temp2.py
git commit -m "add a python file for feature_x"
```

> [feature_x b711439] add a python file for feature_x
> 1 file changed, 0 insertions(+), 0 deletions(-)
> create mode 100644 [temp2.py](http://temp2.py/)

接下来，我们将该分支提交到远端。`git push origin feature_x`

我们在远端进行查看：

首先我们切换到主分支进行查看：

```
git checkout master
ls
```

返回的结果如下：

> [README.md](http://readme.md/) [temp.py](http://temp.py/)

这是第一次我创建的文件

切换到分支进行查看：

```
git checkout feature_x
ls
```

> [README.md](http://readme.md/) [temp2.py](http://temp2.py/) [temp.py](http://temp.py/)

这里就有我们刚才对这个分支的修改。

接下来，我们将feature_x分支合并到master分支上，这里先使用`git checkout master`切换到主分支上，然后`git merge feature_x `，就可以将刚才分支上的改动合并到主分支上了，这时我们使用`dir`指令对当前文件夹进行查看：

> [README.md](http://readme.md/) [temp2.py](http://temp2.py/) [temp.py](http://temp.py/)

说明合并正常。但是这时又有一个新的问题：`feature_x`分支我已經不需要了，如何把它删掉呢？其实很简单：`git branch -d feature_x`，就可以了。当然，在合并之前，可以使用指令`git diff master feature_x`就可以查看两条分支的不同

## 其他

### 添加标签

```
git tag 1.0.0 1b2e1d63ff
```

后面这个十六进制数字，可以通过`git log`语句获得已经使用的编码，我们需要保证后方的编码与已有的不同。当然，也可以通过`git tag 1.0.0 `语句直接添加这个tag

### 替换本地改动

如果我们对本地文件不慎进行了错误的修改，可以通过`git checkout -- <filename>`从远端下载并覆盖本地文件。比如，我将本地的temp.py进行了修改后，再使用`git checkout temp.py`就可以从远端下载`temp.py`文件，并且对其进行覆盖。