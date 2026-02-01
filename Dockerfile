ARG BASE_IMAGE=ubuntu:latest
FROM ${BASE_IMAGE}

# Install base dependencies based on distro detection
RUN if [ -f /usr/bin/apt-get ]; then \
      apt-get update && apt-get install -y python3 cmake build-essential git pkg-config; \
    elif [ -f /usr/bin/dnf ]; then \
      dnf install -y python3 cmake gcc-c++ git pkgconfig; \
    elif [ -f /usr/bin/pacman ]; then \
      pacman -Sy --noconfirm python cmake base-devel git pkg-config; \
    fi

WORKDIR /workspace
COPY . .
RUN rm -rf build
CMD ["python3", "build.py"]
