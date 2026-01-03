FROM --platform=linux/amd64 ubuntu:22.04

RUN apt-get update && apt-get install -y \
    curl \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*

RUN curl -LO https://github.com/open-watcom/open-watcom-v2/releases/download/Current-build/ow-snapshot.tar.xz && \
    mkdir -p /opt/watcom && \
    tar -xf ow-snapshot.tar.xz -C /opt/watcom && \
    rm ow-snapshot.tar.xz

ENV WATCOM=/opt/watcom
ENV PATH=/opt/watcom/binl64:$PATH
ENV INCLUDE=/opt/watcom/h:/opt/watcom/h/nt
ENV LIB=/opt/watcom/lib386:/opt/watcom/lib386/nt

WORKDIR /src

ENTRYPOINT ["wcl386", "-bt=nt", "-l=nt_win"]
