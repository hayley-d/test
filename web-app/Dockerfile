FROM node:20-alpine

WORKDIR /app

# Copy only the backend directory into the container
COPY backend/package*.json ./

# Install dependencies
RUN npm install

COPY backend/ ./

# Expose port 3001
EXPOSE 3001 

# Start the web app
CMD ["node", "server.js"]

