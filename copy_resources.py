"""SPM/手动集成：在签名前复制原始TP资源到宿主主bundle。"""
import argparse
import shutil
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--resources',type=Path,required=True);p.add_argument('--destination',type=Path,required=True);a=p.parse_args()
# 先确认两个源bundle都存在，再改目标，避免发现缺件时只复制半套资源。
for name in ['TradPlusAds.bundle','TradPlusADX.bundle']:
    source=a.resources/name
    if not source.is_dir(): p.error('缺少资源：'+name)
# TP按主bundle查找资源；此步骤应在宿主签名前执行，不替换其它宿主资源。
a.destination.mkdir(parents=True,exist_ok=True)
for name in ['TradPlusAds.bundle','TradPlusADX.bundle']:
    target=a.destination/name
    if target.exists(): shutil.rmtree(target)
    shutil.copytree(a.resources/name,target)
