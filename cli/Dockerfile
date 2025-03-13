# Use an official Node.js image
FROM node:20-slim

# Set working directory
WORKDIR /app

# Copy package.json and package-lock.json first (better caching)
COPY src/package*.json ./

# Install dependencies
RUN npm install

# Copy only the necessary application files
COPY src/ ./

# Expose port 3000
EXPOSE 3000

# Set default command to run the CLI
CMD ["node", "mpdb.js"]

