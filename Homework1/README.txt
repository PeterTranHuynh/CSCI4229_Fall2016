Download the gears program and run it on your system. This is to verify that you have a working
OpenGL environment and get your feet wet with OpenGL.

Submit via moodle:

1) The frame rate reported by the program when the window is different sizes,
specifically 1x1, 300x300 and full screen. Explain your results.

2) On some systems the frame rate is a small round number like 60 or 72 or 85
frames per second, and sometimes it is a large number, thousands or tens of
thousands of frames per second. Explain why this occurs.

3) Time required to complete the assignment.

Please note that the answer to these questions is not just "different hardware
run at different speeds".  I expect a thoughtful and well researched answer.

----

1)

These are the results of running the gears program in an Ubuntu virtual machine:

For a 1x1 window, the frame rate ranged from 1670 FPS to 2330 FPS.

For a 300x300 window, the frame rate ranged from 659 FPS to 789 FPS.

For a full screened window, the frame rate ranged from 54 FPS to 60 FPS.

This is an expected trend, as the window of the gear animation grows larger, the frame rate yielded will drop. The trend can most likely be explained by how the scaling of the gears function. As the window changes size, so does the gear, and thus the required amount of resources used to render, light, and animate it.



2)

The frame rate difference is the result of varying graphics hardware within each computer. For example, my laptop has a mobile variant of nVidia's GeForce GTX 700 series graphic card. The card itself has a dedicated amount of memory (2GB) and a separate processor designed for the sole purpose of processing the matrix math behind graphics. Other devices may possess varying degrees of graphics hardware, some with dedicated cards and other without. This difference in specialized processing power is the result of the differing frame rates. Hardware with more resources for graphical computing typically yield better results in visual performance. However, other aspects may also affect the frame rate, such as background processes, CPU strength, physical memory, and so forth. For example, running the program within a virtual machine most likely played a role in the frame rate, as running it on my native OS could've resulted in better performance. Other things to consider would also be OpenGL swapping intervals. Aspects, such as vSync, caps the frame rate to synchronize with hardware limitations to prevent tearing during run time. Most screens refresh around 60 to 120 hz, and syncing allows the frame buffer to match the refresh rate.



3)

This assignment took roughly ten to fifteen minutes to do. The majority of the time was done completing this write-up.