# Stage 1: lightweight base image with build tools
FROM debian:bookworm-slim

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    make \
    g++ \
    libboost-all-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Working directory for the build
WORKDIR /app

# Copy Everything
COPY . .

# Build the project 
RUN make

# Expose port
EXPOSE 9005 

# Command to run the compiled database engine
CMD ["./build/main"]
