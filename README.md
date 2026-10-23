# JAdbid iOS SDK

JAdbid 是 iOS 聚合广告 SDK，支持开屏、Banner、插屏、原生和激励视频广告，提供 Objective-C 与 Swift 集成示例。版本 **1.0.0** 包含 Core 和 11 个可选广告源 Adapter，可通过 CocoaPods 或 Swift Package Manager 集成。

## 环境要求

- iOS 15.0 及以上。
- arm64 真机或模拟器；不支持 x86_64 模拟器。
- JAdbid 为静态框架，在 Xcode 中选择 **Do Not Embed**。

## CocoaPods

在 `Podfile` 中添加：

```ruby
platform :ios, '15.0'

target 'YourApp' do
  pod 'JAdbid', '1.0.0'
end
```

运行 `pod install`，然后打开生成的 `.xcworkspace`。

默认安装 Core。接入广告源时，将上述声明替换为对应模块，例如 `pod 'JAdbid/AdMob', '1.0.0'`；Adapter 会自动安装 Core 和所需厂商依赖，并处理资源。

## Swift Package Manager

1. 在 Xcode 的 **Package Dependencies** 中添加：
   `https://github.com/jiguangads/jadbid-ios-sdk.git`
2. 版本规则选择 **Exact Version**，填写 **1.0.0**。
3. 选择 `JAdbidCore`，或所需广告源产品，例如 `JAdbidAdMob`。

SPM 不自动安装厂商 SDK。请按[完整集成指南](https://github.com/jiguangads/Adbid-SDK-iOS-Guides/blob/main/docs/integration.md#swift-package-manager)添加对应厂商依赖，并在签名前复制 `TradPlusAds.bundle`、`TradPlusADX.bundle` 和所选厂商要求的资源。不要在同一 target 同时通过 Pods 和 SPM 安装 JAdbid。

## 开始使用

准备 JAdbid App ID 与对应样式的广告位，完成应用所需的隐私流程后初始化 SDK，再加载和展示广告。厂商配置、资源与隐私清单要求见完整指南。

- [快速集成](https://github.com/jiguangads/Adbid-SDK-iOS-Guides/blob/main/docs/quick-start.md)
- [完整集成指南](https://github.com/jiguangads/Adbid-SDK-iOS-Guides/blob/main/docs/integration.md)
- [Objective-C / Swift 示例](https://github.com/jiguangads/Adbid-SDK-iOS-Guides/tree/main/demo)
- [常见问题](https://github.com/jiguangads/Adbid-SDK-iOS-Guides/blob/main/docs/faq.md)
