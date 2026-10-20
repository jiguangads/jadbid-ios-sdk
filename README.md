# JAdbid iOS SDK 二进制分发仓库

**jadbid-1.0.0-beta** 使用 SwiftPM Tag `1.0.0-beta`，包含 Release 构型的 `JAdbidCore.xcframework`、11 个广告源 Adapter、资源、隐私清单和 Swift Package 描述文件。源码与内部 Demo 不在本仓库；对外集成说明和 Objective-C / Swift Demo 见 [iOS Guides](https://github.com/jiguangads/Adbid-SDK-iOS-Guides)。

> 本次 beta 仅通过 SwiftPM 提供；不提交 CocoaPods Trunk。仓内 Podspec 仅保留为与 Tag 对齐的非发布元数据。JAdbid 静态 XCFramework 由宿主链接，不作为动态框架嵌入。

本版取自 Jenkins `jadbid-sdk-local` **build 2**，干净源码提交 `0947b737f05a11a50d84ee1b06f1b6140d3e3528`，原始 ZIP SHA-256 为 `dcdb2fcfe1fba2da2a71dd4655a1e2997dbd677d0a1ba764739be5be512f07fd`。包内 SDK 版本仍为 `1.0.0`，beta 是分发 Tag 而非重新编译的 SDK 版本。有限身份记录见 `BuildIdentity.json`，其中 `dirty=false`、`build=2`。本仓库只提供 Release 构型。

本次候选包含加载中不提前报失败的修复与安全构建身份诊断。SDK普通日志在Debug或显式开启`debugMode`时可见，Release默认关闭普通事件；初始化身份只输出一次。Demo控制台事件和Debug开关由[Guides](https://github.com/jiguangads/Adbid-SDK-iOS-Guides)同步，筛选`[JAdbid]`与`[JAdbidDemo]`可区分来源；不输出原始厂商错误字典或设备标识。

11 源按 Objective-C、Swift 各 11 个单源宿主执行 Release App 构建与最终资源检查；这不等同于真实广告填充、点击验收，也不代表同一 App 多源同装已获支持。

## CocoaPods 状态（本次 beta 不发布）

```ruby
platform :ios, '15.0'

target 'YourApp' do
  pod 'JAdbidSDK/Core', :git => 'https://github.com/jiguangads/jadbid-ios-sdk.git', :commit => 'ae9cabf2dae1a389cc7943d149011bfbce3bbcb5'
  # 按需添加一个或多个广告源：
  # pod 'JAdbidSDK/AdMob', :git => 'https://github.com/jiguangads/jadbid-ios-sdk.git', :commit => 'ae9cabf2dae1a389cc7943d149011bfbce3bbcb5'
end
```

上述是当前在线旧提交的历史联调入口，不是本次 beta 包。Podspec 通过 subspec 声明厂商 SDK 依赖，但本次不进行 Trunk 发布、完整 lint 或 beta CocoaPods 验收。

## Swift Package Manager 开发集成

在 Xcode 添加 `https://github.com/jiguangads/jadbid-ios-sdk.git`，选择精确版本 `1.0.0-beta` 及 `JAdbidCore` 或一个待测 `JAdbid<Source>` 产品。当前 `Package.swift` 为 12 个本地 XCFramework 声明 binary target，**不自动安装厂商 SDK**；宿主须按选中广告源补齐对应厂商依赖及其传递依赖。本次 SPM 全源验收按 11 个广告源分别建立单源宿主，不代表一个 App 同时接入多个源已验证。

SPM 与手动集成还须在签名前把 `Resources/TradPlusAds.bundle`、`Resources/TradPlusADX.bundle` 及所选厂商要求的主 Bundle 资源复制到宿主。运行 `python3 copy_resources.py --help` 查看 `--source`、`--vendor-root` 与 `--verify-final`；该脚本每次仅支持一个源，最终校验会拒绝其它源资源，不能靠顺序调用实现同 App 多源。不能把脚本执行成功替代最终 App 资源检查。JAdbid 静态 XCFramework 只链接，不作为动态框架嵌入。宿主仍需检查最终 App 的隐私清单和申报。

## 内容与边界

- `Artifacts/`：Release 构型的 Core + 11 个可选 Adapter XCFramework。
- `JAdbidSDK.podspec`：CocoaPods Core 与广告源 subspec。
- `Package.swift`、`Support/`：SPM 二进制产品与系统链接设置。
- `Resources/`、`PrivacyInfo.xcprivacy`、`Licenses/`：资源、首方隐私清单及保留的第三方许可。
- `BuildIdentity.json`：与上传Core对应的有限Release构建身份；完整内部构建清单和源码不公开。

最低支持 iOS 15；设备与模拟器均提供 arm64 切片。构建或依赖解析成功不等于广告填充、展示、曝光和回调通过真机验收。
