SeaAsia 西娅西娅 - DX11+CPU RAY TRACE+ImGui Render.
====
here is New version: https://github.com/ChengGongXTU/SeaAsia_v1.2  
This is a simple render lab for real-time and offline render. The user can check and operate much of parameters of render-object in UI, 
achieve real-time render result in windows, and render any scene by ray trace function.  

Update:  
---------  
* 2017.6.2:  
	 1, Add some material class into ray tracer, and bind it with shade function. It means that, every material could have its own shade function, and user can input different material and shader class in the same scene.    
	 2, Add a direct light ray trace function, and create a sample class for every material class.  
	 3, Delete path trace function.  
	 ![](https://github.com/ChengGongXTU/SeaAsia/blob/master/Direct%20Light.bmp)  
 
* 2017.5.25:  
	I add a ray trace function, witch include Diffuse BRDF, path trace and global illumination function.  
	![](https://raw.githubusercontent.com/ChengGongXTU/SeaAsia/master/Ray%20Trace%20render%20result.bmp)

1.Function
----------
* Transparency: much of paramaters can be see and change in UI, each operator will effects the real-time render result.
* Shader Edit: you can change and input diffrent shader codes repeatedly, and watch th render result in window.
* Ray Trace: render a scene by Ray Trace function.
* The future function:  
	* More Ray Trace function.  
	* Some Non photorealistic rendering function.  
	* and so on ...
  
  
2.UI:  
--------  
![](https://github.com/ChengGongXTU/SeaAsia/blob/master/SeaAsia%20sample.png)
 
 
3.How to use it  
------------  
* install and support:  
	* Download project and open "SeaAisa.sln" by Visual Studio (I write C++ by VS 2015).  
	* Make sure  your VS have input DX11 include and libs.
	* Click  "run", and software can begin working.  
	  
* SeaAsia Usage:  
	* First, check Main menu.  
	It is the top of window, you can use it to open every function window.
	![](https://github.com/ChengGongXTU/SeaAsia/blob/master/3.jpg)  
	  
	* Second, Choose a scene you want to render.  
	Key Frame View shows all the scenes witch can be rendered, I set the max number of scene to be 24.  
	![](https://github.com/ChengGongXTU/SeaAsia/blob/master/4.jpg)  
	  
	* Third, setting render resource.  
	Resource Setting View provide the function of input, including triangle mesh, camera and light input.  
		* 1.input unity by loading traingle mesh, material and texture image file. (Now, it can only load ".obj" and ".mtl"file).  
		* 2.setting camera and light resource.  
		* 3.a simple senen has been built.  
  ![](https://github.com/ChengGongXTU/SeaAsia/blob/master/1.jpg)  
	  
	* Forth, setting render pipeline step by step:  
		* 1.Create shader code.  
		* 2.Setting render scene's ID.  
		* 3.Create Camera and Light constant.  
		And the render result will show in windows.  
	![](https://github.com/ChengGongXTU/SeaAsia/blob/master/2.jpg)  
	
	* More, you can use ray trace buttom to render current scene (make sure that "Ke" is not zero in light-source object ).
	  
4.More suggestion：  
-------------------  

* Change it by yourself:  
I will update a framework image to describe each part of this render. and tell you how to use and change source code, make it become your candy cat.  
* If you have some suggestion about this software, or other idea about computer graphic and non-photorealistic rendering (especially in academically research!), send e-mail to me.  

* My e-mail: 'chenggong.office@foxmail.com'.
