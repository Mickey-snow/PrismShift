# PrismShift

**PrismShift** は C++26 で実装されたCPUベースのレイトレーサです


## 🔥 ハイライト

| カテゴリ | 詳細 |
| :-- | :-- |
| **レンダリング方式** | Path Tracing |
| **マテリアル / BxDF** |  - Lambertian Diffuse - Conductor / Dielectric - Rough Conductor & Dielectric（GGX マイクロファセット） |
| **幾何** | Sphere / Triangle / Quad |
| **サンプリング** | Multiple Importance Sampling, Cosine-weighted Hemisphere |
| **加速構造** | SAH ベースの BVH |
| **並列化** | ワークスティール式ジョブプール |

---

## 🚀 クイックスタート

```bash
# 1. リポジトリを取得
git clone --recursive https://github.com/mickey-snow/PrismShift
cd PrismShift
# 2. ワンライナー・ビルド
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
# 3. 実行
build/src/prsh [input_file]
```

サニタイザを利用する場合は、以下のようにオプションを付けてビルドします。

```bash
# 2. ビルド
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZER=ON && cmake --build build -j
# 3. 実行
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 build/src/prsh [input_file]
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 build/src/prsh [input_file]
```

---

## ✨ ギャラリー

![1](https://i.imgur.com/iI6EJY3.jpeg)

18サンプル／ピクセルでレンダリングしたコーネルボックスの比較。

左側:マルチインポータンスサンプリングを用いて滑らかで低ノイズな照明を実現

右側:単一のサンプリング手法のみを使用

![2](https://i.imgur.com/qC1TFzb.jpeg)

アセット提供：[ScottGraham](https://www.blendswap.com/blend/13953)。上記のレンダリング画像はいかなる商業目的にも使用されていません。
