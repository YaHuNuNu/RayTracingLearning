# RayTracingLearning

## 展示

<img src=".\Image\RayTracingByCPUResult.png" alt="RayTracingByCPUResult" style="zoom: 67%;" />

<center>图一	CPU离线渲染</center>

<img src=".\Image\RealTimeRayTracingResult.png" alt="RealTimeRayTracingResult" style="zoom:67%;" />

<center>图二	OpenGL实时光线追踪（单帧降噪）</center>



## 如何构建

项目使用的第三方库都打包到项目中了，可以直接构建，不需要额外配置。

### Ray Tracing By CPU

#### Linux

```
mkdir build
cd build
cmake ..
make
```

#### Windows

```
mkdir build
cd build
cmake ..
```

进入cmake生成的Visual Studio项目，选择Release并生成解决方案，生成结果位于项目根目录中，与CMakeLists.txt处于同级目录。



### Real Time Ray Tracing

#### Linux

由于使用的一些库是在Windows平台上生成的，只支持Windows的API，所以无法在Linux上运行。

#### Windows

```
mkdir build
cd build
cmake ..
```

进入cmake生成的Visual Studio项目，生成解决方案，生成结果位于项目根目录中，与CMakeLists.txt处于同级目录。



## 介绍

### Ray Tracing By CPU

使用 Lambertian 漫反射和 Cook-Torrance 镜面反射组成BRDF：
$$
f_{lambert}=\frac{c}{\pi}
$$

$$
f_{cook−torrance}=\frac{DFG}{4(\omega_{o}\cdot n)(\omega_{i}\cdot n)}
$$

使用的反射方程：
$$
L_{o}(p,\omega_{o})=\int_{\Omega}(k_{d})\frac{c}{\pi}+k_{s}\frac{DFG}{4(\omega_{o}\cdot n)(\omega_{i}\cdot n)}L_{i}(p,\omega_{i})n\cdot\omega_{i}d\omega_{i}
$$
其中法线分布函数使用 Trowbridge-Reitz GGX：
$$
NDF_{GGXTR}(n,h,\alpha)=\frac{\alpha^{2}}{\pi((n\cdot h)^{2}(\alpha^{2}-1)+1)^{2}}
$$
菲涅尔方程使用 Fresnel-Schlick 近似解：
$$
F_{Schlick}(h,v,F_{0})=F_{0}+(1-F_{0})(1-(h\cdot v))^{5}
$$
几何函数G使用 Schlick-GGX：
$$
G_{SchlickGGX}(n,v,k)=\frac{n\cdot v}{(n\cdot v)(1-k)+k}
$$
结合观察方向和光线方向，最终的G项为：
$$
G(n,v,l,k)=G_{sub}(n,v,k)G{sub}(n,l,k)
$$
通过蒙特卡洛方法解渲染积分，对法线分布函数GGX进行重要性采样：
$$
pdf(\phi | \theta)=\frac{1}{2\pi}
$$

$$
\phi=2\pi \xi_{1},\theta=\cos^{-1}\sqrt{\frac{1-\xi_{2}}{(\alpha^{2}-1)\xi_{2}+1}}
$$

这里只列出一些重要的公式，更详细的在下方给出的参考文章中。

综合这些公式每个像素进行大概1000次采样就可以得到不错的结果（图一）。



### Real Time Ray Tracing

使用的渲染原理与上面的 Ray Tracing By CPU 项目相同。

使用第三方模型导入库，支持常见模型格式导入。

对模型网格数据构建 AABB（axis-aligned bounding box），加速光线追踪的求解。

使用HDR贴图提供全局光照。

降噪使用联合双边滤波进行单帧降噪：
$$
J(i,j)=exp(-\frac{\|i-j\|^{2}}{2\sigma^{2}_{p}}-\frac{\|\widetilde{C}[i]-\widetilde{C}[j]\|^{2}}{2\sigma^{2}_{c}}-\frac{D_{normal}(i,j)^{2}}{2\sigma^{2}_{n}}-\frac{D_{plane}(i,j)^{2}}{2\sigma^{2}_{d}})
$$
由于没有进行多帧累加，结果不太理想（图二）。后续会把渲染方程改为迪士尼2012年提出的BRDF，加上多帧累计降噪。



## 参考

[Learn OpenGL](https://learnopengl.com/)

[GAMES101: 现代计算机图形学入门](https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html)

[GAMES202: 高质量实时渲染 ](https://sites.cs.ucsb.edu/~lingqi/teaching/games202.html)

[蒙特卡洛积分 - 知乎](https://zhuanlan.zhihu.com/p/146144853)

[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)

[Disney Principled BRDF实现笔记 - 知乎 ](https://zhuanlan.zhihu.com/p/57771965)
