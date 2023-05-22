# MyCalender

C++课程设计，设计了一个精美的CPP+Qt日历，非常先进。

## 构建需求

- Qt 5.15.3(需要组件 Core Gui Widgets Network Svg)
- C++ 17
- CMake 3.5

## 构建步骤

```bash 
git clone https://github.com/WolalaQAQ/MyCalender.git
cd MyCalender
mkdir build && cd build
cmake ..
make
```

## 使用API

- [和风天气API](https://dev.qweather.com/)
- [高德地图API](https://lbs.amap.com/)

## 使用素材

- [和风天气开源图标字体库](https://icons.qweather.com/)

## TODO

- [x] 日程排序
- [x] 按照日程时间搜索
- [x] 日程显示可能溢出
- [x] 删除日程