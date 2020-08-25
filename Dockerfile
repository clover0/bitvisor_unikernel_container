FROM ubuntu:18.04

RUN apt-get -y update \
 && apt-get -y install \
        build-essential \
        mingw-w64 \
        make \
        gcc

WORKDIR /var/work

USER 1000

CMD /bin/bash