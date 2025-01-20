### SwapChain

* vulkan没有default framebuffer，用来hold render buffer的是swapchain，必须显式创建。
* 可以将swapChain理解为一个image queue，工作方式类似于生产者消费者模型（app去acquire一个image, draw之后等待上屏）

#### Checking For swapChain Support

* 使用```vkEnumerateDeviceExtensionsProperties```获取physical device存在的extensions，然后和required extensions去比对，required的所有如果都在，则说明device是符合需求的。
* present queue family的存在其实已经表示swap chain的存在（只不过教程这里做了显示检查）

#### Enabling device Extensions

* 在logicDevice的createInfo中做出对应改动（enableExtensionCnt, ppEnableExtensionNames)

#### Querying details of swap chain support

* 仅仅swapChain Extension存在是不够的，还需要检查swapChain是否和window surface适配，一般从三个方面进行detailed checking:
  * Basic surface capabilties(surface最大最小数量，surface image的最大最小宽高)
  * Surface format(pixel format, color space, etc)
  * Presentation modes(**这个尤为重要**)
* 查询extension support的过程是类似的，需要注意的是只有extension支持了，我们才需要去看device是否有和surface相符的swapchain，因此swapchain support的检查需要放在extension检查后面，extension检查好了以后，我们才需要检查swapchain support。

#### Choosing the Right Settings for SwapChain

* Choosing the right surface Format

一般来说```format```和```colorSpace```正确即可，正常的srgb各8位，color space为正常的nolinear即可

* Presentation Mode

其实要清楚一个概念，swapchain管理的是一系列图片，我们获取引用，然后可以再发出指令让显示设备去显示这些图片。

显示的过程一般来说是垂直去做的（从上往下），水平刷新比垂直刷新要快的多，垂直刷新一般的频率是60HZ。

Presentation Mode需要解决的问题是应用draw速度和显示器刷新速度匹配的问题。

1. VK_PRESENT_MODE_IMMEDIATE_KHR

顾名思义，就是显示器图片还没有刷新完，就将应用给定的图片给上传了，导致上半部分画的是原来的图片，下半部分画的是新的图片，因此这种模式适用于显示器刷新频率比应用render频率高的情况。

2. VK_PRESENT_MODE_FIFO_KHR

 vertical_blank，指代一个时间点，过了这个时间点我们就可以安全地将呈现给显示器的图片源交换出去。 

vertical blanking period, 两个vertical blank的时间

FIFO会等待下一个vertical blank的到来再去切换显示的data source.

3. VK_PRESENT_MODE_FIFO_RELAXED_KHR

4. VK_PRESENT_MODE_MAILBOX_KHR

* Swap Extent

本质是选择swapchain中image view的resolution，width, height的范围定义在```VkSurfaceCapabilitiesKHR```中，一般来说在```currentExtent```中定义好了，如果设置为uint32_t的最大值，用户可以自行按照window进行匹配。

**Vulkan设置swap extent的单位是pixel, 对于苹果设备的视网膜分辨率，pixel数量可能远大于屏幕坐标，因此我们需要```glfwGetFramebufferSize```提前查询一下pixel width和pixel height**, 最后利用VkSurfaceCapabilitiesKHR中的数据（min/max height/width）做一下clamp即可。

#### Creating the Swap Chain

利用上述信息补充createInfo进行创建，额外提到的几点：

* imageSharingMode
* preTransform
* compositeAlpha
* oldSwapchain

知道这几个对应的含义就ok

### Image Views

其实就是Image不能直接用来draw东西，需要在Image的基础上创建一个类似View的东西，在某种程度上和string_view有点像

## Graphics Pipeline Basics

### Introduction

掠过，讲的一些尝试性质的东西

### Shader Module

* 一帮从glsl到spir-v，使用glslc编译
* shader module只是一个wrapper，真正执行编译和链接的时候是pipeline build的时候， 因此recreate pipeline的时候shader module可以销毁并且重新创建
* 另外有一点要提到的就是```pSpecializationInfo```，这个可以用来指定const varialble的值，const variable的if操作可以被编译器优化掉，因此可以利用const variable使得一个shader module有不同的表现行为，因此，可以多利用这种方式而非利用variable来做condition branch.

### Fixed Functions

* 除了需要配置可编程阶段之外，因为pipeline在创建之后immutable的缘故，我们应该在创建的时候将fixed fucntion stage也配置好。

* 有一些属性可以在不重新创建pipeline的情况下进行修改，同样的，你也需要在每次draw之前手动去设置

  * size of viewport
  * line width
  * blend constant

  需要创建额外的dynamic statecreate info，更多详细信息参考[vulkan spec](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDynamicState.html)

* 所有需要配置的阶段这里稍微列一下，过于冗长，看到关键词去搜索即可

  * Vertex Input
  * Input Assembly
  * Viewports and scissors 
  * Rasterizer
  * Multisampling
  * Depth and stencil testing
  * Color blending
  * Pipeline layout (for uniform usage)

### Render Passes

* 在创建pipeline之前，我们需要告诉vulkan需要的framebuffer attachment信息（不太懂教程为什么把这段放在pipeline后面）
* 每个renderpass需要绑定一系列的attachments以及对应的subpass

## Drawing

### Framebuffers

在最终绘制之前，还需要将每个swapchain中的imageView包一下，framebuffer正是这样一个wrapper.

### Command buffers

* Vulkan中draw不是简单的function call，需要把draw command记录在command buffer object中，然后递交进行绘制。
* 每一个command pool只能递交一种类型的command，所以在创建command pool的时候需要制定command pool对应的queueFamily index。

* command buffer allocation: 与其他struct类似，但是不叫xxxCreateInfo, 叫```VkCommandBufferAllocateInfo```，其余和其他类似结构创建类似。
* 需要注意```VkCommandBufferAllocateInfo.level```， 分为PRIMARY和SECONDARY，只有primary才可以直接递交到队列中执行，second可以被primary所引用，可以作为复用部分。
* command buffer recording: 主要就是flags这个标记需要注意一下，one_time_submit_bit, render_pass_continue_bit, simultaneous_use_bit，分别对应不同的使用场景。
* 另外值得注意的成员是```pInheritanceInfo```, 这仅仅与secondary command buffer相关，描述的是从primary command buffer中继承的属性。
* 如果一个command buffer已经被record过了， 那么vkBeginCommandBuffer将重置这个command buffer，这意味着不能通过这种方法去重置command，因为你的command buffer已经递交了。
* starting a renderpass: 绘制从```vkCmdBeginRenderPass```开始，配置结构为```VkRenderPassBeginInfo```.可以总结出需要先beginCommandBuffer, 然后vkCmdBeginRenderPass，一般来说record command都是vkCmd打头。
* Basic Drawing Commands: 
  * vkCmdBindPipeline
  * vkCmdSetViewport
  * vkCmdSetScissor
  * vkCmdDraw
    * `vertexCount`: Even though we don't have a vertex buffer, we technically still have 3 vertices to draw.
    * `instanceCount`: Used for instanced rendering, use `1` if you're not doing that.
    * `firstVertex`: Used as an offset into the vertex buffer, defines the lowest value of `gl_VertexIndex`.
    * `firstInstance`: Used as an offset for instanced rendering, defines the lowest value of `gl_InstanceIndex`.
* 最后需要```vkEndCommandBuffer```收尾，这个函数是由返回值的，可以由此判断command record是否成功。

### Rendering and Presentation

* Outline of a Frame(由以下几个部分组成):

  * Wait for the previous frame to finish(我觉得这里不准确，不一定要等待，如果swapchain比较大，或者上屏比draw快)
  * Acquire an image from the swap chain
  * Record a command buffer which draws the scene onto that image
  * Submit the recorded command buffer
  * Present the swap chain image

* Synchronization（GPU操作需要显示同步）

  * 很多操作都是异步的，因此需要我们手动去做同步（我的理解是手动去做fence，例如某个操作需要在另外一个操作之后执行，这时候就需要semaphore之类的同步原语，做一个生产者消费者之类机制，当然这只是一个简单例子）

  * vk function call是异步的，我们call一个function，他只是dispatch这个job给gpu去做，但实际上做没做完这个function call是不管的，所以function call的result需要手动查询。

  * draw triangle中需要手动同步的操作（因为这些操作都happen on GPU)

    * Acquire an image from the swap chain
    * Execute commands that draw onto the acquired image
    * Present that image to the screen for presentation, returning it to the swapchain(两步操作，swapchainImage->screen, return image to swapchain)

  * Semaphores

    * 两个种类，一个是binary semaphore, 另外一个是timeline semaphore，前者只有0，1的机制，后者就是一个uint64，到reference value触发特定的操作。
    * 教程主要介绍的是binary semaphore，初始化为unsignaled， 一个queue option结束之后，给另外一个wait的queue发信号，另一个wait的queue发现semaphore被signaled之后开始执行，执行完然后再次将semaphore设置为unsignaled状态。
    * **这里其实有一个问题，不太知道GPU同步的具体机制，semaphore是global还是说两个queue相当于两个thread的概念**

  * Fences

    * cpu同步gpu的信息需要用到这个（cpu需要知道gpu上的某件事已经做完了）

    * attach fence obj to queue work, and wait the fence to be signaled on cpu.

      ```cpp
      VkCommandBuffer A = ... // record command buffer with the transfer
      VkFence F = ... // create the fence
      
      // enqueue A, start work immediately, signal F when done
      vkQueueSubmit(work: A, fence: F)
      
      vkWaitForFence(F) // blocks execution until A has finished executing
      
      save_screenshot_to_disk() // can't run until the transfer has finished
      ```

    * 这种方式不是最优方式（自旋等待），应该是由一个信号去做这个事情，在等待期间去做别的事情，之后可能会提到其他的同步原语。

  * Subpass dependencies

    有点类似与shader graph，指定前面的subPass到哪个stage才可以结束，后面的subPass到哪个stage哪个操作才需要wait。

### A little Summary

这么多set up，具体做了哪些事情呢？

* 创建window(这里不同os上可能不一样)
* 初始化vulkan
  * 创建instance（program和vulkan API沟通的proxy)
  * pick physical device(先枚举出所有的physical device，之后去检测每个device是否qualified，是否qualified的标准是，拥有所有需要的queue family；是否有我们需要的extension，swapChain support是否ok，例如present mode, swapchain formats以及一些details(extend2d)).
  * create logical device(按需创建，指定我们需要的queue family, device feature, device extensions, 以及enable的layers，最后创建logical device)
  * create swapchain(选择对应的format, present mode, extent2D以及和queue family的一系列设置，最后需要query到swapchain内部image的引用vkGetSwapchainImagesKHR)
  * createImageViews：主要要在swapchainImage上面包一层，其实后面还会包一层来构成framebuffer。
  * createRenderPass：我理解就是给定类似于render graph的流程以及每个subpass的资源，包括资源格式种种，细节之后可以继续深入了解。
  * createGraphicPipeline：这里是设定的gpu管线的每个流程，包括program stage以及fixed function stage
  * createFramebuffer: framebuffer就是在imageView上再包了一层
  * createCommandPool: 根据需要的queue Family创建，比如教程中就是创建了graphic queue family的command PoolshandBuffer：在对command pool中，创建command buffer，之后的record以及submit都会使用这个buffer
  * createSyncObjects： vulkan需要显示同步，包括gpu上的work同步，gpu和cpu上的work同步

* mainLoop
  * poolevent： 没什么好说的，每个窗口系统收集事件的方式
  * drawFrame: 同步，record command buffer, submit and present
    * 首先就是wait上一帧结束的fence，应该说上一帧command queue提交之后会设置为signaled的fence
    * 之后就是record command buffer（大致的流程总结）
      * Reset command buffer
      * begin command buffer
      * begin render pass
      * set dynamic props
      * vk cmd draw
      * vk cmd end command buffer
    * 填写command submit info
      * 等待的信号量，stage mask，数量
      * command buffer数量
      * 结束的时候signal semaphore信息
    * 最后递交也可以指定被signal的fence

### Frames In Flight  && Recreate Swapchain

* 提高并行度，其实就是将同步的信号量以及command buffer都设置为大于一的数组，swapchain这里不设置是因为之前已经有设置过了(swapchain之前的数量就不是1， minImageCount + 1)？
* 基础的同步使用```vkDeviceIdle```，还有```vkQueueIdle```之类的函数，之后可以拓展的时候去看。
* 可以从```vkAcquireNextImageKHR```以及```VKQueuePresentKHR```获取swapchain不在匹配的信息，return value是```VK_ERROR_OUT_OF_DATA_KHR```或者```VK_SUBOPTIMAL_KHR```均可表示需要重新创建swapchain
* swap chain extent需要non zero，因此没有对最小化进行额外处理会报swapchain extent为0的错误。

## Vertex Buffers

### Vertex Input Description

* 首先就是layout qualifier, 有的数据类型会占据多个slot，注意下一个data的location qualifier就需要往后夺延几个了。
* 2 types are needed: ```VkVertexInputBindingDescription```以及```VkVertexInputAttributeDescription```, 可以记后缀就好了，bindingDescription, attributeDescription
* 可以这么理解，一个entry里面有几个数据（vertex, color, normal, uv），我们就需要几个attributeDescription.
* 上述两个数据相当于告诉GPU怎么去提取数据，具体如何帮定数据在下一节提到

### Vertex Buffer Creation

* VK创建buffer使用的是一个通用的结构：```VkBufferCreateInfo```， 指定usage即可指定vertex buffer类型。
* Vk中创建了buffer之后，还需要手动的去给buffer分配memory，第一步就是去query memory requirements： ```vkGetBufferMemoryRequirements```
* 需要从物理设备中去query memory的properties, 然后从根据vertexBuffer中生成的```VKMemoryRequireMents```中获取memoryAllocation所需要的信息（size, memoryTypeIndex)
* 拿到分配的memory之后，需要时用VkMapMemory和VkUnmapMemory来写buffer，这里引申出另外一个问题，往对应map的内存地址写数据，cpu可能不会马上拷贝，所以在申请memory的是memoryType需要具有coherent（强行同步）的flag bit。或者是每次在写buffer之后flush，并且在每次read buffer之前InvalidateMappedMemoryRanges
* 最后就是需要在每次record command buffer的时候去bind vertex buffer

### Staging buffer

- 初印象是看上去，不使用coherent mode（```VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT```是更加高效的方式），然后利用staging buffer做中转，而不是直接map到cpu端写vertex buffer的方式。
- 需要用到VK_BUFFER_USAGE_TRANSFER_BIT_SRC和VK_BUFFER_USAGE_TRANSFER_BIT_DST这俩flag，actual vertex buffer设置为device local，staging buffer才需要和cpu端进行同步
- 如果创建vkdevice时候，用到了专门的transfer queue，可以额外创建一个command pool，这样可以对copy memory时memory allocation这一步进行优化
- 继续重复一下cmd buffer的流程，alloc->begin record->record anything you want(vkcmdxxx, vkcmdxx)->vkqueuesubmit ->wait the cmd to finish->free the command buffers, as well as any resources you've allocated.

### Index Buffer

* 概念其实无需多提，学gl的时候已经接触过了，主要熟悉一下API的流程
* 关于资源创建，有一点比较重要，教程中是分开创建buffer，然后offset都为0，但是其实更加高效的方式是创建一个buffer,然后不同的offset，代表不同的数据。甚至两个不同时使用的资源可以使用相同位置的数据（很多vk函数会一些特定的flag来指定这种用法）。

## Uniform Buffers

> 围绕resource descriptor这个概念展开:
>
> * descriptor layout(给出resource的类型，内存排布)
> * Allocate a descriptor set from a descriptor pool（descriptor set这里对应的具体buffer, uniform buffer object是众多descriptors中的一种)
> * bind the descriptor set

### Descriptor Layout

每个VkPipelineLayoutCreateInfo会包含多个descriptor set layout, 每个VkDescriptorSetLayout对应多个bindings，这么设计的原因还不清楚。

### Uniform buffer

给实际的descriptor set数据创建容器(buffer handle, device memory, mapped to host end pointers)

### Descriptor set(create Pool and set instance)

* 通过descriptor pool创建descriptor set
* 比较重要的一点是手动设定alignment, 不清楚可以去回顾一下。
* 这边笔记没有记录得很全，可以之后回顾的时候再次

## Texture Mapping

### Images

添加图片需要如下几个步骤：

1. create image object(backed by device memory)
2. Fill this image object with pixel data from image file
3. create an image sampler
4. add a combined image sampler(我的理解是descriptor和image sampler相结合)

* 首先第一二步，可以像vertex buffer一样创建一个stage image object，map一下，再写入。也可以直接创建一个vkbuffer，将pixel数据写入到buffer中，然后再从buffer拷贝到image object.
* 另外需要注意的点是image layout，不同的layout performance差异比较大
  * VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: best for presentation
  * VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: best for writing colors
  * VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: 最适合用于采样
* pipeline barrier: 用于同步资源操作，在这里可以用于同步image的layout变换，也可以transfer queue family ownership.
* 可以将texture image object类比于Vkbuffer, 本质上是一个handle，对应的memory还需要手动分配，分配buffer的时候, 需要查询memoryType, 根据memoryType allocate不同类型的memory，最后将handle和memory进行绑定(同理，使用的最佳实践是一块memory对应多个handle，根据offset去确定绑定位置)
* VkPhysicalDeviceMemoryProperties, 描述物理设备的内存类型，有VkMemoryType和VkMemoryHeap这两种，
* 一般来说，提到sharing mode，都是和queue访问有关，如果是exclusive，那么就不能并发访问，反之可以并发访问
* VkCommandBufferUsageFlagBits, 指定commandBuffer的使用方式（目前理解是这样），VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, 每次结束都要提交，并且下次需要重新record, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT用于secondary commandBuffer, 目前还未使用过。VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT ，可以被重复提交，目前还未使用过
* VkImageMemoryBarrier中的queueFamilyIndex用于queue family ownership的转换，转换Image layout不需要这个flag, 并且需要手动设置。
* memory同步用到的都是同一个vkCmdPipelineBarrier方法，其中根据barrier的src和dst access mask来确定，调用该方法时设定的src和dst pipeline stage
* 使用memory barrier进行image layout transition的时候，需要指定src和dst pipelinestage，其中VK_PIPELINE_STAGE_TRANSFER_BIT需要指定，但实际上它并不是实际的管线stage， 而是一个虚拟的stage。
* 教程中的layout transition以及copy buffer to image都是分开用一个单独的command buffer来做的，但是，使用一个单独command buffer去做，效率会更高，有时间可以看一下怎么做。

### Image View and Sampler

* 与swapchain类似，访问texture Image的方式也是创建一个image View，而非直接访问，创建过程主要就是填充一个VkImageViewCreateInfo
* Samplers, 相当于再textures的基础上做filtering（bilinear, trilinear）和transformation(UV addressing mode)
* 创建sampler, 需要填充一个VkSamplerCreateInfo
  * 需要指定magFilter, minFilter，分别对应oversample和under sample的问题。
  * 指定各向异性的数据，可以查询物理设备的限制
  * unnormalizedCoordinates （uv是否normalized)
  * compareEnable&&compareOp，和PCF有关，关于gpu gems里面具体的算法还没看懂
  * mipmap相关，Mode,Bias, min, max限制
* Vulkan当中sampler和image是若绑定关系，也就是说一个sampler可以采样2d texture，也可以采样3d texture
* 最后一点需要注意的就是，需要在createLogicalDevice以及挑选物理设备的函数中，分别在VkPhysicalDeviceFeatures中设置samplerAnisotropy为true







