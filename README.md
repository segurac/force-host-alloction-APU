# force-host-allocation-APU
Inspired by https://github.com/pomoke/torch-apu-helper

PyTorch with ROCm does not take GTT into account when calculating usable VRAM on APU platforms.

With this library, by leveraging LD_PRELOAD now we can intercept calls to hipMalloc and forward them into hipHostMalloc dynamically, without needing to change the original PyTorch code.

Works great with PyTorch to use GTT (hostmemory),  and there is no need to tweak VRAM configuration.

## System configuration (only tested in debian)

```sudo apt install rocminfo rocm-smi rocm-device-libs hipcc```

### System configuration for Ubuntu provided by another user. [User report](https://github.com/segurac/force-host-alloction-APU/issues/1#issuecomment-2013727581) :

Links to ROCm and PyTorch versions I used:
[https://www.amd.com/en/support/linux-drivers](https://www.amd.com/en/support/linux-drivers)
[https://pytorch.org/get-started/locally/](https://pytorch.org/get-started/locally/)

ROCm 5.7 with PyTorch for 5.7
```
sudo apt update
wget https://repo.radeon.com/amdgpu-install/5.7.3/ubuntu/jammy/amdgpu-install_5.7.50703-1_all.deb
sudo apt install ./amdgpu-install_5.7.50703-1_all.deb
sudo amdgpu-install -y --usecase=graphics,rocm
sudo usermod -a -G render,video $LOGNAME
pip3 install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/rocm5.7
```
ROCm 6.0 with PyTorch for 6.0
```
sudo apt update
wget https://repo.radeon.com/amdgpu-install/23.40.2/ubuntu/jammy/amdgpu-install_6.0.60002-1_all.deb
sudo apt install ./amdgpu-install_6.0.60002-1_all.deb
sudo amdgpu-install -y --usecase=graphics,rocm
sudo usermod -a -G render,video $LOGNAME
pip3 install --pre torch torchvision torchaudio --index-url https://download.pytorch.org/whl/nightly/rocm6.0
```



## Usage

Compile `forcegttalloc.c` with `CUDA_PATH=/usr/ HIP_PLATFORM="amd" hipcc forcegttalloc.c -o libforcegttalloc.so  -shared -fPIC`.

If `hipcc` is not found, it may reside in `/opt/rocm/bin/hipcc`.

Then, for programs using PyTorch, you will need to use 'LD_PRELOAD' to reroute hipMalloc into hipHostMalloc. In my case I have an Ryzen 7 5700G and therefore I have to prepend 'HSA_OVERRIDE_GFX_VERSION=9.0.0', but this changes depending on your APU.

```LD_PRELOAD=./libforcegttalloc.so  HSA_OVERRIDE_GFX_VERSION=9.0.0 python your_pytorch_python_script.py```

### Usage reports by another user
I have a 5600G APU and Stable Diffusion works for me with `force-host-alloction-APU` and only 512MiB allocated to VRAM.

First, for Ryzen 5 2400GE you should probably use `HSA_OVERRIDE_GFX_VERSION=9.0.0` like me. For Stable Diffusion I'm using [Fooocus](https://github.com/lllyasviel/Fooocus), which I start it like this:
```
LD_PRELOAD=~/force-host-alloction-APU/./libforcegttalloc.so python3 ~/Fooocus/entry_with_update.py --always-high-vram
```
Notice a `./` before `libforcegttalloc.so`. This works on ROCm version 5.7.3, but with one additional environment variable `HSA_ENABLE_SDMA=0` I can use even the newest ROCm 6.0.

## Checking that it works with Stable Diffusion

```
python3 -m venv diffuser_env
source diffuser_env/bin/activate
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/rocm5.7
pip install --upgrade diffusers transformers scipy ftfy accelerate
LD_PRELOAD=./libforcegttalloc.so  HSA_OVERRIDE_GFX_VERSION=9.0.0 python test/diffusion.py 'astronaut sitting on the moon' output.jpg cuda
```

Check that everything is working as expected with 'rocm-smi'.

This takes around 2 minutes to generate in my iGPU and around 3 minutes in my CPU.

If you have something work or not work with HIP on APUs, please share it in discussion.

