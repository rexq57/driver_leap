## 中文说明：
该项目只匹配leap motion模拟的Index控制器
1、下载驱动 https://developer.leapmotion.com/sdk-leap-motion-controller
2、下载leap插件放到steamvr的drivers目录(在库中右键SteamVR浏览本地文件) https://github.com/rexq57/driver_leap/releases

3、修改以下文件

[Steam]\steamapps\common\SteamVR\resources\settings\default.vrsettings

将activateMultipleDrivers的值改为true



## 操作：
* 【菜单】 双手食指相碰
* 【扳机】 手枪手势，食指扣下
* 【B键】 食指、拇指粘合
* 【A键】 小指、拇指粘合
* 【触摸板】主手手枪，副手握拳，副手前后左右移动控制触摸区域，主手扣动扳机表示按下触摸板
* 【摇杆】双手手枪，进入双手摇杆模式，前后移动来控制独立摇杆，可退出任意一只手的状态

注意事项：

手掌张开，表示手势复位。

########################################################

# Driver Leap [![Build status](https://ci.appveyor.com/api/projects/status/2pc49d2hpt2hx944?svg=true)](https://ci.appveyor.com/project/SDraw/driver-leap) [![Release](http://img.shields.io/github/release/SDraw/driver_leap.svg)](../../releases/latest)

Fork with updated vendor libraries.

## Installation (for users)
* Choose one of these options:
  * **For Windows 7, 8, 8.1 and 10:** Install [Orion (v4)](https://developer.leapmotion.com/sdk-leap-motion-controller)
  * **For Windows 10 only:** Install [Gemini (v5)](https://developer.leapmotion.com/gemini-v5-preview)
* Extract [latest release archive](../../releases/latest) for your platform to `<SteamVR_folder>/drivers`
* Add line in section `steamvr` of `<Steam_folder>/config/steamvr.vrsettings` file:
```JSON
"activateMultipleDrivers": true,
```

## Usage
### Settings
Driver settings are configurated by editing `resources/settings.xml`. Available settings:
* `emulatedController`: emulated controllers type. Can be `vive`, `index` or `oculus`. `index` by default.
* `rightHand/leftHand`: sets enabling of virtual controller for specific hand. `true` by default.
* `orientation`: Leap Motion controller mouting type. Can be `hmd` or `desktop`. `hmd` by default.
* `trackingLevel`: skeleton tracking style for OpenVR. Can be `partial` or `full`. `partial` by default.
* `desktopOffset`: global position offset from head in `desktop` orientation.
* `leftHandOffset/rightHandOffset`: local offset position for specific hand controller.
* `leftHandOffsetRotation/rightHandOffsetRotation`: local offset rotation for specific hand controller.
* `handsReset`: marks controllers as out of range if hand for controller isn't detected by Leap Motion. `false` by default.
* `interpolation`: enables internal Leap Motion data capture interpolation. `false` by default.
* `useVelocity`: enables velocity data from Leap Motion for hands. `false` by default.

### Gestures
List of hands gestures that are used in tracking:
* **Grab:** bending of middle, ring and pinky fingers.
* **Trigger:** bending of index finger.
* **Thumb press:** touching of middle segment of index finger by thumb.
* **Opisthenar touch:** tounching of opisthenar by index finger of opposite hand.
* **Palm touch:** touching of palm by index finger of opposite hand.
* **Thumb touch:** touching of thumb finger by index finger of opposite hand.
* **Middle touch:** touching of middle finger by index finger of opposite hand.
* **Palm UV:** pointing index finger of opposite hand to palm.

### Input list
#### Vive wand emulation
* **System** -> **Opisthenar touch**
* **Menu** -> **Palm touch**
* **Grip** -> **Grab**
* **Trigger** -> **Trigger**
* **Touchpad touch** -> slight **Thumb press**
* **Touchpad press** -> full **Thumb press**
* **Touchpad XY** -> **Palm UV**

#### Index controller emulation
* **System** -> **Opisthenar touch**
* **Grip** -> **Grab**
* **Trigger** -> **Trigger**
* **A** -> **Palm touch**
* **B** -> **Middle touch**
* **Touchpad touch** -> slight **Thumb press**
* **Touchpad press** -> full **Thumb press**
* **Touchpad XY** -> **Palm UV**
* **Thumbstick** -> **Thumb touch**
* **Thumbstick XY** -> Not implemented due to lack of free gestures

#### Oculus Touch emulation
* **System** -> **Opisthenar touch**
* **Trigger** -> **Trigger**
* **Grip** -> **Grab**
* **X/A** -> **Middle touch**
* **Y/B** -> **Palm touch**
* **Thumbstick touch** -> slight **Thumb press**
* **Thumbstick press** -> full **Thumb press**
* **Thumbstick XY** -> **Palm UV**

## Notes
Currently there is [a strange behaviour of tracking problems that affect AMD and few Intel systems](../../issues/56). If you're encountering with tracking problems, it's adviced to build driver on your system. Refer to **Building** section below.

## Building (for developers)
* Clone repository with `git`:
```
git clone https://github.com/SDraw/driver_leap.git
```
* Initialize submodules:
```
cd driver_leap
git submodule update --init --depth=1
```
* Open `driver_leap.sln` solution in Visual Studio (2013 and up)
* Build your platform:
  * x64 - build output is in `bin/win64`
  * x86 - build output is in `bin/win32`
* Copy build files to `<SteamVR_folder>/drivers/leap/bin/<your_platform>`:
  * `driver_leap.dll`
  * `leap_monitor.exe`  
  **Note:** There are post-build events for projects to copy build files directly to SteamVR driver folder that can be enabled manually.
* Copy additional shared libraries to `<SteamVR_folder>/drivers/leap/bin/<your_platform>`:
  * `vendor/LeapSDK/bin/<your_platform>/LeapC.dll`
  * `vendor/openvr/bin/<your_platform>/openvr_api.dll`
* Copy `resources` folder from solution root to `<SteamVR_folder>/drivers/leap`. 
