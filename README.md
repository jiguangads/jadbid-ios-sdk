# JAdbid iOS SDK

JAdbid **1.0.0** 提供 Core 和 11 个可选广告源 Adapter，支持 Objective-C 与 Swift 宿主，以及 CocoaPods、Swift Package Manager 和手动集成。完整步骤及两语言示例见 [iOS 集成指南](https://github.com/jiguangads/Adbid-SDK-iOS-Guides)。

## 环境要求

- 最低 iOS **15.0**。
- 设备与模拟器均为 **arm64**；不支持 Intel Mac 的 x86_64 模拟器。
- JAdbid XCFramework 为静态框架，只链接，选择 **Do Not Embed**；厂商动态依赖按其官方要求嵌入和签名。

## CocoaPods

```ruby
platform :ios, '15.0'

target 'YourApp' do
  pod 'JAdbid/Core', '1.0.0'
  # 按需选择广告源，例如：
  # pod 'JAdbid/AdMob', '1.0.0'
end
```

执行 `pod install`。Adapter subspec 会安装同版本 Core 和对应的锁定厂商依赖；业务应用按需选择，避免重复添加相同模块。保留宿主已有的 `$(inherited)` 和 `-ObjC`。

Pod 配置限制模拟器构建为 arm64，并排除 x86_64；模拟器 `ARCHS` 设置也影响同一宿主和其它 Pods。App target 如自行覆盖架构，仍应保持 arm64。真机架构不受此模拟器设置影响。

## Swift Package Manager

在 Xcode 添加 `https://github.com/jiguangads/jadbid-ios-sdk.git`，精确选择 **1.0.0**。包名为 `JAdbid`，按需选择 `JAdbidCore` 或一个 `JAdbid<Source>` 产品。

Package.swift 仅声明 JAdbid 二进制产品，**不自动安装厂商 SDK**。请为所选广告源单独添加其官方产品和传递依赖，并固定兼容版本。AdMob 使用官方 `GoogleMobileAds 13.7.0` 和 `GoogleUserMessagingPlatform 3.1.0`。各渠道依赖与资源设置见[集成说明](https://github.com/jiguangads/Adbid-SDK-iOS-Guides/blob/main/docs/integration.md)。

### 资源

SwiftPM 和手动集成须在签名前把 `Resources/TradPlusAds.bundle`、`Resources/TradPlusADX.bundle` 以及所选厂商要求的主 Bundle 复制到宿主。使用 `python3 copy_resources.py --help` 查看参数，按真实目录指定 `--resources`、`--destination`、`--source`、`--integration` 及必要的 `--vendor-root`。

脚本每次只处理一个广告源，不能通过顺序调用叠加多源资源。构建后使用 `--verify-final` 只读检查最终 App，不在已签名 App 中补拷。CocoaPods 正常安装会处理资源，无需另跑复制脚本；仅手动核对 Pods 来源资源时使用 `--integration pods`。

官方原生 SwiftPM 资源规则：

- AdMob、Facebook、Fyber、IronSource、Unity、Vungle 不要求仅由 Pods 生成的同名主 Bundle；仍保留官方自动资源、框架内资源和隐私清单。
- Mintegral 需要 `MTGResources.bundle`，Pangle 需要 `PAGAdSDK.bundle`，Yandex 需要 `MobileAdsBundle.bundle` 及真实传递依赖。InMobi 无额外主 Bundle。
- Bigo 官方 SwiftPM `6.1.0` 的资源位于动态 `BigoADS.framework` 内。`--vendor-root` 指向所选切片框架，App 中须已嵌入同一框架；核对 `BigoADSRes.bundle` 与 `PrivacyInfo.xcprivacy`，不生成 Pods 专用的独立 `BigoADS.bundle`。Pods 的 Bigo 依赖保持 `5.3.0`。

所有源均需检查最终 App 的厂商自动资源、框架内资源、隐私清单和宿主隐私申报。

## 包内容

- `Artifacts/`：Core 与 11 个可选 Adapter 的 Release XCFramework。
- `JAdbid.podspec`：CocoaPods 模块与厂商依赖声明。
- `Package.swift`、`Support/`：SwiftPM 产品与系统链接设置。
- `Resources/`、`PrivacyInfo.xcprivacy`、`Licenses/`：资源、隐私清单和第三方许可。

## 接入与运行

宿主完成适用的隐私流程后再显式初始化 SDK，ATT 提示及权限描述由应用负责。默认示例不自动申请权限、初始化或请求广告。日志不得包含真实广告配置、设备标识或原始厂商对象。

加载期间等待明确成功或失败回调；不要依赖后台缓存变化自动触发新的加载回调。构建与资源检查用于确认集成完整，实际广告加载、展示、曝光和奖励需在真实设备及正确的平台配置下检查。
