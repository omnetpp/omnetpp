PATH C:\home\omnetpp40\omnetpp\bin;%PATH%
: notification.exe -u Cmdenv -c Fifo1 --cmdenv-status-frequency=10s %*
notification.exe -u Cmdenv -c TestNetwork --cmdenv-status-frequency=10s %*