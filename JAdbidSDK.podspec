Pod::Spec.new do |s|
  s.name = 'JAdbidSDK'
  s.version = '1.0.0'
  s.summary = '基于TradPlus的iOS JAdbid标准聚合广告SDK'
  s.homepage = 'https://github.com/jiguangads/jadbid-ios-sdk'
  s.author = 'Jiguang'
  s.license = { :type => 'Proprietary', :text => 'JAdbid 公开候选包的正式使用条款待发行方确认；第三方许可见 Licenses。' }
  s.source = { :git => 'https://github.com/jiguangads/jadbid-ios-sdk.git', :branch => 'dev' }
  s.ios.deployment_target = '15.0'
  s.static_framework = true
  s.default_subspec = 'Core'
  s.subspec 'Core' do |core|
    core.vendored_frameworks = 'Artifacts/JAdbidCore.xcframework'
    core.resources = 'Resources/*.bundle'
    core.preserve_paths = 'Licenses/*'
    core.frameworks = 'UIKit', 'WebKit', 'StoreKit', 'CoreTelephony', 'SystemConfiguration', 'AVFoundation', 'AdSupport', 'CoreMotion', 'CoreLocation', 'Security', 'CoreMedia', 'SafariServices', 'AudioToolbox', 'MobileCoreServices', 'AppTrackingTransparency', 'JavaScriptCore', 'CoreGraphics', 'QuartzCore', 'MediaPlayer', 'AVKit', 'Accelerate', 'MessageUI', 'CoreImage', 'CoreVideo', 'ImageIO', 'VideoToolbox', 'CFNetwork', 'Network', 'Metal', 'MetalKit', 'Photos'
    core.libraries = 'c++', 'z', 'sqlite3', 'xml2', 'resolv'
    core.user_target_xcconfig = { 'OTHER_LDFLAGS' => '$(inherited) -ObjC' }
  end
  s.subspec 'AdMob' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidAdMobAdapter.xcframework'
    adapter.dependency 'Google-Mobile-Ads-SDK', '= 13.7.0'
  end
  s.subspec 'Bigo' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidBigoAdapter.xcframework'
    adapter.dependency 'BigoADS', '= 5.3.0'
  end
  s.subspec 'Facebook' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidFacebookAdapter.xcframework'
    adapter.dependency 'FBAudienceNetwork', '= 6.22.0'
  end
  s.subspec 'Fyber' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidFyberAdapter.xcframework'
    adapter.dependency 'Fyber_Marketplace_SDK', '= 8.4.7'
  end
  s.subspec 'InMobi' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidInMobiAdapter.xcframework'
    adapter.dependency 'InMobiSDK', '= 11.4.1'
  end
  s.subspec 'IronSource' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidIronSourceAdapter.xcframework'
    adapter.dependency 'IronSourceSDK', '= 9.5.0.0'
  end
  s.subspec 'Mintegral' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidMintegralAdapter.xcframework'
    adapter.dependency 'MintegralAdSDK', '= 8.1.5'
    adapter.dependency 'MintegralAdSDK/All', '= 8.1.5'
  end
  s.subspec 'Pangle' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidPangleAdapter.xcframework'
    adapter.dependency 'Ads-Global', '= 8.2.0.9'
  end
  s.subspec 'Unity' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidUnityAdapter.xcframework'
    adapter.dependency 'UnityAds', '= 4.19.0'
  end
  s.subspec 'Vungle' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidVungleAdapter.xcframework'
    adapter.dependency 'VungleAds', '= 7.7.6'
  end
  s.subspec 'Yandex' do |adapter|
    adapter.dependency 'JAdbidSDK/Core', s.version.to_s
    adapter.vendored_frameworks = 'Artifacts/JAdbidYandexAdapter.xcframework'
    adapter.dependency 'YandexMobileAds', '= 8.3.0'
  end
end
