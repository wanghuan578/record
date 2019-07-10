分布式录制集群

概述：
本框架采用libevent作为网络底层，录制采用fork ffmpeg进程，缓存采用mongodb，回调采用消息队列。

性能：
单台（32核）2000路录制能力，系统负载较大，瓶颈在网卡。

编译：
make clean;make;make install
