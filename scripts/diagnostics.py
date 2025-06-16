import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
import argparse
import sys


def load_image_float(path):
    """RGB PNG → float32 ndarray in linear [0,1]."""
    return np.asarray(Image.open(path).convert("RGB"),
                      dtype=np.float32) / 255.0


def basic_stats(img):
    stats = {}
    for i, name in enumerate("RGB"):
        p = img[..., i]
        stats[name] = dict(mean=float(p.mean()),
                           std=float(p.std()),
                           min=float(p.min()),
                           max=float(p.max()))
    return stats


def _box_blur(img, k=5):
    """Tiny, dependency-free mean filter (k×k, odd k)."""
    pad = k//2
    pad_img = np.pad(img, ((pad,pad),(pad,pad),(0,0)), mode="reflect")
    # summed-area table trick
    cumsum = np.cumsum(np.cumsum(pad_img, axis=0), axis=1)
    out = (cumsum[k:, k:] - cumsum[:-k, k:] -
           cumsum[k:, :-k] + cumsum[:-k, :-k]) / (k*k)
    return out


def estimate_noise(img, k=5):
    """
    Return (sigma, residual_map) where
      residual = img - box_blur(img, k)
      sigma    = residual.std()
    """
    blur = _box_blur(img, k)
    residual = np.resize(img, blur.shape) - blur
    return float(residual.std()), residual


def saturation_ratio(img, thresh=0.98):
    """Fraction of pixels where *any* channel is brighter than thresh."""
    return float((img >= thresh).any(axis=-1).mean())


def border_check(img, thresh=1e-3):
    """
    Quick heuristic for the 1-pixel black-frame bug.
    Returns True if border looks suspiciously darker than the core.
    """
    h, w, _ = img.shape
    border_mean = np.concatenate([
        img[0, :, :], img[-1, :, :], img[:, 0, :], img[:, -1, :]
    ]).mean()
    core_mean   = img[1:h-1, 1:w-1, :].mean()
    return (core_mean - border_mean) > thresh


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="diagnostics")
    parser.add_argument('--filename', required=True)
    args = parser.parse_args()
    
    img = load_image_float(args.filename)

    print("== Basic RGB stats ==")
    for c, s in basic_stats(img).items():
        print(f"{c}: {s}")

    σ, res = estimate_noise(img, k=5)
    print(f"\nApprox. noise σ : {σ:.4f}")
    print(f"Saturated ratio : {saturation_ratio(img):.2%}")
    print("Border too dark :", border_check(img))


    plt.imshow(np.clip(res*4 + 0.5, 0, 1))
    plt.title("Residual (noise) map"); plt.axis("off")
    plt.show()
