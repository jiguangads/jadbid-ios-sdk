# JAdbid iOS SDK 二进制分发仓库

本仓库的 `dev` 分支是 **1.0.0 公开候选包**，包含 Release 构型的 `JAdbidCore.xcframework`、11 个广告源 Adapter、资源、隐私清单，以及 CocoaPods 和 Swift Package Manager 描述文件。源码与内部 Demo 不在本仓库；对外集成说明和 Objective-C / Swift Demo 见 [iOS Guides](https://github.com/jiguangads/Adbid-SDK-iOS-Guides)。

> 当前不是正式发行版：候选包的 `release_ready` 为 `false`，发行许可条款及最终真机广告验收尚待确认。本分支不代表已发布 `1.0.0` Tag 或已提交 CocoaPods Trunk；不要把 `dev` 当作稳定版本用于生产。

当前候选 **build 29** 从干净源码提交 `d059bc3e89be2010ac3222b7c7700a82e29895de` 构建，SDK 版本仍为 `1.0.0`。Debug/Release 候选已通过 12 模块、双切片、隐私/资源和构建身份完整性校验；本仓库只提供 Release 构型。有限身份记录见 `BuildIdentity.json`，其中 `dirty=false`、`build=29`；`source_digest` 用于对应构建内容，不包含私有仓库地址或设备/账号数据。

本次候选包含加载中不提前报失败的修复与安全构建身份诊断。SDK普通日志在Debug或显式开启`debugMode`时可见，Release默认关闭普通事件；初始化身份只输出一次。Demo控制台事件和Debug开关由[Guides](https://github.com/jiguangads/Adbid-SDK-iOS-Guides)同步，筛选`[JAdbid]`与`[JAdbidDemo]`可区分来源；不输出原始厂商错误字典或设备标识。

本次公开Podspec/SPM描述解析、资源与Core身份校验均通过；两语言远端安装/构建结果以Guides内固定的SDK公开提交及其验证说明为准，不能把可变`dev`分支当作已锁定版本。完整CocoaPods lint的AdMob链接问题、全厂商SPM完整消费和真机广告验收仍未通过最终验收。

## CocoaPods 开发集成

```ruby
platform :ios, '15.0'

target 'YourApp' do
  pod 'JAdbidSDK/Core', :git => 'https://github.com/jiguangads/jadbid-ios-sdk.git', :branch => 'dev'
  # 按需添加一个或多个广告源：
  # pod 'JAdbidSDK/AdMob', :git => 'https://github.com/jiguangads/jadbid-ios-sdk.git', :branch => 'dev'
end
```

Podspec 通过 subspec 声明所选广告源的厂商 SDK 精确依赖。默认仅使用 Core；不要在正式应用中照搬所有 11 个源。正式版发布前，Podspec 的 `source` 将从 `dev` 分支改为不可变版本 Tag，并独立验证 CocoaPods Trunk 可解析性。

## Swift Package Manager 开发集成

在 Xcode 添加 `https://github.com/jiguangads/jadbid-ios-sdk.git`，选择 `dev` 分支及 `JAdbidCore` 或所需的 `JAdbid<Source>` 产品。当前 `Package.swift` 为 12 个本地 XCFramework 声明 binary target，**不自动安装厂商 SDK**；宿主须按选中广告源补齐对应厂商依赖及其传递依赖。

SPM 与手动集成还须在签名前把 `Resources/TradPlusAds.bundle` 和 `Resources/TradPlusADX.bundle` 复制到宿主主 Bundle；仓库内的 `copy_resources.py` 可用于该步骤。JAdbid 静态 XCFramework 只链接，不作为动态框架嵌入。宿主仍需检查最终 App 的隐私清单和申报。

## 内容与边界

- `Artifacts/`：Release 构型的 Core + 11 个可选 Adapter XCFramework。
- `JAdbidSDK.podspec`：CocoaPods Core 与广告源 subspec。
- `Package.swift`、`Support/`：SPM 二进制产品与系统链接设置。
- `Resources/`、`PrivacyInfo.xcprivacy`、`Licenses/`：资源、首方隐私清单及保留的第三方许可。
- `BuildIdentity.json`：与上传Core对应的有限Release构建身份；完整内部构建清单和源码不公开。

最低支持 iOS 15；设备与模拟器均提供 arm64 切片。构建或依赖解析成功不等于广告填充、展示、曝光和回调通过真机验收。
