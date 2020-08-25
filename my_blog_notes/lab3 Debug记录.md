# lab3 Debug记录

错误类型

[![image-20200602013703740](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602013550.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602013550.png)

其他的暂时没有看出异常，但是最后进行sw时出现了zzz，遂调出所有关于regfile的信号进行观察。

相关指令如下，逐条进行纠错：

## 使用测试的指令

```
.data
   a:  .word  3
.text  0x0000
main: 
	addi $2, $0, 5 		# initialize $2=5 		0  20020005
	addi $3, $0, 12 	# initialize $3=12 		4  2003000c
	addi $7, $3,-9 		# initialize $7=3 		8  2067fff7
	or $4, $7, $2 		# $4 <= 3 or 5 = 7 		c  00e22025
	and $5, $3, $4 		# $5 <= 12 and 7 = 4 	10 00642824
	add $5, $5, $4 		# $5 = 4 + 7 = 11 		14 00a42820
	beq $5, $7, end 	# shouldn't be taken 	18 10a7000c
	slt $4, $3, $4 		# $4 = 12 < 7 = 0 		1c 0064202a
	beq $4, $0, around 	# should be taken 		20 10800002
	nop					# empty inst			24 00000000
	addi $5, $0, 0 		# shouldn't happen 		28 20050000
around: 
	slt $4, $7, $2 		# $4 = 3 < 5 = 1 		2c 00e2202a
	add $7, $4, $5 		# $7 = 1 + 11 = 12 		30 00853820
	sub $7, $7, $2 		# $7 = 12 - 5 = 7 		34 00e23822
	sw $7, 68($3) 		# [80] = 7 				38 ac670044
	lw $2, 80($0) 		# $2 = [80] = 7 		3c 8c020050
	j end 				# should be taken 		40 08000013
	nop					# empty inst			44 00000000
	addi $2, $0, 1 		# shouldn't happen 		48 20020001
end: 
	sw $2, 84($0)		# write adr 84=7 		4c ac020054
```

## 检查所有取出指令

首先按照程序执行顺序，将执行的指令顺序列出

```
addi $2, $0, 5 		# initialize $2=5 		0  20020005
addi $3, $0, 12 	# initialize $3=12 		4  2003000c
addi $7, $3,-9 		# initialize $7=3 		8  2067fff7
or $4, $7, $2 		# $4 <= 3 or 5 = 7 		c  00e22025
and $5, $3, $4 		# $5 <= 12 and 7 = 4 	10 00642824
add $5, $5, $4 		# $5 = 4 + 7 = 11 		14 00a42820
beq $5, $7, end 	# shouldn't be taken 	18 10a7000c
slt $4, $3, $4 		# $4 = 12 < 7 = 0 		1c 0064202a
beq $4, $0, around 	# should be taken 		20 10800001
slt $4, $7, $2 		# $4 = 3 < 5 = 1 		2c 00e2202a
add $7, $4, $5 		# $7 = 1 + 11 = 12 		30 00853820
sub $7, $7, $2 		# $7 = 12 - 5 = 7 		34 00e23822
sw $7, 68($3) 		# [80] = 7 				38 ac670044
lw $2, 80($0) 		# $2 = [80] = 7 		3c 8c020050
j end 				# should be taken 		40 08000013
sw $2, 84($0)		# write adr 84=7 		4c ac020054
```

每个时钟周期对应的机器码无误。

## 对比从reg中取址位置的信号-instr_look_a和b

| 顺序执行的指令   | 应当对应的a | 应当对应的b |
| :--------------- | :---------- | :---------- |
| addi 2, 0, 5     | 0           | 无所谓      |
| addi 3,0, 12     | 0           | 无所谓      |
| addi 7,3,-9      | 3           | 无所谓      |
| or 4,7, $2       | 7           | 2           |
| and 5, 3, $4     | 3           | 4           |
| add 、5,5, $4    | 5           | 4           |
| beq 5,7, end     | 5           | 7           |
| slt 4, 3, $4     | 3           | 4           |
| beq 4, 0, around | 4           | 0           |
| slt 4, 7, $2     | 7           | 2           |
| add 7, 4, $5     | 4           | 5           |
| sub 7, 7, $2     | 7           | 2           |
| sw 7, 68(3)      | 3           | 无所谓      |
| lw 2, 80(0)      | 0           | 无所谓      |
| j end            | 无所谓      | 无所谓      |
| sw 2, 84(0)      | 0           | 2           |

这里单从信号来看没问题，但我注意到一个严重的事情，那就是：

[![image-20200602020820023](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602020818.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602020818.png)

在执行lw指令的时候，00处的时候出现了高阻，这一步的目的是从data_mem中读取80位置的数据并填写到2号寄存器。我们一开始检测的目标是：`sw $2, 84($0)`语句时，也就是将$2中的内容写到data_mem中的时候，写信号的内容为高阻。现在我们找到了导致他的原因，那就是在从data_mem中读取时，就是高阻。于是现在问题转移为**研究为什么会在读取时出现高阻**，最可能的原因是：没有对blockMem进行写操作。

于是开始检查对blockMemory的操作：

## 检查reg的输出信号以及memwrite（写入dataMem的失能信号）

检查data_mem的使用情况：

[![image-20200602022125514](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_Vivado_20200530231359.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_Vivado_20200530231359.png)

还有另外两页就不放了，所有的勾勾打的都很正确

检查后发现：data_mem 的ip核调用有问题，我是这么吊用的：

```
data_ram dmem(~clk,memwrite,dataadr,writedata,readdata);
```

错误原因：ip核调用错误！！！

## 开始解决问题

这问题很弱智，就是把这些信号规整一下，然后按照ip核的要求填进去就可以了。

主要问题出在：

- 缺少写的目标
- 缺少写入的目标

解决方法：网上找不到资源只能看vivado自带的文档了。（不会吧不会吧，用了一年了不会有人不知道vivado有文档吧），打开方法：

[![image-20200602022559688](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_Vivado_20200602022553.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_Vivado_20200602022553.png)

点红色圈圈，然后选productGuide。

他里面还介绍了AXI4的详细握手过程，可惜现在已经凌晨两点半了，我没大有心情看，从这个报告里面，我知道了：

[![image-20200602023815294](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602023810.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602023810.png)

- addra：寻址端口A读和写操作的存储空间
- clka：时钟信号
- dina：通过端口A将数据输入写入存储器的数据
- douta：通过端口A的读取操作输出的数据
- ena：通过端口A启用读取，写入和重置操作
- wea：通过端口A启用写入操作

感觉看起来ena像是一个电源开关，wea像是控制能否进行写入的信号。但是为什么wea是四位的，这个还不大清楚，只好再往下找。终于，我在下面的一组仿真图上面找到了答案，这里他用的是和我们一样的single-port-ram，仿真图如下：

[![image-20200602024239273](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602024209.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602024209.png)

[![image-20200602024448394](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602024444.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602024444.png)

结合他的文档理解后，得知：wea分为四位，是因为我们写的数据长是32位，wea分别控制32位中的每个字节是否写。由于我们一次写四位，所以正确的做法是：让ena一直为1，将memwrite通过复制的方法拓展为四位信号，然后当memwrite为1时，直接写入，当memwrite为0时候，就不写入。

## 结果&心得

[![image-20200602025036282](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602025035.png)](https://saiyuwang-blog.oss-cn-beijing.aliyuncs.com/tempasd/截图录屏_选择区域_20200602025035.png)

仿真成功，测试结果正确。

### 心得

这个实验前后废了我差不多48小时，其实原理早就懂得不得了了。万万没想到是因为这么个弱智错误，产生错误的原因是：我copy了学长的顶层代码，但是没有选用和他一样的ip核（也有可能是学长的也错了嘿嘿）。主要get到两点：

- debug的时候从代码角度分析问题的次数不要太多，这10多个文件，我前后检查了五遍，一点问题都没有，基本上每次想起来都会检查一遍，从来没检查出过错。
- 从错误发生的地方，向外剖分，一层一层往外剖，总能找到错的，这样比浮躁的检查代码要扎实的多，正验了那句古话**欲速则不达**。