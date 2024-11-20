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
