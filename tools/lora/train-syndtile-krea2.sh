#!/usr/bin/env bash
# Train the Syndicate tile LoRA on Krea2 with musubi-tuner. Trigger word: syndtile.
#
# Three steps: cache the VAE latents, cache the text-encoder outputs, then train
# (rank 32, 2000 steps, saving every 250). Run it from a musubi-tuner checkout
# with the Krea2 model files in place. Paths are for the training workstation, a
# 24GB card. The base is loaded in fp8 so the 26GB model fits.
#
# If a 1024 run edge-deadlocks near the end on a 24GB card, drop resolution to
# 896 in config-syndtile-krea2.toml.
set -euo pipefail

cd "$(dirname "$0")"

KREA2=/mnt/d/ComfyUI/models/krea2
CONFIG=config-syndtile-krea2.toml

echo "=== [0] freeing GPU: docker stop ollama ==="
docker stop ollama || true
trap 'echo "=== restarting ollama ==="; docker start ollama || true' EXIT

echo "=== [1/3] caching latents (VAE) ==="
./.venv/bin/python src/musubi_tuner/krea2_cache_latents.py \
  --dataset_config "$CONFIG" \
  --vae "$KREA2/split_files/vae/qwen_image_vae.safetensors" \
  --skip_existing

echo "=== [2/3] caching text-encoder outputs (Qwen3-VL) ==="
./.venv/bin/python src/musubi_tuner/krea2_cache_text_encoder_outputs.py \
  --dataset_config "$CONFIG" \
  --text_encoder "$KREA2/text_encoders/qwen3vl_4b_bf16.safetensors" \
  --skip_existing

echo "=== [3/3] training (Krea2, rank 32, 2000 steps) ==="
./.venv/bin/accelerate launch --num_cpu_threads_per_process 1 --mixed_precision bf16 \
  src/musubi_tuner/krea2_train_network.py \
  --dit "$KREA2/raw.safetensors" \
  --vae "$KREA2/split_files/vae/qwen_image_vae.safetensors" \
  --dataset_config "$CONFIG" \
  --sdpa --mixed_precision bf16 \
  --fp8_base --fp8_scaled \
  --timestep_sampling krea2_shift --weighting_scheme none \
  --optimizer_type adamw8bit --learning_rate 1e-4 --gradient_checkpointing \
  --max_data_loader_n_workers 2 --persistent_data_loader_workers \
  --network_module networks.lora_krea2 --network_dim 32 --network_alpha 32 \
  --max_train_steps 2000 --save_every_n_steps 250 --save_state --seed 42 \
  --output_dir /mnt/d/musubi-tuner/output \
  --output_name syndtile-krea2-r32
