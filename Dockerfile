FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

# install build dependencies to build and debug 
RUN apt-get update -y \
    && apt-get install -y g++ build-essential make cmake gdb gdbserver \
       rsync zip openssh-server git 

# install application dependencies
RUN apt-get install -y libopenblas-dev liblapack-dev python3-dev \ 
    libncurses5-dev libncursesw5-dev libasio-dev libboost-all-dev

# install other dependencies 
RUN apt-get install -y \
    libjavascriptcoregtk-4.0-dev \
    glib2.0-dev 

# install vim and less for editing files quickly
RUN apt-get install -y \
    less \ 
    vim 

# configure SSH for communication with Visual Studio 
RUN mkdir -p /var/run/sshd

RUN echo 'root:root' | chpasswd \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

RUN mkdir -p /opt/jsc
WORKDIR /opt/jsc

CMD ["/usr/sbin/sshd", "-D"]

EXPOSE 22