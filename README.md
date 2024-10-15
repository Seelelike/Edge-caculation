# 魔棒游戏系统

## 采集动作数据
1. 打开 `magic_wand_train_data_collect/magic_wand_train_data_collect.ino`文件烧录到Arduino板子上。

 每隔5秒采样一次。在led灯闪烁2次后，开始采样，采样时间为3秒，采样率为50Hz。

2. 打开`train_data_collect.py`文件,该文件可以将采集到的数据保存为txt文件，保存数据在`train_data`文件夹下。

3. 采样多次后，按下Arduino板子上的RESET按钮,`.py`文件终止运行，采样停止。

4. 采样后的文件检查第一组数据和最后一组数据，检查数据是否满150个，没有则删除这组数据。打开生成的`.txt`文件

+ 可能每行数据间有空行，需要运行`train_data/txt_2_csv.py`文件，删除空行(也不是一定需要删除空行)`需要修改filepath变量为需要处理的文件路径`

+ 将处理后的文件后缀修改为`.csv`文件

5. 处理后得到多个`.csv`文件,每个文件的名称命名为对应的动作名称，即完成采集动作数据。

## 训练模型
1. 打开google colab，打开`action_recognition.ipynb`文件，连接到google drive，将训练数据上传到google drive，即可进行模型训练。
2. 训练完成后，将生成的文件下载到本地。即完成训练模型。

## 部署到本地
1. 打开`agic_model_test.ino`文件,将下载好的模型文件数组中的数据全部复制替换gesture_model.cpp文件中的数组。
2. 烧录到Arduino板子上。在led灯闪烁2次后，开始动作识别，识别时间为3秒，识别率为50Hz，识别结果在串口输出，输出形式为独热编码。