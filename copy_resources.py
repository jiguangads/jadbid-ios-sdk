"""在宿主签名前复制并核对 JAdbid 与所选厂商的主 Bundle 资源。"""

import argparse
import hashlib
import os
import plistlib
import shutil
import tempfile
from pathlib import Path


CORE = ('TradPlusAds.bundle', 'TradPlusADX.bundle')
# 锁定 Pods 资源脚本的候选名称；非 Pods 的最终归属仍须逐源核对。
VENDOR = {
    'AdMob': ('GoogleMobileAdsResources.bundle', 'UserMessagingPlatformResources.bundle'),
    'Bigo': ('BigoADSRes.bundle', 'BigoADS.bundle'),
    'Facebook': ('FBAudienceNetwork.bundle',),
    'Fyber': ('Fyber_Marketplace_SDK.bundle',),
    'InMobi': (),
    'IronSource': ('IronSourceAdQualityPrivacyInfo.bundle', 'IronSourcePrivacyInfo.bundle'),
    'Mintegral': ('MTGResources.bundle',),
    'Pangle': ('PAGAdSDK.bundle', 'AdsGlobalSDK.bundle', 'TikTokBusinessSDK.bundle'),
    'Unity': ('UnityAdsResources.bundle', 'UnityCoherenceLib.bundle',
              'IronSourceAdQualityPrivacyInfo.bundle'),
    'Vungle': ('VungleAds.bundle',),
    'Yandex': ('MobileAdsBundle.bundle', 'YandexMobileAds_PrivacyInfo.bundle',
               'KSCrashCore.bundle', 'KSCrashRecording.bundle', 'KSCrashRecordingCore.bundle',
               'AppMetricaAdSupport.bundle', 'AppMetricaCore.bundle',
               'AppMetricaCoreExtension.bundle', 'AppMetricaCoreUtils.bundle',
               'AppMetricaCrashes.bundle', 'AppMetricaEncodingUtils.bundle',
               'AppMetricaFMDB.bundle', 'AppMetricaHostState.bundle', 'AppMetricaIDSync.bundle',
               'AppMetricaIdentifiers.bundle', 'AppMetricaKeychain.bundle',
               'AppMetricaLibraryAdapter.bundle', 'AppMetricaLog.bundle',
               'AppMetricaLogSwift.bundle', 'AppMetricaNetwork.bundle',
               'AppMetricaPlatform.bundle', 'AppMetricaProtobuf.bundle',
               'AppMetricaProtobufUtils.bundle', 'AppMetricaStorageUtils.bundle',
               'AppMetricaSynchronization.bundle'),
}


def bundle_digest(path):
    """拒绝链接和特殊文件，按相对路径与内容形成稳定摘要。"""
    if path.is_symlink() or not path.is_dir():
        raise ValueError(f'资源不是普通目录：{path}')
    digest = hashlib.sha256()
    for root, dirs, files in os.walk(path, followlinks=False):
        dirs.sort()
        for name in sorted(dirs + files):
            item = Path(root) / name
            if item.is_symlink():
                raise ValueError(f'资源包含符号链接：{item}')
            if not item.is_dir() and not item.is_file():
                raise ValueError(f'资源包含特殊文件：{item}')
            relative = item.relative_to(path).as_posix().encode()
            digest.update(b'D' if item.is_dir() else b'F')
            digest.update(len(relative).to_bytes(4, 'big'))
            digest.update(relative)
            if item.is_file():
                digest.update(item.stat().st_size.to_bytes(8, 'big'))
                with item.open('rb') as stream:
                    for chunk in iter(lambda: stream.read(1024 * 1024), b''):
                        digest.update(chunk)
    return digest.hexdigest()


def vendor_bundles(root):
    """收集实际 bundle，拒绝符号链接入口。"""
    if root is None:
        return {}
    if root.is_symlink() or not root.is_dir():
        raise ValueError('厂商资源根目录不存在或为符号链接')
    found = {}
    for parent, dirs, _ in os.walk(root, followlinks=False):
        for name in list(dirs):
            path = Path(parent) / name
            if path.is_symlink():
                raise ValueError(f'厂商资源包含符号链接：{path}')
            if name.endswith('.bundle'):
                found.setdefault(name, []).append(path)
                dirs.remove(name)
    return found


def bigo_privacy_manifest(root):
    """5.3.0 Podspec 把设备切片原始隐私清单生成独立 BigoADS bundle。"""
    if root is None:
        return None
    matches = [
        path for path in root.rglob('PrivacyInfo.xcprivacy')
        if path.parts[-4:] == ('BigoADS.xcframework', 'ios-arm64',
                              'BigoADS.framework', 'PrivacyInfo.xcprivacy')
    ]
    if len(matches) > 1:
        raise ValueError('Bigo 隐私清单输入不唯一')
    if not matches:
        return None
    manifest = matches[0]
    if manifest.is_symlink() or not manifest.is_file():
        raise ValueError('Bigo 隐私清单输入不是普通文件')
    plistlib.loads(manifest.read_bytes())
    return manifest


def check_bigo_privacy(bundle, original=None):
    privacy = bundle / 'PrivacyInfo.xcprivacy'
    if privacy.is_symlink() or not privacy.is_file():
        raise ValueError('Bigo 隐私 bundle 缺少原始清单')
    data = privacy.read_bytes()
    try:
        parsed = plistlib.loads(data)
    except (ValueError, TypeError) as error:
        raise ValueError('Bigo 隐私清单格式无效') from error
    if not isinstance(parsed, dict) or not any(key.startswith('NSPrivacy') for key in parsed):
        raise ValueError('Bigo 隐私清单字段无效')
    if original is not None and data != original.read_bytes():
        raise ValueError('Bigo 隐私资源与厂商原件不一致')


def copy_resources(resources, destination, source=None, vendor_root=None, verify_final=False):
    """只复制选源白名单；冲突、缺件和非选源输入在写目标前拒绝。"""
    if source is not None and source not in VENDOR:
        raise ValueError(f'未知广告源：{source}')
    if resources.is_symlink() or destination.is_symlink():
        raise ValueError('资源输入或目标不能是符号链接')
    if vendor_root is not None and source is None:
        raise ValueError('厂商资源目录必须同时指定广告源')
    candidates = vendor_bundles(vendor_root)
    generated = {}
    bigo_manifest = bigo_privacy_manifest(vendor_root) if source == 'Bigo' else None
    if bigo_manifest is not None and 'BigoADS.bundle' not in candidates:
        generated['BigoADS.bundle'] = bigo_manifest
    allowed = set(VENDOR.get(source, ()))
    unexpected = sorted(set(candidates) - allowed)
    if unexpected:
        raise ValueError('厂商目录含未选源或未知资源：' + ', '.join(unexpected))
    names = CORE + VENDOR.get(source, ())
    if not resources.is_dir():
        raise ValueError('JAdbid 资源目录不存在')
    for name in CORE:
        bundle_digest(resources / name)
    known_vendor = set().union(*map(set, VENDOR.values()))
    if destination.exists():
        contamination = sorted(
            path.name for path in destination.iterdir()
            if path.name in known_vendor and path.name not in allowed
        )
        if contamination:
            raise ValueError('最终 App 含未选源资源：' + ', '.join(contamination))
    selected = {}
    for name in names:
        inputs = [resources / name] if name in CORE else candidates.get(name, [])
        if len(inputs) > 1 and len({bundle_digest(path) for path in inputs}) != 1:
            raise ValueError(f'厂商资源同名异摘要：{name}')
        if inputs:
            bundle_digest(inputs[0])
            if name == 'BigoADS.bundle':
                check_bigo_privacy(inputs[0], bigo_manifest)
            selected[name] = inputs[0]
        if verify_final and name not in CORE:
            if not inputs and name not in generated:
                raise ValueError(f'最终核验缺少厂商资源原件：{name}')
            if name == 'BigoADS.bundle' and bigo_manifest is None:
                raise ValueError('最终核验缺少 Bigo 原始隐私清单')
        target = destination / name
        if target.is_symlink():
            raise ValueError(f'目标资源为符号链接：{name}')
        if target.exists():
            target_hash = bundle_digest(target)
            if inputs and target_hash != bundle_digest(inputs[0]):
                raise ValueError(f'目标资源同名异摘要：{name}')
            if name == 'BigoADS.bundle':
                check_bigo_privacy(target, bigo_manifest)
        elif name in CORE and name not in selected:
            raise ValueError(f'缺少资源：{name}')
        elif verify_final:
            raise ValueError(f'最终 App 缺少资源：{name}')
        elif name not in selected and name not in generated:
            raise ValueError(f'缺少所选源资源：{name}')
    if verify_final:
        return
    destination.mkdir(parents=True, exist_ok=True)
    for name, path in selected.items():
        target = destination / name
        if target.exists():
            continue
        with tempfile.TemporaryDirectory(prefix='.jadbid-resource-', dir=destination) as staging:
            temporary = Path(staging) / name
            shutil.copytree(path, temporary, symlinks=True)
            if bundle_digest(temporary) != bundle_digest(path):
                raise ValueError(f'资源复制后摘要漂移：{name}')
            temporary.rename(target)
    for name, manifest in generated.items():
        target = destination / name
        if target.exists():
            continue
        with tempfile.TemporaryDirectory(prefix='.jadbid-resource-', dir=destination) as staging:
            temporary = Path(staging) / name
            temporary.mkdir()
            shutil.copy2(manifest, temporary / 'PrivacyInfo.xcprivacy')
            (temporary / 'Info.plist').write_bytes(plistlib.dumps({
                'CFBundleName': 'BigoADS', 'CFBundlePackageType': 'BNDL',
                'CFBundleShortVersionString': '5.3.0', 'CFBundleVersion': '1',
            }))
            if (temporary / 'PrivacyInfo.xcprivacy').read_bytes() != manifest.read_bytes():
                raise ValueError('Bigo 隐私清单复制后漂移')
            temporary.rename(target)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--resources', type=Path, required=True)
    parser.add_argument('--destination', type=Path, required=True)
    parser.add_argument('--source', choices=tuple(VENDOR))
    parser.add_argument('--vendor-root', type=Path)
    parser.add_argument('--verify-final', action='store_true')
    args = parser.parse_args()
    try:
        copy_resources(args.resources, args.destination, args.source, args.vendor_root,
                       args.verify_final)
    except ValueError as error:
        parser.error(str(error))


if __name__ == '__main__':
    main()
