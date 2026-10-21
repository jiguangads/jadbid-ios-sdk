# JAdbid iOS SDK 二进制分发仓库

**jadbid-1.0.0-beta.2** 对应 SwiftPM Tag `1.0.0-beta.2` 和同版本 CocoaPods Podspec；各渠道是否可用须分别核对远端 Tag 与 Trunk 索引。包含 Release 构型的 `JAdbidCore.xcframework`、11 个广告源 Adapter、资源、隐私清单和 Swift Package 描述文件。旧 Tag `1.0.0-beta`、`1.0.0-beta.1` 保留不变。源码与内部 Demo 不在本仓库；对外集成说明和 Objective-C / Swift Demo 见 [iOS Guides](https://github.com/jiguangads/Adbid-SDK-iOS-Guides)。

> 本文中的本地验收结果不等于相应渠道已发布；使用前请核对精确版本的远端 Tag 或 Trunk 索引。JAdbid 静态 XCFramework 由宿主链接，不作为动态框架嵌入。

本版本沿用 Jenkins `jadbid-sdk-local` **build 4** 的同一 Release 二进制，干净源码提交 `4634ec06b81da7718015aebdd3b025902f153765`，原始 Release ZIP SHA-256 为 `0723de7cfb5921120c6cb72f92dafd00b07531b9a800f547a5020e8dda043e47`。包内 SDK 版本仍为 `1.0.0`，beta.2 是分发版本标识而非重新编译的 SDK 版本。有限身份记录见 `BuildIdentity.json`，其中 `dirty=false`、`build=4`。本仓库只提供 Release 构型。

本版本包含加载中不提前报失败的修复与安全构建身份诊断。SDK普通日志在Debug或显式开启`debugMode`时可见，Release默认关闭普通事件；初始化身份只输出一次。Demo控制台事件和Debug开关由[Guides](https://github.com/jiguangads/Adbid-SDK-iOS-Guides)同步，筛选`[JAdbid]`与`[JAdbidDemo]`可区分来源；不输出原始厂商错误字典或设备标识。

验收契约要求 11 源分别在 Objective-C、Swift 单源宿主执行 Release App 构建与最终资源检查；**beta.2 CocoaPods 本地验收已完成 22/22 个单源 Release App 构建及最终资源检查，另有 Core 两语言通过**。历史 beta.1 SPM 本地候选为 20/22，Yandex 两语言因官方依赖网络问题按例外记录。这些本地构建结果不等同于 Trunk 远端消费或真实广告填充、点击验收，也不代表同一 App 多源同装已获支持。

## CocoaPods（1.0.0-beta.2，安装前核对 Trunk 索引）

```ruby
platform :ios, '15.0'

target 'YourApp' do
  pod 'JAdbidSDK/Core', '1.0.0-beta.2'
  # 按需添加一个单一广告源；多源同装尚未验收：
  # pod 'JAdbidSDK/AdMob', '1.0.0-beta.2'
end
```

上述写法仅在 Trunk 已登记且索引可查后使用。Podspec 的广告源 subspec 声明了对应厂商 SDK 的固定依赖；本地默认 Release 全 subspec `pod lib lint --use-libraries --allow-warnings` 及 Objective-C/Swift 逐源宿主构建已通过。Tag 对应的 `pod spec lint`、Trunk 索引与远端精确版本消费结果须分别以实际执行记录核对，不能由本地构建推断。

本版本仅提供 **arm64 iOS 模拟器**切片，不支持 Intel Mac 的 x86_64 模拟器。CocoaPods 的 Release 验证默认同时构建 arm64/x86_64：Podspec 在自身目标排除 x86_64，并在宿主聚合配置中将模拟器 `ARCHS` 限为 arm64。后者会影响同一宿主及其它 Pods；若 App target 自行覆盖 `ARCHS`，须确保模拟器仍只构建 arm64。真机架构不受此模拟器设置影响。

## Swift Package Manager 开发集成

在 Xcode 添加 `https://github.com/jiguangads/jadbid-ios-sdk.git`；`1.0.0-beta.2` 需以远端 Tag 实际存在为准，旧版本 `1.0.0-beta.1` 保留。选择 `JAdbidCore` 或一个待测 `JAdbid<Source>` 产品。当前 `Package.swift` 为 12 个本地 XCFramework 声明 binary target，**不自动安装厂商 SDK**；宿主须按选中广告源补齐对应厂商依赖及其传递依赖。本次 SPM 全源验收按 11 个广告源分别建立单源宿主，不代表一个 App 同时接入多个源已验证。

SPM 与手动集成还须在签名前把 `Resources/TradPlusAds.bundle`、`Resources/TradPlusADX.bundle` 及所选厂商要求的主 Bundle 资源复制到宿主。运行 `python3 copy_resources.py --help` 查看 `--source`、`--vendor-root` 与 `--verify-final`；该脚本每次仅支持一个源，最终校验会拒绝其它源资源，不能靠顺序调用实现同 App 多源。不能把脚本执行成功替代最终 App 资源检查。JAdbid 静态 XCFramework 只链接，不作为动态框架嵌入。宿主仍需检查最终 App 的隐私清单和申报。

## 内容与边界

- `Artifacts/`：Release 构型的 Core + 11 个可选 Adapter XCFramework。
- `JAdbidSDK.podspec`：CocoaPods Core 与广告源 subspec。
- `Package.swift`、`Support/`：SPM 二进制产品与系统链接设置。
- `Resources/`、`PrivacyInfo.xcprivacy`、`Licenses/`：资源、首方隐私清单及保留的第三方许可。
- `BuildIdentity.json`：与上传Core对应的有限Release构建身份；完整内部构建清单和源码不公开。

最低支持 iOS 15；设备与模拟器均提供 arm64 切片。构建或依赖解析成功不等于广告填充、展示、曝光和回调通过真机验收。
