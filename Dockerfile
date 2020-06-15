FROM ubuntu:16.04

RUN apt-get -y update \
 && apt-get -y install \
        build-essential \
        mingw-w64 \
        make \
        gcc

WORKDIR /var/work

CMD /bin/bash