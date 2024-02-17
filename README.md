# force-host-alloction-APU
Inspired by https://github.com/pomoke/torch-apu-helper

PyTorch with ROCm does not take GTT into account when calculating usable VRAM on APU platforms.

With this library, by leveraging LD_PRELOAD now we can intercept calls to hipMalloc and forward them into hipHostMalloc dynamically, without needing to change the original PyTorch code.

Works great with PyTorch to use GTT (hostmemory),  and there is no need to tweak VRAM configuration.

## System configuration (only tested in debian)

```sudo apt install rocminfo rocm-smi rocm-device-libs hipcc```

## Usage

Compile `forcegttalloc.c` with `CUDA_PATH=/usr/ HIP_PLATFORM="amd" hipcc forcegttalloc.c -o libforcegttalloc.so  -shared -fPIC`.

If `hipcc` is not found, it may reside in `/opt/rocm/bin/hipcc`.

Then, for programs using PyTorch, you will need to use 'LD_PRELOAD' to reroute hipMalloc into hipHostMalloc. In my case I have an Ryzen 7 5700G and therefore I have to prepend 'HSA_OVERRIDE_GFX_VERSION=9.0.0', but this changes depending on your APU.

```LD_PRELOAD=./libforcegttalloc.so  HSA_OVERRIDE_GFX_VERSION=9.0.0 python your_pytorch_python_script.py```

## Checking that it works with Stable Diffusion

```python3 -m venv diffuser_env
source diffuser_env/bin/activate
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/rocm5.7
pip install --upgrade diffusers transformers scipy ftfy accelerate
LD_PRELOAD=./libforcegttalloc.so  HSA_OVERRIDE_GFX_VERSION=9.0.0 python test/diffusion.py 'astronaut sitting on the moon' output.jpg cuda
```

Check that everything is working as expected with 'rocm-smi'.

This takes around 2 minutes to generate in my iGPU and around 3 minutes in my CPU.

If you have something work or not work with HIP on APUs, please share it in discussion.

