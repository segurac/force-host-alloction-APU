from os import environ
from sys import argv
import torch
from diffusers import StableDiffusionPipeline

prompt = argv[1]
output_file = argv[2]
device = argv[3]

model_id = "runwayml/stable-diffusion-v1-5"
device = torch.device(device)

pipe = StableDiffusionPipeline.from_pretrained(
    model_id,
    torch_dtype=torch.float32,
    use_auth_token=True)

pipe.safety_checker = None

pipe = pipe.to(device)
image = pipe(prompt, guidance_scale=7.5).images[0]
image.save(output_file)
