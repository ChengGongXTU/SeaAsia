SeaAsia 西娅西娅 - C++/DX11/ImGui/Optix/Tensorflow Ray tracer
====
![Car](https://lh3.googleusercontent.com/pw/ACtC-3dA_kccMLyU0iQ2uNQ2x4KI0kDsLDHXOOtHX5ZLmvFNYKlQ3WKPGNvmOAxPbU5lXYyccEczXgc8lNmUlehlJj-lt5izt39Mdh0zaghRIhhdkYBYXN-lfasOcq_6FykLr00o8a22FqdiJcVRP6WkZZfT=w1180-h881-no?authuser=0)
![Car](https://lh3.googleusercontent.com/pw/ACtC-3c4pApe-0yyP1MkTfi5er6mg9Ty3azo26EbOaJwfWmJA5P2ETcGSbs4pzBGl7gsQf0PLnu8g1Fv1wKfVQEWIic4S82ifhBPmXzYnPAwTK_-Nc8fYxGbJk7cQWOBJXMiin7tnwmgBFfa9M1QP0tLCwHN=w1567-h881-no?authuser=0)
![Car](https://lh3.googleusercontent.com/pw/ACtC-3eF1HW8t0f7Wn0HJlCG3aeUfuLw0z-NW1dcsW3P2X_FXpvccApkr223HuqQYj3YmP9jPAi-ZbMlQ-xvtDa5__jgkP0M_zpV7w1t22WWPKqiLAN2WxjE2L4LEF9_7tpFynunkYdnj4mvVMb4FmSu6ZEM=w1567-h881-no?authuser=0)

SeaAsia is a ray tracing render  which is developed in Win10, it has two parts:

+ Real-time rendering part:

  A deferred rendering part, which is made by C++ and DirectX11.

  A  interaction UI,  which is made by ImGui.

+ Offline ray tracing part:

  A GPU ray tracing part, which is made by Optix.

  A deep learning denoiser, which is made by Tensorflow, the source code comes from NGPT:

  https://github.com/mkettune/ngpt.  

System of development
----------
* Win10 + Visual studio 2017
* CUDA v10.0
* Anaconda2 : python3.7.3 + tensorflow-gpu

Installing steps
----------
* 1st step: 

  Install Visual studio, include windows SDK 10.0.17763, Visual studio 2017(v141), and DirectX.

* 2nd step: 

  Install CUDA v10.0. Because of Optix v5.0,  CUDA's version can be older than v10.0. Add CUDA path to Windows environment variables. 

* 3rd step: 

  Install Anaconda2, and add path to Windows environment variables. Then, input command" conda create -n tensorflow-gpu python=3" in CMD, to create a python3.7.3 environment. Use command"activate tensorflow-gpu" to activate this environment,  then install tensorflow-gpu by pip or conda.

* 4th: 

  Download source code from Github, and run it.

Function 
----------
+ **Real-time rendering**:

  + UI:  use ImGUI framework, 

  + Scene Loading:  support FBX format.

  + Texture Loading: support HDR/non-HDR image.

  + Pipeline: DX11 deferred rendering pipeline with 4 RTs:

    include these info: position/normal/albedo/roughness/metallic/shadow/depth.

  + Color space: linear space

  + Post-process: tone-mapping and linear-to-sRGB. 

  + Material: similar to UE4 standard material, using roughness and metallic to control material.

  + Light: directional/point/spot light for direct lighting, and IBL for environment lighting.

  + Skybox: use HDR image for skybox and IBL

+ **GPU ray tracing**:

  + Optix ray tracing part: include a basic path tracer, and a gradient path tracer.

  + Light: support directional light and Environment lighting.

  + Material: implement Disney standard PBR material, include several sub-materials. 
  
  + Deep learning part: use NGPT as denoise part for gradient path tracer, more detail see this link:
  
    https://github.com/mkettune/ngpt.

More suggestion：  
-------------------  

* Change it by yourself:  
  you can implement your own ray tracing algorithm based on Optix, or improving real-time rendering appearence based on DX11.

* Real-time ray tracing: 

  Optix can be used for real-time ray tracing, but its efficiency isn't satisfied because of using CUDA Core. This render should update to DX12 to use DXR to implement real-time rat tracing.

* If you have some suggestion about this software, or other idea about computer graphic and photorealistic rendering (especially in academically research!), send e-mail to me.  

* My e-mail: 'chenggong.office@foxmail.com'.
