#!/usr/bin/env python3
"""Generate Syndicate-style tiles from the trained LoRA through the ComfyUI API.

Builds a Krea2 text-to-image graph (base model in fp8, the `krea2` CLIP type,
our LoRA at full strength) and queues one job per prompt. ComfyUI must be running
with the model files and the LoRA on its search path. Point --url at the server.

  python3 gen_tiles_comfy.py "a large leafy tree with a thick trunk" \
                             "an ornate stone water fountain" \
                             --prefix syndtest --seed 1111

Then bring each result down to a game-resolution 64x48 tile with
tools/pixelate_tile.py.
"""
import argparse, json, urllib.request

TRIG = "syndtile isometric pixel-art tile"
TAIL = "dark dystopian city, retro dos-game pixel art"
NEG = "blurry, photorealistic, 3d render, smooth gradients, text, watermark, jpeg artifacts"


def graph(prompt, seed, prefix, w, h, steps, cfg, shift):
    return {
        "1": {"class_type": "UNETLoader",
              "inputs": {"unet_name": "raw.safetensors", "weight_dtype": "fp8_e4m3fn"}},
        "2": {"class_type": "CLIPLoader",
              "inputs": {"clip_name": "qwen3vl_4b_bf16.safetensors", "type": "krea2"}},
        "3": {"class_type": "VAELoader", "inputs": {"vae_name": "qwen_image_vae.safetensors"}},
        "4": {"class_type": "LoraLoaderModelOnly",
              "inputs": {"model": ["1", 0], "lora_name": "syndtile-krea2-r32.safetensors",
                         "strength_model": 1.0}},
        "5": {"class_type": "ModelSamplingAuraFlow", "inputs": {"model": ["4", 0], "shift": shift}},
        "6": {"class_type": "CLIPTextEncode", "inputs": {"clip": ["2", 0], "text": prompt}},
        "7": {"class_type": "CLIPTextEncode", "inputs": {"clip": ["2", 0], "text": NEG}},
        "8": {"class_type": "EmptySD3LatentImage", "inputs": {"width": w, "height": h, "batch_size": 1}},
        "9": {"class_type": "KSampler",
              "inputs": {"model": ["5", 0], "positive": ["6", 0], "negative": ["7", 0],
                         "latent_image": ["8", 0], "seed": seed, "steps": steps, "cfg": cfg,
                         "sampler_name": "euler", "scheduler": "simple", "denoise": 1.0}},
        "10": {"class_type": "VAEDecode", "inputs": {"samples": ["9", 0], "vae": ["3", 0]}},
        "11": {"class_type": "SaveImage", "inputs": {"images": ["10", 0], "filename_prefix": prefix}},
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("prompts", nargs="+", help="tile descriptions (the trigger and style tail are added)")
    ap.add_argument("--url", default="http://127.0.0.1:8188")
    ap.add_argument("--prefix", default="syndtile")
    ap.add_argument("--seed", type=int, default=42)
    ap.add_argument("--size", default="1024x1024")
    ap.add_argument("--steps", type=int, default=28)
    ap.add_argument("--cfg", type=float, default=4.0)
    ap.add_argument("--shift", type=float, default=3.1)
    a = ap.parse_args()
    w, h = (int(v) for v in a.size.split("x"))

    for i, p in enumerate(a.prompts):
        full = "%s, %s, %s" % (TRIG, p, TAIL)
        g = graph(full, a.seed + i, "%s_%02d" % (a.prefix, i), w, h, a.steps, a.cfg, a.shift)
        data = json.dumps({"prompt": g}).encode()
        req = urllib.request.Request(a.url + "/prompt", data=data,
                                     headers={"Content-Type": "application/json"})
        r = json.load(urllib.request.urlopen(req, timeout=20))
        print("queued #%d (%s): %s" % (r.get("number"), p[:40], r.get("prompt_id", "?")[:8]))


if __name__ == "__main__":
    main()
