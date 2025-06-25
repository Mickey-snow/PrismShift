#PrismShift

[![CI](https://github.com/Mickey-snow/PrismShift/actions/workflows/ci.yml/badge.svg)](https://github.com/Mickey-snow/PrismShift/actions/workflows/ci.yml)

**PrismShift** は C++23 で実装されたCPUベースのレイトレーサです


## 🔥 ハイライト

| カテゴリ | 詳細 |
| :-- | :-- |
| **レンダリング方式** | Path Tracing |
| **BxDF** |  - Lambertian Diffuse - Conductor / Dielectric - Rough Conductor & Dielectric（GGX マイクロファセット） |
| **幾何** | Sphere / Triangle / Quad |
| **サンプリング** | Multiple Importance Sampling(MIS), Cosine-weighted Hemisphere |
| **加速構造** | BVH |
| **並列化** | SIMD |

---

## 🚀 クイックスタート

### 必要環境

- C++23 をサポートするコンパイラ (Clang 17 / GCC 13 / MSVC 19.38 以上)

- CMake ≥ 3.23

- Git

- OpenEXR

### ビルド & 実行

```bash
# 1. リポジトリを取得
git clone --recursive https://github.com/mickey-snow/PrismShift
cd PrismShift
# 2. ワンライナー・ビルド
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
# 3. レンダリング
build/src/prsh [scene.json]
```

### SIMD 最適化ビルド

SIMD を有効にすると、SSE/AVX 命令を用いた高速化が有効になります。

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_SIMD=ON && cmake --build build -j
```

### サニタイザ付きビルド (デバッグ用)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZER=ON && cmake --build build -j

ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
build/src/prsh [scene.json]
```

---

## ✨ ギャラリー

![1](https://i.imgur.com/iI6EJY3.jpeg)

> 18サンプル／ピクセルでレンダリングしたコーネルボックスの比較。
>
> 左側:マルチインポータンスサンプリングを用いて滑らかで低ノイズな照明を実現
>
> 右側:単一のサンプリング手法のみを使用

![2](https://i.imgur.com/qC1TFzb.jpeg)

> アセット提供：[ScottGraham](https://www.blendswap.com/blend/13953)。レンダリング画像は商用利用を行っていません。

## 📈 ベンチマーク / パフォーマンス

| シーン                | プリミティブ数 | kRays/s<sup>†</sup> |
| :----------------- | ------: | ------------------: |
| Cornell Box | 23 | 204 |
| Splash  | 3007361 | 37.7 |

<sup>†</sup> **測定環境:** Intel Core i9-10900KF @ 3.70GHz, 20 threads, Release ビルド。
