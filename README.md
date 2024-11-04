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

### Choosing the Right Settings for SwapChain

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

**Vulkan设置swap extent的单位是pixel, 对于苹果设备的视网膜分辨率，pixel数量可能远大于屏幕坐标，因此我们需要```glfwGetFramebufferSize```提前查询一下pixel width和pixel height**





​	





