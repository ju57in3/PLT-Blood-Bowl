FROM debian:stable-slim

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    lcov \
    gcovr 

RUN apt-get update && apt-get install -y \
    libsfml-dev \
    libxml2-dev \
    libboost-all-dev

RUN apt-get update && apt-get install -y \
    dia

# Set the working directory
WORKDIR /app

COPY . /app

# Build the project
RUN mkdir docker-build && cd docker-build && cmake .. && make client

# Run the application
CMD ["./bin/client"]
