#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
NS_ASSUME_NONNULL_BEGIN
/**
 * JAdbid 1.0.0 公开接口。未标 nullable 的参数和返回值不得传入或解释为 nil。
 * 建议在主线程串行配置和使用广告对象；UIKit 视图访问、GDPR 页面和原生渲染必须在主线程。
 * 初始化、地区查询及广告 delegate 回调在主线程交付；load/show 会调度至主线程执行。
 * 宿主须强持有广告对象及其 weak delegate，结束使用时调用 destroy；销毁后对象不可重用。
 * 隐私设置应先于初始化调用；底层设置异步排入主队列，立即读取可能仍是旧状态。
 * SDK 不代替宿主申请 ATT、不提供隐私总开关或全局隐私监听器，本封装不提供 IDFA 读写接口。
 */
/** JAdbid 同步错误域及错误对象的归一化域。 */
FOUNDATION_EXPORT NSErrorDomain const JAdErrorDomain;
/**
 * NSError.userInfo 中的细分失败原因键。
 */
FOUNDATION_EXPORT NSErrorUserInfoKey const JAdReasonKey;
/**
 * 错误分类；已知失败映射至此枚举，未识别失败统一为 Unknown，不暴露第三方错误码或对象。
 * NoFill：无填充；Network：网络错误；Timeout：超时；NotInitialized：未初始化。
 * InvalidParameters：参数或调用线程无效；Loading：已有加载进行中；NotReady：缓存不可展示。
 * Internal：内部或不支持的操作（查看 reason）；AdapterMissing：无法创建适配对象；Unknown：未归类错误。
 */
typedef NS_ENUM(NSInteger, JAdErrorCode) {
  JAdErrorNoFill = 1001,
  JAdErrorNetwork = 1002,
  JAdErrorTimeout = 1003,
  JAdErrorNotInitialized = 1004,
  JAdErrorInvalidParameters = 1005,
  JAdErrorLoading = 1006,
  JAdErrorNotReady = 1007,
  JAdErrorInternal = 1008,
  JAdErrorAdapterMissing = 1009,
  JAdErrorUnknown = 9999
};
/**
 * GDPR 业务状态：Unknown=-1、Denied=0、Granted=1；不可用于 LGPD 数值。
 */
typedef NS_ENUM(NSInteger, JAdConsentState) {
  JAdConsentStateUnknown = -1,
  JAdConsentStateDenied = 0,
  JAdConsentStateGranted = 1
};
/**
 * LGPD 专用状态：Unknown=-1、Granted=0、Denied=1，与 GDPR 取值不同。
 */
typedef NS_ENUM(NSInteger, JAdLGPDConsent) {
  JAdLGPDConsentUnknown = -1,
  JAdLGPDConsentGranted = 0,
  JAdLGPDConsentDenied = 1
};
/**
 * 平台类型；首版仅 Default 可用，A/B 保留且不支持初始化。
 */
typedef NS_ENUM(NSInteger, JAdAdapterType) { JAdAdapterTypeDefault = 1, JAdAdapterTypeA = 2, JAdAdapterTypeB = 3 };
/**
 * 地区适用性：Unknown=-1、No=0、Yes=1；不代表用户同意。
 */
typedef NS_ENUM(NSInteger, JAdApplicability) {
  JAdApplicabilityUnknown = -1,
  JAdApplicabilityNo = 0,
  JAdApplicabilityYes = 1
};
/**
 * 初始化配置快照；构造不发起网络请求，使用前由 prepare 校验。
 */
@interface JAdConfig : NSObject <NSCopying>
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
- (instancetype)init NS_UNAVAILABLE;
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
+ (instancetype)new NS_UNAVAILABLE;
/**
 * 以 appID 创建配置，复制标识；本方法不校验合法性也不初始化 SDK。
 *
 * @param appID JAdbid 应用标识；须为非空白字符串。
 * @return 初始化后的对象；不保证底层 SDK 已初始化或广告已加载。
 */
- (instancetype)initWithAppID:(NSString *)appID NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(appID:));
/**
 * JAdbid 应用标识；不能为空或仅含空白，prepare 时校验。
 */
@property(nonatomic, copy, readonly) NSString *appID;
/**
 * 配置值默认 NO。Debug 制品输出受控诊断；Release 关闭时仅输出警告和错误。
 * 所有构型都不会记录 AppID、广告位、IDFA、URL、token 或原始错误对象。
 */
@property(nonatomic) BOOL debugMode;
/**
 * 测试模式，默认 NO；每次聚合后端初始化回调后、对外完成前传递。
 * 宿主仍须在对应后台登记测试设备并启用广告位测试配置。
 */
@property(nonatomic) BOOL testMode;
/**
 * 适配类型；首版仅支持 Default，A/B 为保留值，prepare 会拒绝。
 */
@property(nonatomic) JAdAdapterType adapterType;
@end
/**
 * 单次广告事件的 JAdbid 信息快照。仅暴露文档化白名单字段，不包含原始字典或第三方对象。
 */
@interface JAdInfo : NSObject
/** 广告来源的可读名称。 */
@property(nonatomic, copy, readonly, nullable) NSString *networkName;
/** 广告来源的 JAdbid 归一化标识。 */
@property(nonatomic, copy, readonly, nullable) NSString *networkIdentifier;
/** 当前广告源的 JAdbid 归一化标识。 */
@property(nonatomic, copy, readonly, nullable) NSString *sourceIdentifier;
/** 当前广告源中的位置标识。 */
@property(nonatomic, copy, readonly, nullable) NSString *sourcePlacementIdentifier;
/** 广告价值字符串；SDK 不自行换算币种或单位。 */
@property(nonatomic, copy, readonly, nullable) NSString *ecpm;
/** 价值精度的 JAdbid 归一化描述。 */
@property(nonatomic, copy, readonly, nullable) NSString *ecpmPrecision;
/** 本次事件的场景标识。 */
@property(nonatomic, copy, readonly, nullable) NSString *sceneIdentifier;
@end
/**
 * 广告异步失败信息；同步 BOOL 接口使用 NSError，同属 JAdErrorDomain。
 */
@interface JAdError : NSObject
/**
 * JAdbid 错误码；无法归一化的后端错误统一为 Unknown。
 */
@property(nonatomic, readonly) JAdErrorCode code;
/**
 * 用于诊断的错误描述；不要作为稳定业务分支条件。
 */
@property(nonatomic, copy, readonly) NSString *message;
/**
 * 细分原因标识；对应 NSError.userInfo[JAdReasonKey]，可能为空。
 */
@property(nonatomic, copy, readonly, nullable) NSString *reason;
@end
/**
 * 底层地区查询快照；仅用于地区判断，不表示用户已同意隐私政策。
 */
@interface JAdRegionResult : NSObject
/**
 * 地区是否尚无法判定；为 YES 时不可将地区标记当作合规授权。
 */
@property(nonatomic, readonly) BOOL unknown;
/**
 * 底层判定是否处于中国地区。
 */
@property(nonatomic, readonly) BOOL CN;
/**
 * 底层判定是否处于加州（CCPA）地区。
 */
@property(nonatomic, readonly) BOOL CA;
/**
 * 底层判定是否处于 GDPR 适用地区。
 */
@property(nonatomic, readonly) BOOL EU;
/**
 * 底层判定是否处于巴西地区。
 */
@property(nonatomic, readonly) BOOL BR;
@end
@class JBannerAd, JInterstitialAd, JRewardedVideoAd, JNativeAd, JSplashAd;
@protocol JAdAdapterAccess;
/**
 * 共享适配入口，所有管理能力与 JAdManager 为同一状态，线程/隐私约束相同。
 */
@protocol JAdAdapterAccess <NSObject>
/**
 * 本进程 SDK 是否已成功初始化；失败不会置为 YES。
 */
@property(nonatomic, readonly, getter=isInitialized) BOOL initialized;
/**
 * 按广告位读取最近一次非空事件信息快照；没有记录返回 nil，不发起请求或保证缓存仍可展示。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 最近一次非空事件快照；尚无记录时为 nil。
 */
- (nullable JAdInfo *)adInfoForAdUnitID:(NSString *)adUnitID;

// 以下管理接口共享 manager 状态，不另建初始化或隐私状态。
/**
 * 尚未初始化且无初始化在途时保存 config 副本，不触发初始化。初始化中或成功后仅同 AppID 且同 testMode 返回 YES
 * 但不替换配置；任一不同返回 NO。非法 AppID/非 Default 类型同样返回 NO 并填 error；Swift 失败抛错。
 *
 * @param config 待校验的配置；是否保存及冲突处理见上文。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)prepareWithConfig:(JAdConfig *)config
                    error:(NSError *_Nullable *_Nullable)error NS_SWIFT_NAME(prepare(config:));
/**
 * 校验并保存 config 后初始化底层；completion 在主线程收到 nil（成功）或
 * NSError。相同配置并发调用合并；成功后重复调用不重复初始化。
 *
 * @param config 待校验的配置；是否保存及冲突处理见上文。
 * @param completion 主线程交付初始化结果；nil 成功，非 nil 为失败原因。
 */
- (void)initializeWithConfig:(JAdConfig *)config
                  completion:(void (^)(NSError *_Nullable error))completion
    NS_SWIFT_NAME(initialize(config:completion:));
/**
 * 使用已 prepare 的配置开始初始化；名称表示由宿主判断同意流程已就绪，本方法不自动判断 GDPR/ATT。未 prepare 时
 * completion 收到 notPrepared，回调在主线程。
 *
 * @param completion 主线程交付初始化结果；nil 成功，非 nil 为失败原因。
 */
- (void)initializeWhenConsentReadyWithCompletion:(void (^)(NSError *_Nullable error))completion
    NS_SWIFT_NAME(initializeWhenConsentReady(completion:));
/**
 * 只等待初始化结果，不触发初始化；timeout 单位秒，0 立即查询，负数/非有限数返回 NO。completion
 * 在主线程交付一次，超时或初始化失败为 NO。
 *
 * @param timeout 等待秒数；0 仅查询，负数或非有限数返回未初始化。
 * @param completion 主线程一次性交付 initialized；超时/失败为 NO，不取消初始化。
 */
- (void)waitForInitializationWithTimeout:(NSTimeInterval)timeout
                              completion:(void (^)(BOOL initialized))completion
    NS_SWIFT_NAME(waitForInitialization(timeout:completion:));
/**
 * 是否已保存有效配置；YES 不表示底层初始化完成。
 */
@property(nonatomic, readonly, getter=isPrepared) BOOL prepared;
/**
 * 已保存配置的副本；尚未 prepare 时为 nil，修改副本不会改变当前配置。
 */
@property(nonatomic, copy, readonly, nullable) JAdConfig *configuration;
/**
 * 配置值默认 NO。Debug 制品始终输出 JAdbid 关键链路；Release 关闭时仅输出精简异常。
 * 所有模式均不输出 AppID、广告位标识、IDFA 或第三方原始数据。
 */
@property(nonatomic) BOOL debugMode;
/**
 * 个性化广告开关，直接映射私有后端；不等同 GDPR 同意或 ATT 授权。
 */
@property(nonatomic) BOOL personalizedAdsEnabled;
/**
 * 设置 GDPR 数据收集许可：allowed=YES 同意，NO 拒绝；初始化前调用，不提供恢复未知操作。底层负责状态存储。
 *
 * @param allowed YES 允许、NO 拒绝此项数据收集；不代表系统 ATT 授权。
 */
- (void)setGDPRDataCollectionAllowed:(BOOL)allowed;
/**
 * 读取底层当前 GDPR 同意状态，未知不等同拒绝；持久化由底层管理。
 */
@property(nonatomic, readonly) JAdConsentState GDPRConsentState;
/**
 * 读取底层 GDPR 适用性，地区未知时可返回 Unknown。
 */
@property(nonatomic, readonly) JAdApplicability GDPRApplicable;
/**
 * 底层当前是否允许收集个人信息的综合判断；不是可写隐私总开关。
 */
@property(nonatomic, readonly) BOOL canCollectPersonalInfo;
/**
 * 设置开发者追踪许可，读取底层综合追踪结果；不会弹出 ATT，也不能越过系统拒绝。
 */
@property(nonatomic) BOOL trackingAllowed;
/**
 * 必须主线程且 controller 已挂到窗口。返回 YES 仅表示已调用底层页面流程；参数/线程错误返回 NO 并设置 error。didShow
 * 为页面显示事件；didDismiss 返回关闭时实际同意状态，不将关闭当作授权成功。回调在主线程，无合成 success/failure 事件。
 *
 * @param controller 用于呈现页面的控制器；GDPR 页面要求其视图已挂到窗口。
 * @param didShow 可选页面显示通知；主线程调用，仅表示显示。
 * @param didDismiss 可选页面关闭通知；主线程返回关闭时真实 GDPR 状态，关闭不等于同意。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)showGDPRConsentDialogFromViewController:(UIViewController *)controller
                                        didShow:(void (^_Nullable)(void))didShow
                                     didDismiss:(void (^_Nullable)(JAdConsentState state))didDismiss
                                          error:(NSError *_Nullable *_Nullable)error
    NS_SWIFT_NAME(showGDPRConsentDialog(from:didShow:didDismiss:));
/**
 * CCPA 数据收集许可：YES 允许，NO 拒绝；原值传入 TP setCCPADoNotSell，getter 为本进程记录（默认 YES）。
 */
@property(nonatomic) BOOL ccpaDataCollectionAllowed;
/**
 * 宿主维护的本地布尔标记，NSUserDefaults 持久化（默认 NO）；不会自动弹窗或证明同意。
 */
@property(nonatomic) BOOL firstGDPRPresentation;
/**
 * TP 消息推送许可；getter 为本进程记录（默认 YES），不代替系统通知授权。
 */
@property(nonatomic) BOOL messagePushAllowed;
/**
 * 本进程最后设置的 LGPD 状态，初始 Unknown；不是读取厂商持久化同意的接口。
 */
@property(nonatomic, readonly) JAdLGPDConsent LGPDConsent;
/**
 * 初始化成功后读取底层国家/地区代码；初始化前或底层无值时为 nil。
 */
@property(nonatomic, copy, readonly, nullable) NSString *ISOCode;
/**
 * 请求底层地区判断；completion 在主线程交付结果，unknown 表示无法判定，不据此自动设置任何隐私状态。
 *
 * @param completion 主线程交付地区快照；unknown 表示无法判断。
 */
- (void)checkCurrentAreaWithCompletion:(void (^)(JAdRegionResult *result))completion
    NS_SWIFT_NAME(checkCurrentArea(completion:));
/**
 * iOS 暂无公开容量调整能力；bytes 不被应用，始终返回 NO，error.code=Internal、reason=unsupportedCapability；Swift
 * 抛错。
 *
 * @param bytes 拟设置的容量字节数；当前不支持，任何值均不应用。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)setMaximumCacheDatabaseSize:(int64_t)bytes
                              error:(NSError *_Nullable *_Nullable)error NS_SWIFT_NAME(setMaximumCacheDatabaseSize(_:));
/**
 * 设置 LGPD：0 同意、1 拒绝、-1 初始未知（不调用底层）。已设置后恢复 Unknown 返回
 * NO/unsupportedConsentReset；无效枚举返回 invalidConsentValue。成功 YES，失败填 error；不额外持久化。
 *
 * @param consent LGPD 专用枚举；0 同意、1 拒绝，-1 仅保留初始未知。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)setLGPDConsent:(JAdLGPDConsent)consent
                 error:(NSError *_Nullable *_Nullable)error NS_SWIFT_NAME(setLGPDConsent(_:));
/**
 * 适配类型；首版仅支持 Default，A/B 为保留值，prepare 会拒绝。
 */
@property(nonatomic, readonly) JAdAdapterType adapterType;
/**
 * 测试模式，默认
 * NO；每次后端初始化回调后、对外完成前传递。底层全局模式在初始化开始时冻结，初始化中或成功后设置不改变已生效值。仍须在广告平台后台登记测试设备并启用对应广告位测试配置。
 */
@property(nonatomic) BOOL testMode;
/**
 * 创建对应广告对象，adUnitID 为广告位标识；不自动加载，宿主须强持有返回对象与 delegate。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 新广告对象；尚未加载，宿主负责强持有。
 */
- (JBannerAd *)makeBannerAdWithAdUnitID:(NSString *)adUnitID;
/**
 * 创建对应广告对象，adUnitID 为广告位标识；不自动加载，宿主须强持有返回对象与 delegate。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 新广告对象；尚未加载，宿主负责强持有。
 */
- (JInterstitialAd *)makeInterstitialAdWithAdUnitID:(NSString *)adUnitID;
/**
 * 创建对应广告对象，adUnitID 为广告位标识；不自动加载，宿主须强持有返回对象与 delegate。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 新广告对象；尚未加载，宿主负责强持有。
 */
- (JRewardedVideoAd *)makeRewardedVideoAdWithAdUnitID:(NSString *)adUnitID;
/**
 * 创建对应广告对象，adUnitID 为广告位标识；不自动加载，宿主须强持有返回对象与 delegate。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 新广告对象；尚未加载，宿主负责强持有。
 */
- (JNativeAd *)makeNativeAdWithAdUnitID:(NSString *)adUnitID;
/**
 * 创建对应广告对象，adUnitID 为广告位标识；不自动加载，宿主须强持有返回对象与 delegate。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 新广告对象；尚未加载，宿主负责强持有。
 */
- (JSplashAd *)makeSplashAdWithAdUnitID:(NSString *)adUnitID;
@end
/**
 * 进程级共享管理器；先配置隐私，再初始化，广告加载须等待初始化成功。
 */
@interface JAdManager : NSObject
/**
 * 禁止默认构造；请使用 sharedInstance（Swift：shared()）获取单例。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
- (instancetype)init NS_UNAVAILABLE;
/**
 * 禁止默认构造；请使用 sharedInstance（Swift：shared()）获取单例。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
+ (instancetype)new NS_UNAVAILABLE;
/**
 * 返回进程共享管理器；不要自行创建多个初始化状态。
 *
 * @return 进程共享管理器。
 */
+ (instancetype)sharedInstance NS_SWIFT_NAME(shared());
/**
 * 尚未初始化且无初始化在途时保存 config 副本，不触发初始化。初始化中或成功后仅同 AppID 且同 testMode 返回 YES
 * 但不替换配置；任一不同返回 NO。非法 AppID/非 Default 类型同样返回 NO 并填 error；Swift 失败抛错。
 *
 * @param config 待校验的配置；是否保存及冲突处理见上文。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)prepareWithConfig:(JAdConfig *)config
                    error:(NSError *_Nullable *_Nullable)error NS_SWIFT_NAME(prepare(config:));
/**
 * 校验并保存 config 后初始化底层；completion 在主线程收到 nil（成功）或
 * NSError。相同配置并发调用合并；成功后重复调用不重复初始化。
 *
 * @param config 待校验的配置；是否保存及冲突处理见上文。
 * @param completion 主线程交付初始化结果；nil 成功，非 nil 为失败原因。
 */
- (void)initializeWithConfig:(JAdConfig *)config
                  completion:(void (^)(NSError *_Nullable error))completion
    NS_SWIFT_NAME(initialize(config:completion:));
/**
 * 使用已 prepare 的配置开始初始化；名称表示由宿主判断同意流程已就绪，本方法不自动判断 GDPR/ATT。未 prepare 时
 * completion 收到 notPrepared，回调在主线程。
 *
 * @param completion 主线程交付初始化结果；nil 成功，非 nil 为失败原因。
 */
- (void)initializeWhenConsentReadyWithCompletion:(void (^)(NSError *_Nullable error))completion
    NS_SWIFT_NAME(initializeWhenConsentReady(completion:));
/**
 * 只等待初始化结果，不触发初始化；timeout 单位秒，0 立即查询，负数/非有限数返回 NO。completion
 * 在主线程交付一次，超时或初始化失败为 NO。
 *
 * @param timeout 等待秒数；0 仅查询，负数或非有限数返回未初始化。
 * @param completion 主线程一次性交付 initialized；超时/失败为 NO，不取消初始化。
 */
- (void)waitForInitializationWithTimeout:(NSTimeInterval)timeout
                              completion:(void (^)(BOOL initialized))completion
    NS_SWIFT_NAME(waitForInitialization(timeout:completion:));
/**
 * 是否已保存有效配置；YES 不表示底层初始化完成。
 */
@property(nonatomic, readonly, getter=isPrepared) BOOL prepared;
/**
 * 本进程 SDK 是否已成功初始化；失败不会置为 YES。
 */
@property(nonatomic, readonly, getter=isInitialized) BOOL initialized;
/**
 * JAdbid 运行时版本字符串，当前为 1.0.0。
 */
@property(nonatomic, copy, readonly) NSString *SDKVersion;
/**
 * 已保存配置的副本；尚未 prepare 时为 nil，修改副本不会改变当前配置。
 */
@property(nonatomic, copy, readonly, nullable) JAdConfig *configuration;
/**
 * prepare 成功后可用的共享适配入口；未准备时为 nil，不创建独立 SDK 状态。
 */
@property(nonatomic, strong, readonly, nullable) id<JAdAdapterAccess> adapter;
/**
 * 配置值默认 NO。Debug 制品始终输出 JAdbid 关键链路；Release 关闭时仅输出精简异常。
 * 所有模式均不输出 AppID、广告位标识、IDFA 或第三方原始数据。
 */
@property(nonatomic) BOOL debugMode;
/**
 * 个性化广告开关，直接映射私有后端；不等同 GDPR 同意或 ATT 授权。
 */
@property(nonatomic) BOOL personalizedAdsEnabled;
/**
 * 设置 GDPR 数据收集许可：allowed=YES 同意，NO 拒绝；初始化前调用，不提供恢复未知操作。底层负责状态存储。
 *
 * @param allowed YES 允许、NO 拒绝此项数据收集；不代表系统 ATT 授权。
 */
- (void)setGDPRDataCollectionAllowed:(BOOL)allowed;
/**
 * 读取底层当前 GDPR 同意状态，未知不等同拒绝；持久化由底层管理。
 */
@property(nonatomic, readonly) JAdConsentState GDPRConsentState;
/**
 * 读取底层 GDPR 适用性，地区未知时可返回 Unknown。
 */
@property(nonatomic, readonly) JAdApplicability GDPRApplicable;
/**
 * 底层当前是否允许收集个人信息的综合判断；不是可写隐私总开关。
 */
@property(nonatomic, readonly) BOOL canCollectPersonalInfo;
/**
 * 设置开发者追踪许可，读取底层综合追踪结果；不会弹出 ATT，也不能越过系统拒绝。
 */
@property(nonatomic) BOOL trackingAllowed;
/**
 * 必须主线程且 controller 已挂到窗口。返回 YES 仅表示已调用底层页面流程；参数/线程错误返回 NO 并设置 error。didShow
 * 为页面显示事件；didDismiss 返回关闭时实际同意状态，不将关闭当作授权成功。回调在主线程，无合成 success/failure 事件。
 *
 * @param controller 用于呈现页面的控制器；GDPR 页面要求其视图已挂到窗口。
 * @param didShow 可选页面显示通知；主线程调用，仅表示显示。
 * @param didDismiss 可选页面关闭通知；主线程返回关闭时真实 GDPR 状态，关闭不等于同意。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)showGDPRConsentDialogFromViewController:(UIViewController *)controller
                                        didShow:(void (^_Nullable)(void))didShow
                                     didDismiss:(void (^_Nullable)(JAdConsentState state))didDismiss
                                          error:(NSError *_Nullable *_Nullable)error
    NS_SWIFT_NAME(showGDPRConsentDialog(from:didShow:didDismiss:));
/**
 * CCPA 数据收集许可：YES 允许，NO 拒绝；原值传入 TP setCCPADoNotSell，getter 为本进程记录（默认 YES）。
 */
@property(nonatomic) BOOL ccpaDataCollectionAllowed;
/**
 * 宿主维护的本地布尔标记，NSUserDefaults 持久化（默认 NO）；不会自动弹窗或证明同意。
 */
@property(nonatomic) BOOL firstGDPRPresentation;
/**
 * TP 消息推送许可；getter 为本进程记录（默认 YES），不代替系统通知授权。
 */
@property(nonatomic) BOOL messagePushAllowed;
/**
 * 本进程最后设置的 LGPD 状态，初始 Unknown；不是读取厂商持久化同意的接口。
 */
@property(nonatomic, readonly) JAdLGPDConsent LGPDConsent;
/**
 * 初始化成功后读取底层国家/地区代码；初始化前或底层无值时为 nil。
 */
@property(nonatomic, copy, readonly, nullable) NSString *ISOCode;
/**
 * 按广告位读取最近一次非空事件信息快照；没有记录返回 nil，不发起请求或保证缓存仍可展示。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 最近一次非空事件快照；尚无记录时为 nil。
 */
- (nullable JAdInfo *)adInfoForAdUnitID:(NSString *)adUnitID;
/**
 * 请求底层地区判断；completion 在主线程交付结果，unknown 表示无法判定，不据此自动设置任何隐私状态。
 *
 * @param completion 主线程交付地区快照；unknown 表示无法判断。
 */
- (void)checkCurrentAreaWithCompletion:(void (^)(JAdRegionResult *result))completion
    NS_SWIFT_NAME(checkCurrentArea(completion:));
/**
 * iOS 暂无公开容量调整能力；bytes 不被应用，始终返回 NO，error.code=Internal、reason=unsupportedCapability；Swift
 * 抛错。
 *
 * @param bytes 拟设置的容量字节数；当前不支持，任何值均不应用。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)setMaximumCacheDatabaseSize:(int64_t)bytes
                              error:(NSError *_Nullable *_Nullable)error NS_SWIFT_NAME(setMaximumCacheDatabaseSize(_:));
/**
 * 设置 LGPD：0 同意、1 拒绝、-1 初始未知（不调用底层）。已设置后恢复 Unknown 返回
 * NO/unsupportedConsentReset；无效枚举返回 invalidConsentValue。成功 YES，失败填 error；不额外持久化。
 *
 * @param consent LGPD 专用枚举；0 同意、1 拒绝，-1 仅保留初始未知。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)setLGPDConsent:(JAdLGPDConsent)consent
                 error:(NSError *_Nullable *_Nullable)error NS_SWIFT_NAME(setLGPDConsent(_:));
@end
/**
 * 原生素材视图协议；可选方法返回当前素材视图自身或其子视图，未提供素材返回 nil，须主线程访问。
 */
@protocol JAdNativeRendering <NSObject>
@optional
/**
 * 标题文本控件；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UILabel *)nativeTitleTextLabel;
/**
 * 正文文本控件；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UILabel *)nativeMainTextLabel;
/**
 * 图标控件；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UIImageView *)nativeIconImageView;
/**
 * 主图控件；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UIImageView *)nativeMainImageView;
/**
 * 媒体承载视图；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UIView *)nativeMediaView;
/**
 * 行动按钮文字控件；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UILabel *)nativeCallToActionTextLabel;
/**
 * 行动按钮视图；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UIView *)nativeCallToActionView;
/**
 * 隐私信息图标；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UIImageView *)nativePrivacyInformationIconImageView;
/**
 * 广告选择标识视图；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UIView *)nativeAdChoiceView;
/**
 * 可点击视图数组，元素必须位于素材视图层级内；nil 交由底层处理；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable NSArray<UIView *> *)clickViewArray;
/**
 * 关闭按钮；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
- (nullable UIButton *)nativeCloseButton;
/**
 * 可选素材 Nib，由底层渲染器按需使用；返回 nil 表示未提供。
 *
 * @return 所述素材控件/对象；未提供时为 nil，必须遵循素材视图层级约束。
 */
+ (nullable UINib *)nibForAd;
@end
@class JBannerAd, JInterstitialAd, JRewardedVideoAd, JNativeAd, JSplashAd;
/**
 * 可选广告事件协议；所有回调在主线程，不保证各广告源都提供每一种事件。
 */
@protocol JBannerAdDelegate <NSObject>
@optional
/**
 * 加载完成；info 可能为空，不代表已展示。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JBannerAd *)ad didLoad:(JAdInfo *_Nullable)info;
/**
 * 加载失败，error 含归一化原因及可用底层错误；重复在途请求不会取消原请求。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JBannerAd *)ad didFailToLoad:(JAdError *)error;
/**
 * 底层展示事件；不以 load/show 方法返回替代此事件。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JBannerAd *)ad didShow:(JAdInfo *_Nullable)info;
/**
 * 底层点击事件，info 可能为空。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JBannerAd *)ad didClick:(JAdInfo *_Nullable)info;
/**
 * 底层关闭事件；不表示获得奖励。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JBannerAd *)ad didClose:(JAdInfo *_Nullable)info;
/**
 * 展示或渲染失败，包含 Banner/Native 的异步失败；查看 error 原因。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JBannerAd *)ad didFailToShow:(JAdError *)error;
/**
 * Banner 实际有效尺寸变化（单位 point）；宿主据此更新外部布局。
 *
 * @param ad 产生此次事件的广告对象。
 * @param size Banner 新尺寸（point），宽高均为有限正数。
 */
- (void)ad:(JBannerAd *)ad didChangeSize:(CGSize)size;
@end
/**
 * 可选广告事件协议；所有回调在主线程，不保证各广告源都提供每一种事件。
 */
@protocol JInterstitialAdDelegate <NSObject>
@optional
/**
 * 加载完成；info 可能为空，不代表已展示。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JInterstitialAd *)ad didLoad:(JAdInfo *_Nullable)info;
/**
 * 加载失败，error 含归一化原因及可用底层错误；重复在途请求不会取消原请求。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JInterstitialAd *)ad didFailToLoad:(JAdError *)error;
/**
 * 底层展示事件；不以 load/show 方法返回替代此事件。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JInterstitialAd *)ad didShow:(JAdInfo *_Nullable)info;
/**
 * 底层点击事件，info 可能为空。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JInterstitialAd *)ad didClick:(JAdInfo *_Nullable)info;
/**
 * 底层关闭事件；不表示获得奖励。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JInterstitialAd *)ad didClose:(JAdInfo *_Nullable)info;
/**
 * 展示或渲染失败，包含 Banner/Native 的异步失败；查看 error 原因。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JInterstitialAd *)ad didFailToShow:(JAdError *)error;
/**
 * 底层视频开始事件，仅在广告源提供时交付。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JInterstitialAd *)ad didStartVideo:(JAdInfo *_Nullable)info;
/**
 * 底层视频结束事件，不等于激励发奖。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JInterstitialAd *)ad didEndVideo:(JAdInfo *_Nullable)info;
@end
/**
 * 可选广告事件协议；所有回调在主线程，不保证各广告源都提供每一种事件。
 */
@protocol JRewardedVideoAdDelegate <NSObject>
@optional
/**
 * 加载完成；info 可能为空，不代表已展示。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JRewardedVideoAd *)ad didLoad:(JAdInfo *_Nullable)info;
/**
 * 加载失败，error 含归一化原因及可用底层错误；重复在途请求不会取消原请求。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JRewardedVideoAd *)ad didFailToLoad:(JAdError *)error;
/**
 * 底层展示事件；不以 load/show 方法返回替代此事件。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JRewardedVideoAd *)ad didShow:(JAdInfo *_Nullable)info;
/**
 * 底层点击事件，info 可能为空。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JRewardedVideoAd *)ad didClick:(JAdInfo *_Nullable)info;
/**
 * 底层关闭事件；不表示获得奖励。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JRewardedVideoAd *)ad didClose:(JAdInfo *_Nullable)info;
/**
 * 展示或渲染失败，包含 Banner/Native 的异步失败；查看 error 原因。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JRewardedVideoAd *)ad didFailToShow:(JAdError *)error;
/**
 * 奖励事件，同一次展示最多交付一次；宿主在此处理奖励，必要时自行核验服务端凭据。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JRewardedVideoAd *)ad didReward:(JAdInfo *_Nullable)info;
/**
 * 底层视频开始事件，仅在广告源提供时交付。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JRewardedVideoAd *)ad didStartVideo:(JAdInfo *_Nullable)info;
/**
 * 底层视频结束事件，不等于激励发奖。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JRewardedVideoAd *)ad didEndVideo:(JAdInfo *_Nullable)info;
@end
/**
 * 可选广告事件协议；所有回调在主线程，不保证各广告源都提供每一种事件。
 */
@protocol JNativeAdDelegate <NSObject>
@optional
/**
 * 加载完成；info 可能为空，不代表已展示。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JNativeAd *)ad didLoad:(JAdInfo *_Nullable)info;
/**
 * 加载失败，error 含归一化原因及可用底层错误；重复在途请求不会取消原请求。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JNativeAd *)ad didFailToLoad:(JAdError *)error;
/**
 * 底层展示事件；不以 load/show 方法返回替代此事件。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JNativeAd *)ad didShow:(JAdInfo *_Nullable)info;
/**
 * 底层点击事件，info 可能为空。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JNativeAd *)ad didClick:(JAdInfo *_Nullable)info;
/**
 * 底层关闭事件；不表示获得奖励。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JNativeAd *)ad didClose:(JAdInfo *_Nullable)info;
/**
 * 展示或渲染失败，包含 Banner/Native 的异步失败；查看 error 原因。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JNativeAd *)ad didFailToShow:(JAdError *)error;
@end
/**
 * 可选广告事件协议；所有回调在主线程，不保证各广告源都提供每一种事件。
 */
@protocol JSplashAdDelegate <NSObject>
@optional
/**
 * 加载完成；info 可能为空，不代表已展示。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JSplashAd *)ad didLoad:(JAdInfo *_Nullable)info;
/**
 * 加载失败，error 含归一化原因及可用底层错误；重复在途请求不会取消原请求。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JSplashAd *)ad didFailToLoad:(JAdError *)error;
/**
 * 底层展示事件；不以 load/show 方法返回替代此事件。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JSplashAd *)ad didShow:(JAdInfo *_Nullable)info;
/**
 * 底层点击事件，info 可能为空。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JSplashAd *)ad didClick:(JAdInfo *_Nullable)info;
/**
 * 底层关闭事件；不表示获得奖励。
 *
 * @param ad 产生此次事件的广告对象。
 * @param info 底层该次事件的信息快照，可能为 nil。
 */
- (void)ad:(JSplashAd *)ad didClose:(JAdInfo *_Nullable)info;
/**
 * 展示或渲染失败，包含 Banner/Native 的异步失败；查看 error 原因。
 *
 * @param ad 产生此次事件的广告对象。
 * @param error 失败事件对象，包含细分原因及可用底层错误。
 */
- (void)ad:(JSplashAd *)ad didFailToShow:(JAdError *)error;
@end
/**
 * 横幅广告；同一对象复用底层实例，加载后自动展示到 bannerView，无独立 show 方法。
 */
@interface JBannerAd : NSObject
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
- (instancetype)init NS_UNAVAILABLE;
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
+ (instancetype)new NS_UNAVAILABLE;
/**
 * 以 adUnitID 创建可复用广告对象，复制标识；不加载广告，非法空白标识在 load 时异步报错。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 初始化后的对象；不保证底层 SDK 已初始化或广告已加载。
 */
- (instancetype)initWithAdUnitID:(NSString *)adUnitID NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(adUnitID:));
/**
 * 构造时复制的广告位标识；空白值在 load 时通过 didFailToLoad 报错。
 */
@property(nonatomic, copy, readonly) NSString *adUnitID;
/**
 * 可选场景标识；Banner 在加载、其它样式在展示/渲染时传给底层；nil 表示默认场景。
 */
@property(nonatomic, copy, nullable) NSString *sceneID;
/**
 * 可选自定义信息快照；仅保留字符串/数值及其字典数组，加载和展示/渲染前传递，勿放凭据。
 */
@property(nonatomic, copy, nullable) NSDictionary<NSString *, id> *customAdInfo;
/**
 * 弱引用事件接收者，宿主负责保活；销毁后尚未交付的事件会被屏蔽。
 */
@property(nonatomic, weak, nullable) id<JBannerAdDelegate> delegate;
/**
 * 异步切主线程加载，viewController 用于广告交互呈现；须先初始化成功。重复在途 load 回调
 * alreadyLoading，不取消首请求；结果走 didLoad/didFailToLoad。同一对象复用底层实例，不增加自定义自动重试。
 *
 * @param viewController 用于广告交互呈现的有效控制器，由宿主管理生命周期。
 */
- (void)loadWithViewController:(UIViewController *)viewController NS_SWIFT_NAME(load(viewController:));
/**
 * 终止此对象后续交付并异步释放底层/移除视图，可重复调用；已进入的回调无法撤回。销毁后 load/show
 * 不再生效，需创建新对象。
 */
- (void)destroy;
/**
 * Banner 加载前设置的尺寸，默认 320×50；宽高须为有限正数，实际尺寸变化由 delegate 通知。
 */
@property(nonatomic) CGSize adSize;
/**
 * 主线程获取稳定的 Banner 容器并加入宿主布局；加载前可获取，后台线程或销毁后为 nil。
 */
@property(nonatomic, strong, readonly, nullable) UIView *bannerView;
@end
/**
 * 插屏广告；load 与 show 分离，依据 ready 查询底层缓存并在合适时机展示。
 */
@interface JInterstitialAd : NSObject
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
- (instancetype)init NS_UNAVAILABLE;
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
+ (instancetype)new NS_UNAVAILABLE;
/**
 * 以 adUnitID 创建可复用广告对象，复制标识；不加载广告，非法空白标识在 load 时异步报错。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 初始化后的对象；不保证底层 SDK 已初始化或广告已加载。
 */
- (instancetype)initWithAdUnitID:(NSString *)adUnitID NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(adUnitID:));
/**
 * 构造时复制的广告位标识；空白值在 load 时通过 didFailToLoad 报错。
 */
@property(nonatomic, copy, readonly) NSString *adUnitID;
/**
 * 可选场景标识；Banner 在加载、其它样式在展示/渲染时传给底层；nil 表示默认场景。
 */
@property(nonatomic, copy, nullable) NSString *sceneID;
/**
 * 可选自定义信息快照；仅保留字符串/数值及其字典数组，加载和展示/渲染前传递，勿放凭据。
 */
@property(nonatomic, copy, nullable) NSDictionary<NSString *, id> *customAdInfo;
/**
 * 弱引用事件接收者，宿主负责保活；销毁后尚未交付的事件会被屏蔽。
 */
@property(nonatomic, weak, nullable) id<JInterstitialAdDelegate> delegate;
/**
 * 异步切主线程加载，viewController 用于广告交互呈现；须先初始化成功。重复在途 load 回调
 * alreadyLoading，不取消首请求；结果走 didLoad/didFailToLoad。同一对象复用底层实例，不增加自定义自动重试。
 *
 * @param viewController 用于广告交互呈现的有效控制器，由宿主管理生命周期。
 */
- (void)loadWithViewController:(UIViewController *)viewController NS_SWIFT_NAME(load(viewController:));
/**
 * 终止此对象后续交付并异步释放底层/移除视图，可重复调用；已进入的回调无法撤回。销毁后 load/show
 * 不再生效，需创建新对象。
 */
- (void)destroy;
/**
 * 底层缓存当前是否可展示；销毁或全屏展示中为 NO，不预留缓存，展示时仍可能失败。
 */
@property(nonatomic, readonly, getter=isReady) BOOL ready;
/**
 * 从有效 viewController 展示已就绪缓存，异步切主线程；未就绪/重复展示通过 didFailToShow
 * 报错。调用返回不代表展示成功，以 delegate 为准。
 *
 * @param viewController 用于广告交互呈现的有效控制器，由宿主管理生命周期。
 */
- (void)showFromViewController:(UIViewController *)viewController NS_SWIFT_NAME(show(from:));
@end
/**
 * 激励视频；仅 didReward 表示奖励事件，视频结束/关闭不能替代奖励，不提供“再看一次”接口。
 */
@interface JRewardedVideoAd : NSObject
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
- (instancetype)init NS_UNAVAILABLE;
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
+ (instancetype)new NS_UNAVAILABLE;
/**
 * 以 adUnitID 创建可复用广告对象，复制标识；不加载广告，非法空白标识在 load 时异步报错。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 初始化后的对象；不保证底层 SDK 已初始化或广告已加载。
 */
- (instancetype)initWithAdUnitID:(NSString *)adUnitID NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(adUnitID:));
/**
 * 构造时复制的广告位标识；空白值在 load 时通过 didFailToLoad 报错。
 */
@property(nonatomic, copy, readonly) NSString *adUnitID;
/**
 * 可选场景标识；Banner 在加载、其它样式在展示/渲染时传给底层；nil 表示默认场景。
 */
@property(nonatomic, copy, nullable) NSString *sceneID;
/**
 * 可选自定义信息快照；仅保留字符串/数值及其字典数组，加载和展示/渲染前传递，勿放凭据。
 */
@property(nonatomic, copy, nullable) NSDictionary<NSString *, id> *customAdInfo;
/**
 * 弱引用事件接收者，宿主负责保活；销毁后尚未交付的事件会被屏蔽。
 */
@property(nonatomic, weak, nullable) id<JRewardedVideoAdDelegate> delegate;
/**
 * 异步切主线程加载，viewController 用于广告交互呈现；须先初始化成功。重复在途 load 回调
 * alreadyLoading，不取消首请求；结果走 didLoad/didFailToLoad。同一对象复用底层实例，不增加自定义自动重试。
 *
 * @param viewController 用于广告交互呈现的有效控制器，由宿主管理生命周期。
 */
- (void)loadWithViewController:(UIViewController *)viewController NS_SWIFT_NAME(load(viewController:));
/**
 * 终止此对象后续交付并异步释放底层/移除视图，可重复调用；已进入的回调无法撤回。销毁后 load/show
 * 不再生效，需创建新对象。
 */
- (void)destroy;
/**
 * 底层缓存当前是否可展示；销毁或全屏展示中为 NO，不预留缓存，展示时仍可能失败。
 */
@property(nonatomic, readonly, getter=isReady) BOOL ready;
/**
 * 从有效 viewController 展示已就绪缓存，异步切主线程；未就绪/重复展示通过 didFailToShow
 * 报错。调用返回不代表展示成功，以 delegate 为准。
 *
 * @param viewController 用于广告交互呈现的有效控制器，由宿主管理生命周期。
 */
- (void)showFromViewController:(UIViewController *)viewController NS_SWIFT_NAME(show(from:));
@end
/**
 * 原生广告；load 不消费缓存，render 时才取可用对象；首版不暴露多缓存管理。
 */
@interface JNativeAd : NSObject
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
- (instancetype)init NS_UNAVAILABLE;
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
+ (instancetype)new NS_UNAVAILABLE;
/**
 * 以 adUnitID 创建可复用广告对象，复制标识；不加载广告，非法空白标识在 load 时异步报错。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 初始化后的对象；不保证底层 SDK 已初始化或广告已加载。
 */
- (instancetype)initWithAdUnitID:(NSString *)adUnitID NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(adUnitID:));
/**
 * 构造时复制的广告位标识；空白值在 load 时通过 didFailToLoad 报错。
 */
@property(nonatomic, copy, readonly) NSString *adUnitID;
/**
 * 可选场景标识；Banner 在加载、其它样式在展示/渲染时传给底层；nil 表示默认场景。
 */
@property(nonatomic, copy, nullable) NSString *sceneID;
/**
 * 可选自定义信息快照；仅保留字符串/数值及其字典数组，加载和展示/渲染前传递，勿放凭据。
 */
@property(nonatomic, copy, nullable) NSDictionary<NSString *, id> *customAdInfo;
/**
 * 弱引用事件接收者，宿主负责保活；销毁后尚未交付的事件会被屏蔽。
 */
@property(nonatomic, weak, nullable) id<JNativeAdDelegate> delegate;
/**
 * 异步切主线程加载，viewController 用于广告交互呈现；须先初始化成功。重复在途 load 回调
 * alreadyLoading，不取消首请求；结果走 didLoad/didFailToLoad。同一对象复用底层实例，不增加自定义自动重试。
 *
 * @param viewController 用于广告交互呈现的有效控制器，由宿主管理生命周期。
 */
- (void)loadWithViewController:(UIViewController *)viewController NS_SWIFT_NAME(load(viewController:));
/**
 * 终止此对象后续交付并异步释放底层/移除视图，可重复调用；已进入的回调无法撤回。销毁后 load/show
 * 不再生效，需创建新对象。
 */
- (void)destroy;
/**
 * 底层缓存当前是否可展示；销毁或全屏展示中为 NO，不预留缓存，展示时仍可能失败。
 */
@property(nonatomic, readonly, getter=isReady) BOOL ready;
/**
 * 原生模板加载前设置的尺寸；默认 CGSizeZero 交给底层处理，禁止负数或非有限数。
 */
@property(nonatomic) CGSize templateRenderSize;
/**
 * 主线程将 assetView 提供的素材渲染到独立 container，二者不得相同，container 不能是 assetView
 * 的子孙视图。viewController 用于交互；所有素材/点击视图必须属于 assetView。渲染时消费一条就绪缓存；返回 YES
 * 仅表示提交渲染，异步失败仍走 didFailToShow。同步失败 NO 并填 error（参数/线程/未就绪），Swift 抛错。
 *
 * @param assetView 实现素材协议的视图，所有素材/点击视图须位于其层级内。
 * @param container 独立承载容器，不得与 assetView 相同或位于其子孙层级。
 * @param viewController 用于广告交互呈现的有效控制器，由宿主管理生命周期。
 * @param error 可选错误输出指针；失败时写入原因，成功时清空。Swift 接口按 throws 处理。
 * @return YES 表示本次同步操作已接受，NO 表示拒绝；具体结果和异步事件边界见上文。
 */
- (BOOL)renderAssetView:(UIView<JAdNativeRendering> *)assetView
            inContainer:(UIView *)container
         viewController:(UIViewController *)viewController
                  error:(NSError *_Nullable *_Nullable)error NS_SWIFT_NAME(render(assetView:in:viewController:));
/**
 * 已消费并渲染的原生广告底层自定义对象；仅供读取，未渲染/无对象/后台线程/销毁后为 nil。
 */
@property(nonatomic, strong, readonly, nullable) id nativeAdObject;
@end
/**
 * 开屏广告；load/show 使用 UIWindow，窗口须有有效 rootViewController，宿主维护窗口生命周期。
 */
@interface JSplashAd : NSObject
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
- (instancetype)init NS_UNAVAILABLE;
/**
 * 禁止默认构造；请使用该类指定的初始化方法。
 *
 * @return 此构造入口不可用，请使用上文指定入口。
 */
+ (instancetype)new NS_UNAVAILABLE;
/**
 * 以 adUnitID 创建可复用广告对象，复制标识；不加载广告，非法空白标识在 load 时异步报错。
 *
 * @param adUnitID 对应广告样式的广告位标识；不在构造时发起请求。
 * @return 初始化后的对象；不保证底层 SDK 已初始化或广告已加载。
 */
- (instancetype)initWithAdUnitID:(NSString *)adUnitID NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(adUnitID:));
/**
 * 构造时复制的广告位标识；空白值在 load 时通过 didFailToLoad 报错。
 */
@property(nonatomic, copy, readonly) NSString *adUnitID;
/**
 * 可选场景标识；Banner 在加载、其它样式在展示/渲染时传给底层；nil 表示默认场景。
 */
@property(nonatomic, copy, nullable) NSString *sceneID;
/**
 * 可选自定义信息快照；仅保留字符串/数值及其字典数组，加载和展示/渲染前传递，勿放凭据。
 */
@property(nonatomic, copy, nullable) NSDictionary<NSString *, id> *customAdInfo;
/**
 * 弱引用事件接收者，宿主负责保活；销毁后尚未交付的事件会被屏蔽。
 */
@property(nonatomic, weak, nullable) id<JSplashAdDelegate> delegate;
/**
 * 从 window.rootViewController
 * 加载开屏，切主线程读取窗口；窗口无有效根控制器异步报参数错误。加载不等于展示，结果走加载回调。
 *
 * @param window 宿主开屏窗口，须有有效 rootViewController。
 */
- (void)loadWithWindow:(UIWindow *)window NS_SWIFT_NAME(load(window:));
/**
 * 终止此对象后续交付并异步释放底层/移除视图，可重复调用；已进入的回调无法撤回。销毁后 load/show
 * 不再生效，需创建新对象。
 */
- (void)destroy;
/**
 * 底层缓存当前是否可展示；销毁或全屏展示中为 NO，不预留缓存，展示时仍可能失败。
 */
@property(nonatomic, readonly, getter=isReady) BOOL ready;
/**
 * 在 window 中展示已就绪开屏；须有有效根控制器。异步切主线程，参数错误或无可用缓存走 didFailToShow。
 *
 * @param window 宿主开屏窗口，须有有效 rootViewController。
 */
- (void)showInWindow:(UIWindow *)window NS_SWIFT_NAME(show(in:));
@end
NS_ASSUME_NONNULL_END
